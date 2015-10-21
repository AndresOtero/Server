/*
 * Interprete.h
 *
 *  Created on: 21/10/2015
 *      Author: ger
 */

#ifndef INTERPRETE_H_
#define INTERPRETE_H_

#include <string>
#include "GameControllerSrc/GameControllerServer.h"
#include "User.h"

using namespace std;

class Interprete {
public:
	Interprete();
	string getKeepAliveMsg();
	string getReceivedMsg();

	string getQuit();
	bool isQuit(string& quit);

	string getActualizarRecursosMsg(int oro, int madera,int piedra);

	void postLoginMsg(string& msg, User* user);

	void notifyUpdate(string& msg);
	string getFinalUpdates();

	virtual ~Interprete();

private:
	GameControllerServer* gameCtrl;

};

#endif /* INTERPRETE_H_ */
