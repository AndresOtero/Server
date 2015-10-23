/*
 * Yaml.cpp
 *
 *  Created on: 12 de set. de 2015
 *      Author: andres
 */

#include "Yaml.h"
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
	config_filepath =path;
}
Yaml::Yaml() {
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

typedef struct {
	int x;
	int y;
	std::string tipo;
} Entidad_t;

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

typedef struct  {
   std::string nombre;
   std::string imagen;
   int ancho_base;
   int alto_base;
   int pixel_ref_x ;
   int pixel_ref_y ;
   int fps;
   int delay;
} Objeto_mapa_t;

typedef struct {
	Pantalla_t pantalla;
	Configuracion_t configuracion;
	Escenario_t escenario;

}ConfiguracionJuego_t;

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

Juego* Yaml::read() {
	Juego* juego;
	//remove( "Log.txt" );
	plog::init(plog::warning, "Log.txt");

	try {
		std::ifstream fin(config_filepath);
		YAML::Parser parser(fin);
		YAML::Node doc;
		parser.GetNextDocument(doc);

		ConfiguracionJuego_t conf;
		Pantalla* pantalla;
		Configuracion* configuracion;
		std::map<std::string, ObjetoMapa*> tipos;
		Escenario* escenario;
		Personaje* protagonista;

		if (const YAML::Node *pPantalla = doc.FindValue(tag_pantalla)) {
			if (const YAML::Node *pPantallaAncho = (*pPantalla).FindValue(
					tag_pantalla_ancho)) {
				(*pPantallaAncho) >> conf.pantalla.ancho;
				if (const YAML::Node *pPantallaAlto = (*pPantalla).FindValue(
						tag_pantalla_alto)) {
					(*pPantallaAlto) >> conf.pantalla.alto;
					pantalla = new Pantalla(conf.pantalla.ancho,
							conf.pantalla.alto);
				} else {
					LOG_WARNING
							<< "Se define configuracion de pantalla para el ancho pero no para el alto";
					pantalla = new Pantalla();
				}
			} else {
				//log conf pantalla sin ancho
				LOG_WARNING
						<< "Se define configuracion de pantalla pero no su ancho";
				pantalla = new Pantalla();
			}

		} else {
			// log no tiene pantalla
			LOG_WARNING
					<< "No se define configuracion de pantalla, se usa Default 1024x728";
			pantalla = new Pantalla();
		}

		if (const YAML::Node *pConfiguracion = doc.FindValue(tag_config)) {
			if (const YAML::Node *pConfigVel = (*pConfiguracion).FindValue(
					tag_config_vel_personaje)) {
				(*pConfigVel) >> conf.configuracion.vel_personaje;
				if (const YAML::Node *pConfigMargen =
						(*pConfiguracion).FindValue(tag_config_margen_scroll)) {
					(*pConfigMargen) >> conf.configuracion.margen_scroll;
					configuracion = new Configuracion(
							conf.configuracion.vel_personaje,
							conf.configuracion.margen_scroll);
				} else {
					LOG_WARNING << "No se define el margen para el scroll";
					configuracion = new Configuracion();
				}
			} else {
				LOG_WARNING << "No se define el velocidad del personaje";
				configuracion = new Configuracion();
			}
		} else {
			LOG_WARNING
					<< "No se define configuracion para el margen del scroll y velocidad del personaje";
			configuracion = new Configuracion();
		}
		if (const YAML::Node *pTipos = doc.FindValue(tag_tipos)) {
			for (unsigned i = 0; i < (*pTipos).size(); i++) {
				Objeto_mapa_t tipo;
				if (const YAML::Node *pTipoNombre = ((*pTipos)[i]).FindValue(
						tag_tipos_nombre)) {
					(*pTipoNombre) >> tipo.nombre;

					if (const YAML::Node *pTipoImagen =
							((*pTipos)[i]).FindValue(tag_tipos_imagen)) {
						*pTipoImagen >> tipo.imagen;

						ObjetoMapa* objeto = new ObjetoMapa(tipo.nombre,
								tipo.imagen);

						if (const YAML::Node *pTipoAnchoBase =
								((*pTipos)[i]).FindValue(
										tag_tipos_ancho_base)) {
							*pTipoAnchoBase >> tipo.ancho_base;
							if (const YAML::Node *pAltoBase =
									((*pTipos)[i]).FindValue(
											tag_tipos_alto_base)) {
								*pAltoBase >> tipo.alto_base;
								objeto->baseLogica->ancho = tipo.ancho_base;
								objeto->baseLogica->alto = tipo.alto_base;
							} else {
								LOG_WARNING
										<< "Se define el ancho pero no el alto para la base logica";
							}
						}

						if (const YAML::Node *pPixelRefX =
								((*pTipos)[i]).FindValue(
										tag_tipos_pixel_ref_x)) {
							*pPixelRefX >> tipo.pixel_ref_x;
							if (const YAML::Node *pPixelRefY =
									((*pTipos)[i]).FindValue(
											tag_tipos_pixel_ref_y)) {
								*pPixelRefY >> tipo.pixel_ref_y;
								objeto->pixelsReferencia->x = tipo.pixel_ref_x;
								objeto->pixelsReferencia->y = tipo.pixel_ref_y;
							} else {
								LOG_WARNING
										<< "Se define el pixel de referencia x pero no el pixel de referencia y";
							}
						}
						if (const YAML::Node *pFps = ((*pTipos)[i]).FindValue(
								tag_tipos_fps)) {
							*pFps >> tipo.fps;
							objeto->fps = tipo.fps;
						}
						if (const YAML::Node *pDelay = ((*pTipos)[i]).FindValue(
								tag_tipos_delay)) {
							*pDelay >> tipo.delay;
							objeto->delay = tipo.delay;
						}
						tipos[tipo.nombre] = objeto;
					} else {
						LOG_WARNING
								<< "No se define la ruta de imagen para el tipo";
					}
				} else {
					LOG_WARNING << "No se define nombre para el tipo";
				}
			}
		} else {
			LOG_WARNING << "No se define ningun tipo";
		}
		if (const YAML::Node *pEscenario = doc.FindValue(tag_escenario)) {
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
					LOG_WARNING
							<< "Se define el tamaño en x del escenario pero no el y ";
					escenario = new Escenario();
				}
			} else {
				LOG_WARNING << "No se define el tamaño del escenario";
				escenario = new Escenario();
			}
			bool tieneProt = false;
			if (const YAML::Node *pPersonaje = (*pEscenario).FindValue(
					tag_escenario_protagonista)) {
				if (const YAML::Node *pTipo = (*pPersonaje)[0].FindValue(
						tag_escenario_protagonista_tipo)) {
					(*pTipo) >> conf.escenario.protagonista.tipo;
					if (const YAML::Node *pX = (*pPersonaje)[0].FindValue(
							tag_escenario_protagonista_x)) {
						(*pX) >> conf.escenario.protagonista.x;
						if (const YAML::Node *pY = (*pPersonaje)[0].FindValue(
								tag_escenario_protagonista_y)) {
							(*pY) >> conf.escenario.protagonista.y;
							if (ObjetoMapa* obj =
									tipos[conf.escenario.protagonista.tipo]) {
								protagonista = new Personaje(obj,
										conf.escenario.protagonista.x,
										conf.escenario.protagonista.y);
								escenario->protagonista = protagonista;
								tieneProt = true;
							} else {
								LOG_WARNING << "No existe el tipo '"
										<< conf.escenario.protagonista.tipo
										<< "' definido para el protagonista";
							}
						} else {
							LOG_WARNING
									<< "No se define la posicion y del protagonista";
						}
					} else {
						LOG_WARNING
								<< "La posicion x del protagonista no es valida";
					}
				} else {
					LOG_WARNING
							<< "No se define una posicion inicial para el protagonista";
				}
			} else {
				LOG_WARNING << "No se define un protagonista para el escenario";
			}
			if (tieneProt == false) {
				protagonista = new Personaje();
				escenario->protagonista = protagonista;

			}
			if (const YAML::Node *pEntidades = (*pEscenario).FindValue(
					tag_escenario_entidades)) {
				for (unsigned i = 0; i < (*pEntidades).size(); i++) {
					Entidad_t entidad;
					Entidad* ent;
					if (const YAML::Node *pX = ((*pEntidades)[i]).FindValue(
							tag_escenario_entidades_x)) {
						(*pX) >> entidad.x;
						if (const YAML::Node *pY = ((*pEntidades)[i]).FindValue(
								tag_escenario_entidades_y)) {
							(*pY) >> entidad.y;
							if (const YAML::Node *pTipo =
									((*pEntidades)[i]).FindValue(
											tag_escenario_entidades_tipo)) {
								(*pTipo) >> entidad.tipo;
								if (ObjetoMapa* obj = tipos[entidad.tipo]) {
									ent = elegirEntidad(obj, entidad);
									escenario->entidades.push_back(ent);
								} else {
									LOG_WARNING << "No existe el tipo "
											<< entidad.tipo
											<< " especificado por la entidad";
								}
							} else {
								LOG_WARNING
										<< "No se especifica el tipo de la entidad";
							}
						} else {
							LOG_WARNING
									<< "Se define la posicion inicial x de la entidad pero no la posicion y";
						}
					} else {
						LOG_WARNING << "No se define la posicion de la entidad";
					}
				}
			} else {
				LOG_WARNING << "No hay entidades a instanciar inicialmente";
			}

		} else {
			escenario = new Escenario();
		}

		juego = new Juego(pantalla, configuracion, escenario, tipos);

	   /*std:: cout << "Pantalla: " << juego->pantalla->getAncho() << " " << juego->pantalla->getAlto()  << "\n";

	   std:: cout << "Configuracion: " << juego->conf->get_vel_personaje() << " " << juego->conf->get_margen_scroll() << "\n";

	   ObjetoMapa*  tipo = juego->tipos["tierra"];
	   std:: cout << "Tipo" << " " << tipo->nombre << " " << tipo->imagen << tipo->baseLogica->ancho << " "<< tipo->baseLogica->alto << " "
			 << tipo->pixelsReferencia->x << " "<< tipo->pixelsReferencia->y << "\n"  ;

	   std:: cout << "Escenario:  " << juego->escenario->nombre << " " << juego->escenario->size_x << " " << juego->escenario->size_y << "\n"  ;
	   for(unsigned i=0;i< juego->escenario->entidades.size();i++) {
			 Entidad* entidad = juego->escenario->entidades[i];
			 std:: cout << "    Entidad" << i  << " " << entidad->posicion->x << " " << entidad->posicion->y << " " <<entidad->objetoMapa->nombre << "\n"  ;
	   }

	   std::cout << "    Protagonista: " << juego->escenario->protagonista->objetoMapaAnimado->nombre << " " <<juego->escenario->protagonista->objetoMapaAnimado->fps <<" " << juego->escenario->protagonista->posicion->x << " " << juego->escenario->protagonista->posicion->y << "\n"  ;
	   std::cout << "end\n";*/


	} catch(YAML::Exception& e) {
			juego = new Juego();
			LOG_WARNING << "Problemas para abrir el archivo" << e.what();
	}
   return juego;
}

Yaml::~Yaml() {
}
