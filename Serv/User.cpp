#include "User.h"

User::User(string& clientIP){
	this->clientIP = clientIP;
	this->connectedFlag = true;
}
User::~User() {
	delete &clientIP;
	delete &colaNotificaciones;
}

