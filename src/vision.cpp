/*
 * vision.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#include "vision.h"

vision::vision() {
	this->vCapture = new cv::VideoCapture(0);
}

vision::~vision() {
	// TODO Auto-generated destructor stub
}

cv::Mat vision::getFrame(){
	cv::Mat img;
	vCapture->read(img);
	return img;
}
