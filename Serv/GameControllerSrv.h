/*
 * GameControllerSrv.h
 *
 *  Created on: 21/10/2015
 *      Author: ger
 */

#ifndef GAMECONTROLLERSRV_H_
#define GAMECONTROLLERSRV_H_

#include <string>
using namespace std;

class GameControllerSrv {
public:
	GameControllerSrv();
	string getClientName();
	virtual ~GameControllerSrv();
};

#endif /* GAMECONTROLLERSRV_H_ */
