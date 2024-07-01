#include "RequestHandler.hpp"
#include <fcntl.h>
#include <ctime>
#include <fstream>
#include "EventManager.hpp"
#include "FileManager.hpp"
#include "RangeRequestReader.hpp"
#include "SysException.hpp"

RequestHandler::RequestHandler(std::map<int, Connection*>& connectionsMap, const int socket)
: mConnectionsMap(connectionsMap)
, mSocket(socket)
, mRequestMessage(connectionsMap[socket]->request)
, mResponseMessage()
, mServerConfig(connectionsMap[socket]->serverConfig)
, mLocConfig()
, mPath()
, mQueryString()
, mbIsCGI(false)
{
}

ResponseMessage* RequestHandler::handleRequest(void)
{
    processRequestPath();
    try
    {
        if (mbIsCGI)
        {
            executeCGI();
            return NULL;
        }
    }
    catch (const SysException& e)
    {
        mResponseMessage = new ResponseMessage();
        mResponseMessage->setByStatusCode(SERVICE_UNAVAILABLE, mServerConfig);
        return mResponseMessage;
    }
    mResponseMessage = new ResponseMessage();

    int statusCode = handleMethod();
    if (checkStatusCode(statusCode) == false)
    {
        mResponseMessage->setByStatusCode(statusCode, mServerConfig);
        return mResponseMessage;
    }

    addConnectionHeader();

    return mResponseMessage;
}
void RequestHandler::processRequestPath(void)
{
    const std::string& reqTarget = mRequestMessage->getRequestLine().getRequestTarget();
    const std::map<std::string, LocationConfig>& locations = mServerConfig.locations;

    if (matchExactLocation(reqTarget, locations))
    {
        return;
    }
    matchClosestLocation(reqTarget, locations);
}
bool RequestHandler::matchExactLocation(const std::string& reqTarget, const std::map<std::string, LocationConfig>& locations)
{
    std::map<std::string, LocationConfig>::const_iterator it = locations.find(reqTarget);
    if (it != locations.end())
    {
        mLocConfig = it->CONFIG;
        mPath = mLocConfig.root + it->LOCATION;
        return true;
    }

    if (reqTarget.back() == '/')
    {
        return false;
    }

    it = locations.find(reqTarget + "/");
    if (it != locations.end())
    {
        mLocConfig = it->CONFIG;
        mPath = mLocConfig.root + it->LOCATION;
        return true;
    }

    return false;
}
void RequestHandler::matchClosestLocation(const std::string& reqTarget, const std::map<std::string, LocationConfig>& locations)
{
    const LocationConfig* locConf;
    size_t maxMatchCnt = 0;
    for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (it->LOCATION.front() == '.' && identifyCGIRequest(reqTarget, it))
        {
            mbIsCGI = true;
            return;
        }
        size_t matchCnt = 0;
        size_t trailingSlash = it->LOCATION.size() - (it->LOCATION.back() == '/');
        if (reqTarget.find(it->LOCATION) == 0 && reqTarget[trailingSlash] == '/')
        {
            matchCnt = it->LOCATION.size();
        }
        if (matchCnt > maxMatchCnt)
        {
            maxMatchCnt = matchCnt;
            locConf = &(it->CONFIG);
        }
    }
    mPath = locConf->root + reqTarget;
    mLocConfig = *locConf;
    return;
}
bool RequestHandler::identifyCGIRequest(const std::string& reqTarget, std::map<std::string, LocationConfig>::const_iterator& locIt)
{
    const std::string& locationPath = locIt->LOCATION;
    size_t dotIdx = reqTarget.find(locationPath);

    if (dotIdx == std::string::npos || reqTarget.substr(dotIdx, locationPath.size()) != locationPath)
    {
        return false;
    }

    size_t queryIdx = reqTarget.find('?');
    if ((queryIdx == std::string::npos && reqTarget.size() > dotIdx + locationPath.size()) ||
        (queryIdx != std::string::npos && queryIdx != dotIdx + locationPath.size()))
    {
        return false;
    }
    mLocConfig = locIt->CONFIG;
    mPath = mLocConfig.root + reqTarget.substr(0, dotIdx + locationPath.size());
    if (queryIdx != std::string::npos)
    {
        mQueryString = reqTarget.substr(queryIdx + 1);
    }
    return true;
}
void RequestHandler::executeCGI(void)
{
    int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
    {
        throw SysException(FAILED_TO_CREATE_PIPE);
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        throw SysException(FAILED_TO_FORK);
    }
    if (pid == 0)
    {
        // Child process
        close(pipe_in[WRITE_END]);
        close(pipe_out[READ_END]);
        dup2(pipe_in[READ_END], STDIN_FILENO);
        dup2(pipe_out[WRITE_END], STDOUT_FILENO);

        std::vector<char> interpreter_cstr(mLocConfig.cgi_interpreter.begin(), mLocConfig.cgi_interpreter.end());
        std::vector<char> path_cstr(mPath.begin(), mPath.end());
        interpreter_cstr.push_back('\0');
        path_cstr.push_back('\0');
        char* argv[] = {interpreter_cstr.data(), path_cstr.data(), NULL};

        std::string queryStringEnv = "QUERY_STRING=" + mQueryString;
        std::string requestMethodEnv = "REQUEST_METHOD=" + mRequestMessage->getRequestLine().getMethod();
        std::vector<char> queryStringEnv_cstr(queryStringEnv.begin(), queryStringEnv.end());
        std::vector<char> requestMethodEnv_cstr(requestMethodEnv.begin(), requestMethodEnv.end());
        queryStringEnv_cstr.push_back('\0');
        requestMethodEnv_cstr.push_back('\0');
        char* envp[] = {queryStringEnv_cstr.data(), requestMethodEnv_cstr.data(), NULL};

        execve(argv[READ_END], argv, envp);
        throw SysException(FAILED_TO_EXEC);
    }
    else
    {
        // Parent process
        close(pipe_in[READ_END]);
        close(pipe_out[WRITE_END]);

        fileManager::setNonBlocking(pipe_in[WRITE_END]);
        fileManager::setNonBlocking(pipe_out[READ_END]);

        Connection* parentConnection = mConnectionsMap[mSocket];

        parentConnection->childSocket[WRITE_END] = pipe_in[WRITE_END];
        parentConnection->childSocket[READ_END] = pipe_out[READ_END];
        mConnectionsMap[pipe_in[WRITE_END]] = new Connection(pipe_in[WRITE_END], parentConnection->serverConfig, mSocket, mRequestMessage->getMessageBody().toString());
        mConnectionsMap[pipe_out[READ_END]] = new Connection(pipe_out[READ_END], parentConnection->serverConfig, mSocket);

        EventManager::getInstance().addWriteEvent(pipe_in[WRITE_END]);
    }
}
// method에 따른 분기 처리
int RequestHandler::handleMethod(void)
{
    std::string method = mRequestMessage->getRequestLine().getMethod();

    if (mLocConfig.allow_methods.find(method) == mLocConfig.allow_methods.end())
    {
        return METHOD_NOT_ALLOWED;
    }

    if (!mLocConfig.redirect.empty())
    {
        return redirect();
    }
    else if (method == "GET")
    {
        return getRequest();
    }
    else if (method == "HEAD")
    {
        return headRequest();
    }
    else if (method == "POST")
    {
        return postRequest();
    }
    else if (method == "DELETE")
    {
        return deleteRequest();
    }
    else
    {
        return METHOD_NOT_ALLOWED;
    }
}
int RequestHandler::redirect(void)
{
    mResponseMessage->addMessageBody("<html><head><title>302 Found</title></head><body><h1>302 Found</h1><p>This resource has been moved to <a href=\"" + mLocConfig.redirect + "\">" + mLocConfig.redirect + "</a>.</p></body></html>");
    mResponseMessage->setStatusLine("HTTP/1.1", FOUND, "Found");
    mResponseMessage->addResponseHeaderField("Location", mLocConfig.redirect);
    mResponseMessage->addResponseHeaderField("Connection", "close");
    mResponseMessage->addSemanticHeaderFields();
    return FOUND;
}
int RequestHandler::getRequest(void)
{
    handleIndex();
    int fStatus = fileManager::getFileStatus(mPath);
    if (fStatus == fileManager::DIRECTORY && mLocConfig.directory_listing)
        return handleAutoindex();

    if (fStatus == fileManager::NONE)
    {
        return NOT_FOUND;
    }

    std::ifstream file(mPath, std::ios::binary);
    if (!file.is_open() || fStatus != fileManager::FILE)
    {
        return FORBIDDEN;
    }
    // NOTE: read, write 사용하여 폐기
    // Range 요청 판별
    // if (mRequestMessage->getRequestHeaderFields().hasField("Range"))
    // {
    //     std::string rangeHeader = mRequestMessage->getRequestHeaderFields().getField("Range");
    //     // Check if the header starts with "bytes="
    //     if (rangeHeader.substr(0, 6) == "bytes=")
    //     {
    //         file.close();
    //         return rangeRequest();
    //     }
    // }
    std::ostringstream oss;
    oss << file.rdbuf();

    mResponseMessage->setStatusLine(mRequestMessage->getRequestLine().getHTTPVersion(), OK, "OK");
    mResponseMessage->addMessageBody(oss.str());
    mResponseMessage->addSemanticHeaderFields();
    addContentType();

    file.close();
    return OK;
}
int RequestHandler::headRequest(void)
{
    int statusCode = getRequest();
    mResponseMessage->clearMessageBody();

    return statusCode;
}
int RequestHandler::postRequest(void)
{
    // NOTE: 파일에 대해 있다면 Append, 없다면 Create 후 바디를 쓴다.
    // 디렉토리 경로에 `Content-Disposition` 헤더 필드 안으로 filename이 있다면 그 파일명으로 저장

    // 디렉토리에 index 붙이기
    handleIndex();
    int fStatus = fileManager::getFileStatus(mPath);
    // 디렉토리가 아닐 때, 파일이 존재하면 파일 뒤에 데이터를 추가, 파일이 없으면 파일을 생성하고 데이터를 추가
    if (fStatus != fileManager::DIRECTORY)
    {
        return handleFileUpload(fStatus);
    }

    // 디렉토리인데, Content-Disposition 헤더 필드가 없으면 403 Forbidden
    const std::string filename = parseContentDisposition();
    if (filename.empty())
    {
        return FORBIDDEN;
    }

    // mPath의 뒤에 filename을 붙여서 파일 경로를 만든다.
    mPath.back() != '/' ? mPath += "/" : mPath;
    mPath += filename;

    fStatus = fileManager::getFileStatus(mPath);
    return handleFileUpload(fStatus);
}
int RequestHandler::deleteRequest()
{
    if (fileManager::isExist(mPath) == false)
    {
        return NOT_FOUND;
    }
    if (fileManager::deleteFile(mPath) == false)
    {
        return FORBIDDEN;
    }
    mResponseMessage->setStatusLine(mRequestMessage->getRequestLine().getHTTPVersion(), OK, "OK");
    mResponseMessage->addResponseHeaderField("Content-Type", "text/html");
    mResponseMessage->addMessageBody("<html><body><h1>File deleted.</h1></body></html>");
    mResponseMessage->addSemanticHeaderFields();

    return OK;
}
int RequestHandler::handleFileUpload(const int fStatus)
{
    std::ofstream file(mPath, std::ios::app | std::ios::binary);
    if (!file.is_open())
    {
        return FORBIDDEN;
    }
    file << mRequestMessage->getMessageBody().toString();
    file.close();
    if (fStatus == fileManager::FILE)
    {
        mResponseMessage->setStatusLine(mRequestMessage->getRequestLine().getHTTPVersion(), OK, "OK");
        mResponseMessage->addMessageBody("<html><body><h1>File uploaded.</h1><h2>path: " + mPath + "</h2></body></html>");
    }
    else
    {
        mResponseMessage->setStatusLine(mRequestMessage->getRequestLine().getHTTPVersion(), CREATED, "Created");
        mResponseMessage->addMessageBody("<html><body><h1>File created.</h1><h2>path: " + mPath + "</h2></body></html>");
    }
    mResponseMessage->addResponseHeaderField("Content-Type", "text/html");
    mResponseMessage->addSemanticHeaderFields();
    return (fStatus == fileManager::FILE ? OK : CREATED);
}
std::string RequestHandler::parseContentDisposition(void)
{
    std::string filename = mRequestMessage->getRequestHeaderFields().getField("Content-Disposition");
    if (filename.empty())
    {
        filename = mRequestMessage->getRequestHeaderFields().getField("content-disposition");
    }
    if (filename.empty())
    {
        return "";
    }

    // Content-Disposition 헤더 필드에서 filename을 추출
    size_t idx = filename.find("filename=");
    if (idx == std::string::npos)
    {
        return "";
    }
    filename = filename.substr(idx + 10);
    idx = filename.find("\"");
    if (idx == std::string::npos)
    {
        return "";
    }
    filename = filename.substr(0, idx);
    return filename;
}
int RequestHandler::handleAutoindex()
{
    const std::string dirList = fileManager::listDirectoryContents(mPath);
    if (dirList.empty())
    {
        return FORBIDDEN;
    }
    mResponseMessage->setStatusLine(mRequestMessage->getRequestLine().getHTTPVersion(), OK, "OK");
    mResponseMessage->addMessageBody(dirList);
    mResponseMessage->addSemanticHeaderFields();
    return OK;
}
void RequestHandler::handleIndex()
{
    if (fileManager::getFileStatus(mPath) != fileManager::DIRECTORY || mLocConfig.index.empty())
    {
        return;
    }
    if (mPath.back() != '/')
    {
        mPath += "/";
    }
    mPath += mLocConfig.index;
    return;
}
// Connection 헤더 필드 추가
void RequestHandler::addConnectionHeader(void)
{
    if (mRequestMessage->getRequestHeaderFields().hasField("Connection") == true)
    {
        mResponseMessage->addResponseHeaderField("Connection", mRequestMessage->getRequestHeaderFields().getField("Connection"));
    }
    else
    {
        mResponseMessage->addResponseHeaderField("Connection", "keep-alive");
    }
}
void RequestHandler::addContentType(void)
{
    // NOTE: Config에 MIME 타입 추가
    std::string ext = mPath.substr(mPath.find_last_of('.') + 1);
    if (ext == "html")
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "text/html");
    }
    else if (ext == "css")
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "text/css");
    }
    else if (ext == "js")
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "text/javascript");
    }
    else if (ext == "txt")
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "text/plain");
    }
    else if (ext == "jpg" || ext == "jpeg")
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "image/jpeg");
    }
    else if (ext == "png")
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "image/png");
    }
    else
    {
        mResponseMessage->addResponseHeaderField("Content-Type", "application/octet-stream");
    }
}
/* NOTE: 폐기 */
int RequestHandler::rangeRequest(void)
{
    // Extract range values
    std::string rangeHeader = mRequestMessage->getRequestHeaderFields().getField("Range");
    std::vector<std::pair<size_t, size_t> > ranges;  // 벡터 타입 명시적으로 지정

    // Remove "bytes=" from the header
    rangeHeader = rangeHeader.substr(6);

    // Split the header by commas to handle multiple ranges
    std::istringstream iss(rangeHeader);
    std::string rangePart;
    while (std::getline(iss, rangePart, ','))
    {
        size_t dashPos = rangePart.find('-');
        if (dashPos != std::string::npos)
        {
            size_t rangeStart = std::stoul(rangePart.substr(0, dashPos));
            size_t rangeEnd = std::stoul(rangePart.substr(dashPos + 1));
            ranges.push_back(std::make_pair(rangeStart, rangeEnd));
        }
    }

    // Create a RangeRequestReader instance
    RangeRequestReader reader(mPath);

    // Add all ranges to the reader
    for (std::vector<std::pair<size_t, size_t> >::const_iterator it = ranges.begin(); it != ranges.end(); ++it)
    {
        reader.addRange(it->LOCATION, it->CONFIG);  // 반복자 사용하여 요소 접근
    }

    // Process the request and get the response body
    std::string responseBody = reader.processRequest();

    // Set HTTP response status line and headers
    mResponseMessage->setStatusLine(mRequestMessage->getRequestLine().getHTTPVersion(), PARTIAL_CONTENT, "Partial Content");
    mResponseMessage->addResponseHeaderField("Content-Type", "multipart/byteranges; boundary=BOUNDARY_STRING");
    mResponseMessage->addMessageBody(responseBody);

    return OK;
}
