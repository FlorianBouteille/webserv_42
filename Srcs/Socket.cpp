/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:59:15 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 16:59:15 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

Socket::Socket() : _fd(-1), _port(0)
{
}

Socket::Socket(std::string &adress, int &port) : _fd(-1), _port(port), _stringAddr(adress)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0)
		std::cerr << "Socket creation failed for port " << port << std::endl;
	else
		setSocketFlags(_fd);
}

Socket::Socket(const Socket &other)
{
	*this = other;
}

Socket &Socket::operator=(const Socket &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_port = other._port;
		_stringAddr = other._stringAddr;
		_addr = other._addr;
		_configs = other._configs;
		_clients = other._clients;
	}
	return *this;
}

Socket::~Socket()
{
}

void Socket::addConfig(Config config)

{
	_configs.push_back(config);
}

in_addr_t buildAdress(std::string str)
{
	in_addr_t adress = 0;

	int begin = 0;
	int vals[4];
	int i = 0;
	size_t pos = 0;
	while ((pos = str.find('.', begin)) != std::string::npos)
	{
		vals[i++] = atoi(str.substr(begin, pos - begin).c_str());
		begin = pos + 1;
	}
	vals[3] = atoi(str.substr(begin).c_str());
	adress = htonl((vals[0] << 24) | (vals[1] << 16) | (vals[2] << 8) | vals[3]);
	std::cout << PINK << adress << std::endl;
	return (adress);
}

bool Socket::bindSocket(std::pair<std::string, int> listenPair)
{
	if (_fd < 0)
		return false;

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(listenPair.second);
	_addr.sin_addr.s_addr = buildAdress(listenPair.first);

	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
	{
		std::cerr << "Bind failed for port " << _port << std::endl;
		return false;
	}

	std::cout << ORANGE << "Socket " << _fd << " bound to port " << BOLD_YELLOW << _port << RESET << std::endl;
	return true;
}

bool Socket::listenSocket()
{
	if (_fd < 0)
		return false;

	if (listen(_fd, SOMAXCONN) < 0)
	{
		std::cerr << "Listen failed for port " << _port << std::endl;
		return false;
	}

	std::cout << GREEN << "Socket " << _fd << " listening on port " << _port << RESET << std::endl;
	return true;
}

void Socket::acceptClient(void)
{
	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int client_fd = accept(_fd, (struct sockaddr *)&clientAddr, &addrLen);

	if (client_fd < 0)
	{
		std::cerr << "accept() failed" << std::endl;
		return;
	}
	setSocketFlags(client_fd);
	_clients.push_back(Client(client_fd));
	_clients.back().updateLastActivity();
	std::cout << GREEN << "New client accepted on fd " << client_fd << RESET << std::endl;
}

// SI on a un chunked transfer, on verifie les chunks, la fin et on renvoie la taille du body
static size_t parseChunkedBody(const std::string &buffer, size_t body_start)
{
	size_t pos = body_start;
	size_t total_body_len = 0;

	while (pos < buffer.size())
	{
		size_t line_end = buffer.find("\r\n", pos);
		if (line_end == std::string::npos)
			return 0;

		std::string chunk_size_str = buffer.substr(pos, line_end - pos);
		size_t semicolon = chunk_size_str.find(';');
		if (semicolon != std::string::npos)
			chunk_size_str = chunk_size_str.substr(0, semicolon);

		char *endptr;
		size_t chunk_size = std::strtoul(chunk_size_str.c_str(), &endptr, 16);
		if (endptr == chunk_size_str.c_str())
			return 0;

		pos = line_end + 2;

		if (chunk_size == 0)
		{
			if (pos + 2 <= buffer.size() && buffer.substr(pos, 2) == "\r\n")
				return total_body_len;
			return 0;
		}
		if (pos + chunk_size + 2 > buffer.size())
			return 0;

		total_body_len += chunk_size;
		pos += chunk_size + 2;
	}

	return 0;
}

