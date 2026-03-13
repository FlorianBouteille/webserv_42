/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 12:29:57 by mhanarte          #+#    #+#             */
/*   Updated: 2025/12/19 15:47:58 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

Response::Response() : _statusCode(0), _contentLength(0), _method("GET"), _cgiPending(false)
{
}
Response::~Response()
{
}

Response::Response(const Response &other)
{
	_statusCode = other._statusCode;
	_contentLength = other._contentLength;
	_statusMessage = other._statusMessage;
	_headers = other._headers;
	_body = other._body;
}

Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		_statusCode = other._statusCode;
		_contentLength = other._contentLength;
		_statusMessage = other._statusMessage;
		_headers = other._headers;
		_body = other._body;
	}
	return (*this);
}

// Setters

void Response::setStatus(int code, std::string &message)
{
	_statusCode = code;
	_statusMessage = message;
}
void Response::setHeader(std::string &key, std::string &value)
{
	_headers[key] = value;
}
void Response::setBody(std::string &data)
{
	_body = data;
}

void Response::setHttpVersion(std::string &reqVersion)
{
	_httpVersion = reqVersion;
}
void Response::setFileType(std::string &type)
{
	_fileType = type;
}

void Response::setCgiState(bool value)
{
	_cgiPending = value;
}

void Response::setMethod(const std::string &method)
{
	_method = method;
}

void Response::setCookie(const std::string &name, const std::string &value, int maxAge, const std::string &path)
{
	std::string cookie = name + "=" + value;
	if (maxAge >= 0)
	{
		std::stringstream ss;
		ss << maxAge;
		cookie += "; Max-Age=" + ss.str();
	}
	cookie += "; Path=" + path;
	_cookies.push_back(cookie);
}

void Response::addRawCookie(const std::string &cookieLine)
{
	_cookies.push_back(cookieLine);
}

// Getters

std::string Response::getFileType(void) const
{
	return (_fileType);
}

std::string Response::getBody(void) const
{
	return (_body);
}

int Response::getStatusCode(void) const
{
	return (_statusCode);
}

std::string Response::getStatusMessage(void) const
{
	return (_statusMessage);
}

std::string Response::getHttpVersion(void) const
{
	return (_httpVersion);
}

bool Response::getCgiState() const
{
	return (_cgiPending);
}

std::string Response::getMethod() const
{
	return (_method);
}

std::string Response::buildResponseString() const
{
	std::string resp = "";
	resp += _httpVersion;
	resp += " ";
	std::stringstream os;
	os << _statusCode;
	resp += os.str();
	resp += " ";
	resp += _statusMessage;
	resp += "\r\n";
	resp += "Content-Type: ";
	resp += _fileType;
	resp += "\r\n";
	resp += "Content-Length: ";
	std::stringstream os1;
	os1 << _body.size();
	resp += os1.str();
	resp += "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		resp += it->first;
		resp += ": ";
		resp += it->second;
		resp += "\r\n";
	}
	for (size_t i = 0; i < _cookies.size(); i++)
	{
		resp += "Set-Cookie: ";
		resp += _cookies[i];
		resp += "\r\n";
	}
	resp += "\r\n";

	if (_method != "HEAD")
		resp += _body;

	return (resp);
}

// Debug
void Response::printResponse()
{
	std::cout << LIGHT_BLUE << "___RESPONSE OBJECT : __" << RESET << std::endl;
	std::cout << BLUE << "status : " << RESET << _statusCode << " : " << _statusMessage << std::endl;
	std::cout << BLUE << "file type : " << RESET << _fileType << std::endl;
	// std::cout << BLUE << "body : " << RESET << _body << std::endl;
}