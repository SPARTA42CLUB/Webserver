#include "EventManager.hpp"
#include <unistd.h>
#include "SysException.hpp"

EventManager& EventManager::getInstance()
{
    static EventManager instance;
    return instance;
}
EventManager::EventManager()
: kq(kqueue())
{
    if (kq == -1)
    {
        throw SysException(FAILED_TO_CREATE_KQUEUE);
    }
}
EventManager::~EventManager()
{
    close(kq);
}
void EventManager::addReadEvent(int socket)
{
    addEvent(socket, EVFILT_READ, EV_ADD | EV_ENABLE);
}

void EventManager::addWriteEvent(int socket)
{
    addEvent(socket, EVFILT_WRITE, EV_ADD | EV_ENABLE);
}

void EventManager::deleteReadEvent(int socket)
{
    addEvent(socket, EVFILT_READ, EV_DELETE);
}

void EventManager::deleteWriteEvent(int socket)
{
    addEvent(socket, EVFILT_WRITE, EV_DELETE);
}

// throw-safe 함수 (에러 처리 해야 할 수도?)
void EventManager::addEvent(const int socket, const int16_t filter, const uint16_t flags)
{
    struct kevent evSet;
    EV_SET(&evSet, socket, filter, flags, 0, 0, NULL);

    if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
    {
        close(socket);
    }
}

std::vector<struct kevent> EventManager::getCurrentEvents()
{
    /*
    kevent는 이벤트가 발생해야 반환되는데,
    timeout을 설정하면 이벤트가 발생하지 않아도
    kevent함수가 반환된다.
    */
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    /*
     * !!수신이 감지된 이벤트를 가져온다!!
     * events 이벤트 배열에 현재 변경이 감지된 이벤트를 저장한다.
     * numEvents에는 배열에 담긴 이벤트의 수가 담긴다.
     */
    struct kevent events[1024];
    int numEvents = kevent(kq, NULL, 0, events, 1024, &timeout);
    if (numEvents == -1)
    {
        throw SysException(FAILED_TO_GET_KEVENT);
    }

    return std::vector<struct kevent>(events, events + numEvents);
}
