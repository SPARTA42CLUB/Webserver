#include "CgiRequestHandler.hpp"
#include "../../global/eventManager/EventManager.hpp"
#include "../server/connection/Connection.hpp"
CgiRequestHandler::CgiRequestHandler(Connection &connection,RequestMessage& req,std::string &uri,std::string &requestBody, std::string &method, std::string &cgiInterpreter, int &clientSocket)
: mConnection(connection)
, mReq(req)
, mUri(uri)
, mRequestBody(requestBody)
, mMethod(method)
, mCgiInterpreter(cgiInterpreter)
, mClientSocket(clientSocket)
{
	// mResponseMeassage 초기화?
}

void CgiRequestHandler::excuteCgi()
{
// extract query string from request body
    std::string cgi_path = mUri;
	std::string queryString = "";
	size_t queryPos = mUri.find("?");
	if (queryPos != std::string::npos)
	{
		cgi_path = mUri.substr(0, queryPos);
		queryString = mUri.substr(queryPos + 1);
	}
	int pipe_in[2];
    int pipe_out[2];
	if(pipe(pipe_in) == -1);// error4
	if(pipe(pipe_out) == -1);// error5
	pid_t pid = fork();
	if(pid == -1);// error6
	if (pid == 0) {
        // Child process
        close(pipe_in[1]);
        close(pipe_out[0]);
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
		char *argv[] = {mCgiInterpreter.c_str(), cgi_path.c_str(), NULL};
		char *envp[] = {"QUERY_STRING=" + queryString, "REQUEST_METHOD=" + mMethod, NULL};
		execve(mCgiInterpreter.c_str(), argv, envp);
		//error7
	}
	else
	{
        // Parent process
        close(pipe_in[0]);
        close(pipe_out[1]);

        write(pipe_in[1], mRequestBody.c_str(), mRequestBody.size());
        close(pipe_in[1]);
		mConnection.socket = pipe_out[0];
		EventManager::addReadEvent(pipe_out[0]);
	}
}
// 	if(mMethod == "GET")
// 	{
// 		int pipe[2];
// 		if(pipe(pipe) == -1);// error1
// 		pid_t pid = fork();
// 		if(pid == -1);// error2
// 		if(pid == 0)
// 		{
// 			dup2(pipe[1], STDOUT_FILENO);
// 			close(pipe[0]);
// 			close(pipe[1]);
// 			char *argv[] = {mCgiInterpreter.c_str(), cgi_path.c_str(), NULL};
// 			char *envp[] = {"QUERY_STRING=" + queryString, NULL};
// 			execve(mCgiInterpreter.c_str(), argv, envp);
// 			//error 3
// 		}
// 		else
// 		{
// 			close(pipe[1]);
// 			EventManager::addReadEvent(pipe[0]);

// 		}

// 	}
// 	else if(mMethod == "POST")
// 	{
//         int pipe_in[2];
//         int pipe_out[2];
// 		if(pipe(pipe_in) == -1);// error4
// 		if(pipe(pipe_out) == -1);// error5
// 		pid_t pid = fork();
// 		if(pid == -1);// error6
// 		if (pid == 0) {
//             // Child process
//             close(pipe_in[1]);
//             close(pipe_out[0]);
//             dup2(pipe_in[0], STDIN_FILENO);
//             dup2(pipe_out[1], STDOUT_FILENO);
// 			char *argv[] = {mCgiInterpreter.c_str(), cgi_path.c_str(), NULL};
// 			char *envp[] = {"QUERY_STRING=" + queryString, "REQUEST_METHOD=POST", NULL};
// 			execve(mCgiInterpreter.c_str(), argv, envp);
// 			//error 7
// 		}
// 		else
// 		{
//             // Parent process
//             close(pipe_in[0]);
//             close(pipe_out[1]);

//             write(pipe_in[1], mRequestBody.c_str(), mRequestBody.size());
//             close(pipe_in[1]);
// 			EventManager::addReadEvent(pipe_out[0]);
// 		}
// 	}
// 	else if(mMethod == "DELETE")
// 	{
// 		// delete
// 	}
// 	else if(mMethod == "HEAD")
// 	{
// 		// head
// 	}
// }

void CgiRequestHandler::makeCgiResponse()
{
	mResponseMessage.setStatusLine(req.getRequestLine().getHTTPVersion(), OK, "OK"); // <- RequestHandler에서 발취.
	mResponseMessage.addMessageBody()
	EventManager::addWriteEvent(mClientSocket);
}

CgiRequestHandler::~CgiRequestHandler()
{
}