/*
 * Yaml.cpp
 *
 *  Created on: 12 de set. de 2015
 *      Author: andres
 */

#include "Yaml.h"
#include "ModeloSrc/Jugador.h"
#include "ModeloSrc/ObjetoMapa.h"
#include "ModeloSrc/Pantalla.h"
#include "ModeloSrc/Configuracion.h"
#include "ModeloSrc/Escenario.h"
#include "ModeloSrc/Oro.h"
#include "ModeloSrc/Piedra.h"
#include "ModeloSrc/Madera.h"
#include <plog/Log.h>


//#include "../ModeloSrc/ObjetoMapaAnimado.h"
#include <map>
Yaml::Yaml(string path) {
	cantidad_de_entidades=0;
	cantidad_de_objetos=0;
	config_filepath =path;



}
Yaml::Yaml() {
	cantidad_de_entidades=0;
	cantidad_de_objetos=0;
	config_filepath = "YAML/configuracionServer.yaml";

}

const std::string tag_pantalla = "pantalla";
const std::string tag_pantalla_ancho = "ancho";
const std::string tag_pantalla_alto = "alto";

const std::string tag_config = "configuracion";
const std::string tag_config_vel_personaje = "vel_personaje";
const std::string tag_config_margen_scroll = "margen_scroll";

const std::string tag_tipos = "tipos";
const std::string tag_tipos_nombre = "nombre";
const std::string tag_tipos_imagen = "imagen";
const std::string tag_tipos_ancho_base = "ancho_base";
const std::string tag_tipos_alto_base = "alto_base";
const std::string tag_tipos_pixel_ref_x = "pixel_ref_x";
const std::string tag_tipos_pixel_ref_y = "pixel_ref_y";
const std::string tag_tipos_delay = "delay";
const std::string tag_tipos_fps = "fps";
const std::string tag_tipos_velocidad = "velocidad";
const std::string tag_tipos_fuerza = "fuerza";
const std::string tag_tipos_armadura = "armadura";
const std::string tag_tipos_vida = "vida";
const std::string tag_tipos_recoleccion = "recoleccion";
const std::string tag_tipos_construccion = "construccion";
const std::string tag_tipos_raza = "raza";
const std::string tag_tipos_velocidad_construccion = "velocidad_de_construccion";


const std::string tag_escenario = "escenario";
const std::string tag_escenario_nombre = "nombre";
const std::string tag_escenario_size_x = "size_x";
const std::string tag_escenario_size_y = "size_y";
const std::string tag_escenario_entidades = "entidades";
const std::string tag_escenario_entidades_x = "x";
const std::string tag_escenario_entidades_y = "y";
const std::string tag_escenario_entidades_tipo = "tipo";
const std::string tag_escenario_protagonista = "protagonista";
const std::string tag_escenario_protagonista_x = "x";
const std::string tag_escenario_protagonista_y = "y";
const std::string tag_escenario_protagonista_tipo = "tipo";

const std::string tag_jugador = "jugador";
const std::string tag_jugador_nombre = "nombre";
const std::string tag_jugador_ip = "ip";
const std::string tag_jugador_raza = "raza";





