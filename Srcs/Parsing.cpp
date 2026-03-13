/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 11:55:23 by csolari           #+#    #+#             */
/*   Updated: 2025/12/19 15:05:24 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

int skipHeader(std::ifstream &file, std::string &line)
{
	while (getline(file, line))
	{
		line = trimSpaces(line);
		std::string firstWord = getFirstWord(line);

		if (firstWord == "server")
			return 1;
		else if (firstWord != "")
			return 0;
	}
	return 0;
}

int parsing(Webserv *webserv, char *configFile)
{
	std::ifstream file(configFile);
	if (!file)
		throw ErrorException("Error: Configuration file can not be opened");
	std::string line;
	if (!skipHeader(file, line))
		throw ErrorException("Invalid configuration file");
	int server = 1;
	while (server == 1)
	{
		server = 0;
		std::vector<std::string> serverBloc;
		serverBloc.push_back(line);
		while (getline(file, line))
		{
			if (getFirstWord(line) == "server")
			{
				server = 1;
				break;
			}
			serverBloc.push_back(line);
		}
		ParsedServer parsedServerBloc(serverBloc);
		webserv->addParsedServer(parsedServerBloc);
	}
	return (1);
}
