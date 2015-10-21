/*
 * MySocket.h
 *
 *  Created on: 19/10/2015
 *      Author: ger
 */

#ifndef MYSOCKET_H_
#define MYSOCKET_H_

#include <iostream>
#include <unistd.h>  // problem solved! it compiles!
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int PORTNUM = 51717;
const int MSG_HEADER_LEN = 6;

class MySocket {

protected:
	int portNumber;        // Socket port number
	int socketId;          // Socket file descriptor

	bool connected;          // ConnectedFlag flag

	struct sockaddr_in clientAddr;

	struct sockaddr_in serverAddress;

public:

	MySocket();
	MySocket(int);

	virtual ~MySocket(){close(socketId);}

	int getSocketId() { return socketId; }

	int getPortNumber() { return portNumber; }

	void setKeepAlive(int timeOut);

	void setSocketId(int socketFd) { socketId = socketFd; }

	bool isConnected(){return connected;}

	/*
	   Sends a message to a connected host. The number of bytes sent is returned
	   -  server call or client call
	*/
	int sendMessage( std::string& message);

	/*
	   receive messages and stores the message in a buffer
	*/
	int recieveMessage(std::string& message);

	/*
	   Binds the socket to an address and port number - a server call
	*/
	void bindSocket();

	/*
	   accepts a connecting client.  The address of the connected client
	   is stored in the parameter -  a server call
	*/
	MySocket* acceptClient(std::string& clientName);

	// Listens to connecting clients, a server call
	void listenToClient(int numPorts = 5);

	void verifyNumbytes(int numbytes);

	// connect to the server, a client call
	virtual void connectToServer(const char* serverAddr);

	void reconnectToServer();

};

#endif /* MYSOCKET_H_ */
