#ifndef HTTP_EXCEPTION_HPP
#define HTTP_EXCEPTION_HPP

#include <exception>
#include <string>
#include "StartLine.hpp"

class HttpException : public std::exception
{
private:
    int mStatusCode;

public:
    HttpException(const int statusCode);
    virtual ~HttpException() _NOEXCEPT;
    int getStatusCode() const;
};

#endif