Entidad* elegirEntidad(ObjetoMapa * objeto,Entidad_t entidad_t){
	Entidad* entidad;
	int x=entidad_t.x,y=entidad_t.y;
	if (objeto->nombre.compare("oro")==0)
		entidad = new Oro(objeto,x,y);
	else if (objeto->nombre.compare("piedra")==0)
		entidad = new Piedra(objeto,x,y);
	else if (objeto->nombre.compare("madera")==0)
		entidad = new Madera(objeto,x,y);
	else
		entidad = new Entidad(objeto,x,y);
	return entidad;
}
Entidad* Yaml::cargarEntidad(const
		YAML::Node* pEntidades) {
	Entidad_t entidad;
	Entidad* ent;
	if (const YAML::Node *pX = ((*pEntidades)[cantidad_de_entidades]).FindValue(
			tag_escenario_entidades_x)) {
		(*pX) >> entidad.x;
		if (const YAML::Node *pY = ((*pEntidades)[cantidad_de_entidades]).FindValue(
				tag_escenario_entidades_y)) {
			(*pY) >> entidad.y;
			if (const YAML::Node *pTipo = ((*pEntidades)[cantidad_de_entidades]).FindValue(
					tag_escenario_entidades_tipo)) {
				(*pTipo) >> entidad.tipo;
				if (ObjetoMapa* obj = tipos[entidad.tipo]) {
					ent = elegirEntidad(obj, entidad);
				} else {
					//LOG_WARNING << "No existe el tipo " << entidad.tipo
						//	<< " especificado por la entidad";
				}
			} else {
				//LOG_WARNING << "No se especifica el tipo de la entidad";
			}

		} else {
			//LOG_WARNING<< "Se define la posicion inicial x de la entidad pero no la posicion y";
		}
	} else {
		//LOG_WARNING << "No se define la posicion de la entidad";
	}
	return ent;
}
Personaje* Yaml::cargarPersonaje(ConfiguracionJuego_t conf, const YAML::Node* pEscenario) {

	Personaje* protagonista=NULL;
	if (const YAML::Node *pPersonaje = (*pEscenario).FindValue(
			tag_escenario_protagonista)) {
		if (const YAML::Node *pTipo = (*pPersonaje)[0].FindValue(
				tag_escenario_protagonista_tipo)) {
			(*pTipo) >> conf.escenario.protagonista.tipo;
					if (ObjetoMapa* obj =
							tipos[conf.escenario.protagonista.tipo]) {
						protagonista = new Personaje(obj,
								conf.escenario.protagonista.x,
								conf.escenario.protagonista.y);
						return protagonista;
					} else {
						//LOG_WARNING << "No existe el tipo '"								<< conf.escenario.protagonista.tipo<< "' definido para el protagonista";
					}
		}
	}
	return protagonista;

}
Escenario* Yaml::cargarEscenario(ConfiguracionJuego_t conf,const YAML::Node* pEscenario) {
	Escenario* escenario;
	if (const YAML::Node *pNombre = (*pEscenario).FindValue(
						tag_escenario_nombre)) {
					(*pNombre) >> conf.escenario.nombre;
				} else {
					LOG_WARNING << "No se define el nombre del escenario";
				}
				if (const YAML::Node *pSizeX = (*pEscenario).FindValue(
						tag_escenario_size_x)) {
					(*pSizeX) >> conf.escenario.size_x;

					if (const YAML::Node *pSizeY = (*pEscenario).FindValue(
							tag_escenario_size_y)) {
						(*pSizeY) >> conf.escenario.size_y;
						escenario = new Escenario(conf.escenario.nombre,
								conf.escenario.size_x, conf.escenario.size_y);
					} else {
						//LOG_WARNING								<< "Se define el tamaño en x del escenario pero no el y ";
						escenario = new Escenario();
					}
				} else {
					//LOG_WARNING << "No se define el tamaño del escenario";
					escenario = new Escenario();
				}
				return escenario;

}
void Yaml::cargarObjetoMapa(const YAML::Node* pTipos) {
	Objeto_mapa_t tipo;
	if (const YAML::Node *pTipoNombre =
			((*pTipos)[cantidad_de_objetos]).FindValue(tag_tipos_nombre)) {
		(*pTipoNombre) >> tipo.nombre;

		if (const YAML::Node *pTipoImagen =
				((*pTipos)[cantidad_de_objetos]).FindValue(tag_tipos_imagen)) {
			*pTipoImagen >> tipo.imagen;

			ObjetoMapa* objeto = new ObjetoMapa(tipo.nombre, tipo.imagen);

			if (const YAML::Node *pTipoAnchoBase =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_ancho_base)) {
				*pTipoAnchoBase >> tipo.ancho_base;
				if (const YAML::Node *pAltoBase =
						((*pTipos)[cantidad_de_objetos]).FindValue(
								tag_tipos_alto_base)) {
					*pAltoBase >> tipo.alto_base;
					objeto->baseLogica->ancho = tipo.ancho_base;
					objeto->baseLogica->alto = tipo.alto_base;
				} else {
					//LOG_WARNING							<< "Se define el ancho pero no el alto para la base logica";
				}
			}

			if (const YAML::Node *pPixelRefX =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_pixel_ref_x)) {
				*pPixelRefX >> tipo.pixel_ref_x;
				if (const YAML::Node *pPixelRefY =
						((*pTipos)[cantidad_de_objetos]).FindValue(
								tag_tipos_pixel_ref_y)) {
					*pPixelRefY >> tipo.pixel_ref_y;
					objeto->pixelsReferencia->x = tipo.pixel_ref_x;
					objeto->pixelsReferencia->y = tipo.pixel_ref_y;
				} else {
					//LOG_WARNING							<< "Se define el pixel de referencia x pero no el pixel de referencia y";
				}
			}
			if (const YAML::Node *pFps =
					((*pTipos)[cantidad_de_objetos]).FindValue(tag_tipos_fps)) {
				*pFps >> tipo.fps;
				objeto->fps = tipo.fps;
			}

			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_delay)) {
				*pDelay >> tipo.delay;
				objeto->delay = tipo.delay;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_velocidad)) {
				*pDelay >> tipo.velocidad;
				objeto->velocidad = tipo.velocidad;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_fuerza)) {
				*pDelay >> tipo.fuerza;
				objeto->fuerza = tipo.fuerza;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_armadura)) {
				*pDelay >> tipo.armadura;
				objeto->armadura = tipo.armadura;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_recoleccion)) {
				*pDelay >> tipo.recoleccion;
				objeto->recoleccion = tipo.recoleccion;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_vida)) {
				*pDelay >> tipo.vida;
				objeto->vida = tipo.vida;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_raza)) {
				*pDelay >> tipo.raza;
				objeto->raza = tipo.raza;
			}
			if (const YAML::Node *pDelay =
					((*pTipos)[cantidad_de_objetos]).FindValue(
							tag_tipos_velocidad_construccion)) {
				*pDelay >> tipo.velocidad_construcccion;
				objeto->velocidad_construcccion = tipo.velocidad_construcccion;
			}
			tipos[tipo.nombre] = objeto;
			cantidad_de_objetos ++;
		} else {
			//LOG_WARNING << "No se define la ruta de imagen para el tipo";
		}
	} else {
		//LOG_WARNING << "No se define nombre para el tipo";
	}
}
Configuracion* Yaml::cargarConfiguracion(ConfiguracionJuego_t conf,YAML::Node* doc) {
	Configuracion* configuracion;
	if (const YAML::Node *pConfiguracion = doc->FindValue(tag_config)) {
		if (const YAML::Node *pConfigVel = (*pConfiguracion).FindValue(
				tag_config_vel_personaje)) {
			(*pConfigVel) >> conf.configuracion.vel_personaje;
			if (const YAML::Node *pConfigMargen = (*pConfiguracion).FindValue(
					tag_config_margen_scroll)) {
				(*pConfigMargen) >> conf.configuracion.margen_scroll;
				configuracion = new Configuracion(
						conf.configuracion.vel_personaje,
						conf.configuracion.margen_scroll);
			} else {
				//LOG_WARNING << "No se define el margen para el scroll";
				configuracion = new Configuracion();
			}
		} else {
			//LOG_WARNING << "No se define el velocidad del personaje";
			configuracion = new Configuracion();
		}
	} else {
		//LOG_WARNING				<< "No se define configuracion para el margen del scroll y velocidad del personaje";
		configuracion = new Configuracion();
	}
	return configuracion;
}
Pantalla* Yaml::cargarPantalla(ConfiguracionJuego_t conf, YAML::Node* doc) {
	Pantalla* pantalla;
	if (const YAML::Node *pPantalla = doc->FindValue(tag_pantalla)) {
		if (const YAML::Node *pPantallaAncho = (*pPantalla).FindValue(
				tag_pantalla_ancho)) {
			(*pPantallaAncho) >> conf.pantalla.ancho;
			if (const YAML::Node *pPantallaAlto = (*pPantalla).FindValue(
					tag_pantalla_alto)) {
				(*pPantallaAlto) >> conf.pantalla.alto;
				pantalla = new Pantalla(conf.pantalla.ancho,
						conf.pantalla.alto);
			} else {
				//LOG_WARNING						<< "Se define configuracion de pantalla para el ancho pero no para el alto";
				pantalla = new Pantalla();
			}
		} else {
			//log conf pantalla sin ancho
			//LOG_WARNING					<< "Se define configuracion de pantalla pero no su ancho";
			pantalla = new Pantalla();
		}

	} else {
		// log no tiene pantalla
		//LOG_WARNING<< "No se define configuracion de pantalla, se usa Default 1024x728";
		pantalla = new Pantalla();
	}
	return pantalla;
}
Jugador* Yaml::cargarJugador(YAML::Node* doc, Personaje* pers) {
	Jugador* jugador = NULL;
	Jugador_t jug;
	if (const YAML::Node *pJugador = doc->FindValue(tag_jugador)) {
		if (const YAML::Node *pJugadorNombre = (*pJugador).FindValue(
				tag_jugador_nombre)) {
			(*pJugadorNombre) >> jug.nombre;
			if (const YAML::Node *pJugadorIp = (*pJugador).FindValue(
					tag_jugador_ip)) {
				(*pJugadorIp) >> jug.ip;
				if (const YAML::Node *pJugadorRaza = (*pJugador).FindValue(
						tag_jugador_raza)) {
					(*pJugadorRaza) >> jug.raza;
					jugador = new Jugador(jug.nombre, jug.ip, jug.raza);
				}
			} else {
				//LOG_WARNING						<< "No se define un nombre para el jugador";
			}
		} else {
			//LOG_WARNING					<< "No se define Ip";
		}

	} else {
		// log no tiene pantalla
		//LOG_WARNING				<< "No se define un jugador";
	}
	return jugador;
}

