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

using namespace std;

class User {
public:

	string clientIP;
	User(string& clientIP );

	void agregarNotificacion(string evento){
		colaNotificaciones.push(evento);
	}
	bool isColaVacia(){return colaNotificaciones.empty();}

	string popNotifiacion(){
		string r= colaNotificaciones.front();
		colaNotificaciones.pop();

		return r ;
	}
	void setLoginName(string& login){
		loginName = clientIP;
	}
	void setConnectedFlag(bool flag){ this->connectedFlag = flag;};

	bool isConnected(){return this->connectedFlag;}
	string getLoginName(){return loginName;}
	string getClientIP(){return clientIP;}
	virtual ~User();

private:
	bool connectedFlag;

	string loginName;
	queue <string> colaNotificaciones;
};

#endif /* USER_H_ */
