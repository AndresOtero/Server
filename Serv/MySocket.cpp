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
	msg_size = sizeof(msg_t );
}

MySocket::MySocket(int pNumber){
	 portNumber = pNumber;
	 connected = false;
	 msg_size = sizeof(msg_t );

	if ( (socketId=socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		LOG_WARNING << "SocketError: ERROR abriendo el socket";

	}

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = htons(portNumber);
}

void MySocket::setKeepAlive(int timeOut)
{
	struct timeval tv;

	tv.tv_sec = timeOut;  /*  Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
	if ( setsockopt(socketId,SOL_SOCKET,SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval)) == -1 )
	{
		LOG_WARNING << "SocketError : error seteando el timout para el keepAlive";
	}
}
void MySocket::bindSocket()
{
	if (bind(socketId,(struct sockaddr *)&clientAddr,sizeof(struct sockaddr_in))==-1)
	{
		LOG_WARNING << "SocketError :error en bind()";

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
		LOG_WARNING << "SocketError: no such host";

	    }
	bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portNumber);


	if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
	{
		LOG_WARNING << "SocketError: error al conectar con el server";

	}
	connected = true;
}

void MySocket::reconnectToServer(){
	if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) < 0)
	{
		//errorLog("error al conectar con el server /n");
	}
	connected = true;
}

MySocket* MySocket::acceptClient(std::string& clientName)
{
	int newSocket;

	socklen_t clientAddressLen = sizeof(struct sockaddr_in);

    struct sockaddr_in clientAddress;

	if ((newSocket = accept(socketId, (struct sockaddr *)&clientAddress, &clientAddressLen)) == -1)
	{
		LOG_WARNING << "SocketError: error en accept()";

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
		LOG_WARNING << "SocketError: error en listen()";

	}
}

int MySocket::sendMessage(msg_t& message)
{

	int n;

	n = write(socketId, &message, msg_size);

	return n;
}

msg_t MySocket::recieveMessage()
{
	msg_t msg_buffer;
	int n = 0;
	int bytes_read = 0;
	int error_counter = 0;
	while ((bytes_read < msg_size) && (connected)){
		setKeepAlive(2);
		n = read(socketId, &(msg_buffer) + bytes_read, msg_size - bytes_read);
		verifyNumbytes(error_counter, n);
		bytes_read += n;
	}
    return msg_buffer;
}

void  MySocket::verifyNumbytes(int &error_counter, int numbytes){
	if (numbytes <= 0){
		if (error_counter <= 0) {
			error_counter = error_counter + 1;
		} else {
			connected = false;
		}
	} else {
		error_counter = 0;
	}
}
