#include "MySocket.h"
#include <thread>
#include <vector>
#include <SDL2/SDL.h>
#include "Cola.h"
#include "User.h"
#include "Interprete.h"
#include <queue>
#include "ModeloSrc/Modelo.h"
#include "VistaSrc/Vista.h"
#include "ModeloSrc/Configuracion.h"
#include "VistaSrc/CambioDeCoordendas.h"
#include "GameControllerSrc/GameControllerCliente.h"
#include "Yaml.h"

const unsigned int MAX_NUM_CLIENTS = 8;
using namespace std;

struct cola_data{
	User* senderIP;
	string evento;
};

struct thread_data{
	MySocket*  newClient;
	queue <cola_data>* colaEventos;
	SDL_mutex *mutex;
	User* user;
	Interprete* interprete;
};

struct thread_ppal_data{
	queue <cola_data>* colaEventos;
	vector<User*>* users;
	Interprete* interprete;
};



void enviarKeepAlive(MySocket* myClient, Interprete* interprete){

	string messageToServer = interprete->getKeepAliveMsg();

	myClient->sendMessage(messageToServer);

}

void agregarACola(queue <cola_data>* colaEventos,string evento,User* client, SDL_mutex *mutex){

	struct cola_data cola_data;

	cola_data.senderIP =  client;
	cola_data.evento =  evento;

	if (SDL_LockMutex(mutex) == 0) {

		colaEventos->push(cola_data);
		SDL_UnlockMutex(mutex);
	} else {
	  fprintf(stderr, "Couldn't lock mutex\n");
	}
}

void notifyClient(MySocket* socket,User* user, Interprete* interprete  ){
	string notificacion;

	if( (user->isColaVacia() == false) &&  (socket->isConnected())){

		 notificacion = user->popNotifiacion();

		 socket->sendMessage(notificacion);
	}else{
		enviarKeepAlive(socket, interprete);
	}

}

User* establecerLogin(MySocket* socket, vector<User*> &users, Interprete* interprete, string clientIP, unsigned int &counter){

	string msgFromClient = socket->recieveMessage();

	if (socket->isConnected()){
		printf("Login: %s \n", msgFromClient.c_str());
		//interprete->postLoginMsg(msgFromClient, user);

		for (unsigned int i = 0; i <= counter; i++){
		   User* tempUser = users [i];
			if (tempUser != NULL) {
				//comparar por el nombre del jugador tambien
				if(tempUser->clientIP.compare(clientIP) == 0){
					//notificar a todos reconexion
					printf("reconexion del jugador \n");
					tempUser->setConnectedFlag(true);
					return tempUser;
				}
			}else{
				//notificar a todos conexion
				users [i] = new User(clientIP);

				users [i] ->setConnectedFlag(true);
				counter = counter + 1;
				printf("conexion del jugador \n");
				return users [i];
			}
		}
	}

   return NULL;

}

void* acceptedClientThread(void *threadArg ){

	struct thread_data *my_data;
	string messageFromClient, messageToClient;

	/* Recive argumentos */
	my_data = (struct thread_data *) threadArg;
	MySocket* socket = my_data ->newClient;
	queue <cola_data>* colaEventos = my_data -> colaEventos;
	SDL_mutex *mutex = my_data -> mutex;
	User* user = my_data -> user;
	Interprete* interprete = my_data ->interprete;
	/* FIN Recive argumentos */

   while ( (!interprete->isQuit(messageFromClient)) && (socket->isConnected())){

	  notifyClient(socket,user,interprete);

	  messageFromClient = socket->recieveMessage();

	  if (socket->isConnected()){
		  //printf("recibe: %s \n", messageFromClient.c_str());
		  agregarACola(colaEventos,messageFromClient,user, mutex);
	  }
    }
   user->setConnectedFlag(false);

   printf("se desconecta cliente  \n");

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
		fprintf(stderr, "Couldn't create mutex\n");
	}
	unsigned int counter = 0;
	while (  counter < MAX_NUM_CLIENTS )
	{
	   string clientIP;
	   MySocket* newClient = myServer.acceptClient(clientIP); //conexion dedicada al nuevo cliente

	   User* user = establecerLogin(newClient,users,interprete,clientIP, counter);

	   threadArg[counter].colaEventos = colaEventos;
	   threadArg[counter].newClient = newClient;
	   threadArg[counter].mutex = mutex;
	   threadArg[counter].user = user;
	   threadArg[counter].interprete = interprete;

	   pthread_create(&tAcceptedClient[counter], NULL, acceptedClientThread,(void *) &threadArg[counter]);

	}
	SDL_DestroyMutex(mutex);
}

void simularEventosEnCola(queue <cola_data>* colaEventos, vector<User*> users){
	struct cola_data cola_dato;
	double tiempo_actual,tiempo_viejo=0;
	tiempo_viejo=SDL_GetTicks();

	while (1){
		while(!colaEventos->empty()){
			 cola_dato = colaEventos->front();
			 colaEventos->pop();

			 //printf("Server- Procesa: %s \n", cola_dato.evento.c_str());

			 //tengo en cola_dato el Ip del cliente que envio el evento y el mensaje

			 //mandar al interprete para que decodifique con todos los users para poder agregar mensajes en sus colas
			 // en cada user se tiene un flag para ver si esta conecatado o no (para agregar o no la notificacion nueva)
		}
		 usleep((40 - (tiempo_actual-tiempo_viejo))*1000);
		 tiempo_actual= SDL_GetTicks();
		 tiempo_viejo=tiempo_actual;
	}

}

int main(int argc, char *argv[]) {
	/**Main del juego**/
	bool reiniciar = true;

	while (reiniciar) {
		Yaml* reader = new Yaml();
		Juego* juego = reader->read();
		delete reader;
		Modelo* modelo = new Modelo(juego);
		Vista* vista = new Vista(modelo);
		vista->init();
		vista->loadMedia();
		reiniciar = vista->run();
		delete modelo;
		delete vista;
	}
	/**Fin del main del juego**/
	queue <cola_data>  colaEventos;
	Interprete interprete;
	vector<User*> users(MAX_NUM_CLIENTS) ;
	struct thread_ppal_data  threadArgu;

	/* abre thread que controla a los clientes */
	threadArgu.colaEventos = &colaEventos;
	threadArgu.users = &users;
	threadArgu.interprete = &interprete;

	thread tServer(serverHandleThread, (void*)&threadArgu );
	/* FIN abre thread que controla a los clientes */

	simularEventosEnCola(&colaEventos, users);

	tServer.join();

	for (unsigned int i= 0; i < users.size(); i++){
		if (users [i]) delete users [i];
	}

	return 0;
}
