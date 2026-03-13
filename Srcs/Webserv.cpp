/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 14:12:36 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 14:12:36 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

static void trimLeftInPlace(std::string &s)
{
	size_t i = 0;
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
		++i;
	if (i > 0)
		s.erase(0, i);
}

static std::string toLowerCopy(std::string s)
{
	for (size_t i = 0; i < s.size(); ++i)
		s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
	return s;
}

static Response buildResponseFromCgiRaw(const std::string &raw, const Request &req)
{
	Response resp;
	std::string version = req.getHttpVersion();
	if (version.empty())
		version = "HTTP/1.1";
	resp.setHttpVersion(version);

	int statusCode = 200;
	std::string statusMessage = getStatusMessage(200);
	std::string body = raw;
	std::string contentType;
	if (raw.empty())
	{
		setError(resp, 502);
		contentType = "text/html";
		body.clear();
	}
	size_t sep = raw.find("\r\n\r\n");
	size_t headerLen = 4;
	if (sep == std::string::npos)
	{
		sep = raw.find("\n\n");
		headerLen = 2;
	}
	if (!raw.empty() && sep != std::string::npos)
	{
		std::string headerBlock = raw.substr(0, sep);
		body = raw.substr(sep + headerLen);

		std::istringstream iss(headerBlock);
		std::string line;
		while (std::getline(iss, line))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (line.empty())
				continue;
			size_t colon = line.find(':');
			if (colon == std::string::npos)
				continue;
			std::string key = line.substr(0, colon);
			std::string value = line.substr(colon + 1);
			trimLeftInPlace(value);

			std::string lowerKey = toLowerCopy(key);
			if (lowerKey == "status")
			{
				std::istringstream vs(value);
				vs >> statusCode;
				std::getline(vs, statusMessage);
				trimLeftInPlace(statusMessage);
				if (statusMessage.empty())
					statusMessage = getStatusMessage(statusCode);
				continue;
			}
			if (lowerKey == "content-type")
			{
				contentType = value;
				continue;
			}
			// On calcule content-length nous meme comme des grands comme ca on s'emmerde pas
			if (lowerKey == "content-length" || lowerKey == "connection" || lowerKey == "transfer-encoding")
				continue;
			if (lowerKey == "set-cookie")
			{
				resp.addRawCookie(value);
				continue;
			}
			resp.setHeader(key, value);
		}
	}

	if (statusCode < 100 || statusCode > 599)
	{
		setError(resp, 502);
		body.clear();
		statusCode = 502;
		statusMessage = getStatusMessage(502);
	}

	if (resp.getStatusCode() == 0)
	{
		resp.setStatus(statusCode, statusMessage);
		if (statusCode >= 400)
			body.clear();
	}

	if (contentType.empty())
		contentType = "text/plain";
	resp.setFileType(contentType);
	resp.setBody(body);
	return resp;
}



int Webserv::findSocket(std::pair<std::string, int> listen)
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		std::cout << RED;
		std::cout << "str = " << _sockets[i].getStringAddr() << std::endl;
		std::cout << "port sock = " << _sockets[i].getPort() << std::endl;
		std::cout << PINK << "str = " << listen.first << std::endl;
		std::cout << "port sock = " << listen.second << std::endl;
		std::cout << RESET << std::endl;

		if ((_sockets[i].getStringAddr() == listen.first) && (_sockets[i].getPort() == listen.second))
		{
			std::cout << YELLOW << "match found on index " << i << std::endl;
			return (i);
		}
	}
	return (-1);
}

void Webserv::setUpSockets()
{
	int countSocks = 0;
	for (size_t i = 0; i < _parsed.size(); i++)
	{
		std::vector<std::pair<std::string, int> > listens = _parsed[i].getListens();
		for (size_t j = 0; j < listens.size(); j++)
		{
			int index = findSocket(listens[j]);
			if (index != -1)
			{
				_sockets[index].addConfig(_parsed[i].getConfig());
				continue;
			}
			int port = listens[j].second;
			std::string adress = listens[j].first;
			Socket sock(adress, port);
			sock.addConfig(_parsed[i].getConfig());
			if (!(sock.bindSocket(listens[j])) || !(sock.listenSocket()))
			{
				std::cerr << "Failed to setup socket on port " << port << std::endl;
				if (sock.getFd() >= 0)
					close(sock.getFd());
				continue;
			}
			_sockets.push_back(sock);
			std::cout << BOLD_CYAN << "Socket number " << countSocks << " added config '" << "' to port " << port << RESET << std::endl;
			countSocks++;
		}
	}
}

