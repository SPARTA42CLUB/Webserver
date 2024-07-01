#include "MessageBody.hpp"

MessageBody::MessageBody()
: mContent("")
{
}
MessageBody::~MessageBody()
{
}
void MessageBody::addBody(const std::string& body)
{
    mContent += body;
}
const std::string& MessageBody::toString() const
{
    return mContent;
}
size_t MessageBody::size() const
{
    return mContent.size();
}
void MessageBody::clear()
{
    mContent.clear();
}
