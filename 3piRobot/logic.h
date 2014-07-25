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
#include <opencv\highgui.h>
#include <opencv\cv.h>

class logic {
public:
	logic(controllerInterface *, visionInterface *);
	virtual ~logic();

	void runLogic();
	void separateRegion(cv::Mat const & src, cv::Mat & dest);
	bool separateR1(int R, int G, int B);
	void getContours(cv::Mat const & src, cv::Mat & dest, std::vector<std::vector<cv::Point> > & contours);

private:
	controllerInterface* c_module;
	visionInterface* v_module;
};

#endif /* LOGIC_H_ */
