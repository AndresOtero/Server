#include "MySocket.h"
#include <thread>
#include <vector>
#include <SDL2/SDL.h>
#include "User.h"
#include "Interprete.h"
#include <queue>
#include "Yaml.h"
#include "GameControllerSrc/GameControllerServer.h"
#include "GameControllerSrc/mensaje.h"
#include <plog/Log.h>

#define RITMO_RECURSO 5

const unsigned int MAX_NUM_CLIENTS = 8;
using namespace std;

struct cola_data{
	User* senderUser;
	msg_t evento;
};

struct thread_data{
	MySocket*  newClient;
	queue <cola_data>* colaEventos;
	SDL_mutex *mutex;
	User* user;
	Interprete* interprete;
	vector<User*>* users;
};

struct thread_ppal_data{
	queue <cola_data>* colaEventos;
	vector<User*>* users;
	Interprete* interprete;
};



void enviarKeepAlive(MySocket* myClient, Interprete* interprete){

	msg_t messageToServer = interprete->getKeepAliveMsg();

	myClient->sendMessage(messageToServer);

}

void enviarMensajeUsuarioTomado(MySocket* socket){

	msg_t messageToClient;
	messageToClient.type = ERROR_NOMBRE_TOMADO;

	socket->sendMessage(messageToClient);

}

void agregarACola(queue <cola_data>* colaEventos,msg_t evento,User* client, SDL_mutex *mutex){

	struct cola_data cola_data;

	cola_data.senderUser =  client;
	cola_data.evento =  evento;

	if (SDL_LockMutex(mutex) == 0) {

		colaEventos->push(cola_data);
		SDL_UnlockMutex(mutex);
	} else {
		//LOG_WARNING << "Couldn't lock mutex\n";

	}
}

void notifyClient(MySocket* socket,User* user, Interprete* interprete  ){
	msg_t notificacion;
	while( (user->isColaVacia()==false) &&  (socket->isConnected())){
			notificacion =user->popNotifiacion();
			socket->sendMessage(notificacion);
	}
	enviarKeepAlive(socket, interprete);
}

User* establecerLogin(MySocket* socket, vector<User*> &users,
		Interprete* interprete, string clientIP, unsigned int &counter,
		SDL_mutex *mutexGameCtrl) {
	//plog::init(plog::warning, "Log.txt");
	msg_t msgFromClient = socket->recieveMessage();

	if (socket->isConnected()) {
		//LOG_WARNING << "Login: " << msgFromClient.paramNombre;

		for (unsigned int i = 0; i <= counter; i++) {
			User* tempUser = users[i];
			if (tempUser != NULL) {

				string loginName = string(msgFromClient.paramNombre);

				//TODO VER QUE SE FIJA POR EL NOMBRE DE JUGADOR Y NO POR LA IP
				if (tempUser->getLoginName() == loginName) {

					if (!tempUser->isConnected()) {
						enviarKeepAlive(socket, interprete); //Porque aca el cliente espera un mensaje por si el usuario esta tomado.
						//LOG_WARNING << "Reconexion";
						tempUser->setConnectedFlag(true);
						interprete->inicializarModelo(socket); //inicializar modelo
						interprete->notifyReccconection(tempUser);

						return tempUser;

					} else {
						enviarMensajeUsuarioTomado(socket);
						return NULL;
					}
				}

			} else {
				enviarKeepAlive(socket, interprete); //Porque aca el cliente espera un mensaje por si el usuario esta tomado.
				users[i] = new User(clientIP);
				users[i]->setMutex(mutexGameCtrl);
				users[i]->setConnectedFlag(true);
				interprete->setUsers(&users);
				//LOG_WARNING << "Primera conexion";

				interprete->inicializarModelo(socket); //incializar modelo

				interprete->postLoginMsg(msgFromClient, users[i]);
				interprete->notifyNewUser(users[i]);
				counter = counter + 1;
				return users[i];
			}
		}
	}
	return NULL;
}

void* acceptedClientThread(void *threadArg ){

	struct thread_data *my_data;
	msg_t messageFromClient;

	/* Recive argumentos */
	my_data = (struct thread_data *) threadArg;
	MySocket* socket = my_data ->newClient;
	queue <cola_data>* colaEventos = my_data -> colaEventos;
	SDL_mutex *mutex = my_data -> mutex;
	User* user = my_data -> user;
	Interprete* interprete = my_data ->interprete;
	vector<User*> users = *(my_data -> users);
	//interprete->setUsers(&users);
	/* FIN Recive argumentos */

	user->setConnectedFlag(true);

	//inicializacion modelo


   while (socket->isConnected()){

	  //interprete->enviarActualizacionesDelModeloAUsuarios(user->getMutex());

	  notifyClient(socket,user,interprete);

	  messageFromClient = socket->recieveMessage();

	  if (socket->isConnected()){
		  agregarACola(colaEventos,messageFromClient,user, mutex);
	  }
    }
   user->setConnectedFlag(false);

   interprete->notifyLostUserConnection(user);

   pthread_exit(NULL);
}



