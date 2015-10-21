#include "User.h"

User::User(string& clientIP){
	this->colaNotificaciones = new Cola();
	this->clientIP = clientIP;
	this->connectedFlag = true;
}
User::~User() {
	delete &clientIP;
	delete this->colaNotificaciones;
}