void Webserv::setAllFds(fd_set *read_fds, fd_set *write_fds, int *max_fd)
{
	FD_ZERO(read_fds);
	FD_ZERO(write_fds);

	for (std::vector<Socket>::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
	{
		int fd = it->getFd();
		FD_SET(fd, read_fds);
		if (fd > *max_fd)
			*max_fd = fd;

		std::vector<Client> &clients = it->getClients();
		for (size_t i = 0; i < clients.size(); i++)
		{
			int client_fd = clients[i].getFd();
			int flag = 0;
			for (size_t j = 0; j < _cgis.size(); ++j)
			{
				if (_cgis[j].getClientFd() == client_fd)
				{
					flag = 1;
					break;
				}
			}
			if (client_fd >= 0 && !flag)
			{
				if (clients[i].isReadClosed() == false)
					FD_SET(client_fd, read_fds);
				if (clients[i].hasPendingWrite())
				{
					FD_SET(client_fd, write_fds);
				}
				if (client_fd > *max_fd)
					*max_fd = client_fd;
			}
		}
		for (size_t i = 0; i < _cgis.size(); i++)
		{
			Cgi &cgi = _cgis[i];
			int fdOut = cgi.getFdOut();
			int fdIn = cgi.getFdIn();
			if (fdOut >= 0)
			{
				FD_SET(fdOut, read_fds);
				if (fdOut > *max_fd)
					*max_fd = fdOut;
			}
			if (fdIn >= 0)
			{
				FD_SET(fdIn, write_fds);
				if (fdIn > *max_fd)
					*max_fd = fdIn;
			}
		}
	}
}

Client &Webserv::findClientByFd(int clientFd)
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		std::vector<Client> &clients = _sockets[i].getClients();
		for (size_t j = 0; j < clients.size(); j++)
		{
			if (clients[j].getFd() == clientFd)
				return (clients[j]);
		}
	}
	throw ErrorException("No clients found");
}

bool Webserv::queueErrorResponseForClient(int clientFd, Request &req, int statusCode)
{
	try
	{
		Client &client = findClientByFd(clientFd);
		Response resp;
		std::string version = req.getHttpVersion();
		if (version.empty())
			version = "HTTP/1.1";
		resp.setHttpVersion(version);
		setError(resp, statusCode);

		std::string host;
		std::map<std::string, std::string> headers = req.getHeaders();
		std::map<std::string, std::string>::const_iterator hit = headers.find("host");
		if (hit != headers.end())
			host = hit->second;

		Socket *sock = NULL;
		for (size_t si = 0; si < _sockets.size(); ++si)
		{
			std::vector<Client> &cs = _sockets[si].getClients();
			for (size_t ci = 0; ci < cs.size(); ++ci)
			{
				if (cs[ci].getFd() == clientFd)
				{
					sock = &_sockets[si];
					break;
				}
			}
			if (sock)
				break;
		}
		if (sock)
		{
			const Config &conf = sock->selectConfig(host);
			const Location &loc = getLocation(req.getPath(), conf);
			buildErrorPage(resp, conf, loc, req);
		}

		std::string response = resp.buildResponseString();
		client.addToResponse(response);
		client.setReadClosed(true);
		return true;
	}
	catch (ErrorException &e)
	{
		std::cerr << "ERROR: " << e.what() << " (looking for fd " << clientFd << ")" << std::endl;
	}
	return false;
}

void Webserv::handleAcceptFds(fd_set &read_fds)
{
	for (std::vector<Socket>::iterator sockIt = _sockets.begin(); sockIt != _sockets.end(); ++sockIt)
	{
		if (FD_ISSET(sockIt->getFd(), &read_fds))
		{
			sockIt->acceptClient();
		}
	}
}

