/*
 * Interprete.cpp
 *
 *  Created on: 21/10/2015
 *      Author: ger
 */

#include "Interprete.h"

Interprete::Interprete() {
	this->gameCtrl = NULL;
}
string Interprete:: getKeepAliveMsg(){
	return string("ALIVE");
}
string Interprete:: getReceivedMsg(){
	return string("OK");
}
string Interprete:: getQuit(){
	return string("QUIT");
}
bool Interprete::isQuit(string& quit){
	return (quit.compare("QUIT") == 0);
}
string Interprete:: getActualizarRecursosMsg(int oro, int madera,int piedra){
	char buffer[50];

	sprintf(buffer,"RECURSOS %10d %10d %10d",oro,madera,piedra);

	string result = string(buffer);

	return result;
}
string Interprete::getFinalUpdates(){

	return string("FINAL_UPDATES");
}

void Interprete:: notifyUpdate(string& msg){
//aca hacer la decodificacion de msgs y ejecutar el comando del gameCtrl;
}

void Interprete::postLoginMsg(string& msg, User* user){
	//decoficar msg y meter en user el name
}

Interprete::~Interprete() {

}

