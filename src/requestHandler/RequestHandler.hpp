#pragma once

#include <string>
#include "Config.hpp"
#include "Connection.hpp"
#include "HttpException.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"

class RequestHandler
{
private:
    std::map<int, Connection*>& mConnectionsMap;
    const int mSocket;
    const RequestMessage* mRequestMessage;
    ResponseMessage* mResponseMessage;  // 동적할당 해서 반환
    const ServerConfig& mServerConfig;
    LocationConfig mLocConfig;
    std::string mPath;
    std::string mQueryString;
    bool mbIsCGI;

    // Processing Request
    void processRequestPath(void);
    bool matchExactLocation(const std::string& reqTarget, const std::map<std::string, LocationConfig>& locations);
    void matchClosestLocation(const std::string& reqTarget, const std::map<std::string, LocationConfig>& locations);
    bool identifyCGIRequest(const std::string& reqTarget, std::map<std::string, LocationConfig>::const_iterator& locIt);

    int handleMethod(void);
    // Response (Success)
    int getRequest(void);
    int rangeRequest(void);
    int headRequest(void);
    int postRequest(void);
    int deleteRequest(void);
    // 3xx
    int redirect(void);

    int handleFileUpload(const int fStatus);
    std::string parseContentDisposition(void);
    void handleIndex(void);
    int handleAutoindex(void);

    void addContentType(void);
    void addConnectionHeader(void);

    void executeCGI(void);

public:
    RequestHandler(std::map<int, Connection*>& connectionsMap, const int socket);
    ResponseMessage* handleRequest(void);
};
