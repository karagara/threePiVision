/*
 * logic.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 */

#include "logic.h"
#include <iostream>
#include <cmath>

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
		cv::Mat cameraFrame,bwFrame,finalFrame;
		cameraFrame=v_module->getFrame();

		//do a basic blur
	    cv::medianBlur(cameraFrame, finalFrame, 5);

	    //threshold region based on rule separateR1
	    separateRegion(finalFrame, finalFrame);

	    //close frame
	    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7), cv::Point(3,3));
	    cv::morphologyEx(finalFrame, finalFrame, cv::MORPH_CLOSE, element);

	    //get contours
	    cv::cvtColor(finalFrame, finalFrame, CV_RGB2GRAY);
	    std::vector<std::vector<cv::Point> > contours;
	    getContours(finalFrame, cameraFrame, contours);

	    //get bounding box and COM
	    int com_x, com_y, bbox_y, bbox_len;
	    if(contours.size() > 0){
		    cv::Rect bBox;
		    cv::Moments moment;
	    	bBox = cv::boundingRect(contours[0]);
	    	moment = cv::moments(contours[0]);
			com_x = moment.m10 / moment.m00;
			com_y = moment.m01 / moment.m00;
			bbox_y = bBox.y;
			bbox_len = bBox.height;

	    	cv::rectangle(cameraFrame,bBox,cv::Scalar(0, 255, 0));
	    	cv::circle(cameraFrame, cv::Point(com_x, com_y), 30, cv::Scalar(255, 0, 0), 2);
	    }
	    cv::Size s = cameraFrame.size();
	    int desired_x = s.width/2;
	    int desired_len = 200;
	    int deadspot = 10;
	    int l_motor = 0, r_motor = 0;
	    if (desired_len > (bbox_len - deadspot)){
	    	//move forward
	    	l_motor = 30;
	    	r_motor = 30;
	    }else if((desired_len < (bbox_len + deadspot))){
	    	l_motor = -30;
	    	r_motor = -30;
	    }


		//show image
		cv::imshow("cam", cameraFrame);
		if (cv::waitKey(30) >= 0) break;
	}
}

void logic::getContours(cv::Mat const &src, cv::Mat &dest, std::vector<std::vector<cv::Point> > & contours){
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(src,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	cv::drawContours(dest,contours,-1,cv::Scalar(0, 0, 255), 3, 8, hierarchy);
}

bool logic::separateR1(int R, int G, int B){
	double rgRatio = (double)R/(double)G;
	bool isSkin = false;
	if ( rgRatio > 1.05 && rgRatio < 4.0){
		isSkin = true;
	}
	return isSkin;
}

void logic::separateRegion(cv::Mat const &src, cv::Mat &dest){
	dest = src.clone();

	    cv::Vec3b cwhite = cv::Vec3b::all(255);
	    cv::Vec3b cblack = cv::Vec3b::all(0);
	    for(int i = 0; i < src.rows; i++) {
	        for(int j = 0; j < src.cols; j++) {
	        	cv::Vec3b pix_bgr = src.ptr<cv::Vec3b>(i)[j];
	            int B = pix_bgr.val[0];
	            int G = pix_bgr.val[1];
	            int R = pix_bgr.val[2];
	            // apply rgb rule
	            bool a = separateR1(R,G,B);

	            //fill based on rule
	            if(a)
	            	dest.ptr<cv::Vec3b>(i)[j] = cwhite;
	            else
	            	dest.ptr<cv::Vec3b>(i)[j] = cblack;
	        }
	    }
}


