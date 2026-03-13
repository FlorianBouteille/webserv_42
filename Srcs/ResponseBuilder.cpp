/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbouteil <fbouteil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 10:59:52 by csolari           #+#    #+#             */
/*   Updated: 2025/12/10 15:48:37by fbouteil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservIncludes.hpp"

int matchCalc(std::string path, std::string locName)
{
	if (locName == "/")
		return (1);
	if (path.length() < locName.length())
		return (0);
	for (size_t i = 0; i < locName.length(); i++)
	{
		if (locName[i] != path[i])
			return (0);
	}
	if (path.length() == locName.length())
		return (locName.length());
	else if (path[locName.length()] == '/')
		return (locName.length());
	else
		return (0);
}
std::string getStatusMessage(int code)
{
	switch (code)
	{
	case 0:
		return "Waiting status";
	case 200:
		return "OK";
	case 201:
		return "Created";
	case 204:
		return "No Content";
	case 301:
		return "Moved Permanently";
	case 302:
		return "Found";
	case 303:
		return "See Other";
	case 304:
		return "Not Modified";
	case 400:
		return "Bad Request";
	case 401:
		return "Unauthorized";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	case 405:
		return "Method Not Allowed";
	case 408:
		return "Request Timeout";
	case 411:
		return "Length Required";
	case 413:
		return "Payload Too Large";
	case 414:
		return "URI Too Long";
	case 415:
		return "Unsupported Media Type";
	case 500:
		return "Internal Server Error";
	case 501:
		return "Not Implemented";
	case 502:
		return "Bad Gateway";
	case 503:
		return "Service Unavailable";
	case 504:
		return "Gateway Timeout";
	case 505:
		return "HTTP Version Not Supported";
	default:
		return "Unknown Status";
	}
}

void setError(Response &resp, int code)
{
	if (code == 0)
		return;
	if (resp.getStatusCode() != 0)
		return;
	std::string message = getStatusMessage(code);
	resp.setStatus(code, message);
}

const Location &getLocation(std::string path, const Config &conf)
{
	static Location defaultLocation("/");
	static bool initialized = false;
	if (conf.getLocations().empty())
	{
		if (!initialized)
		{
			defaultLocation = Location("/");
			defaultLocation.setAllowMethods("GET POST DELETE");
			initialized = true;
		}
		return defaultLocation;
	}
	int bestMatch = 0;
	std::vector<Location>::const_iterator match = conf.getLocations().begin();
	for (std::vector<Location>::const_iterator it = conf.getLocations().begin(); it != conf.getLocations().end(); ++it)
	{
		if (matchCalc(path, it->getName()) > bestMatch)
		{
			bestMatch = matchCalc(path, it->getName());
			match = it;
		}
	}
	return (*match);
}

std::string findFirstIndex(std::vector<std::string> indexes, std::string filePath)
{
	std::string path;
	for (size_t i = 0; i < indexes.size(); i++)
	{
		struct stat st;
		if (filePath[filePath.size() - 1] != '/' && indexes[i][0] != '/') 
			filePath = filePath + '/';
		path = filePath + indexes[i];
		if (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
			return (path);
	}
	return ("");
}

std::string removeLocation(std::string absolutePath, std::string locationName)
{
	std::string result;

	if (absolutePath == locationName)
		return (result);
	if (absolutePath.find(locationName) == 0)
		result = absolutePath.substr(locationName.size());
	if (result[0] == '/')
		result = result.substr(1);
	return (result);
}
std::string getFilePath(Request &req, const Location &loc, const Config &conf)
{
	std::string root;
	std::vector<std::string> index;
	std::string filePath;

	if (loc.getRoot().empty())
		root = conf.getRoot();
	else
		root = loc.getRoot();
	if (loc.getIndex().empty())
		index = conf.getIndex();
	else
		index = loc.getIndex();

	if (root.length() > 1 && root[root.length() - 1] == '/')
		root = root.substr(0, root.length() - 1);

	std::string relativePath = removeLocation(req.getPath(), loc.getName());
	if (root[root.size() - 1] != '/' && relativePath[0] != '/')
		root = root + "/";
	filePath = root + relativePath;
	struct stat buffer;
	if (stat(filePath.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode))
		return (findFirstIndex(index, filePath));
	return (filePath);
}

void buildForGet(Request &req, const Location &loc, const Config &conf, Response &resp)
{
	std::string filePath = getFilePath(req, loc, conf);
	if (access(filePath.c_str(), F_OK) != 0)
	{
		setError(resp, 404);
	}
	else if (access(filePath.c_str(), R_OK) != 0)
	{
		setError(resp, 403);
	}

	const std::string &ext = getExtension(filePath);
	std::string type = getMimeType(ext);
	resp.setFileType(type);
	int fd = open(filePath.c_str(), O_RDONLY);
	if (fd < 0)
	{
		setError(resp, 500);
	}

	char buffer[8192];
	ssize_t bytesRead;
	if (resp.getStatusCode() == 0)
	{
		while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
		{
			std::string oldBody = resp.getBody();
			oldBody.append(buffer, static_cast<size_t>(bytesRead));
			resp.setBody(oldBody);
		}
		if (bytesRead < 0)
			setError(resp, 500);
		close(fd);
		setError(resp, 200);
	}
}

int checkDeletePath(std::string path)
{
	if (path.find("..") != std::string::npos)
		return (0);
	if (path.find("%2e") != std::string::npos)
		return (0);
	if (path.find("%2E") != std::string::npos)
		return (0);
	return (1);
}

void buildForDelete(Request &req, const Location &loc, const Config &conf, Response &resp)
{
	std::string filePath;
	std::string root;
	loc.getRoot().empty() ? root = conf.getRoot() : root = loc.getRoot();
	filePath = root + req.getPath();
	if (!checkDeletePath(filePath))
	{
		std::cout << "NO hacking pls" << std::endl;
		return (setError(resp, 403));
	}
	struct stat st;
	if (stat(filePath.c_str(), &st) != 0)
		return (setError(resp, 404));
	else if (access(root.c_str(), W_OK) != 0 || !S_ISREG(st.st_mode))
		return (setError(resp, 403));
	if ((unlink(filePath.c_str())) != 0)
		return (setError(resp, 500));
	else
		return (setError(resp, 204));
}

std::string findFileName(std::string body)
{
	std::string toFind = body.substr(body.find("filename=") + 10);
	std::string fileName;
	for (int i = 0; toFind[i] != '\"'; ++i)
	{
		fileName.push_back(toFind[i]);
	}
	return fileName;
}

int writeBodyInFile(int fd, std::string body, std::string boundary, size_t bodySize)
{
	body = body.substr(body.find(boundary) + boundary.size());
	body = body.substr(body.find("\r\n\r\n") + 4);
	int end = body.find(boundary);
	std::string cpy = body.substr(0, end - 2);
	if (cpy.size() > bodySize)
		return (413);
	size_t total = 0;
	while (total < cpy.size())
	{
		ssize_t wn = write(fd, cpy.data() + total, cpy.size() - total);
		if (wn > 0)
			total += static_cast<size_t>(wn);
		else if (wn == 0)
			break;
		else
			return (500);
	}
	if (total != cpy.size())
		return (500);
	close(fd);
	return (204);
}

bool isAuthorized(std::string ext)
{
	std::string whiteList[] = {"txt", "jpg", "gif", "pdf", "png", "mp3", "md", "xpm", "jpeg", "html", "css", "c", "cpp", "hpp"};
	for (size_t i = 0; i < 14; i++) // Attention a bien augmenter la taille ici si on ajoute des types a la whitelist //
	{
		if (ext == whiteList[i])
			return (true);
	}
	return (false);
}

void handleFileUpload(Request &req, const Location &loc, const Config &conf, Response &resp, size_t bodySize)
{
	std::string root = (loc.getRoot().empty() ? conf.getRoot() : loc.getRoot());
	std::string multipart = req.getHeaders()["content-type"];
	size_t pos = multipart.find("boundary=");
	if (pos == std::string::npos)
		return (setError(resp, 400));
	multipart = multipart.substr(pos + 9);
	
	struct stat st;
	if (stat(root.c_str(), &st) != 0)
	{
		if (mkdir(root.c_str(), 0755) != 0)
			return (setError(resp, 500));
	}
	if (access(root.c_str(), W_OK) != 0)
		return (setError(resp, 403));
		
	std::string fileName = root + findFileName(req.getBody());
	if (!isAuthorized(getExtension(fileName)))
		return (setError(resp, 415));
	if ((access(fileName.c_str(), F_OK) == 0) && (access(fileName.c_str(), W_OK) != 0))
		return (setError(resp, 403));
	int fd = open(fileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, 00777);
	if (fd < 0)
		return (setError(resp, 500));
	int code = writeBodyInFile(fd, req.getBody(), multipart, bodySize);
	return (setError(resp, code));
}

void buildForPost(Request &req, const Location &loc, const Config &conf, Response &resp)
{
	size_t bodySize;
	if (loc.hasSizeChanged())
		bodySize = loc.getClientMaxBodySize();
	else
		bodySize = conf.getClientMaxBodySize();
	if (req.getContentLength() <= 0)
	{
		setError(resp, 411);
		return;
	}
	else if (req.getContentLength() > bodySize)
	{
		setError(resp, 413);
		return;
	}
	std::string contentType = req.getHeaders()["content-type"];
	if (contentType.find("multipart/form-data") != std::string::npos)
	{
		handleFileUpload(req, loc, conf, resp, bodySize);
	}
	else
	{
		std::string statusMsg = "OK";
		std::string body = "POST request received successfully";
		std::string fileType = "text/plain";
		resp.setStatus(200, statusMsg);
		resp.setBody(body);
		resp.setFileType(fileType);
	}
}

size_t getUriLen(Request req)
{
	if (req.getPath().empty())
		return (0);
	size_t len = req.getPath().length();
	return (len);
}

bool isCgi(Request &req, const Location &loc, const Config &conf)
{
	std::string filePath = req.getPath();
	std::string extension = getExtension(filePath);
	std::string extWithDot = "." + extension;
	std::vector<std::string> locCgiExt = split(loc.getCgiExt());
	if (!locCgiExt.empty())
	{
		for (size_t i = 0; i < locCgiExt.size(); i++)
		{
			if (extWithDot == locCgiExt[i])
				return (true);
		}
		return (false);
	}

	std::vector<std::string> confCgiExt = split(conf.getCgiExt());
	for (size_t i = 0; i < confCgiExt.size(); i++)
	{
		if (extWithDot == confCgiExt[i])
		{
			if (req.getMethod() != "POST")
				return (false);
			return (true);
		}
	}
	return (false);
}

std::string getExecutable(const std::string &filePath, const Location &loc, const Config &conf)
{
	std::string filePathCopy = filePath;
	std::string extension = getExtension(filePathCopy);
	std::string extWithDot = "." + extension;

	std::vector<std::string> allowedExtensions = split(loc.getCgiExt());
	std::vector<std::string> executables = split(loc.getCgiPath());
	if (!allowedExtensions.empty())
	{
		if (allowedExtensions.size() != executables.size())
			return ("no");
		for (size_t i = 0; i < allowedExtensions.size(); i++)
		{
			if (allowedExtensions[i] == extWithDot)
				return (executables[i]);
		}
		return ("no");
	}

	allowedExtensions = split(conf.getCgiExt());
	executables = split(conf.getCgiPath());
	if (allowedExtensions.size() != executables.size())
		return ("no");
	for (size_t i = 0; i < allowedExtensions.size(); i++)
	{
		if (allowedExtensions[i] == extWithDot)
			return (executables[i]);
	}
	return ("no");
}

void executeCgi(Request &req, const Location &loc, const Config &conf, Client &client, Response &resp, std::vector<Cgi> &cgis)
{
	Cgi newCgi(client.getFd(), req);
	size_t maxSize;
	if (loc.hasSizeChanged())
		maxSize = loc.getClientMaxBodySize();
	else
		maxSize = conf.getClientMaxBodySize();
	newCgi.setMaxBodySize(maxSize);
	int pipeIn[2];
	int pipeOut[2];
	pid_t pid;
	std::string filePath = getFilePath(req, loc, conf);
	if (access(filePath.c_str(), F_OK) != 0)
		return (setError(resp, 404));
	if (access(filePath.c_str(), X_OK) != 0)
		return (setError(resp, 403));
	std::string executable = getExecutable(filePath, loc, conf);
	if (executable == "no")
		return (setError(resp, 403));
	if ((pipe(pipeIn)) == -1 || (pipe(pipeOut)) == -1)
		return (setError(resp, 500));
	pid = fork();
	if (pid == -1)
		return (setError(resp, 500));
	else if (!pid)
	{
		close(pipeIn[1]);
		dup2(pipeIn[0], STDIN_FILENO);
		close(pipeIn[0]);

		close(pipeOut[0]);
		dup2(pipeOut[1], STDOUT_FILENO);
		close(pipeOut[1]);
		char *argv[] = {
			const_cast<char *>(executable.c_str()),
			const_cast<char *>(filePath.c_str()),
			NULL};

		std::map<std::string, std::string> headers = req.getHeaders();
		std::map<std::string, std::string> cookies = req.getCookies();
		std::vector<std::string> envStrings;
		std::vector<char *> envPtrs;

		if (!cookies.empty())
		{
			std::string cookieHeader = "";
			for (std::map<std::string, std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it)
			{
				if (!cookieHeader.empty())
					cookieHeader += "; ";
				cookieHeader += it->first + "=" + it->second;
			}
			envStrings.push_back("HTTP_COOKIE=" + cookieHeader);
		}
		envStrings.push_back("REQUEST_METHOD=" + req.getMethod());
		envStrings.push_back("SCRIPT_FILENAME=" + filePath);
		envStrings.push_back("PATH_INFO=" + req.getPath());
		envStrings.push_back("SERVER_PROTOCOL=" + req.getHttpVersion());
		if (headers.find("content-length") != headers.end())
			envStrings.push_back("CONTENT_LENGTH=" + headers["content-length"]);
		if (headers.find("content-type") != headers.end())
			envStrings.push_back("CONTENT_TYPE=" + headers["content-type"]);

		for (size_t i = 0; i < envStrings.size(); i++)
			envPtrs.push_back(const_cast<char *>(envStrings[i].c_str()));
		envPtrs.push_back(NULL);
		if (execve(executable.c_str(), argv, &envPtrs[0]) == -1)
			throw ;
	}
	else
	{
		int status;

		close(pipeIn[0]);
		close(pipeOut[1]);

		setSocketFlags(pipeOut[0]);
		setSocketFlags(pipeIn[1]);

		// on stocke le body et on l'enverra plus tard via select()
		std::string body = req.getBody();
		newCgi.setBody(body);
		newCgi.setByteSent(0);
		waitpid(pid, &status, WNOHANG);
	}
	if (newCgi.getBody().empty())
	{
		close(pipeIn[1]);
		pipeIn[1] = -1;
	}
	if (pipeIn[1] != -1)
		setSocketFlags(pipeIn[1]);
	setSocketFlags(pipeOut[0]);
	newCgi.setFdIn(pipeIn[1]);
	newCgi.setFdOut(pipeOut[0]);
	newCgi.setPid(pid);
	cgis.push_back(newCgi);
	resp.setCgiState(true);
}

void buildErrorPage(Response &resp, const Config conf, Location loc, Request req)
{
	(void)req;

	if (!resp.getBody().empty())
	{
		if (resp.getFileType().empty())
		{
			std::string type = "text/plain";
			resp.setFileType(type);
		}
		return;
	}

	std::string root = conf.getRoot();
	if (!root.empty() && root[root.size() - 1] != '/')
		root += '/';

	std::string errorFile;

	std::map<int, std::string> pages;
	pages = loc.getErrorPage();
	std::map<int, std::string>::const_iterator it = pages.find(resp.getStatusCode());
	if (it != pages.end())
		errorFile = it->second;
	if (errorFile.empty())
	{
		pages = conf.getErrorPage();
		std::map<int, std::string>::const_iterator it = pages.find(resp.getStatusCode());
		if (it != pages.end())
			errorFile = it->second;
	}
	std::string errorPath;
	if (!errorFile.empty())
	{
		if (!errorFile.empty() && errorFile[0] == '/')
			errorPath = errorFile;
		else
			errorPath = root + errorFile;
	}
	bool loaded = false;
	std::string body;
	if (!errorPath.empty())
	{
		struct stat st;
		if (stat(errorPath.c_str(), &st) == 0 && S_ISREG(st.st_mode) && access(errorPath.c_str(), R_OK) == 0)
		{
			int fd = open(errorPath.c_str(), O_RDONLY);
			if (fd >= 0)
			{
				body.reserve(static_cast<size_t>(st.st_size));
				char buffer[BUFFER_SIZE];
				ssize_t n;
				while ((n = read(fd, buffer, BUFFER_SIZE)) > 0)
					body.append(buffer, static_cast<size_t>(n));
				close(fd);
				if (n >= 0)
					loaded = true;
			}
		}
	}

	if (loaded)
	{
		std::string type = getMimeType(getExtension(errorPath));
		if (type.empty())
			type = "text/html";
		resp.setFileType(type);
		resp.setBody(body);
		return;
	}

	std::string type = "text/html";
	resp.setFileType(type);
	std::ostringstream out;
	out << "<html><head><title>" << resp.getStatusCode() << " " << resp.getStatusMessage() << "</title></head>";
	out << "<body><h1>" << resp.getStatusCode() << "</h1><p>" << resp.getStatusMessage() << "</p></body></html>";
	std::string fallbackBody = out.str();
	resp.setBody(fallbackBody);
}

bool methodExists(std::string method)
{
	std::string methods[] = {"GET", "POST", "DELETE", "HEAD", "OPTIONS", "TRACE", "PUT", "PATCH", "CONNECT"};
	for (size_t i = 0; i < 9; i++)
	{
		if (method == methods[i])
			return (true);
	}
	return (false);
}

Response ResponseBuilder::build(Request &req, const Config &conf, Client &client, std::vector<Cgi> &cgis)
{
	Response resp;
	bool methodAllowed = false;
	resp.setCgiState(false);

	resp.setMethod(req.getMethod());
	std::string version = req.getHttpVersion();
	if (version.empty())
		version = "HTTP/1.1";
	resp.setHttpVersion(version);
	const Location &loc = getLocation(req.getPath(), conf);
	if (loc.getRedirectCode() != 0)
	{
		int code = loc.getRedirectCode();
		setError(resp, code);
		if (code >= 300 && code < 400)
		{
			std::string locationHeader = "Location";
			std::string redirectUrl = loc.getRedirectUrl();
			resp.setHeader(locationHeader, redirectUrl);
			buildErrorPage(resp, conf, loc, req);
			return resp;
		}
	}
	int error = req.getErrorCode();
	std::string message = getStatusMessage(error);
	setError(resp, error);

	if (version != "HTTP/1.1")
		setError(resp, 505);
	if (!methodExists(req.getMethod()))
		setError(resp, 501);
	std::vector<std::string> allowMethods = loc.getAllowMethods();
	for (std::vector<std::string>::const_iterator it = allowMethods.begin(); it != allowMethods.end(); ++it)
	{
		if (req.getMethod() == *it)
		{
			methodAllowed = true;
			break;
		}
	}
	if (methodAllowed == false)
		setError(resp, 405);
	if (getUriLen(req) > MAX_URI_LEN)
		setError(resp, 414);
	if (resp.getStatusCode() == 0)
	{
		if (isCgi(req, loc, conf))
			executeCgi(req, loc, conf, client, resp, cgis);
		else if (req.getMethod() == "GET")
			buildForGet(req, loc, conf, resp);
		else if (req.getMethod() == "POST")
			buildForPost(req, loc, conf, resp);
		else if (req.getMethod() == "DELETE")
			buildForDelete(req, loc, conf, resp);
	}
	if (resp.getStatusCode() >= 400)
		buildErrorPage(resp, conf, loc, req);
	
	return (resp);
}