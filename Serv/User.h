/*
 * User.h
 *
 *  Created on: 20/10/2015
 *      Author: ger
 */

#ifndef USER_H_
#define USER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <queue>
#include "mensaje.h"

using namespace std;

class User {
public:


	User(string& clientIP );

	void agregarNotificacion(msg_t evento){
		colaNotificaciones.push(evento);
	}
	bool isColaVacia(){return colaNotificaciones.empty();}

	msg_t popNotifiacion(){
		msg_t r= colaNotificaciones.front();
		colaNotificaciones.pop();

		return r ;
	}
	void setLoginName(string login){
		loginName = login;
	}
	void setConnectedFlag(bool flag){ this->connectedFlag = flag;};

	bool isConnected(){return this->connectedFlag;}

	string getLoginName(){
		string name = loginName;
		return name;
	}

	string getClientIP(){return clientIP;}
	virtual ~User();

private:
	bool connectedFlag;

	string clientIP;
	string loginName;
	queue <msg_t> colaNotificaciones;
};

#endif /* USER_H_ */
