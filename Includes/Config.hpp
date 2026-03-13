/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:37:58 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 17:37:58 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "WebservIncludes.hpp"

class Location;

class Config
{
	private:
		std::vector<std::string>	_serverNames;
		std::string					_root;
		std::vector<std::string>	_index;
		std::map<int, std::string>	_errorPage;
		size_t						_maxBodySize;
		std::string					_cgiPath;
		std::string					_cgiExt;
		std::vector<Location>		_locations;
		bool _maxBodySet;
		
	public:
		Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();
		
		void	setServerName(std::string value);
		void	setRoot(std::string value);
		void	setIndex(std::string value);
		void	setErrorPage(std::string value);
		void	setMaxBodySize(std::string value);
		void	setLocation(Location location);
		void	setCgiPath(std::string value);
		void	setCgiExt(std::string value);

		std::vector<std::string> getNames(void) const;
		std::string getRoot() const;
		std::vector<std::string> getIndex() const;
		const std::vector<Location> &getLocations() const;
		std::map<int, std::string> getErrorPage() const;
		size_t getClientMaxBodySize() const;
		std::string getCgiPath() const;
		std::string getCgiExt() const;
		bool hasSizeChanged() const;

		
		void	printConfig();
		void 	printServerNames();


};

#endif
