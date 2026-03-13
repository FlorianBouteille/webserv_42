/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorException.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:09:32 by csolari           #+#    #+#             */
/*   Updated: 2025/12/03 11:46:01 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROREXCEPTION_HPP
#define ERROREXCEPTION_HPP

#include "WebservIncludes.hpp"

class ErrorException : public std::exception
{
public:
	ErrorException(const char *message) : _message(message) {}
	virtual const char *what() const throw()
	{
		return (_message.c_str());
	}
	virtual ~ErrorException() throw() {}

private:
	std::string _message;
};
#endif