/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 14:12:36 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 14:12:36 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "WebservIncludes.hpp"

class	Socket;
class	ParsedServer;
class Cgi;

class Webserv
{
	private:
		std::vector<Socket>			_sockets;
		std::vector<ParsedServer>	_parsed;

		void handleAcceptFds(fd_set &read_fds);
		void handleSocketClients(Socket &sock, fd_set &read_fds, fd_set &write_fds, int &clientFdClosed);
		void handleAllCgis(fd_set &read_fds, fd_set &write_fds, int clientFdClosed);
		std::vector<Cgi>			_cgis;

	public:
		Webserv();
		Webserv(const Webserv& other);
		Webserv& operator=(const Webserv& other);
		~Webserv();
		
		void	run(void);
		void	setUpSockets(void);

		void	addParsedServer(const ParsedServer& server);
		void 	printWebserv();
		int 	findSocket(std::pair<std::string, int> listen);
		void	setAllFds(fd_set *read_fds, fd_set *write_fds, int *max_fd);
	void 	handleSelect(fd_set read_fds, fd_set write_fds);
	Client 	&findClientByFd(int clientFd);
	bool	queueErrorResponseForClient(int clientFd, Request &req, int statusCode);
	std::vector<ParsedServer>& getParsed();};

#endif
