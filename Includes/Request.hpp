#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "WebservIncludes.hpp"

class Request
{
public:
	Request();
	Request(std::string request);
	~Request();
	Request(const Request &other);
	Request &operator=(const Request &other);
	void printRequest();
	std::string getMethod();
	std::string getPath();
	std::string getHttpVersion() const;
	std::map<std::string, std::string> getHeaders();
	size_t getContentLength();
	std::string getBody();
	int getErrorCode() const;
	std::map<std::string, std::string> getCookies();
	bool getChunked() const;

	void setChunked(bool value);

private:
	std::string method;
	std::string path;
	std::string httpVersion;
	std::map<std::string, std::string> headers;
	std::string body;
	size_t 		contentLength;
	std::map<std::string, std::string> cookies;
	int errorCode;
	bool _chunked;
};

std::string trimSpaces(std::string &str);

#endif // REQUEST_HPP
