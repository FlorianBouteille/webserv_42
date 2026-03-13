/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:11:14 by csolari           #+#    #+#             */
/*   Updated: 2025/12/18 13:54:41 by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

std::vector<std::string> split(const std::string &request)
{
	std::vector<std::string> words;
	std::stringstream ss(request);
	std::string word;
	int limit = countWords(request);
	int i = 0;

	while (ss >> word && i++ < limit)
		words.push_back(word);

	return (words);
}

std::string trimSpaces(const std::string &str)
{
	size_t start = 0;
	size_t end = str.length();

	while (start < str.length() && isspace(str[start]))
		start++;
	if (start == str.length())
		return "";
	end = str.length() - 1;
	while (end > start && isspace(str[end]))
		end--;
	return str.substr(start, end - start + 1);
}

int countWords(const std::string &str)
{
	std::stringstream ss(str);
	std::string word;
	int count = 0;

	while (ss >> word)
		count++;

	return count;
}

std::string getFirstWord(const std::string &line)
{
	std::stringstream ss(line);
	std::string first;
	ss >> first;
	return first;
}

std::string getLastWord(const std::string &line)
{
	std::stringstream ss(line);
	std::string word, last;

	while (ss >> word)
		last = word;

	return last;
}

std::string getExtension(std::string &filePath)
{
	size_t dot = filePath.find_last_of('.');
	if (dot == std::string::npos)
		return "";
	return (filePath.substr(dot + 1));
}
std::string getMimeType(const std::string &ext)
{
	if (ext == "html" || ext == "htm")
		return "text/html";
	if (ext == "css")
		return "text/css";
	if (ext == "js")
		return "application/javascript";
	if (ext == "png")
		return "image/png";
	if (ext == "jpg" || ext == "jpeg")
		return "image/jpeg";
	if (ext == "gif")
		return "image/gif";
	if (ext == "svg")
		return "image/svg+xml";
	if (ext == "ico")
		return "image/x-icon";
	if (ext == "txt")
		return "text/plain";
	return "application/octet-stream";
}

void setSocketFlags(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags != -1)
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	int fd_flags = fcntl(fd, F_GETFD, 0);
	if (fd_flags != -1)
		fcntl(fd, F_SETFD, fd_flags | FD_CLOEXEC);
}
std::string toLower(std::string base)
{
	std::string lowerStr;
	if (base.length() == 0)
		return (base);
	lowerStr.resize(base.length());
	for (size_t i = 0; i < base.length(); i++)
	{
		lowerStr[i] = tolower(base[i]);
	}
	return (lowerStr);
}

void signalHandler(int sig)
{
	if (sig == SIGINT)
		throw ErrorException(" detected");
}
std::string build408Response(const std::string &httpVersion)
{
	const std::string body =
		"<!doctype html><html><head><meta charset=\"utf-8\"><title>408 Request Timeout</title>"
		"</head><body><h1>408 Request Timeout</h1><p>Request timed out.</p></body></html>";

	std::ostringstream oss;
	oss << body.size();

	std::string resp;
	resp += (httpVersion.empty() ? "HTTP/1.1" : httpVersion);
	resp += " 408 Request Timeout\r\n";
	resp += "Content-Type: text/html\r\n";
	resp += "Content-Length: " + oss.str() + "\r\n";
	resp += "Connection: close\r\n";
	resp += "\r\n";
	resp += body;
	return resp;
}