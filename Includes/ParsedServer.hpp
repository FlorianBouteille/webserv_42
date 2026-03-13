/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsedServer.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 11:49:10 by csolari           #+#    #+#             */
/*   Updated: 2025/12/03 11:49:10 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSEDSERVER_HPP
#define PARSEDSERVER_HPP

#include "WebservIncludes.hpp"

class Config;

class ParsedServer
{
	private:
		ParsedServer();
		std::vector<std::pair<std::string, int> >	_listens;
		Config										_config;
		bool										_errorFound;
		
	public:
		ParsedServer(std::vector<std::string> serVect);
		ParsedServer(const ParsedServer& other);
		ParsedServer& operator=(const ParsedServer& other);
		~ParsedServer();

		bool	fillConfigAttributs(std::string line, std::string value);
		void	fillListens( std::string value);
		bool	fillLocationAttributs(std::string line, std::string value, Location *tmp);

		std::vector<std::pair<std::string, int> > getListens(void);

		Config	getConfig(void);

		void printParsedServer();
};

#endif
