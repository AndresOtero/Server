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
#include "GameControllerSrc/mensaje.h"
#include "MySocket.h"
using namespace std;

class Interprete {
public:
	GameControllerServer* gameCtrl;
	Interprete(SDL_mutex *mutexGameCtrl);
	msg_t getKeepAliveMsg();

	msg_t getQuit();
	bool isQuit(msg_t quit);
	void crearModelo();

	void setJuego(Juego * juego);

	void postLoginMsg(msg_t msg, User* user);

	void procesarMensajeDeCliente(msg_t msg,User* user);

	void notifyReccconection(User* user);

	void notifyLostUserConnection(User* user);

	void notifyNewUser(User* user);

	void generarRecursoRandom();

	void inicializarModelo(MySocket* socket);
	/**
	 	 Recibe las actualizaciones provenientes del modelo y envia los mensajes correspondientes a todos los users.
	 */
	void enviarActualizacionesDelModeloAUsuarios(SDL_mutex *mutexGameCtrl);

	void notifyUsersMovimientoPersonaje(int id, double x, double y);

	virtual ~Interprete();

	vector<User*>* getUsers() const {
		return users;
	}

	void setUsers(vector<User*>* users) {
		this->users = users;
	}

	SDL_mutex* getMutexGameCtrl(){
		return this->mutexGameCtrl;
	}

private:
	SDL_mutex *mutexGameCtrl;
	vector<User*>* users;
	char* string_to_char_array(string str);
	void enviar_mensaje_a_users(msg_t msg);

};

#endif /* INTERPRETE_H_ */
