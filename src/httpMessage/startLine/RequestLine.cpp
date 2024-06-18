#include "RequestLine.hpp"
#include <sstream>

RequestLine::RequestLine()
: mMethod("")
, mRequestTarget("")
, mHTTPVersion("")
{
}
RequestLine::~RequestLine()
{
}
void RequestLine::parseRequestLine(const std::string& requestLine)
{
    // TODO: parse request line
    // request-line = <method> SP <request-URL> SP <HTTP-version> CRLF

    std::istringstream iss(requestLine);
    if (iss >> mMethod >> mRequestTarget >> mHTTPVersion)
    {
        // success
    }
    else
    {
        // fail
        mMethod = "";
        mRequestTarget = "";
        mHTTPVersion = "";
        throw std::exception();
    }
    
}
std::string RequestLine::getMethod(void) const
{
    return mMethod;
}
std::string RequestLine::getRequestTarget(void) const
{
    return mRequestTarget;
}
std::string RequestLine::getHTTPVersion(void) const
{
    return mHTTPVersion;
}
