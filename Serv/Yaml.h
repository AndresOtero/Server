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

#ifndef YAML_H_
#define YAML_H_

class Yaml {
	string config_filepath;
public:
	Yaml();
	Yaml(string path);
	Juego* read();
	virtual ~Yaml();
};

#endif /* YAML_H_ */
