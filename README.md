# webserv_42

A lightweight HTTP server written in C as part of the 42 school curriculum.

About

Webserv is a custom implementation of a minimal HTTP server built from scratch using low-level system calls.
The goal of this project is to understand how real web servers work by recreating the core mechanisms behind HTTP communication, request parsing, and connection handling.

Instead of relying on existing servers like Nginx or Apache, this project focuses on building the essential components of a web server manually.

#Features

HTTP/1.1 request parsing

Static file serving

Support for GET, POST, and DELETE

Multiple server configurations (virtual hosts)

File uploads

CGI execution

Custom error pages

Non-blocking I/O handling


#Key Concepts

During this project we explored several fundamental topics:

TCP socket programming

HTTP protocol internals

Event-driven server architecture

Concurrent client handling

Process management for CGI execution

Configuration parsing


#Usage

Build the project:

make

Run the server with a configuration file:

./webserv config.conf

Then open:

http://localhost:8080

#Project Context

This project was developed as part of the 42 curriculum, which focuses on low-level programming and system understanding.
