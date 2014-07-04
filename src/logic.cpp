/*
 * logic.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#include "logic.h"
#include <iostream>

logic::logic(controllerInterface *controller, visionInterface *vision) {
	this->c_module = controller;
	this->v_module = vision;
}

logic::~logic() {
	// TODO Auto-generated destructor stub
}

void logic::runLogic(){
	while (true) {
		//get image from stream
		cv::Mat cameraFrame;
		cv::Mat blurFrame;
		cameraFrame=v_module->getFrame();

		//do a basic blur
	    cv::medianBlur (cameraFrame, blurFrame, 5);

		//show image
		imshow("cam", blurFrame);
		if (cv::waitKey(30) >= 0) break;
	}
}