std::vector<std::string> buildRequestsFromBuffer(std::string &buffer)
{
	std::vector<std::string> requestVector;
	size_t pos = 0;
	while (true)
	{
		size_t header_end = buffer.find("\r\n\r\n", pos);
		if (header_end == std::string::npos)
			break;

		size_t headers_len = header_end + 4;
		std::string headers = buffer.substr(pos, headers_len - pos);

		std::string lower_headers = headers;
		for (size_t i = 0; i < lower_headers.size(); ++i)
			lower_headers[i] = std::tolower(lower_headers[i]);
		
		// Pn regarde si on a un chunked-encoding : 
		bool is_chunked = false;
		size_t te_pos = lower_headers.find("transfer-encoding:");
		if (te_pos != std::string::npos)
		{
			te_pos += std::string("transfer-encoding:").size();
			while (te_pos < lower_headers.size() && (lower_headers[te_pos] == ' ' || lower_headers[te_pos] == '\t'))
				++te_pos;
			if (lower_headers.substr(te_pos, 7) == "chunked")
				is_chunked = true;
		}
		size_t content_length = 0;
		size_t total_len = headers_len;

		if (is_chunked)
		{
			size_t chunked_body_len = parseChunkedBody(buffer, pos + headers_len);
			if (chunked_body_len == 0 && buffer.find("0\r\n\r\n", pos + headers_len) == std::string::npos)
			{
				break;
			}
			size_t body_start = pos + headers_len;
			size_t body_pos = body_start;
			while (body_pos < buffer.size())
			{
				size_t line_end = buffer.find("\r\n", body_pos);
				if (line_end == std::string::npos)
					break;
				std::string size_line = buffer.substr(body_pos, line_end - body_pos);
				size_t semicolon = size_line.find(';');
				if (semicolon != std::string::npos)
					size_line = size_line.substr(0, semicolon);
				char *endptr;
				size_t chunk_size = std::strtoul(size_line.c_str(), &endptr, 16);
				body_pos = line_end + 2;
				if (chunk_size == 0)
				{
					body_pos += 2;
					break;
				}
				body_pos += chunk_size + 2;
			}
			total_len = body_pos - pos;
		}
		else
		{
			size_t cl_pos = lower_headers.find("content-length:");
			if (cl_pos != std::string::npos)
			{
				cl_pos += std::string("content-length:").size();
				while (cl_pos < lower_headers.size() && (lower_headers[cl_pos] == ' ' || lower_headers[cl_pos] == '\t'))
					++cl_pos;
				size_t endpos = cl_pos;
				while (endpos < lower_headers.size() && isdigit(lower_headers[endpos]))
					++endpos;
				if (endpos > cl_pos)
				{
					std::string num = lower_headers.substr(cl_pos, endpos - cl_pos);
					content_length = static_cast<size_t>(std::strtoul(num.c_str(), NULL, 10));
				}
			}
			total_len = headers_len + content_length;
		}

		if (buffer.size() < pos + total_len)
			break;
		std::string request = buffer.substr(pos, total_len);

		if (!request.empty())
		{
			requestVector.push_back(request);
		}
		pos += total_len;
	}

	if (pos > 0)
		buffer.erase(0, pos);

	return requestVector;
}
const Config &Socket::selectConfig(const std::string &host) const
{
	if (_configs.empty())
		throw std::runtime_error("Socket has no configs");
	std::string hostname = host;
	size_t colon = hostname.find(':');
	if (colon != std::string::npos)
		hostname = hostname.substr(0, colon);
	for (size_t i = 0; i < _configs.size(); i++)
	{
		std::vector<std::string> names = _configs[i].getNames();
		for (size_t w = 0; w < names.size(); w++)
		{
			if (hostname == names[w])
				return (_configs[i]);
		}
	}
	return _configs[0];
}

Response Socket::getResponseObject(std::string &request, const Config &config, Client &client, std::vector<Cgi> &cgis)
{
	Request requestObj(request);
	Response responseObj = ResponseBuilder::build(requestObj, config, client, cgis);
	return responseObj;
}

void Socket::setResponse(Client &client, const std::string &hostHeader, std::vector<Cgi> &cgis)
{
	std::vector<std::string> requests;
	std::string reqBuf = client.getRequestBuffer();
	requests = buildRequestsFromBuffer(reqBuf);
	client.setRequestBuffer(reqBuf);

	for (size_t i = 0; i < requests.size(); i++)
	{
		bool requestTimedOut = false;
		if (client.getBeginRequest() == 0)
			client.setBeginRequest();
		if (time(NULL) - client.getBeginRequest() >= TIMEOUT - 3)
			requestTimedOut = true;
		const Config &config = selectConfig(hostHeader);
		Response responseObj = getResponseObject(requests[i], config, client, cgis);
		if (responseObj.getCgiState())
		{
			std::cout << PINK << "CGI launched for client " << client.getFd() << " — deferring response until CGI completes" << RESET << std::endl;
		}
		else
		{
			std::string response = responseObj.buildResponseString();
			client.setPendingWrite(true);
			client.addToResponse(response);
		}
	}
}

