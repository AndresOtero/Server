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

void Interprete::setJuego(Juego * juego){
	this->gameCtrl->insertarJuego(juego);

}

void Interprete::crearModelo(){
	this->gameCtrl->crearModelo();
}

void Interprete::enviar_mensaje_a_users(msg_t msg){
	printf("Mandar a todos lo users\n");
	for (User* user : *users){
		if(user != NULL){
			printf("manda a usuario\n");
			if(user-> isConnected())
				user->agregarNotificacion(msg);
		}
	}
}

Interprete::Interprete() {
	this->gameCtrl = new GameControllerServer();
}

msg_t Interprete:: getKeepAliveMsg(){
	msg_t r;
	r.type = KEEPALIVE;
	return r;
}

msg_t Interprete:: getQuit(){
	msg_t r;
	r.type = QUIT;
	return r;
}

bool Interprete::isQuit(msg_t quit){
	return (quit.type == QUIT);
}

void Interprete::procesarMensajeDeCliente(msg_t msg, User* user) {
//TODO aca hacer la decodificacion de msgs y ejecutar el comando del gameCtrl;
	switch (msg.type) {
	case MOVER_PERSONAJE:

		this->gameCtrl->cambiar_destino_personaje(msg.paramNombre, msg.paramDouble1, msg.paramDouble2);
		break;

	case ACTUALIZACION_RECURSOS:
		//Esto por ahora se resuelve en el cliente.
		break;

	case KEEPALIVE:
		//no se hace nada
		break;

	default:
		break;
	}
}

void Interprete::postLoginMsg(msg_t msg, User* user){

	string loginName= string(msg.paramNombre);
	user->setLoginName(loginName);
}
void Interprete::notifyReccconection(User* user){
	// el nombre del usuario que se reconecto, que va a tener el mismo nombre que el jugador.
	this->gameCtrl->reconectar(user->getLoginName());
	msg_t mensajeReconexion;
	mensajeReconexion.type = RECONNECT;
	memcpy(mensajeReconexion.paramNombre,string_to_char_array(user->getLoginName()),sizeof(mensajeReconexion.paramNombre));
	enviar_mensaje_a_users(mensajeReconexion);
}

void Interprete::notifyNewUser(User* user){

	this->gameCtrl->agregarCliente(user->getLoginName(), "soldado");
}

void Interprete::notifyLostUserConnection(User* user){
	//TODO la idea es que desconectar reciba el nombre del usuario que se reconecto, que va a tener el mismo nombre que el jugador.
	this->gameCtrl->desconectar(user->getLoginName());
	msg_t mensajeDesconexion;
	mensajeDesconexion.type = QUIT;
	memcpy(mensajeDesconexion.paramNombre,string_to_char_array(user->getLoginName()),sizeof(mensajeDesconexion.paramNombre));
	enviar_mensaje_a_users(mensajeDesconexion);
}

void Interprete::generarRecursoRandom(){
	this->gameCtrl->generarRecursoRandom();
}

void Interprete::enviarActualizacionesDelModeloAUsuarios(){
	this->gameCtrl->actualizar();
	while(this->gameCtrl->hayEventos()){
		msg_t actualizacion = gameCtrl->sacarMensaje();

		 printf("Server- Manda %d \n", actualizacion .type);
		enviar_mensaje_a_users(actualizacion);
	}
}
void Interprete::inicializarModelo(MySocket * socket){

	queue <msg_t> colaInicializacion=this->gameCtrl->inicializacion();

	while(!colaInicializacion.empty()){
		msg_t mensaje = colaInicializacion.front();
		colaInicializacion.pop();
		socket->sendMessage(mensaje);
	}
	msg_t fin;
	fin.type = FIN_INICIALIZACION;
	socket->sendMessage(fin);
}
Interprete::~Interprete() {
	delete this->gameCtrl;
}
