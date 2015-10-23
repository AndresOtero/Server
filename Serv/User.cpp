#include "User.h"

User::User(string& clientIP){
	string noti = "primera noti";
	this->agregarNotificacion(noti);
	string seg = "segnoti";
	this->agregarNotificacion(seg);
	this->clientIP = clientIP;
	this->connectedFlag = true;
}
User::~User() {
	delete &clientIP;
	delete &colaNotificaciones;
}

