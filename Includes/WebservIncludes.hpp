/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservIncludes.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 14:16:09 by csolari           #+#    #+#             */
/*   Updated: 2025/12/19 15:27:08 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVINCLUDES_HPP
#define WEBSERVINCLUDES_HPP

// =====================================================
// 🌈 GLOBAL PROJECT HEADER
// =====================================================

// === Standard Library Includes ===
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <map>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>

// === Project Includes ===
#include "Client.hpp"
#include "Colors.hpp"
#include "Config.hpp"
#include "ErrorException.hpp"
#include "Location.hpp"
#include "ParsedServer.hpp"
#include "Request.hpp"
#include "Cgi.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "Socket.hpp"
#include "Webserv.hpp"

#define BUFFER_SIZE 2048
#define MAX_URI_LEN 1024
#define TIMEOUT 60
#define CGI_TIMEOUT 10

// === Utility ===

std::vector<std::string> split(const std::string &request);
std::string trimSpaces(const std::string &str);
int countWords(const std::string &str);
std::string getFirstWord(const std::string &line);
std::string getLastWord(const std::string &line);
std::string getExtension(std::string &filePath);
std::string getMimeType(const std::string &ext);
void setSocketFlags(int fd);
void signalHandler(int sig);
std::string getStatusMessage(int code);
void setError(Response &resp, int code);
std::string toLower(std::string base);
std::string build408Response(const std::string &httpVersion);
void buildErrorPage(Response &resp, const Config conf, Location loc, Request req);

// === Parsing.cpp ===

int parsing(Webserv *webserv, char *configFile);

#endif // WEBSERVINCLUDES_HPP
