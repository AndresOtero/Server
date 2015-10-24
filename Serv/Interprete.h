/*
 * Interprete.h
 *
 *  Created on: 21/10/2015
 *      Author: ger
 */

#ifndef INTERPRETE_H_
#define INTERPRETE_H_

#include <string>
#include "GameControllerSrv.h"
#include "User.h"
#include "mensaje.h"

using namespace std;

class Interprete {
public:
	Interprete();
	msg_t getKeepAliveMsg();

	msg_t getQuit();
	bool isQuit(msg_t quit);

	void postLoginMsg(msg_t msg, User* user);

	void notifyUpdate(msg_t msg,User* user, vector<User*> users);

	void notifyReccconection(User* user, vector<User*> users);

	void notifyLostUserConnection(User* user, vector<User*> users);

	void notifyNewUser(User* user, vector<User*> users);

	void notifyQuitUser(User* user, vector<User*> users);

	virtual ~Interprete();

private:
	GameControllerSrv* gameCtrl;
};

#endif /* INTERPRETE_H_ */
