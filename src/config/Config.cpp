#include "Config.hpp"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parse.hpp"
#include "ConfigException.hpp"
#include "color.hpp"

Config::Config(const std::string& configFilePath)
: keepalive_timeout(DEFAULT_TIMEOUT)
, serverConfigs()
{
    try
    {
        parse(configFilePath);
        verifyConfig();
    }
    catch(const ConfigException& e)
    {
        throw e;
    }

}

Config::~Config()
{
}

// Config 파일 파싱
void Config::parse(const std::string& configFilePath)
{
    std::ifstream file(configFilePath.c_str());
    if (!file.is_open())
    {
        throw ConfigException(FAILED_TO_OPEN_CONFIG_FILE);
    }

    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        try
        {
            if (line.find("keepalive_timeout") != std::string::npos)
            {
                parseKeepAliveTimeout(line);
            }
            if (line.find("server {") != std::string::npos)
            {
                parseServer(file);
            }
        }
        catch(const ConfigException& e)
        {
            throw e;
        }
    }
    file.close();
}
void Config::parseKeepAliveTimeout(std::string& line)
{
    std::istringstream iss(line);
    std::string key, value;
    iss >> key;
    getline(iss, value);
    if (key != "keepalive_timeout")
    {
        throw ConfigException(INVALID_CONFIG_FILE);
    }
    if (!isValidValue(value) || !isDigitStr(value))
    {
        throw ConfigException(INVALID_CONFIG_FILE);
    }
    keepalive_timeout = atoi(value.c_str());
    if (keepalive_timeout > MAX_TIMEOUT || keepalive_timeout == 0)
    {
        throw ConfigException(INVALID_CONFIG_FILE);
    }
}
// 서버 설정 파싱
void Config::parseServer(std::ifstream& file)
{
    ServerConfig serverConfig;
    std::string line;
    std::map<std::string, bool> duplicateCheck;

    // 'server {' 문자열의 끝인 '}'가 나올 때까지 반복
    while (std::getline(file, line) && line.find("}") == std::string::npos)
    {
        if (line.empty())
            continue;
        std::istringstream iss(line);
        std::string key, value;
        iss >> key;
        getline(iss, value);

        if (duplicateCheck[key])
            throw ConfigException(INVALID_SERVER_CONFIG);
        try
        {
            if (key == "host") serverConfig.parseHost(value);
            else if (key == "port") serverConfig.parsePort(value);
            else if (key == "root") serverConfig.parseRoot(value);
            else if (key == "index") serverConfig.parseIndex(value);
            else if (key == "client_max_body_size") serverConfig.parseClientMaxBodySize(value);
            else if (key == "error_page") serverConfig.parseErrorPage(value);
            else if (key == "location")
            {
                if (!serverConfig.isValidLocationPath(value))
                {
                    throw ConfigException(INVALID_LOCATION_CONFIG);
                }
                serverConfig.parseLocation(file, value);
            }
            else throw ConfigException(INVALID_SERVER_CONFIG);
        }
        catch(const ConfigException& e)
        {
            throw e;
        }
        if (key != "location" && key != "error_page")
        {
            duplicateCheck[key] = true;
        }
    }
    serverConfigs.push_back(serverConfig);
}
void Config::verifyConfig(void)
{
    if (serverConfigs.empty())
    {
        throw ConfigException(EMPTY_SERVER_CONFIG);
    }
    for (size_t i = 0; i < serverConfigs.size(); i++)
    {
        if (serverConfigs[i].host.empty())
        {
            throw ConfigException(HOST_NOT_EXIST);
        }
        if (serverConfigs[i].port == -1)
        {
            throw ConfigException(PORT_NOT_EXIST);
        }
        for (std::map<std::string, LocationConfig>::iterator locIt = serverConfigs[i].locations.begin(); locIt != serverConfigs[i].locations.end(); ++locIt)
        {
            if (!serverConfigs[i].root.empty() && locIt->CONFIG.root.empty())
            {
                locIt->CONFIG.root = serverConfigs[i].root;
            }
            // root + target uri(무조건 /로 시작함)가 붙으므로 미리 '/'를 제거해준다.
            // www/ + / -> www/
            if (locIt->CONFIG.root.back() == '/')
            {
                locIt->CONFIG.root.pop_back();
            }
            if (serverConfigs[i].root.empty() && locIt->CONFIG.root.empty() && locIt->CONFIG.redirect.empty())
            {
                throw ConfigException(ROOT_NOT_EXIST);
            }
            if (!serverConfigs[i].index.empty() && locIt->CONFIG.index.empty())
            {
                locIt->CONFIG.index = serverConfigs[i].index;
            }
            if (locIt->CONFIG.allow_methods.empty())
            {
                for (size_t j = 0; j < LocationConfig::implementMethodsSize; j++)
                {
                    locIt->CONFIG.allow_methods[LocationConfig::implementMethods[j]] = true;
                }
            }
        }
    }
}
const std::vector<ServerConfig>& Config::getServerConfigs() const
{
    return serverConfigs;
}
void Config::print(void) const
{
    std::cout << color::FG_WHITE << "Keepalive timeout: " << keepalive_timeout << "\n\n";

    for (size_t i = 0; i < serverConfigs.size(); i++)
    {
        std::cout << color::FG_WHITE << "Server " << i + 1 << ":\n" << color::RESET
        << "host: " << serverConfigs[i].host << '\n'
        << "port: " << serverConfigs[i].port << '\n'
        << "root: " << serverConfigs[i].root << '\n'
        << "index: " << serverConfigs[i].index << '\n'
        << "client_max_body_size: " << serverConfigs[i].client_max_body_size << '\n'
        << "error_pages:\n";
        for (std::map<size_t, std::string>::const_iterator it = serverConfigs[i].error_pages.begin(); it != serverConfigs[i].error_pages.end(); ++it)
        {
            std::cout << "    " << it->first << " " << it->second << '\n';
        }
        for (std::map<std::string, LocationConfig>::const_iterator it = serverConfigs[i].locations.begin(); it != serverConfigs[i].locations.end(); ++it)
        {
            std::cout << color::FG_YELLOW << "locations: " << it->LOCATION << ":" << '\n' << color::RESET
            << "    " << "root: " << it->CONFIG.root << '\n'
            << "    " << "index: " << it->CONFIG.index << '\n'
            << "    " << "allow_methods: ";
            for (std::map<std::string, bool>::const_iterator it2 = it->CONFIG.allow_methods.begin(); it2 != it->CONFIG.allow_methods.end(); ++it2)
            {
                std::cout << it2->first << " ";
            }
            std::cout << "\n    " << "directory_listing: " << (it->CONFIG.directory_listing ? "on" : "off") << '\n'
            << "    " << "redirect: " << it->CONFIG.redirect << '\n'
            << "    " << "cgi_interpreter: " << it->CONFIG.cgi_interpreter << '\n';
        }
        std::cout << std::endl;
    }
}
size_t Config::getKeepAliveTime() const
{
    return keepalive_timeout;
}


