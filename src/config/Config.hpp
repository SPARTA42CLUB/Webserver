#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerConfig.hpp"

class Config
{
private:
    size_t keepalive_timeout;
    size_t cgi_timeout;
    std::vector<ServerConfig> serverConfigs;

    void parse(const std::string& configFilePath);
    void parseKeepAliveTimeout(std::string& line);
    void parseServer(std::ifstream& file);
    void verifyConfig(void);

    Config(const Config& rhs);
    Config& operator=(const Config& rhs);

public:
    Config(const std::string& configFilePath);
    ~Config();
    const std::vector<ServerConfig>& getServerConfigs() const;
    size_t getKeepAliveTime() const;
    size_t getCgiTimeout() const;

    // TEST: 파싱 결과 출력
    void print(void) const;
};

#endif