void Webserv::handleSocketClients(Socket &sock, fd_set &read_fds, fd_set &write_fds, int &clientFdClosed)
{
	std::vector<Client> &clients = sock.getClients();
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end();)
	{
		bool hasCgiPending = false;
		for (size_t k = 0; k < _cgis.size(); k++)
		{
			// On verifie que le client n'a pas de CGI en cours, si il en a une, on skip ce client. 
			if (_cgis[k].getClientFd() == it->getFd())
			{
				hasCgiPending = true;
				break;
			}
		}
		if (hasCgiPending)
		{
			++it;
			continue;
		}
		bool erased = false;
		if ((*it).isTimedOut(TIMEOUT - 1))
		{
			std::cout << YELLOW << "Client " << (*it).getFd() << " timed out after " << TIMEOUT << " s" << RESET << std::endl;
			if (!it->getRequestBuffer().empty())
			{
				std::string resp = build408Response("HTTP/1.1");
				it->addToResponse(resp);
				it->setReadClosed(true);
				it->setPendingWrite(true);
				std::string empty;
				it->setRequestBuffer(empty);
				++it;
				continue;
			}
			else if (it->isTimedOut(TIMEOUT))
			{
				clientFdClosed = it->getFd();
				close((*it).getFd());
				clients.erase(it);
				continue;
			}
		}
		if (FD_ISSET(it->getFd(), &read_fds))
		{
			int closed = sock.receiveReqAndBuildResp(it, _cgis);
			if (closed == -1)
			{
				it = clients.erase(it);
				erased = true;
			}
			if (closed)
			{
				// si ya une cgi en cours, on ferme pas, sinon, on ferme.  
				bool hasCgiPending = false;
				for (size_t k = 0; k < _cgis.size(); k++)
				{
					if (_cgis[k].getClientFd() == it->getFd())
					{
						hasCgiPending = true;
						break;
					}
				}
				if (!hasCgiPending)
				{
					it = clients.erase(it);
					erased = true;
				}
			}
		}
		else if (FD_ISSET((*it).getFd(), &write_fds) && !erased)
		{
			// std::cout << BOLD_BLUE << "Writting response for client " << it->getFd() << RESET << std::endl;
			bool sendErased = sock.sendResponses(it);
			if (sendErased)
			{
				it = clients.erase(it);
				erased = true;
			}
		}
		if (!erased)
			++it;
		continue;
	}
}

void Webserv::handleAllCgis(fd_set &read_fds, fd_set &write_fds, int clientFdClosed)
{
	for (size_t i = 0; i < _cgis.size(); i++)
	{
		Cgi &cgi = _cgis[i];
		int fdOut = cgi.getFdOut();
		int fdIn = cgi.getFdIn();
			int clientFd = cgi.getClientFd();
		// TimeOut CGI : PAS LE MEME QUE L AUTRE TIMEOUT, On kill au bout de 10 secondes (modifiable) et on rebvoie une 504.
		time_t now = time(NULL);
		if (cgi.getPid() > 0 && (now - cgi.getStartTime()) > CGI_TIMEOUT)
		{
			kill(cgi.getPid(), SIGKILL);
			waitpid(cgi.getPid(), NULL, WNOHANG);
			if (fdOut >= 0)
				close(fdOut);
			if (fdIn >= 0)
				close(fdIn);
			Request req = cgi.getRequest();
			queueErrorResponseForClient(clientFd, req, 504);
			_cgis.erase(_cgis.begin() + i);
			i--;
			continue;
		}
		bool didIoThisTick = false;
		if (clientFd == clientFdClosed)
		{
			std::cout << BOLD_RED << "Client closed: " << clientFd << " Pid killed: " << cgi.getPid() << " Fdout closed: " << fdOut << RESET << std::endl;
			kill(cgi.getPid(), SIGKILL);
			_cgis.erase(_cgis.begin() + i);
			i--;
			if (fdOut >= 0)
				close(fdOut);
			if (fdIn >= 0)
				close(fdIn);
			continue;
		}
		if (fdIn >= 0 && FD_ISSET(fdIn, &write_fds))
		{
			const std::string &body = cgi.getBody();
			std::cout << BOLD_RED<< "\n\n\n\n" << body.size() << "\n\n\n\n" << RESET << std::endl;
			size_t sent = cgi.getBytesSent();
			size_t remaining = body.size() - sent;
			size_t chunk = (remaining > BUFFER_SIZE ? BUFFER_SIZE : remaining);
			ssize_t wn = write(fdIn, body.data() + sent, chunk);
			didIoThisTick = true;
			if (wn > 0)
			{
				cgi.setByteSent(sent + static_cast<size_t>(wn));
				if (cgi.getBytesSent() >= body.size())
				{
					close(fdIn);
					cgi.setFdIn(-1);
				}
			}
			else if (wn == 0)
			{
				// Rien d'ecrit mais on laisse ouvert, on retentera au prochain tick (pour respecter la consigne)
			}
			else
			{
				close(fdIn);
				cgi.setFdIn(-1);
			}
			continue;
		}
		if (!didIoThisTick && fdOut >= 0 && FD_ISSET(fdOut, &read_fds))
		{
			char buffer[BUFFER_SIZE];
			ssize_t n = read(fdOut, buffer, sizeof(buffer));
			std::cout << BOLD_ORANGE << "Read du execve: " << buffer << RESET <<std::endl;
			if (n > 0)
			{
				std::string buf(buffer, n);
				cgi.appendResponse(buf, n);
			}
			else if (n < 0)
			{
				Request req = cgi.getRequest();
				queueErrorResponseForClient(clientFd, req, 502);
				close(fdOut);
				if (fdIn >= 0)
					close(fdIn);
				_cgis.erase(_cgis.begin() + i);
				i--;
				continue;
			}
			else if (n == 0)
			{
				std::cout << RED << "CGI DONE !" << RESET << std::endl;
				try
				{
					Client &client = findClientByFd(clientFd);
					Request req = cgi.getRequest();
					Response resp = buildResponseFromCgiRaw(cgi.getResponse(), req);
					if (resp.getStatusCode() >= 400)
					{
						queueErrorResponseForClient(clientFd, req, resp.getStatusCode());
						close(fdOut);
						if (fdIn >= 0)
							close(fdIn);
						_cgis.erase(_cgis.begin() + i);
						i--;
						continue;
					}
					std::string response = resp.buildResponseString();
					client.addToResponse(response);
					close(fdOut);
					if (fdIn >= 0)
						close(fdIn);
					_cgis.erase(_cgis.begin() + i);
					i--;
					continue;
				}
				catch (ErrorException &e)
				{
					std::cerr << "ERROR: " << e.what() << " (looking for fd " << clientFd << ")" << std::endl;
				}
			}
			std::cout << YELLOW << "Im out !" << RESET << std::endl;
		}
	}
}

