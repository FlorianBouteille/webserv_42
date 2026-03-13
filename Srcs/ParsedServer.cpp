/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedServer.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 11:49:10 by csolari           #+#    #+#             */
/*   Updated: 2025/12/03 11:49:10 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

void checkBrackets(std::vector<std::string> serVect)
{
	std::string line;
	std::string first;
	std::string last;
	bool block_server = false;
	bool block_location = false;
	bool server_found = false;
	bool server_closed = false;

	for (std::vector<std::string>::const_iterator it = serVect.begin(); it != serVect.end(); ++it)
	{
		line = trimSpaces(*it);
		first = getFirstWord(line);
		if (first != "" && first[0] == '#')
			;
		else
		{
			last = getLastWord(line);
			if (first == "server" && !server_closed)
			{
				if (server_found)
				{
					std::cout << ORANGE << line << " : " << RESET;
					throw ErrorException("char found outside a server block");
				}
				server_found = true;
			}
			else if (!line.empty() && !server_found)
			{
				std::cout << ORANGE << line << " : " << RESET;
				throw ErrorException("Unexpected content before or after server block");
			}
			else if (server_found && server_closed && !line.empty())
			{
				std::cout << ORANGE << line << " : " << RESET;
				throw ErrorException("Unexpected content before or after server block");
			}
			for (size_t i = 0; i < line.length(); ++i)
			{
				if (line[i] == '{')
				{
					if (last != "{")
					{
						std::cout << ORANGE << line << " : " << RESET;
						throw ErrorException("Syntax error: unexpected content afer '{' on same line");
					}
					if (!block_server)
						block_server = true;
					else if (block_location)
					{
						std::cout << ORANGE << line << " : " << RESET;
						throw ErrorException("Unexpected '{' in a location block");
					}
					else if (first != "location")
					{
						std::cout << ORANGE << line << " : " << RESET;
						throw ErrorException("Unexpected '{' in a server block");
					}
				}
				else if (line[i] == '}')
				{
					if (last != "}")
					{
						std::cout << ORANGE << line << " : " << RESET;
						throw ErrorException("Syntax error: unexpected content afer '}' on same line");
					}
					if (!block_server)
					{
						std::cout << ORANGE << line << " : " << RESET;
						throw ErrorException("Unexpected '}' in a server block");
					}
					else if (!block_location && block_server)
					{
						block_server = false;
						server_closed = true;
					}
					else if (block_location && block_server)
						block_location = false;
				}
				else if (!server_found && (!line.empty() || !first.empty()))
				{
					std::cout << ORANGE << line << " : " << RESET;
					throw ErrorException("Unexpected content before or after server block");
				}
			}
			if (first == "location")
			{
				int flag;
				flag = 0;
				std::vector<std::string> parts = split(line);
				if (parts.size() < 2 || (parts.size() == 2 && last == "{") || (parts.size() == 3 && last != "{") || parts.size() > 3)
				{
					std::cout << ORANGE << line << " : " << RESET;
					throw ErrorException("Error: syntax location: 'location path {'");
				}
				for (std::vector<std::string>::iterator itt = parts.begin(); itt != parts.end(); ++itt)
				{
					if (itt->find("{") != std::string::npos)
						flag = 1;
				}
				if (flag)
					block_location = true;
				else
				{
					std::vector<std::string>::const_iterator next_it = it + 1;
					if (next_it != serVect.end())
					{
						std::string next = trimSpaces(*next_it);
						std::string next_first = getFirstWord(next);
						std::string next_last = getLastWord(next);
						if (next_first != "{" || next_last != "{")
						{
							std::cout << ORANGE << line << " : " << RESET;
							throw ErrorException("Expected nothing after '{'");
						}
						else
							block_location = true;
					}
					else
					{
						std::cout << ORANGE << line << " : " << RESET;
						throw ErrorException("Expected '{' after location declaration");
					}
				}
			}
		}
	}
	if (!server_found)
		throw ErrorException("Missing server block");
	if (block_server)
		throw ErrorException("Server block not close");
	if (block_location)
		throw ErrorException("location block not close");
}

void checkSyntax(const std::string &value)
{
	static const std::string keys[] = {
		"listen",
		"server_name",
		"root",
		"index",
		"error_page",
		"autoindex",
		"client_max_body_size",
		"allow_methods",
		"cgi_path",
		"cgi_ext",
	};

	size_t keyCount = sizeof(keys) / sizeof(keys[0]);

	std::stringstream ss(value);
	std::string token;

	while (ss >> token)
	{
		if (!token.empty() && token[token.size() - 1] == ';')
			token = token.substr(0, token.size() - 1);

		for (size_t j = 0; j < keyCount; j++)
		{
			if (token == keys[j])
				throw ErrorException("Multiple directives on same line");
		}
	}
}

std::string checkEndLine(std::string value)
{
	if (!value.empty() && value[value.size() - 1] == ';')
	{
		value = value.substr(0, value.size() - 1);
		value = trimSpaces(value);
		return value;
	}
	if (value.size() >= 2 && value[value.size() - 1] == '}' && value[value.size() - 2] == ';')
	{
		value = value.substr(0, value.size() - 2);
		value = trimSpaces(value);
		return value;
	}
	throw ErrorException("Syntax error: directive must end with ';'");
	return value;
}

std::string findAttribute(std::string line, std::string toFind)
{
	std::string cpy;

	cpy = getFirstWord(line);
	std::string value;
	if (cpy == toFind)
	{
		line = trimSpaces(line);
		value = line.substr(cpy.size());
		value = trimSpaces(value);
		value = checkEndLine(value);
		if (value.empty())
			throw ErrorException("Webserv: [emerg] invalid number of arguments in value directive");
		checkSyntax(value);
		if (value == "location")
			throw ErrorException("'location' is a key word and can't be a value");
		return (value);
	}
	return "";
}

int checkFormatAdress(std::string str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!(isdigit(str[i]) || str[i] == '.'))
			return (0);
	}
	size_t i = 0;
	int count_dots = 0;
	while (size_t pos = str.find(".", i))
	{
		if (pos == i)
			return (0);
		if (pos != std::string::npos)
			count_dots++;
		int num = atoi(str.substr(i, pos).c_str());
		if (num > 255 || num < 0)
			return (0);
		if (pos == std::string::npos)
			break;
		i = pos + 1;
	}
	if (count_dots != 3)
		return (0);
	return (1);
}
int checkFormatPort(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!isdigit(str[i]))
			return (0);
	}
	int num = atoi(str.c_str());
	if (num < 1 || num > 65535)
		return (0);
	return (1);
}

void ParsedServer::fillListens(std::string listenPair)
{
	if (listenPair.empty())
		throw ErrorException("listen directive requires a value");
	size_t pos = listenPair.find(":");
	std::string adress;
	int port = 0;
	if (pos != std::string::npos)
	{
		adress = listenPair.substr(0, pos);
		if (!checkFormatAdress(adress))
			throw ErrorException("Wrong ip format adress");
		std::string portStr = listenPair.substr(pos + 1, listenPair.size());
		if (!checkFormatPort(portStr))
			throw ErrorException("Wrong port format");
		port = atoi(portStr.c_str());
	}
	else
	{
		if (checkFormatAdress(listenPair))
		{
			adress = listenPair;
			port = 8000;
		}
		else if (checkFormatPort(listenPair))
		{
			adress = "0.0.0.0";
			port = atoi(listenPair.c_str());
		}
		else
			throw ErrorException("Wrong listen pair format");
	}
	std::pair<std::string, int> p = make_pair(adress, port);
	_listens.push_back(p);
}

bool ParsedServer::fillConfigAttributs(std::string line, std::string value)
{
	value = findAttribute(line, "listen");
	if (value != "")
		return (fillListens(value), true);

	value = findAttribute(line, "server_name");
	if (value != "")
		return (_config.setServerName(value), true);

	value = findAttribute(line, "index");
	if (value != "")
		return (_config.setIndex(value), true);

	value = findAttribute(line, "root");
	if (value != "")
		return (_config.setRoot(value), true);

	value = findAttribute(line, "error_page");
	if (value != "")
		return (_config.setErrorPage(value), true);

	value = findAttribute(line, "client_max_body_size");
	if (value != "")
		return (_config.setMaxBodySize(value), true);

	value = findAttribute(line, "cgi_path");
	if (value != "")
		return (_config.setCgiPath(value), true);

	value = findAttribute(line, "cgi_ext");
	if (value != "")
		return (_config.setCgiExt(value), true);

	return false;
}

bool ParsedServer::fillLocationAttributs(std::string line, std::string value, Location *tmp)
{
	value = findAttribute(line, "autoindex");
	if (value != "")
		return (tmp->setAutoIndex(value), true);

	value = findAttribute(line, "index");
	if (value != "")
		return (tmp->setIndex(value), true);

	value = findAttribute(line, "root");
	if (value != "")
		return (tmp->setRoot(value), true);

	value = findAttribute(line, "allow_methods");
	if (value != "")
		return (tmp->setAllowMethods(value), true);

	value = findAttribute(line, "cgi_path");
	if (value != "")
		return (tmp->setCgiPath(value), true);

	value = findAttribute(line, "cgi_ext");
	if (value != "")
		return (tmp->setCgiExt(value), true);

	value = findAttribute(line, "error_page");
	if (value != "")
		return (tmp->setErrorPage(value), true);

	value = findAttribute(line, "client_max_body_size");
	if (value != "")
		return (tmp->setClientMaxBodySize(value), true);

	value = findAttribute(line, "return");
	if (value != "")
	{
		std::istringstream iss(value);
		int code;
		std::string url;
		if (iss >> code)
		{
			tmp->setRedirectCode(code);
			if (iss >> url)
				tmp->setRedirectUrl(url);
			return true;
		}
	}

	return false;
}

std::string getLocationName(std::string line)
{
	std::vector<std::string> splitedLine = split(line);
	std::string name = splitedLine[1];
	return (name);
}

ParsedServer::ParsedServer(std::vector<std::string> serVect)
{
	try
	{
		_errorFound = false;
		checkBrackets(serVect);

		bool hasContent = false;
		for (std::vector<std::string>::const_iterator it = serVect.begin(); it != serVect.end(); ++it)
		{
			std::string trimmed = trimSpaces(*it);
			std::string first = getFirstWord(trimmed);
			if (!trimmed.empty() && trimmed != "{" && trimmed != "}" &&
				trimmed != "server" && first != "server" &&
				(first.empty() || first[0] != '#'))
			{
				hasContent = true;
				break;
			}
		}
		if (!hasContent)
			throw ErrorException("Empty server block: server must contain at least one directive");

		std::string value;
		std::string line;
		for (std::vector<std::string>::const_iterator it = serVect.begin(); it != serVect.end(); ++it)
		{
			line = *it;
			if (trimSpaces(line) == "server")
				continue;
			if (fillConfigAttributs(line, value) == true)
				;
			else if (getFirstWord(line) == "location")
			{
				Location tmp = Location(getLocationName(line));
				++it;
				while (it != serVect.end())
				{
					line = *it;
					int flag = 0;
					if (line.find("}") != std::string::npos)
						flag = 1;
					if (fillLocationAttributs(line, value, &tmp))
					{
						if (flag)
							break;
					}
					else
					{
						if (flag)
							break;
						line = trimSpaces(line);
						std::string firstWord = getFirstWord(line);
						std::string lastWord = getLastWord(line);

						if (line != "" && line != "{" && line != "}" && firstWord[0] != '#' && lastWord != "{")
						{
							std::cout << ORANGE << line << ": " << RESET;
							throw ErrorException("Invalid line in location block");
						}
					}
					++it;
				}
				_config.setLocation(Location(tmp));
				if (it != serVect.end())
					--it;
			}
			else
			{
				line = trimSpaces(line);
				std::string firstWord = getFirstWord(line);

				if (line != "" && line != "{" && line != "}" && firstWord[0] != '#')
				{
					std::cout << ORANGE << line << ": " << RESET;
					throw ErrorException("Invalid line in conf_file");
					return;
				}
			}
		}
		if (!_config.hasSizeChanged())
			_config.setMaxBodySize("104857600");
	}
	catch (std::exception &e)
	{
		_errorFound = true;
		throw;
	}
}

std::vector<std::pair<std::string, int> > ParsedServer::getListens(void)
{
	return (_listens);
}

Config ParsedServer::getConfig(void)
{
	return (_config);
}

ParsedServer::ParsedServer()
{
}

ParsedServer::ParsedServer(const ParsedServer &other)
{
	*this = other;
}

ParsedServer &ParsedServer::operator=(const ParsedServer &other)
{
	if (this != &other)
	{
		_listens = other._listens;
		_config = other._config;
	}
	return *this;
}

ParsedServer::~ParsedServer()
{
}

void ParsedServer::printParsedServer()
{

	std::cout << "    " << BLUE << "----------Print Parsed Server----------" << RESET << std::endl;
	std::cout << "    " << BOLD_CYAN << "Listen : " << RESET;
	std::cout << PINK << "|";
	for (size_t i = 0; i < _listens.size(); ++i)
	{
		std::cout << PINK << _listens[i].first << " : "
				  << _listens[i].second << "|" << RESET;
	}
	std::cout << std::endl;
	_config.printConfig();
}
