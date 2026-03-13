#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "WebservIncludes.hpp"

class Response
{
public:
	Response();
	~Response();
	Response(const Response &other);
	Response &operator=(const Response &other);

	// setters
	void setStatus(int code, std::string &message);
	void setHttpVersion(std::string &reqVersion);
	void setHeader(std::string &key, std::string &value);
	void setBody(std::string &data);
	void setFileType(std::string &type);
	void setCgiState(bool value);
	void setMethod(const std::string &method);
	void setCookie(const std::string &name, const std::string &value, int maxAge = -1, const std::string &path = "/");
	void addRawCookie(const std::string &cookieLine);

	// getters
	std::string getFileType(void) const;
	std::string getBody(void) const;
	bool getCgiState() const;
	std::string getMethod() const;

	int getStatusCode(void) const;
	std::string getStatusMessage(void) const;
	std::string getHttpVersion(void) const;

	void printResponse();
	std::string buildResponseString() const;

private:
	int _statusCode;
	size_t _contentLength;
	std::string _httpVersion;
	std::string _statusMessage;
	std::string _fileType;
	std::map<std::string, std::string> _headers;
	std::string _body;
	std::string _method;
	bool _cgiPending;
	std::vector<std::string> _cookies;
};

#endif // RESPONSE_HPP