int Socket::receiveReqAndBuildResp(std::vector<Client>::iterator it, std::vector<Cgi> &cgis)
{
	char buffer[BUFFER_SIZE];
	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;
	int rec = recv((*it).getFd(), &buffer, BUFFER_SIZE, 0);
	if (rec == 0)
	{
		if ((*it).hasPendingWrite())
		{
			it->setReadClosed(1);
			return 0;
		}
		else
		{
			close((*it).getFd());
			return 1;
		}
	}
	else if (rec < 0)
	{
		close((*it).getFd());
		return -1;
	}
	std::string tempBuffer = (*it).getRequestBuffer();
	tempBuffer.append(buffer, rec);
	(*it).setRequestBuffer(tempBuffer);

	(*it).updateLastActivity();

	size_t headerEndPos = tempBuffer.find("\r\n\r\n");
	if (headerEndPos != std::string::npos)
	{
		std::string hostHeader = "";
		size_t hostPos = tempBuffer.find("Host:");
		if (hostPos != std::string::npos)
		{
			size_t lineEnd = tempBuffer.find("\r\n", hostPos);
			if (lineEnd != std::string::npos)
			{
				hostHeader = tempBuffer.substr(hostPos + 5, lineEnd - hostPos - 5);
				size_t start = hostHeader.find_first_not_of(" \t");
				if (start != std::string::npos)
					hostHeader = hostHeader.substr(start);
			}
		}
		setResponse(*it, hostHeader, cgis);
	}
	return false;
}

bool Socket::sendResponses(std::vector<Client>::iterator it)
{
	(*it).updateLastActivity();
	if (!(*it).getResponseBuffer().empty() || !(*it).getResponseVector().empty())
	{
		std::string &current = (*it).getResponseBuffer();

		if (current.empty())
		{
			if ((*it).getResponseVector().empty())
				return false;

			current.swap((*it).getResponseVector()[0]);
			(*it).removeFromResponses(0);
		}
		ssize_t sent = send((*it).getFd(), current.c_str(), current.size(), 0);
		if (sent > 0)
		{
			(*it).updateLastActivity();
			if (static_cast<size_t>(sent) >= current.size())
			{
				current.clear();
				if ((*it).getResponseVector().empty())
				{
					it->setPendingWrite(false);
					if (it->isReadClosed())
					{
						close(it->getFd());
						return true;
					}
				}
				else
					it->setPendingWrite(true);
			}
			else
			{
				current.erase(0, sent);
				it->setPendingWrite(true);
			}
		}
		else if (sent == 0)
		{
			close((*it).getFd());
			return true;
		}
		else if (sent < 0)
		{
			close((*it).getFd());
			return true;
		}
	}
	else
		it->setPendingWrite(false);

	return false;
}

//// GETTERS

std::string Socket::getStringAddr(void)
{
	return (_stringAddr);
}
int Socket::getPort(void)
{
	return (_port);
}

int Socket::getFd()
{
	return (_fd);
}
std::vector<Client> &Socket::getClients(void)
{
	return (_clients);
}

//// DEBUG

void Socket::printSocket()
{
	std::cout << BOLD_ORANGE << "Socket : \n ================= \n";
	std::cout << ORANGE << "Fd = " << YELLOW << _fd << std::endl;
	std::cout << ORANGE << "Port = " << YELLOW << _port << std::endl;
	std::cout << ORANGE << "Adress = " << YELLOW << _stringAddr << std::endl;
	std::cout << YELLOW << _configs.size() << ORANGE << " configs found : " << std::endl;
	for (size_t i = 0; i < _configs.size(); i++)
	{
		_configs[i].printServerNames();
	}
	std::cout << YELLOW << _clients.size() << ORANGE << " clients currently on this socket." << std::endl;
}