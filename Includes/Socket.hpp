/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:59:15 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 16:59:15 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "WebservIncludes.hpp"

class	Client;
class	Config;
class Cgi;

class Socket
{
	private:
		int						_fd;
		int						_port;
		std::string 			_stringAddr;
		sockaddr_in 			_addr;
		std::vector<Config>		_configs;
		std::vector<Client>		_clients;
		Socket();
		
	public:
		Socket(std::string &adress, int &port);
		Socket(const Socket& other);
		Socket& operator=(const Socket& other);
		~Socket();

		bool	bindSocket(std::pair<std::string, int>);
		bool 	listenSocket(void);
		void	addConfig(Config config);
		void 	acceptClient(void);
		void 	printSocket(void);
		int 	receiveReqAndBuildResp(std::vector<Client>::iterator it, std::vector<Cgi> &cgis);
		bool 	sendResponses(std::vector<Client>::iterator it);
		void 	setResponse(Client &client, const std::string &hostHeader, std::vector<Cgi> &cgis);
		const 	Config& selectConfig(const std::string &host) const;
		Response getResponseObject(std::string &request, const Config &config, Client &client, std::vector<Cgi> &cgis);

	
		std::string getStringAddr(void);
		int 		getPort(void);
		int			getFd();
		std::vector<Client> &getClients(void);
};

#endif
