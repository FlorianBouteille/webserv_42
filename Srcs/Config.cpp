/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:37:58 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 17:37:58 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

// ********** Constructors **********

Config::Config() : _maxBodySize(0) , _maxBodySet(false)
{
}

Config::Config(const Config &other)
{
	*this = other;
}

Config &Config::operator=(const Config &other)
{
	if (this != &other)
	{
		_serverNames = other._serverNames;
		_root = other._root;
		_index = other._index;
		_errorPage = other._errorPage;
		_maxBodySize = other._maxBodySize;
		_cgiPath = other._cgiPath;
		_cgiExt = other._cgiExt;
		_locations = other._locations;
		_maxBodySet = other._maxBodySet;
	}
	return *this;
}

// ********** Destructor **********

Config::~Config()
{
}

// ********** Setters **********

void Config::setServerName(std::string value)
{
	if (!_serverNames.empty())
		throw ErrorException("server_name already defined");
	_serverNames = split(value);
	if (_serverNames.empty())
		throw ErrorException("server_name empty");

	for (size_t i = 0; i < _serverNames.size(); ++i)
	{
		std::string s = _serverNames[i];
		if (s.length() > 255)
			throw ErrorException("server_name too long");
		if (s[0] == '-' || s[s.length() - 1] == '-')
			throw ErrorException("invalid server_name");
		if (s.find(':') != std::string::npos)
			throw ErrorException("server_name must not contain port");
		for (size_t j = 0; j < s.length(); ++j)
			if (!isalnum(s[j]) && s[j] != '.' && s[j] != '-')
				throw ErrorException("server_name invalid char");
	}
}

void Config::setRoot(std::string value)
{
	if (!_root.empty())
		throw ErrorException("root already defined");
	_root = value;
	if (value.empty())
		throw ErrorException("root empty");
	if (value[value.size() - 1] != '/')
		throw ErrorException("root must be ended by '/'");
	if (value.find("..") != std::string::npos)
		throw ErrorException("root cannot contain ..");
	for (size_t i = 0; i < value.size(); ++i)
		if (value[i] == '*' || value[i] == '?' ||
			value[i] == '<' || value[i] == '>')
			throw ErrorException("root invalid char");

	struct stat st;
	if (stat(value.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
		throw ErrorException("root must be an existing directory");
}

void Config::setIndex(std::string value)
{
	if (!_index.empty())
		throw ErrorException("index already defined");
	_index = split(value);
	if (_index.empty())
		throw ErrorException("index empty");

	for (size_t i = 0; i < _index.size(); ++i)
	{
		std::string f = _index[i];
		if (f.find('/') != std::string::npos)
			throw ErrorException("index must be a filename");
		if (f.find('.') == std::string::npos)
			throw ErrorException("index needs extension");
		for (size_t j = 0; j < f.size(); ++j)
			if (!isalnum(f[j]) && f[j] != '.' && f[j] != '_' && f[j] != '-')
				throw ErrorException("index invalid char");
	}
}

void Config::setErrorPage(std::string value)
{
	if (!_errorPage.empty())
		throw ErrorException("error_page already defined");
	std::vector<std::string> splitted = split(value);
	if (splitted.size() < 2 || splitted.size() % 2 != 0)
		throw ErrorException("error_page requires code and file");
	for (size_t i = 0; i < splitted.size(); i += 2)
	{
		for (size_t j = 0; j < splitted[i].size(); j++)
		{
			if (!isdigit(splitted[i][j]))
				throw ErrorException("invalid error code in error_page");
		}
		_errorPage[atoi(splitted[i].c_str())] = splitted[i + 1];
	}

	for (std::map<int, std::string>::iterator it = _errorPage.begin(); it != _errorPage.end(); ++it)
	{
		int code = it->first;
		std::string path = it->second;
		if (code < 300 || code > 599)
			throw ErrorException("invalid error_page code");
	}
}

void Config::setMaxBodySize(std::string value)
{
	for (size_t j = 0; j < value.length(); ++j)
		if (!isdigit(value[j]))
			throw ErrorException("max_body_size must be numeric");
	if (_maxBodySet)
		throw ErrorException("client_max_body_size already defined");
	_maxBodySize = atoi(value.c_str());
	_maxBodySet = true;
	if (_maxBodySize == 0)
		throw ErrorException("max_body_size must be > 0");
	if (_maxBodySize > 100 * 1024 * 1024)
		throw ErrorException("max_body_size too large");
}

void Config::setLocation(Location location)
{
	_locations.push_back(location);
}

void Config::setCgiPath(std::string value)
{
	if (!_cgiPath.empty())
		throw ErrorException("cgi_path already defined");
	_cgiPath = value;
	if (value.empty())
		throw ErrorException("cgi_path empty");
}

void Config::setCgiExt(std::string value)
{
	if (!_cgiExt.empty())
		throw ErrorException("cgi_ext already defined");
	_cgiExt = value;
	if (value.empty())
		throw ErrorException("cgi_ext empty");
}

// ********** Getters **********

std::vector<std::string> Config::getNames(void) const
{
	return (_serverNames);
}

std::string Config::getRoot() const
{
	return (_root);
}

std::vector<std::string> Config::getIndex() const
{
	return (_index);
}

const std::vector<Location> &Config::getLocations() const
{
	return (_locations);
}

std::map<int, std::string> Config::getErrorPage() const
{
	return (_errorPage);
}

size_t Config::getClientMaxBodySize() const
{
	return (_maxBodySize);
}

std::string Config::getCgiPath() const
{
	return (_cgiPath);
}

std::string Config::getCgiExt() const
{
	return (_cgiExt);
}

bool Config::hasSizeChanged() const
{
	return (_maxBodySet);	
}

// ********** Methods **********

void Config::printConfig()
{
	std::cout << "        " << BLUE << "----------Print config----------" << RESET << std::endl;
	//****************************************************************************
	std::cout << "        " << BOLD_CYAN << "Server Name : " << RESET;
	for (size_t i = 0; i < _serverNames.size(); ++i)
	{
		std::cout << PINK << _serverNames[i] << RESET << " ";
	}
	std::cout << std::endl;
	//****************************************************************************
	std::cout << "        " << BOLD_CYAN << "Root : " << RESET << PINK << _root << RESET << std::endl;
	//****************************************************************************
	std::cout << "        " << BOLD_CYAN << "Index : " << RESET;
	for (size_t i = 0; i < _index.size(); ++i)
	{
		std::cout << PINK << _index[i] << RESET << " ";
	}
	std::cout << std::endl;
	//****************************************************************************
	std::cout << "        " << BOLD_CYAN << "Error page : " << RESET;
	for (std::map<int, std::string>::iterator it = _errorPage.begin(); it != _errorPage.end(); ++it)
		std::cout << PINK << it->first << " " << it->second << RESET;
	std::cout << std::endl;
	//****************************************************************************
	std::cout << "        " << BOLD_CYAN << "Max body size : " << RESET << PINK << _maxBodySize << RESET << std::endl;
	//****************************************************************************
	std::cout << "        " << BOLD_CYAN << "CGI Path : " << RESET << PINK << _cgiPath << RESET << std::endl;
	std::cout << "        " << BOLD_CYAN << "CGI Ext : " << RESET << PINK << _cgiExt << RESET << std::endl;
	//****************************************************************************
	for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it)
		(*it).printLocation();
}

void Config::printServerNames()
{
	std::cout << BOLD_CYAN << "Server Names : " << RESET;
	for (size_t i = 0; i < _serverNames.size(); ++i)
	{
		std::cout << PINK << _serverNames[i] << RESET << " ";
	}
	std::cout << std::endl;
}
