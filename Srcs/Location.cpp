/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:21:06 by csolari           #+#    #+#             */
/*   Updated: 2025/12/19 14:37:13 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*   Updated: 2025/12/15 14:51:19 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

// ********** Constructors **********

Location::Location() : _name(""), _autoIndex(false), _autoIndexSet(false), _clientMaxBodySize(0), _maxBodySet(false), _redirectCode(0), _redirectUrl("")
{
}

Location::Location(std::string name) : _name(name), _autoIndex(false), _autoIndexSet(false), _clientMaxBodySize(0), _maxBodySet(false), _redirectCode(0), _redirectUrl("")
{
}

Location::Location(const Location &other)
{
	*this = other;
}

Location &Location::operator=(const Location &other)
{
	if (this != &other)
	{
		_name = other._name;
		_autoIndex = other._autoIndex;
		_autoIndexSet = other._autoIndexSet;
		_index = other._index;
		_errorPage = other._errorPage;
		_root = other._root;
		_cgiPath = other._cgiPath;
		_cgiExt = other._cgiExt;
		_allowMethods = other._allowMethods;
		_clientMaxBodySize = other._clientMaxBodySize;
		_maxBodySet = other._maxBodySet;
		_redirectCode = other._redirectCode;
		_redirectUrl = other._redirectUrl;
	}
	return *this;
}

// ********** Destructor **********

Location::~Location()
{
}

// ********** Setters **********

void Location::setName(std::string name)
{
	if (!_name.empty())
		throw ErrorException("server_name already defined");
	if (name.empty())
		throw ErrorException("Location: location name empty");
	if (name[0] != '/')
		throw ErrorException("Location: location name must start with /");
	if (name.find("..") != std::string::npos)
		throw ErrorException("Location: location name contains ..");
	for (size_t i = 0; i < name.size(); ++i)
		if (name[i] == '*' || name[i] == '?' || name[i] == ':')
			throw ErrorException("Location: invalid character in location name");
	_name = name;
}

void Location::setAutoIndex(std::string autoIndex)
{
	if (_autoIndexSet)
		throw ErrorException("Location: autoindex already defined");
	if (autoIndex == "on")
		_autoIndex = true;
	else if (autoIndex == "off")
		_autoIndex = false;
	else
		throw ErrorException("Location: autoindex must be 'on' or 'off'");
	_autoIndexSet = true;
}

void Location::setIndex(std::string index)
{
	if (!_index.empty())
		throw ErrorException("Location: index already defined");
	std::vector<std::string> v = split(index);
	if (v.empty())
		throw ErrorException("Location: index empty");

	for (size_t i = 0; i < v.size(); ++i)
	{
		std::string f = v[i];
		if (f.find('/') != std::string::npos)
			throw ErrorException("Location: index must be a filename");
		if (f.find('.') == std::string::npos)
			throw ErrorException("Location: index must have extension");
		for (size_t j = 0; j < f.size(); ++j)
			if (!isalnum(f[j]) && f[j] != '.' && f[j] != '_' && f[j] != '-')
				throw ErrorException("Location: invalid char in index");
	}
	_index = v;
}

void Location::setErrorPage(std::string value)
{
	if (!_errorPage.empty())
		throw ErrorException("Location: error_page already defined");
	std::vector<std::string> splitted = split(value);
	if (splitted.size() < 2 || splitted.size() % 2 != 0)
		throw ErrorException("Location: error_page requires code and file");
	for (size_t i = 0; i < splitted.size(); i += 2)
	{
		for (size_t j = 0; j < splitted[i].size(); j++)
		{
			if (!isdigit(splitted[i][j]))
				throw ErrorException("Location: invalid error code in error_page");
		}
		_errorPage[atoi(splitted[i].c_str())] = splitted[i + 1];
	}

	for (std::map<int, std::string>::iterator it = _errorPage.begin(); it != _errorPage.end(); ++it)
	{
		int code = it->first;
		std::string path = it->second;
		if (code < 300 || code > 599)
			throw ErrorException("Location: invalid error_page code");
	}
}

void Location::setRoot(std::string root)
{
	if (!_root.empty())
		throw ErrorException("Location: root already defined");
	if (root.empty())
		throw ErrorException("Location: location root empty");
	if (root[root.size() - 1] != '/')
		throw ErrorException("Location: root must be ended by '/'");
	if (root.find("..") != std::string::npos)
		throw ErrorException("Location: root cannot contain ..");
	for (size_t i = 0; i < root.size(); ++i)
		if (root[i] == '*' || root[i] == '?' ||
			root[i] == '<' || root[i] == '>')
			throw ErrorException("Location: root invalid char");

	_root = root;
}

void Location::setCgiPath(std::string value)
{
	if (!_cgiPath.empty())
		throw ErrorException("Location: cgi_path already defined");
	if (value.empty() || value[0] != '/')
		throw ErrorException("cgi_path must be absolute");
	_cgiPath = value;
}

void Location::setCgiExt(std::string value)
{
	if (!_cgiExt.empty())
		throw ErrorException("Location: cgi_ext already defined");
	if (value.empty() || value[0] != '.')
		throw ErrorException("cgi extension must start with '.'");
	std::vector<std::string> splitted = split(value);
	for (size_t j = 0; j < splitted.size(); ++j)
	{
		for (size_t i = 1; i < splitted[j].size(); ++i)
		{
			if (!isalnum(splitted[j][i]))
				throw ErrorException("invalid cgi extension");
		}
	}

	_cgiExt = value;
}

void Location::setClientMaxBodySize(std::string value)
{
	for (size_t j = 0; j < value.length(); ++j)
		if (!isdigit(value[j]))
			throw ErrorException("max_body_size must be numeric");

	if (_maxBodySet)
		throw ErrorException("client_max_body_size already defined");
	_clientMaxBodySize = atoi(value.c_str());
	_maxBodySet = true;
	if (_clientMaxBodySize == 0)
		throw ErrorException("max_body_size must be > 0");
	if (_clientMaxBodySize > 100 * 1024 * 1024)
		throw ErrorException("max_body_size too large");
}
void Location::setAllowMethods(std::string allowMethods)
{
	if (!_allowMethods.empty())
		throw ErrorException("Location: allow_methods already defined");
	std::vector<std::string> v = split(allowMethods);
	if (v.empty())
		throw ErrorException("Location: allow_methods empty");
	for (size_t i = 0; i < v.size(); ++i)
		if (v[i] != "GET" && v[i] != "POST" && v[i] != "DELETE")
			throw ErrorException("Location: invalid method in allow_methods");

	_allowMethods = v;
}

// ********** getters *********

const std::string Location::getName() const
{
	return _name;
}

const std::string Location::getRoot() const
{
	return _root;
}

const std::vector<std::string> Location::getAllowMethods() const
{
	return _allowMethods;
}

const std::vector<std::string> Location::getIndex() const
{
	return _index;
}

const std::string Location::getCgiPath() const
{
	return _cgiPath;
}

const std::string Location::getCgiExt() const
{
	return _cgiExt;
}

const std::map<int, std::string> Location::getErrorPage() const
{
	return _errorPage;
}

bool Location::hasSizeChanged() const
{
	return (_maxBodySet);	
}

size_t Location::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

bool Location::getAutoIndex() const { return _autoIndex; }

// ********** method **********

void Location::printLocation()
{
	std::cout << "            " << BLUE << "----------Print Location----------" << RESET << std::endl;

	std::cout << "            " << BOLD_CYAN << "Location name : " << RESET << PINK << _name << RESET << std::endl;

	std::cout << "            " << BOLD_CYAN << "autoindex : " << RESET << PINK << _autoIndex << RESET << std::endl;

	std::cout << "            " << BOLD_CYAN << "Index : " << RESET;
	for (std::vector<std::string>::iterator it = _index.begin(); it != _index.end(); ++it)
		std::cout << PINK << *it << " " << RESET;
	std::cout << std::endl;

	std::cout << "            " << BOLD_CYAN << "Error page : " << RESET;
	for (std::map<int, std::string>::iterator it = _errorPage.begin(); it != _errorPage.end(); ++it)
		std::cout << PINK << it->first << " " << it->second << RESET;
	std::cout << std::endl;

	std::cout << "            " << BOLD_CYAN << "root : " << RESET << PINK << _root << RESET << std::endl;

	std::cout << "            " << BOLD_CYAN << "Cgi Path : " << RESET << PINK << _cgiPath << RESET << std::endl;

	std::cout << "            " << BOLD_CYAN << "Cgi Ext : " << RESET << PINK << _cgiExt << RESET << std::endl;
	
	std::cout << "            " << BOLD_CYAN << "Max Body size : " << RESET << PINK << _clientMaxBodySize << RESET << std::endl;

	std::cout << "            " << BOLD_CYAN << "allow Methods : " << RESET;
	for (std::vector<std::string>::iterator it = _allowMethods.begin(); it != _allowMethods.end(); ++it)
		std::cout << PINK << *it << " " << RESET;
	std::cout << std::endl;
	
	if (_redirectCode != 0)
	{
		std::cout << "            " << BOLD_CYAN << "Redirect : " << RESET << PINK << _redirectCode << " -> " << _redirectUrl << RESET << std::endl;
	}
}

// ********** Redirect Getters/Setters **********

int Location::getRedirectCode() const
{
	return _redirectCode;
}

const std::string Location::getRedirectUrl() const
{
	return _redirectUrl;
}

void Location::setRedirectCode(int code)
{
	_redirectCode = code;
}

void Location::setRedirectUrl(std::string url)
{
	_redirectUrl = url;
}