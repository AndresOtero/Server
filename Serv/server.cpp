#include "MySocket.h"
#include <thread>
#include <vector>
#include <SDL2/SDL.h>
#include "User.h"
#include "Interprete.h"
#include <queue>
#include "GameControllerSrc/GameControllerServer.h"
#include "GameControllerSrc/mensaje.h"
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

void agregarACola(queue <cola_data>* colaEventos,msg_t evento,User* client, SDL_mutex *mutex){

	struct cola_data cola_data;

	cola_data.senderUser =  client;
	cola_data.evento =  evento;

	if (SDL_LockMutex(mutex) == 0) {

		colaEventos->push(cola_data);
		SDL_UnlockMutex(mutex);
	} else {
	  fprintf(stderr, "Couldn't lock mutex\n");
	}
}

void notifyClient(MySocket* socket,User* user, Interprete* interprete  ){
	msg_t notificacion;

	if( (user->isColaVacia() == false) &&  (socket->isConnected())){

		 notificacion = user->popNotifiacion();

		 socket->sendMessage(notificacion);
	}else{
		enviarKeepAlive(socket, interprete);
	}

}

User* establecerLogin(MySocket* socket, vector<User*> &users, Interprete* interprete, string clientIP, unsigned int &counter){

	msg_t msgFromClient = socket->recieveMessage();

	if (socket->isConnected()){
		printf("Login: %s \n", msgFromClient.paramNombre);

		for (unsigned int i = 0; i <= counter; i++){
		   User* tempUser = users [i];
			if (tempUser != NULL) {

				string loginName= string(msgFromClient.paramNombre);

				//TODO VER QUE SE FIJA POR EL NOMBRE DE JUGADOR Y NO POR LA IP
				if(tempUser->getLoginName() == loginName){

					printf("reconexion del jugador \n");
					tempUser->setConnectedFlag(true);

					interprete->notifyReccconection(tempUser,users);

					return tempUser;
				}
			}else{

				users [i] = new User(clientIP);
				users [i] ->setConnectedFlag(true);

				interprete->postLoginMsg(msgFromClient, users [i]);
				interprete->notifyNewUser(users [i],users);

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
	msg_t messageFromClient;

	/* Recive argumentos */
	my_data = (struct thread_data *) threadArg;
	MySocket* socket = my_data ->newClient;
	queue <cola_data>* colaEventos = my_data -> colaEventos;
	SDL_mutex *mutex = my_data -> mutex;
	User* user = my_data -> user;
	Interprete* interprete = my_data ->interprete;
	vector<User*> users = *(my_data -> users);

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

   //TODO ver si fue QUIT notificar ese QUIT (no va a servir ahora me parece)
   interprete->notifyLostUserConnection(user,users);

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
	   threadArg[counter].users = &users;

	   pthread_create(&tAcceptedClient[counter], NULL, acceptedClientThread,(void *) &threadArg[counter]);

	}
	SDL_DestroyMutex(mutex);
}

void simularEventosEnCola(queue <cola_data>* colaEventos, Interprete* interprete, vector<User*> users){
	struct cola_data cola_dato;
	double tiempo_actual,tiempo_viejo=0;
	GameControllerServer gameController;
	tiempo_viejo=SDL_GetTicks();
	double acumulado=tiempo_viejo;
	while (1){

		//Recibe las actualizaciones provenientes del modelo y envia los mensajes correspondientes a todos los users.
		interprete->enviarActualizacionesDelModeloAUsuarios(users);

		while(!colaEventos->empty()){
			 cola_dato = colaEventos->front();
			 colaEventos->pop();

			 //printf("Server- Procesa: %d \n", cola_dato.evento.type);

			 interprete->processUpdate(cola_dato.evento,cola_dato.senderUser,users);

			 //TODO mandar al interprete para que decodifique con todos los users para poder agregar mensajes en sus colas
			 // en cada user se tiene un flag para ver si esta conecatado o no (para agregar o no la notificacion nueva)
		}
		 usleep((40 - (tiempo_actual-tiempo_viejo))*1000);
		 tiempo_actual= SDL_GetTicks();
		 tiempo_viejo=tiempo_actual;
		 if ((tiempo_actual-acumulado)/1000 > (double)RITMO_RECURSO){
			//cargaria en game controller el mensaje de crear recurso si crea
			 //hay que ver como cargarla en la cola eventos para todos los usuarios
			 gameController.generarRecursoRandom();
		 }
	}

}

int main(int argc, char *argv[]) {

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

	simularEventosEnCola(&colaEventos,&interprete, users);

	tServer.join();

	for (unsigned int i= 0; i < users.size(); i++){
		if (users [i]) delete users [i];
	}

	return 0;
}
