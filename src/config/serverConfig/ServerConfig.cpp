#include "ServerConfig.hpp"
#include <sstream>
#include "ConfigException.hpp"
#include "parse.hpp"
#include <vector>

ServerConfig::ServerConfig()
: host()
, port(-1)
, root()
, index()
, client_max_body_size(DEFAULT_MAX_BODY_SIZE)
, error_pages()
, locations()
{
}
ServerConfig::~ServerConfig()
{
}
ServerConfig::ServerConfig(const ServerConfig& rhs)
: host(rhs.host)
, port(rhs.port)
, root(rhs.root)
, index(rhs.index)
, client_max_body_size(rhs.client_max_body_size)
, error_pages(rhs.error_pages)
, locations(rhs.locations)
{
}
ServerConfig& ServerConfig::operator=(const ServerConfig& rhs)
{
    if (this == &rhs)
        return *this;
    host = rhs.host;
    port = rhs.port;
    root = rhs.root;
    index = rhs.index;
    client_max_body_size = rhs.client_max_body_size;
    error_pages = rhs.error_pages;
    locations = rhs.locations;
    return *this;
}
void ServerConfig::parseLocation(std::ifstream& file, std::string& locationPath, LocationConfig* parentsLocation)
{
    LocationConfig locationConfig;
    std::string line;
    std::map<std::string, bool> duplicateCheck;
    if (parentsLocation)
        locationConfig = *parentsLocation;

    while (std::getline(file, line) && line.find("}") == std::string::npos)
    {
        if (line.empty())
            continue;
        std::istringstream iss(line);
        std::string key, value;
        iss >> key;
        getline(iss, value);
        if (duplicateCheck[key])
            throw ConfigException(INVALID_LOCATION_CONFIG);
        try
        {
            if (key == "root") locationConfig.parseRoot(value);
            else if (key == "index") locationConfig.parseIndex(value);
            else if (key == "allow_methods") locationConfig.parseAllowMethods(value);
            else if (key == "directory_listing") locationConfig.parseDirectoryListing(value);
            else if (key == "redirect") locationConfig.parseRedirect(value);
            else if (key == "cgi_interpreter") locationConfig.parseCGI(value);
            else if (key == "location")
            {
                if (!isValidLocationPath(value))
                {
                    throw ConfigException(INVALID_LOCATION_CONFIG);
                }
                if (value.find(locationPath) != 0)
                {
                    throw ConfigException(INVALID_LOCATION_CONFIG);
                }
                parseLocation(file, value, &locationConfig);
            }
            else
                throw ConfigException(INVALID_LOCATION_CONFIG);
        }
        catch (const ConfigException& e)
        {
            throw e;
        }
        if (key != "location")
        {
            duplicateCheck[key] = true;
        }
    }
    locations[locationPath] = locationConfig;
}
void ServerConfig::parseHost(std::string& value)
{
    if (!isValidValue(value))
        throw ConfigException(INVALID_SERVER_CONFIG);
    host = value;
}
void ServerConfig::parsePort(std::string& value)
{
    if (!isValidValue(value) || !isDigitStr(value))
        throw ConfigException(INVALID_SERVER_CONFIG);
    port = atoi(value.c_str());
    if (port > MAX_PORT_NUM || port == 0)
    {
        throw ConfigException(INVALID_SERVER_CONFIG);
    }
}
void ServerConfig::parseRoot(std::string& value)
{
    if (!isValidValue(value))
        throw ConfigException(INVALID_SERVER_CONFIG);
    root = value;
}
void ServerConfig::parseIndex(std::string& value)
{
    if (!isValidValue(value))
        throw ConfigException(INVALID_SERVER_CONFIG);
    index = value;
}
void ServerConfig::parseClientMaxBodySize(std::string& value)
{
    if (!isValidValue(value) || !isDigitStr(value))
        throw ConfigException(INVALID_SERVER_CONFIG);
    client_max_body_size = atoi(value.c_str());
    if (client_max_body_size == 0)
        throw ConfigException(INVALID_SERVER_CONFIG);
}
void ServerConfig::parseErrorPage(std::string& value)
{
    trim(value);
    std::istringstream iss(value);
    std::vector<int> statuses;
    std::string tmp;
    while (getline(iss, tmp, ' '))
    {
        if (iss.eof())
            break;
        if (!isDigitStr(tmp) || tmp.empty())
            throw ConfigException(INVALID_SERVER_CONFIG);
        size_t status = atoi(tmp.c_str());
        if (status < 100 || status > 599)
            throw ConfigException(INVALID_SERVER_CONFIG);
        statuses.push_back(status);
    }
    if (!isValidValue(tmp))
        throw ConfigException(INVALID_SERVER_CONFIG);
    for (std::vector<int>::iterator it = statuses.begin(); it != statuses.end(); ++it)
        error_pages[*it] = tmp;
}
bool ServerConfig::isValidLocationPath(std::string& locationPath)
{
    if (locationPath.back() != '{' || locationPath.find("//") != std::string::npos)
    {
        return false;
    }
    locationPath.pop_back();
    trim(locationPath);
    if (locationPath.empty())
    {
        return false;
    }
    if (find(locationPath.begin(), locationPath.end(), ' ') != locationPath.end())
    {
        return false;
    }
    if (locationPath.front() != '/' && locationPath.front() != '.')
    {
        return false;
    }
    if (locations.find(locationPath) != locations.end())
    {
        return false;
    }
    return true;
}

size_t ServerConfig::getClientMaxBodySize() const
{
    return client_max_body_size;
}
