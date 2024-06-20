#pragma once

#include <vector>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include "Config.hpp"
#include "EventManager.hpp"
#include "RequestMessage.hpp"
#include "ResponseMessage.hpp"

class Server {
public:
	Server(const Config& config);
	~Server();

	void run();

private:
	const Config& config;
	EventManager eventManager;
	std::vector<int> serverSockets;
	std::vector<int> clientSockets;
	std::map<int, ServerConfig> socketToConfigMap;
	std::map<int, time_t> last_activity_map;
	std::map<int, std::string> recvDataMap;
	std::map<int, std::vector<ResponseMessage> > responsesMap;
	std::map<int, bool> isChunkedMap;

	void setupServerSockets();
	void setNonBlocking(int socket);

	void acceptClient(int serverSocket);
	void handleClientReadEvent(struct kevent& event);
	bool isCompleteRequest(const std::string& data, size_t& requestLength, bool& isChunked);
	bool isCompleteChunk(const std::string& data, size_t& requestLength, bool& isLastChunk);

	void handleChunkedRequest(uintptr_t socket, std::string& requestData);
	void handleNormalRequest(uintptr_t socket, std::string& requestData, size_t requestLength);

	void handleClientWriteEvent(struct kevent& event);

	const ResponseMessage &Server::createResponse(const std::string& requestData, const ServerConfig& config) const;

    void logHTTPMessage(int socket, ResponseMessage& res, const std::string& reqData);
	void sendResponse(int socket, ResponseMessage& res);
	void closeConnection(int socket);

	void update_last_activity(int socket);
	void checkTimeout();
};
