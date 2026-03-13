/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 12:29:38 by mhanarte          #+#    #+#             */
/*   Updated: 2025/11/20 15:22:49:38fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

std::string trimSpaces(std::string &str)
{
	size_t start = 0;
	size_t end = str.length();

	while (start < str.length() && isspace(str[start]))
		start++;
	if (start == str.length())
		return "";
	end = str.length() - 1;
	while (end > start && isspace(str[end]))
		end--;
	return str.substr(start, end - start + 1);
}

Request::Request() : contentLength(0), errorCode(0), _chunked(false)
{
}

Request::Request(std::string request) : contentLength(0), errorCode(0), _chunked(false)
{
	std::vector<std::string> words;
	if (request.empty())
	{
		errorCode = 400;
		return ;
	}
	words = split(request);
	if (words.size() < 3)
	{
		errorCode = 400;
		return ;
	}

	method = words[0];
	path = words[1];
	httpVersion = words[2];
	std::stringstream input(request);
	std::string currentString;
	if (!std::getline(input, currentString))
	{
		errorCode = 400;
		return ;
	}
	while (std::getline(input, currentString) && currentString != "\r")
	{
		size_t pos = currentString.find(':');
		if (pos == std::string::npos)
		{
			errorCode = 400;
			return ;
		}
		std::string key = toLower(currentString.substr(0, pos));
		std::string value = toLower(currentString.substr(pos + 1));
		value = trimSpaces(value);
		headers[key] = value;

		if (key == "transfer-encoding" && value.find("chunked") != std::string::npos)
		{
			_chunked = true;
		}

		if (key == "cookie")
		{
			std::string cookieStr = value;
			size_t start = 0;
			while (start < cookieStr.length())
			{
				size_t eqPos = cookieStr.find('=', start);
				if (eqPos == std::string::npos)
					break;
				size_t semiPos = cookieStr.find(';', eqPos);
				if (semiPos == std::string::npos)
					semiPos = cookieStr.length();

				std::string cookieName = cookieStr.substr(start, eqPos - start);
				std::string cookieValue = cookieStr.substr(eqPos + 1, semiPos - eqPos - 1);
				cookieName = trimSpaces(cookieName);
				cookieValue = trimSpaces(cookieValue);
				cookies[cookieName] = cookieValue;

				start = semiPos + 1;
				while (start < cookieStr.length() && cookieStr[start] == ' ')
					start++;
			}
		}
	}
	if (headers["host"].empty())
	{
		errorCode = 400;
		return ;
	}
	if (method == "POST")
	{
		size_t content_length = atoi(headers["content-length"].c_str());
		contentLength = content_length;
		body.resize(contentLength);
		input.read(&body[0], contentLength);
	}
}

void Request::printRequest()
{
	std::cout << BLUE << "REQUEST : \n____________\n"
			  << RESET << std::endl;
	std::cout << LIGHT_BLUE << "Method       : " << RESET << this->method << std::endl;
	std::cout << LIGHT_BLUE << "Path         : " << RESET << this->path << std::endl;
	std::cout << LIGHT_BLUE << "HTTP version : " << RESET << this->httpVersion << std::endl;
	std::cout << LIGHT_BLUE << "Headers\n______" << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::cout << LIGHT_BLUE << it->first << " : " << RESET << it->second << std::endl;
	}
	if (!cookies.empty())
	{
		std::cout << LIGHT_BLUE << "Cookies\n______" << RESET << std::endl;
		for (std::map<std::string, std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it)
		{
			std::cout << LIGHT_BLUE << it->first << " = " << RESET << it->second << std::endl;
		}
	}
	if (method == "POST")
	{
		std::cout << LIGHT_BLUE << "Body : " << std::endl;
		std::cout << body << std::endl;
	}
}
Request::~Request()
{
}

Request::Request(const Request &other)
	: method(other.method),
	  path(other.path),
	  httpVersion(other.httpVersion),
	  headers(other.headers),
	  body(other.body),
	  contentLength(other.contentLength),
	  cookies(other.cookies),
	  errorCode(other.errorCode),
	  _chunked(other._chunked)
{
}

Request &Request::operator=(const Request &other)
{
	if (&other != this)
	{
		method = other.method;
		path = other.path;
		httpVersion = other.httpVersion;
		headers = other.headers;
		body = other.body;
		contentLength = other.contentLength;
		cookies = other.cookies;
		errorCode = other.errorCode;
		_chunked = other._chunked;
	}
	return (*this);
}

// Getters

std::string Request::getMethod()
{
	return (method);
}
std::string Request::getPath()
{
	return (path);
}
std::string Request::getHttpVersion() const
{
	return (httpVersion);
}
std::map<std::string, std::string> Request::getHeaders()
{
	return (headers);
}
std::string Request::getBody()
{
	return (body);
}

size_t Request::getContentLength()
{
	return (contentLength);
}

std::map<std::string, std::string> Request::getCookies()
{
	return (cookies);
}
int Request::getErrorCode() const
{
	return (errorCode);
}

bool Request::getChunked() const
{
	return (_chunked);
}

// Setters

void Request::setChunked(bool value)
{
	_chunked = value;
}