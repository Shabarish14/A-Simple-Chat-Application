Project 3: A Simple Chat Application

## Objectives

1. Signal handling and thread programming
2. Socket-based networking application development
3. Application-layer network protocol design and development

## Project Description

This project consists of a chat server and chat clients. The chat server maintains user account information and facilitates the chat functionality. The chat clients send either broadcast or targeted messages to other clients.

Contents in my directory proj3
| chat_client configuration_file
| chat_server configuration_file
| client.c
| server.c
| Makefile

## Compiling

To compile the server and client programs, run the following command in the `proj3` directory:

`make all`

This will generate two executable files, 'server.x' and 'client.x'.

Running the chat application:

To begin, launch the server with the command ./server.x or ./server.x chat_server configuration_file. After the server has been started, we can run the clients by running the command./client.x or./client.x chat_client configuration_file. In the client, we can now use the commands login, chat, logout, and exit.To begin using the chat application, you must first login with the command "login" followed by a user name. After logging in, use the command "chat" followed by your message. After you logged in, you can only use two commands: "chat" and "logout". If you want to exit the chat application, you must first logout before using the "exit" command. 



### Pthread and Select() Usage

In this project, we have used pthread for handling multiple clients on the server side. The server spawns a new thread for each client connection

to manage their requests independently. The select() system call is used in the client program to handle user input and incoming messages simultaneously, enabling a responsive chat experience.


### Application Layer Protocol

We have designed a custom application layer protocol for the message exchange between the client and server. The protocol uses a simple text-based format for commands and message forwarding, similar to HTTP and SMTP. The protocol supports login, logout, exit, and chat commands.
