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
	Interprete();
	msg_t getKeepAliveMsg();

	msg_t getQuit();
	bool isQuit(msg_t quit);

	void postLoginMsg(msg_t msg, User* user);

	void procesarMensajeDeCliente(msg_t msg,User* user, vector<User*> users);

	void notifyReccconection(User* user, vector<User*> users);

	void notifyLostUserConnection(User* user, vector<User*> users);

	void notifyNewUser(User* user, vector<User*> users);

	void generarRecursoRandom();

	void inicializarModelo(MySocket* socket);
	/**
	 	 Recibe las actualizaciones provenientes del modelo y envia los mensajes correspondientes a todos los users.
	 */
	void enviarActualizacionesDelModeloAUsuarios(vector<User*> users);

	void notifyUsersMovimientoPersonaje(int id, double x, double y);

	virtual ~Interprete();

private:
	GameControllerServer* gameCtrl;

	char* string_to_char_array(string str);

	void enviar_mensaje_a_users(msg_t msg, vector<User*> users);
};

#endif /* INTERPRETE_H_ */
