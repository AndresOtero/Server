/*
 * Interprete.cpp
 *
 *  Created on: 21/10/2015
 *      Author: ger
 */

#include "Interprete.h"
#include "GameControllerSrc/GameControllerServer.h"
char* Interprete::string_to_char_array(string str){
	int str_size = str.size();
	char charArray[PARAM_STRING_LEN];
	for (int a = 0; a <= str_size; a++) {
		charArray[a] = str[a];
	}
	return charArray;
}

void Interprete::enviar_mensaje_a_users(msg_t msg, vector<User*> users){
	for (User* user : users){
		user->agregarNotificacion(msg);
	}
}

Interprete::Interprete() {
	this->gameCtrl = new GameControllerServer();
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

void Interprete:: processUpdate(msg_t msg,User* user, vector<User*> users){
//TODO aca hacer la decodificacion de msgs y ejecutar el comando del gameCtrl;
	switch (msg.type)
	{
	  case MOVER_PERSONAJE:
		 // this->gameCtrl->cambiar_destino_personaje(msg.paramInt1, msg.paramDouble1, msg.paramDouble2);
		  break;

	  case ACTUALIZACION_RECURSOS:
		  //Esto por ahora se resuelve en el cliente.
		  break;

	  default:
		  break;
	}
}

void Interprete::postLoginMsg(msg_t msg, User* user){

	string loginName= string(msg.paramNombre);
	user->setLoginName(loginName);
}
void Interprete::notifyReccconection(User* user, vector<User*> users){
	//TODO la idea es que reconectar reciba el nombre del usuario que se reconecto, que va a tener el mismo nombre que el jugador.
	this->gameCtrl->reconectar(user->getLoginName());
	msg_t mensajeReconexion;
	mensajeReconexion.type = RECONNECT;
	strcpy(mensajeReconexion.paramNombre, string_to_char_array(user->getLoginName()));
	enviar_mensaje_a_users(mensajeReconexion, users);
}

void Interprete::notifyNewUser(User* user, vector<User*> users){
//	TODO la idea es que agregarCliente reciba el nombre del usuario a agregar, que va a tener el mismo nombre que el jugador.
	this->gameCtrl->agregarCliente(user->getLoginName());
	msg_t mensajeLogin;
	mensajeLogin.type = LOGIN;
	strcpy(mensajeLogin.paramNombre, string_to_char_array(user->getLoginName()));
	enviar_mensaje_a_users(mensajeLogin, users);
}

//TODO mismo que notifyQuitUser?
void Interprete::notifyLostUserConnection(User* user, vector<User*> users){
	//TODO la idea es que desconectar reciba el nombre del usuario que se reconecto, que va a tener el mismo nombre que el jugador.
	this->gameCtrl->desconectar(user->getLoginName());
	msg_t mensajeDesconexion;
	mensajeDesconexion.type = QUIT;
	strcpy(mensajeDesconexion.paramNombre, string_to_char_array(user->getLoginName()));
	enviar_mensaje_a_users(mensajeDesconexion, users);
}

//TODO mismo que notifyLostUserConnection?
void Interprete::notifyQuitUser(User* user, vector<User*> users){
	//TODO la idea es que desconectar reciba el nombre del usuario que se reconecto, que va a tener el mismo nombre que el jugador.
	this->gameCtrl->desconectar(user->getLoginName());
	msg_t mensajeDesconexion;
	mensajeDesconexion.type = QUIT;
	strcpy(mensajeDesconexion.paramNombre, string_to_char_array(user->getLoginName()));
	enviar_mensaje_a_users(mensajeDesconexion, users);
}

void Interprete::enviarActualizacionesDelModeloAUsuarios(vector<User*> users){
//	TODO gameCtrl va a tener una cola de actualizaciones con mensajes para mandar a los usuarios (ej: tal personaje se movio).
	while(this->gameCtrl->hayEventos()){
		msg_t actualizacion = gameCtrl->sacarMensaje();
		enviar_mensaje_a_users(actualizacion, users);
	}
}

Interprete::~Interprete() {
	delete this->gameCtrl;
}

