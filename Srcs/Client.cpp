/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:18:59 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 17:18:59 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

Client::Client()
{
}

Client::Client(int fd)
{
	_fd = fd;
	_requestComplete = false;
	_readClosed = false;
	_hasPendingWrite = false;
	_lastActivity = time(NULL);
    _beginRequest = 0;
}

Client::~Client()
{
}

Client::Client(const Client &other)
{
	*this = other;
}

Client &Client::operator=(const Client &other)
{
	if (&other != this)
	{
		_fd = other._fd;
		_requestComplete = other._requestComplete;
		_requestBuffer = other._requestBuffer;
		_responses = other._responses;
		_hasPendingWrite = other._hasPendingWrite;
		_readClosed = other._readClosed;
		_responseBuffer = other._responseBuffer;
		_lastActivity = other._lastActivity;
        _beginRequest = other._beginRequest;
    }
    return (*this);
}

// Getters

int Client::getFd() const
{
	return (_fd);
}

std::string Client::getRequestBuffer() const
{
	return (_requestBuffer);
}

std::string &Client::getResponseBuffer()
{
	return (_responseBuffer);
}
bool Client::hasPendingWrite(void) const
{
	return (_hasPendingWrite);
}

bool Client::isReadClosed(void) const
{
	return (_readClosed);
}
// Setters

void Client::setRequestBuffer(std::string &buffer)
{
	_requestBuffer = buffer;
}

void Client::addToResponse(std::string &response)
{
	_responses.push_back(response);
	_hasPendingWrite = true;
}

void Client::setResponse(std::string &response, int index)
{
	_responses[index] = response;
}

void Client::setBeginRequest(void)
{
    _beginRequest = time(NULL);
}
void Client::removeFromResponses(int index)
{
	_responses.erase(_responses.begin() + index);
}

std::vector<std::string> &Client::getResponseVector()
{
	return (_responses);
}

void Client::setResponseBuffer(std::string &buffer)
{
	_responseBuffer = buffer;
}

void Client::setPendingWrite(bool value)
{
	_hasPendingWrite = value;
}
void Client::setReadClosed(bool value)
{
	_readClosed = value;
}

time_t Client::getLastActivity(void) const
{
	return (_lastActivity);
}

time_t Client::getBeginRequest() const
{
    return (_beginRequest);
}

void	Client::updateLastActivity()
{
	_lastActivity = time(NULL);
}

bool Client::isTimedOut(time_t timeout) const
{
	return (time(NULL) - _lastActivity > timeout);
}