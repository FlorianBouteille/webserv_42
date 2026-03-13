/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csolari <csolari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 17:18:59 by csolari           #+#    #+#             */
/*   Updated: 2025/12/02 17:18:59 by csolari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "WebservIncludes.hpp"

class Client
{
    public:
        Client();
        Client(int fd);
        ~Client();
        Client(const Client& other);
        Client& operator=(const Client& other); 
        int getFd() const;
        std::string getRequestBuffer() const;
        std::string &getResponseBuffer();
        bool hasPendingWrite(void) const;
        bool isReadClosed(void) const;
        void addToResponse(std::string & response);
        void setResponse(std::string & response, int index);
        void removeFromResponses(int index);
        void setBeginRequest(void);
        std::vector<std::string> & getResponseVector();
        void setRequestBuffer(std::string &buffer);
        void setResponseBuffer(std::string &buffer);
        void setPendingWrite(bool value);
        void setReadClosed(bool value);
		bool isTimedOut(time_t timeOut) const;
		void updateLastActivity();
		time_t getLastActivity() const;
		time_t getBeginRequest() const;

    private:
        int _fd;
        std::string _requestBuffer;
        std::string _responseBuffer;
        std::vector<std::string> _responses;
        bool _requestComplete;
        bool _hasPendingWrite;
        bool _readClosed;
		time_t _lastActivity;
        time_t _beginRequest;
};

#endif
