/*
 * controller.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "controllerInterface.h"

class controller: public controllerInterface {
public:
	controller();
	virtual ~controller();

	void moveWheels(int,int);
};

#endif /* CONTROLLER_H_ */
