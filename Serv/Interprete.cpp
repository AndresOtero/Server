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
string Interprete:: getKeepAliveMsg(){
	string r = "ALIVE";
	return r;
}
string Interprete:: getReceivedMsg(){
	string r = "OK";
	return r;
}
string Interprete:: getQuit(){
	string r = "QUIT";
	return r;
}
bool Interprete::isQuit(string& quit){
	return (quit.compare(getQuit()) == 0);
}
string Interprete:: getActualizarRecursosMsg(int oro, int madera,int piedra){
	char buffer[50];

	sprintf(buffer,"RECURSOS %10d %10d %10d",oro,madera,piedra);

	string result = string(buffer);

	return result;
}
string Interprete::getFinalUpdates(){

	string r = "FINAL_UPDATES";
	return r;
}

void Interprete:: notifyUpdate(string& msg){
//aca hacer la decodificacion de msgs y ejecutar el comando del gameCtrl;
}

void Interprete::postLoginMsg(string& msg, User* user){
	//decoficar msg y meter en user el name
}

Interprete::~Interprete() {
	delete this->gameCtrl;
}