void serverHandleThread(void* threadArgPpal){

	struct thread_ppal_data* ppal_data;
	ppal_data = (struct thread_ppal_data*) threadArgPpal;

	queue <cola_data>* colaEventos = ppal_data ->colaEventos;
	vector<User*> users = *(ppal_data -> users);
	Interprete* interprete = ppal_data -> interprete;

	MySocket myServer(PORTNUM);

	myServer.bindSocket();
	myServer.listenToClient();

	pthread_t tAcceptedClient [MAX_NUM_CLIENTS];

	struct thread_data threadArg[MAX_NUM_CLIENTS];

	SDL_mutex *mutex;
	mutex = SDL_CreateMutex();
	if (!mutex) {
		//LOG_WARNING << "SocketError:Couldn't create mutex\n";
	}

	SDL_mutex* mutexGameCtrl = interprete->getMutexGameCtrl();

	unsigned int counter = 0;
	while (  counter < MAX_NUM_CLIENTS )
	{
	   string clientIP;
	   MySocket* newClient = myServer.acceptClient(clientIP); //conexion dedicada al nuevo cliente

	   User* user = establecerLogin(newClient,users,interprete,clientIP, counter,mutexGameCtrl);

	   if (user != NULL){
		   threadArg[counter].colaEventos = colaEventos;
		   threadArg[counter].newClient = newClient;
		   threadArg[counter].mutex = mutex;
		   threadArg[counter].user = user;
		   threadArg[counter].interprete = interprete;
		   threadArg[counter].users = &users;

		   pthread_create(&tAcceptedClient[counter], NULL, acceptedClientThread,(void *) &threadArg[counter]);
	   }
	}
	SDL_DestroyMutex(mutex);
}

void simularEventosEnCola(queue <cola_data>* colaEventos, Interprete* interprete, vector<User*> users){

	struct cola_data cola_dato;
	double tiempo_actual,tiempo_viejo=0;
	tiempo_viejo=SDL_GetTicks();
	double acumulado=tiempo_viejo;
	//plog::init(plog::warning, "Log.txt");
	while (1){

		//Recibe las actualizaciones provenientes del modelo y envia los mensajes correspondientes a todos los users.


		while(!colaEventos->empty()){
			 cola_dato = colaEventos->front();
			 colaEventos->pop();

			 interprete->procesarMensajeDeCliente(cola_dato.evento,cola_dato.senderUser);
			 interprete->enviarActualizacionesDelModeloAUsuarios(interprete->getMutexGameCtrl());
			 //interprete->enviarActualizacionesDelModeloAUsuarios(interprete->getMutexGameCtrl());
			 //TODO mandar al interprete para que decodifique con todos los users para poder agregar mensajes en sus colas
			 // en cada user se tiene un flag para ver si esta conecatado o no (para agregar o no la notificacion nueva)


		}
		usleep((40 - (tiempo_actual - tiempo_viejo)) * 1000);
		tiempo_actual = SDL_GetTicks();
		tiempo_viejo = tiempo_actual;
		if (((tiempo_actual - acumulado) / 1000) > (double) RITMO_RECURSO) {
			//cargaria en game controller el mensaje de crear recurso si crea
			//hay que ver como cargarla en la cola eventos para todos los usuarios
			interprete->generarRecursoRandom();
			acumulado = tiempo_actual;
		}

	}

}

int main(int argc, char *argv[]) {
	//plog::init(plog::warning, "Log.txt");
	queue <cola_data>  colaEventos;
	SDL_mutex *mutexGameCtrl;
	mutexGameCtrl = SDL_CreateMutex();
	if (!mutexGameCtrl) {
		//LOG_WARNING << "Couldn't create mutexGameCtrl\n";
	}
	Interprete interprete(mutexGameCtrl);
	Yaml * i = new Yaml("YAML/configuracionServer.yaml");
	Juego * juego = i->readServer();
	delete i;
	interprete.setJuego(juego);
	interprete.crearModelo();
	vector<User*> users(MAX_NUM_CLIENTS,NULL);
	struct thread_ppal_data  threadArgu;

	/* abre thread que controla a los clientes */
	threadArgu.colaEventos = &colaEventos;
	threadArgu.users = &users;
	threadArgu.interprete = &interprete;

	thread tServer(serverHandleThread, (void*)&threadArgu );
	/* FIN abre thread que controla a los clientes */

	simularEventosEnCola(&colaEventos,&interprete, users);

	tServer.join();

	for (unsigned int i= 0; i < users.size(); i++){
		if (users [i]) delete users [i];
	}

	return 0;
}
