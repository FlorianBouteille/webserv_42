#ifndef CGI_HPP
#define CGI_HPP

#include "WebservIncludes.hpp"

class Request;

class Cgi
{
    public:
        ~Cgi();
        Cgi();
        Cgi(const Cgi& other);
        Cgi& operator=(const Cgi& other); 
        Cgi(int clientFd, Request req);

        int     getPid(void) const;
        int     getFdIn(void) const;
        int     getFdOut(void) const;
        int     getClientFd(void) const;
        std::string getBody(void) const;
        size_t getBytesSent(void) const;
        std::string getResponse(void) const;
        Request getRequest(void) const;
        time_t getStartTime(void) const;
        size_t getMaxBodySize(void) const;
        
        void setPid(int value);
        void setFdIn(int value);
        void setFdOut(int value);
        void setBody(std::string value);
        void setByteSent(size_t value);
        void setResponse(std::string value);
        void setStartTime(time_t value);
        void setMaxBodySize(size_t value);

        void appendResponse(std::string buffer, ssize_t n);
        
    private:
        int         _pid;
        int         _fdIn;
        int         _fdOut;
        int         _clientFd;
        size_t      _maxBodySize;

        std::string _body;
        size_t      _bytesSent;
        std::string _response;
        Request     _request;

        time_t      _startTime;
};

#endif // CGI_HPP
