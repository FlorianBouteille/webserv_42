/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:21:11 by csolari           #+#    #+#             */
/*   Updated: 2025/12/19 14:37:20 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "WebservIncludes.hpp"

class Location
{
public:
	// ********** constructor **********

	Location();
	Location(std::string name);
	~Location();
	Location(const Location &other);
	Location &operator=(const Location &other);

	// ********** setters **********
	void setName(std::string name);
	void setAutoIndex(std::string autoIndex);
	void setIndex(std::string index);
	void setErrorPage(std::string value);
	void setRoot(std::string root);
	void setCgiPath(std::string value);
	void setCgiExt(std::string value);
	void setAllowMethods(std::string allowMethods);
	void setClientMaxBodySize(std::string value);
	void setRedirectCode(int code);
	void setRedirectUrl(std::string url);

	// ********** method **********
	void printLocation();

	// ********** Getters **********
	const std::string getName() const;
	bool getAutoIndex() const;
	const std::vector<std::string> getIndex() const;
	const std::string getRoot() const;
	const std::vector<std::string> getAllowMethods() const;
	const std::string getCgiPath() const;
	const std::string getCgiExt() const;
	const std::map<int, std::string> getErrorPage() const;
	size_t getClientMaxBodySize() const;
	bool hasSizeChanged() const;
	int getRedirectCode() const;
	const std::string getRedirectUrl() const;

private:
	std::string _name;
	bool _autoIndex;
	bool _autoIndexSet;
	std::vector<std::string> _index;
	std::map<int, std::string>	_errorPage;
	std::string _root;
	std::string _cgiPath;
	std::string _cgiExt;
	std::vector<std::string> _allowMethods;
	size_t _clientMaxBodySize;
	bool _maxBodySet;
	int _redirectCode;
	std::string _redirectUrl;
};

#endif // LOCATION_HPP
