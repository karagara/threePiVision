/*
 * vision.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#ifndef VISION_H_
#define VISION_H_

#include "visionInterface.h"
//#include <opencv2/imgproc/imgproc.hpp>

class vision : public visionInterface {
public:
	//constructors
	vision();
	virtual ~vision();

	cv::Mat getFrame();

private:
	cv::VideoCapture* vCapture;
};

#endif /* VISION_H_ */