Juego* Yaml::read() {
	Juego* juego;
	//remove( "Log.txt" );
	//plog::init(plog::warning, "Log.txt");

	try {
		std::ifstream fin(config_filepath);
		YAML::Parser parser(fin);
		YAML::Node doc;
		parser.GetNextDocument(doc);

		ConfiguracionJuego_t conf;
		Pantalla* pantalla;
		Configuracion* configuracion;
		Escenario* escenario;
		Personaje* protagonista;

		pantalla=cargarPantalla(conf,&doc);
		configuracion=cargarConfiguracion(conf,&doc);
		const YAML::Node *pTipos = doc.FindValue(tag_tipos);
		if (pTipos) {
			for (unsigned i = 0; i < (*pTipos).size(); i++) {
				cargarObjetoMapa(pTipos);
			}
		} else {
			//LOG_WARNING << "No se define ningun tipo";
		}
		if (const YAML::Node *pEscenario = doc.FindValue(tag_escenario)) {
			escenario= cargarEscenario(conf,pEscenario);
			Personaje* protagonista =cargarPersonaje(conf,pEscenario);
			escenario->protagonista = protagonista;

			if (const YAML::Node *pEntidades = (*pEscenario).FindValue(
					tag_escenario_entidades)) {
				for (unsigned i = 0; i < (*pEntidades).size(); i++) {
					entidades.push_back(cargarEntidad(pEntidades));
				}
				escenario->entidades=entidades;

			}else {
				LOG_WARNING << "No hay entidades a instanciar inicialmente";
				}


		} else {
			escenario = new Escenario();
		}
		Jugador* jugador=cargarJugador(&doc,escenario->protagonista);
		if(!jugador)return NULL;
		escenario->jugador=jugador;
		juego = new Juego(pantalla, configuracion, escenario, tipos);
	} catch(YAML::Exception& e) {
			juego = new Juego();
			//LOG_WARNING << "Problemas para abrir el archivo" << e.what();
	}
   return juego;
}
Juego* Yaml::readServer(){
		Juego* juego;
		//remove( "Log.txt" );
		//plog::init(plog::warning, "Log.txt");

		try {
			std::ifstream fin(config_filepath);
			YAML::Parser parser(fin);
			YAML::Node doc;
			parser.GetNextDocument(doc);

			ConfiguracionJuego_t conf;
			Pantalla* pantalla;
			Configuracion* configuracion;
			Escenario* escenario;
			Personaje* protagonista;

			pantalla=cargarPantalla(conf,&doc);
			configuracion=cargarConfiguracion(conf,&doc);
			const YAML::Node *pTipos = doc.FindValue(tag_tipos);
			if (pTipos) {
				for (unsigned i = 0; i < (*pTipos).size(); i++) {
					cargarObjetoMapa(pTipos);
				}
			} else {
			//LOG_WARNING << "No se define ningun tipo";
		}

		if (const YAML::Node *pEscenario = doc.FindValue(tag_escenario)) {
			escenario = cargarEscenario(conf, pEscenario);
			if (const YAML::Node *pEntidades = (*pEscenario).FindValue(
					tag_escenario_entidades)) {
				for (unsigned i = 0; i < (*pEntidades).size(); i++) {
					Entidad* ent = cargarEntidad(pEntidades);
					if (ent != NULL) {
						entidades.push_back(ent);
						cantidad_de_entidades++;

					}
				}
				escenario->entidades = entidades;

				}else {
					//LOG_WARNING << "No hay entidades a instanciar inicialmente";
				}
			} else {
				escenario = new Escenario();
			}

			juego = new Juego(pantalla, configuracion, escenario, tipos);
		} catch(YAML::Exception& e) {
				juego = new Juego();
				//LOG_WARNING << "Problemas para abrir el archivo" << e.what();
		}
	   return juego;
}
Juego* Yaml::readCliente() {
	Juego* juego;
	//remove( "Log.txt" );
	//plog::init(plog::warning, "Log.txt");

	try {
		std::ifstream fin(config_filepath);
		YAML::Parser parser(fin);
		YAML::Node doc;
		parser.GetNextDocument(doc);

		ConfiguracionJuego_t conf;
		Pantalla* pantalla;
		Configuracion* configuracion;
		Escenario* escenario;
		Personaje* protagonista;

		pantalla = cargarPantalla(conf, &doc);
		const YAML::Node *pTipos = doc.FindValue(tag_tipos);
		if (pTipos) {
			for (unsigned i = 0; i < (*pTipos).size(); i++) {
				cargarObjetoMapa(pTipos);
			}
		} else {
			//LOG_WARNING << "No se define ningun tipo";
		}
		protagonista = cargarPersonaje(conf, &doc);
		configuracion = new Configuracion();
		escenario = new Escenario();
		escenario->protagonista=protagonista;

		Jugador* jugador=cargarJugador(&doc,escenario->protagonista);
		if(!jugador)return NULL;
		escenario->jugador=jugador;
		juego = new Juego(pantalla, configuracion, escenario, tipos);
	} catch (YAML::Exception& e) {
		juego = new Juego();
		//LOG_WARNING << "Problemas para abrir el archivo" << e.what();
	}
	return juego;

}

Yaml::~Yaml() {
}

