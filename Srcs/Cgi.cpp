#include "Cgi.hpp"

Cgi::Cgi()
{
}
Cgi::Cgi(const Cgi &other)
{
	*this = other;
}
Cgi &Cgi::operator=(const Cgi &other)
{
	if (this != &other)
	{
		_pid = other._pid;
		_fdIn = other._fdIn;
		_fdOut = other._fdOut;
		_clientFd = other._clientFd;
		_body = other._body;
		_bytesSent = other._bytesSent;
		_response = other._response;
		_request = other._request;
		_startTime = other._startTime;
		_maxBodySize = other._maxBodySize;
	}
	return (*this);
}

Cgi::~Cgi()
{
}
Cgi::Cgi(int clientFd, Request req)
{
	_pid = -1;
	_fdIn = -1;
	_fdOut = -1;
	_clientFd = clientFd;
	_body = "";
	_bytesSent = 0;
	_response = "";
	_request = req;
	_startTime = time(NULL);
}
void Cgi::appendResponse(std::string buffer, ssize_t n)
{
	_response.append(buffer.substr(0, n));
}

/// Getters //////

int Cgi::getPid(void) const
{
	return (_pid);
}

int Cgi::getFdIn(void) const
{
	return (_fdIn);
}

int Cgi::getFdOut(void) const
{
	return (_fdOut);
}

int Cgi::getClientFd(void) const
{
	return (_clientFd);
}

std::string Cgi::getBody(void) const
{
	return (_body);
}

size_t Cgi::getBytesSent(void) const
{
	return (_bytesSent);
}

std::string Cgi::getResponse(void) const
{
	return (_response);
}

Request Cgi::getRequest(void) const
{
	return (_request);
}

time_t Cgi::getStartTime(void) const
{
	return (_startTime);
}

size_t Cgi::getMaxBodySize(void) const
{
	return (_maxBodySize);
}

/// Setters /////

void Cgi::setPid(int value)
{
	_pid = value;
}
void Cgi::setFdIn(int value)
{
	_fdIn = value;
}
void Cgi::setFdOut(int value)
{
	_fdOut = value;
}

void Cgi::setBody(std::string value)
{
	_body = value;
}
void Cgi::setByteSent(size_t value)
{
	_bytesSent = value;
}
void Cgi::setResponse(std::string value)
{
	_response = value;
}
void Cgi::setStartTime(time_t value)
{
	_startTime = value;
}

void Cgi::setMaxBodySize(size_t value)
{
	_maxBodySize = value;
}