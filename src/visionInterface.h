/*
 * visionInterface.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#ifndef VISIONINTERFACE_H_
#define VISIONINTERFACE_H_

#include <opencv2/imgproc/imgproc.hpp>

class visionInterface{
	public:
		visionInterface();
		virtual ~visionInterface();

		virtual cv::Mat getFrame()=0;
};




#endif /* VISIONINTERFACE_H_ */