void Webserv::handleSelect(fd_set read_fds, fd_set write_fds)
{
	int clientFdClosed = 0;
	handleAcceptFds(read_fds);
	for (std::vector<Socket>::iterator sockIt = _sockets.begin(); sockIt != _sockets.end(); ++sockIt)
	{
		handleSocketClients(*sockIt, read_fds, write_fds, clientFdClosed);
	}
	handleAllCgis(read_fds, write_fds, clientFdClosed);
}

void Webserv::run()
{
	setUpSockets();
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		_sockets[i].printSocket();
	}
	if (_sockets.empty())
	{
		std::cerr << "No sockets to run!" << std::endl;
		return;
	}
	fd_set read_fds, write_fds;
	while (true)
	{
		signal(SIGINT, signalHandler);
		int max_fd = 0;
		setAllFds(&read_fds, &write_fds, &max_fd);

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int ret = select(max_fd + 1, &read_fds, &write_fds, NULL, &tv);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue; 
			strerror(errno);
			std::cerr << "select() failed" << std::endl;
			break;
		}
		handleSelect(read_fds, write_fds);
	}
}

// ********** Constructors **********

Webserv::Webserv()
{
}

Webserv::Webserv(const Webserv &other)
{
	*this = other;
}

Webserv &Webserv::operator=(const Webserv &other)
{
	if (this != &other)
	{
		_sockets = other._sockets;
		_parsed = other._parsed;
	}
	return *this;
}

Webserv::~Webserv()
{
	for (size_t i = 0; i < _sockets.size(); ++i)
	{
		for (std ::vector<Client>::iterator it = _sockets[i].getClients().begin(); it != _sockets[i].getClients().end(); ++it)
		{
			std::cout << BOLD_RED << "Fd client close : " << it->getFd() << RESET << std::endl;
			close((*it).getFd());
		}
		std::cout << BOLD_RED << "Fd socket close : " << _sockets[i].getFd() << RESET << std::endl;
		close(_sockets[i].getFd());
	}

	for (size_t i = 0; i < _cgis.size(); ++i)
	{
		if (_cgis[i].getFdIn() >= 0)
			close(_cgis[i].getFdIn());
		if (_cgis[i].getFdOut() >= 0)
			close(_cgis[i].getFdOut());
		if (_cgis[i].getPid()> 0)
			kill(_cgis[i].getPid(), SIGKILL);
	}
}


void Webserv::addParsedServer(const ParsedServer &server)
{
	_parsed.push_back(server);
}

void Webserv::printWebserv()
{
	std::cout << BLUE << "----------Print WebServ----------" << RESET << std::endl;
	for (size_t i = 0; i < _parsed.size(); ++i)
	{
		//_parsed[i].printParsedServer();
		std::cout << std::endl;
	}
}


std::vector<ParsedServer>& Webserv::getParsed() {return _parsed;}