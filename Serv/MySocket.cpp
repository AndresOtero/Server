/*
 * MySocket.cpp
 *
 *  Created on: 19/10/2015
 *      Author: ger
 */

#include "MySocket.h"

void errorLog(const char *msg) {
    perror(msg);
    exit(1);
}

MySocket::MySocket(){
	portNumber = 0;
	connected = true;
	socketId = 0;

	//setKeepAlive(2);
}

MySocket::MySocket(int pNumber){
	 portNumber = pNumber;
	 connected = false;

	if ( (socketId=socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		errorLog("ERROR abriendo el socket");
	}

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = htons(portNumber);

	//setKeepAlive(2);
}

void MySocket::setKeepAlive(int timeOut)
{
	struct timeval tv;

	tv.tv_sec = timeOut;  /*  Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
	if ( setsockopt(socketId,SOL_SOCKET,SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval)) == -1 )
	{
		errorLog("error seteando el timout para el keepAlive");
		connected = false;
	}
	connected = true;
}
void MySocket::bindSocket()
{
	if (bind(socketId,(struct sockaddr *)&clientAddr,sizeof(struct sockaddr_in))==-1)
	{
		errorLog("error en bind() /n");
	}
}
void MySocket::connectToServer(const char* serverAddr)
{
	/*
	   when this method is called, a client socket has been built already,
	   so we have the socketId and portNumber ready.

	   serverAddr puede ser "127.0.0.1"
	*/

	struct hostent *server;

	server = gethostbyname(serverAddr);
	if (server == NULL) {
		errorLog("ERROR, no such host /n");
	    }
	bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portNumber);


	if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
	{
		errorLog("error al conectar con el server /n");
	}

}

void MySocket::reconnectToServer(){
	if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
	{
		errorLog("error al conectar con el server /n");
	}
}

MySocket* MySocket::acceptClient(std::string& clientName)
{
	int newSocket;

	socklen_t clientAddressLen = sizeof(struct sockaddr_in);

    struct sockaddr_in clientAddress;

	if ((newSocket = accept(socketId, (struct sockaddr *)&clientAddress, &clientAddressLen)) == -1)
	{
		errorLog("error en accept()");
	}

	 clientName = inet_ntoa((struct in_addr)clientAddress.sin_addr); //return the IP

    // Create and return the new MySocket object
    MySocket* retSocket = new MySocket();
	retSocket->setSocketId(newSocket);
    return retSocket;
}
void MySocket::listenToClient(int totalNumPorts)
{
	if (listen(socketId,totalNumPorts) == -1)
	{
		errorLog("error en listen()");
	}
}

int MySocket::sendMessage(std::string& message)
{
	int numBytes;  // the number of bytes sent

	char msgLength[MSG_HEADER_LEN+1];
    sprintf(msgLength,"%6zu",message.size());
	std::string sendMsg = std::string(msgLength);
    sendMsg += message;

	// Sends the message to the connected host
    numBytes = send(socketId,sendMsg.c_str(),sendMsg.size(),0);

    verifyNumbytes(numBytes);

	return numBytes;
}

int MySocket::recieveMessage(std::string& message)
{
	int numBytes;

	char msgLength[MSG_HEADER_LEN+1];
	memset(msgLength,0,sizeof(msgLength));

	setKeepAlive(2);
	numBytes = recv(socketId,msgLength,MSG_HEADER_LEN,0); // es lo mismo que un read con el parametro en 0
	if (numBytes > 0){
		numBytes = recv(socketId,(char*)(message.c_str()),atoi(msgLength),0);
	}

	verifyNumbytes(numBytes);

    return numBytes;
}

void  MySocket::verifyNumbytes(int numbytes){
	if ( numbytes <= 0) {
			connected = false;
	}
}
