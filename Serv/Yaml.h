/*
 * Yaml.h
 *
 *  Created on: 12 de set. de 2015
 *      Author: andres
 */
#include <iostream>
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <string>
#include <vector>
#include "ModeloSrc/Juego.h"
#include "ModeloSrc/Jugador.h"

#ifndef YAML_H_
#define YAML_H_
typedef struct {
	int x;
	int y;
	std::string tipo;
} Entidad_t;
typedef struct {
	std::string nombre;
	std::string ip;
	std::string raza;
} Jugador_t;
typedef struct {
	std::string tipo;
	int x;
	int y;
} Protagonista_t;

typedef struct {
	std::string nombre;
	int size_x;
	int size_y;
	Protagonista_t protagonista;
} Escenario_t;

typedef struct {
	int ancho;
	int alto;
} Pantalla_t;

typedef struct {
	int vel_personaje;
	int margen_scroll;
} Configuracion_t;

typedef struct {
	std::string nombre;
	std::string imagen;
	std::string imagen_atacando;
	std::string imagen_quieto;
	int ancho_base;
	int alto_base;
	int pixel_ref_x;
	int pixel_ref_y;
	int fps;
	int delay;
	int velocidad;
	int fuerza;
	int armadura;
	int recoleccion;
	int construccion;
	int vida;
	int comida;
	int oro;
	int piedra;
	int madera;
	string raza;
	int velocidad_construcccion;
	string edificio_constructor;
	int rango;
} Objeto_mapa_t;

typedef struct {
	Pantalla_t pantalla;
	Configuracion_t configuracion;
	Escenario_t escenario;
}ConfiguracionJuego_t;

class Yaml {
	string config_filepath;
public:
	Yaml();
	Yaml(string path);

	Juego* readServer();
	Juego* readCliente();
	virtual ~Yaml();

private:
	std::map<std::string, ObjetoMapa*> tipos;
	int cantidad_de_objetos;
	std::vector< Entidad*> entidades;
	int cantidad_de_entidades;

	Pantalla* cargarPantalla(ConfiguracionJuego_t conf, YAML::Node* doc);
	Configuracion* cargarConfiguracion(ConfiguracionJuego_t conf, YAML::Node* doc);
	void cargarObjetoMapa(const YAML::Node* pTipos);
	Escenario* cargarEscenario(ConfiguracionJuego_t conf,  const YAML::Node* pEscenario);
	Personaje* cargarPersonaje(ConfiguracionJuego_t conf,const  YAML::Node* pEscenario);
	Jugador* cargarJugador( YAML::Node* doc,Personaje* pers);
	Entidad* cargarEntidad(const YAML::Node* pEntidades);

};

#endif /* YAML_H_ */
