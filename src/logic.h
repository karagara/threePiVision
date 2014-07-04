/*
 * logic.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#ifndef LOGIC_H_
#define LOGIC_H_

#include "controllerInterface.h"
#include "visionInterface.h"

class logic {
public:
	logic(controllerInterface *, visionInterface *);
	virtual ~logic();

	void runLogic();

private:
	controllerInterface* c_module;
	visionInterface* v_module;
};

#endif /* LOGIC_H_ */
