# webserv_42

A lightweight HTTP server written in C as part of the 42 curriculum.

Overview

Webserv is a custom implementation of a minimal HTTP server designed to better understand how the web actually works under the hood. Instead of relying on existing servers like Nginx or Apache, the goal of this project is to build one from scratch using low-level system calls.

The server listens for incoming TCP connections, parses HTTP requests, and returns appropriate HTTP responses while serving static files or executing CGI scripts.

Features

HTTP/1.1 request parsing

Static file serving

Multiple server configuration (similar to virtual hosts)

Support for GET, POST and DELETE methods

File uploads

CGI execution

Custom error pages

Non-blocking I/O with event multiplexing

Basic configuration file support

Technical Challenges

Building a web server from scratch reveals several core challenges:

Network programming using sockets

Handling multiple clients simultaneously without blocking

Correctly parsing HTTP requests and headers

Managing file I/O and large responses

Designing a configuration system similar to real web servers

Implementing CGI execution and process management

Special attention was given to efficient connection handling using non-blocking sockets and an event-driven architecture.

Learning Outcomes

This project provides a deeper understanding of:

How HTTP communication works

How production web servers manage connections

Low-level UNIX system calls

Event-driven server design

Robust error handling in network applications

Usage

Compile the project:

make

Run the server with a configuration file:

./webserv config.conf

Then open a browser and connect to:

http://localhost:8080
Authors

Project developed as part of the 42 curriculum.
