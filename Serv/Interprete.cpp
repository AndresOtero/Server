/*
 * Interprete.cpp
 *
 *  Created on: 21/10/2015
 *      Author: ger
 */

#include "Interprete.h"

Interprete::Interprete() {
	this->gameCtrl = new GameControllerSrv();
}
msg_t Interprete:: getKeepAliveMsg(){
	msg_t r = {KEEPALIVE,"",0,0};
	return r;
}
msg_t Interprete:: getQuit(){
	msg_t r = {QUIT,"",0,0};
	return r;
}
bool Interprete::isQuit(msg_t quit){
	return (quit.type == QUIT);
}

void Interprete:: notifyUpdate(msg_t msg,User* user, vector<User*> users){
//aca hacer la decodificacion de msgs y ejecutar el comando del gameCtrl;
}

void Interprete::postLoginMsg(msg_t msg, User* user){

	string loginName= string(msg.paramNombre);
	user->setLoginName(loginName);
}
void Interprete::notifyReccconection(User* user, vector<User*> users){

}

void Interprete::notifyNewUser(User* user, vector<User*> users){

}

void Interprete::notifyLostUserConnection(User* user, vector<User*> users){


}

void Interprete::notifyQuitUser(User* user, vector<User*> users){

}

Interprete::~Interprete() {
	delete this->gameCtrl;
}

