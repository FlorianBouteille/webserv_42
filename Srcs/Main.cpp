/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 14:11:49 by csolari           #+#    #+#             */
/*   Updated: 2025/12/19 15:46:03 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << ITALIC_PINK << "Usage : ./webserv configfile" << std::endl;
		return (1);
	}

	Webserv webserv;
	signal(SIGPIPE, SIG_IGN);

	try
	{
		parsing(&webserv, argv[1]);
		webserv.printWebserv();
	}
	catch (std::exception &e)
	{
		std::cout << ITALIC_RED << "Error: " << e.what() << RESET << std::endl;
		return (0);
	}

	try
	{
		webserv.run();
	}
	catch (std::exception &e)
	{
		std::cout << BOLD_RED << e.what() << RESET << std::endl;
	}
	return (0);
}