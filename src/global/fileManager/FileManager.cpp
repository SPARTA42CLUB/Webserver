#include "FileManager.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include "Logger.hpp"
#include "SysException.hpp"

#include <iostream>

const int NGINX_MAX_FILEPATH = 50;

namespace fileManager
{
    // 소켓 논블로킹 설정
    void setNonBlocking(int fd)
    {
        if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        {
            close(fd);
            throw SysException(FAILED_TO_SET_NON_BLOCKING);
        }
    }
    int getFileStatus(const std::string& path)
    {
        struct stat info;
        // 파일이 없거나 접근 권한이 없는 경우
        if (stat(path.c_str(), &info) != 0)
        {
            return NONE;
        }
        // 디렉토리인지 확인
        if (info.st_mode & S_IFDIR)
        {
            return DIRECTORY;
        }
        // 일반 파일인지 확인
        else if (info.st_mode & S_IFREG)
        {
            return FILE;
        }
        else
        {
            return NONE;
        }
    }
    bool isReadable(const std::string& path)
    {
        return (access(path.c_str(), R_OK) == 0);
    }
    bool isWritable(const std::string& path)
    {
        return (access(path.c_str(), W_OK) == 0);
    }
    bool isExist(const std::string& path)
    {
        return (access(path.c_str(), F_OK) == 0);
    }
    bool deleteFile(const std::string& path)
    {
        return (std::remove(path.c_str()) == 0);
    }
    const std::string listDirectoryContents(const std::string& path)
    {
        struct dirent* entry;
        struct stat statbuf;
        Logger& logger = Logger::getInstance();

        DIR* dir = opendir(path.c_str());
        std::ostringstream oss;

        oss << "<html>"
            << "<head><title>Index of " << path << "</title></head>"
            << "<body>"
            << "<h1>Index of " << path << "</h1>"
            << "<hr>"
            << "<pre>"
            << "<a href=\"../\">../</a>\n";
        if (dir == NULL)
        {
            logger.logError("Failed to open directory");
            return "";
        }
        while ((entry = readdir(dir)) != NULL)
        {
            // 경로와 파일명을 결합하여 전체 경로 생성
            std::string dirName(entry->d_name);
            std::string fullPath = path + (path.back() == '/' ? "" : "/") + dirName;

            std::cout << fullPath << std::endl;

            // 파일 상태 정보 얻기
            if (stat(fullPath.c_str(), &statbuf) == -1)
            {
                logger.logError("Failed to get file status for " + fullPath);
                continue;
            }
            if (dirName == "." || dirName == "..")
            {
                continue;
            }

            size_t width = NGINX_MAX_FILEPATH + 1;  // 50글자보다 길 경우에 substr(0, 47) + "..>" 하여 50글자로 제한
            oss << "<a href=\"" << dirName << (S_ISDIR(statbuf.st_mode) ? "/" : "") << "\">";
            if (dirName.size() > NGINX_MAX_FILEPATH)
            {
                oss << dirName.substr(0, 47) << "..>"
                    << "</a>";
                width = 1;
            }
            else
            {
                oss << dirName << (S_ISDIR(statbuf.st_mode) ? "/" : "") << "</a>";
                width -= dirName.size() + (S_ISDIR(statbuf.st_mode) ? 1 : 0);
            }

            std::tm* tm = std::localtime(&statbuf.st_mtime);
            char buffer[100];
            std::strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M", tm);

            std::string spaces(width, ' ');
            oss << spaces << buffer;
            oss << std::right << std::setw(20) << (S_ISDIR(statbuf.st_mode) ? "-" : std::to_string(statbuf.st_size)) << '\n';
        }
        oss << "</pre><hr></body></html>";
        closedir(dir);
        return oss.str();
    }
}
