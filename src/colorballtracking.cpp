/*
 * colorballtracking.cpp
 *
 *  Created on: Jul 12, 2014
 *      Author: Yan Liu and Colten Normore
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
using namespace cv;
using namespace std;

const int Frame_Width = 640;
const int Frame_Height = 480;
void filterSmallContours(vector<vector<Point> > const &contours, vector<vector<Point> > &largeContours, int maxArea);
string integerToString(int num);
void trackObject(Mat src, Mat &dest){
	Rect boundRect;
	int largestObj = 0;
	vector< vector<Point> > contoursSet;
	vector<vector<Point> > contours;//store all the contours
	vector<Vec4i> hierarchy;
	double area = 0;
	double maxArea = 0;
    findContours(src, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    filterSmallContours(contoursSet,contours,2000);
    for( unsigned int i = 0; i < contours.size(); i++){
    	Mat tempContour = Mat(contours[i]);
    	area = contourArea(tempContour);
    	if( area > maxArea){
    		maxArea = area;
    		largestObj = i;
    		boundRect = boundingRect( contours[largestObj] );
    	}
    }
	//draw the boundary of the object
	drawContours(dest, contours, largestObj, Scalar(0, 0, 255), 3, 8, hierarchy);

	rectangle( dest, boundRect, Scalar(0, 0, 255), 2, 8, 0 );
    Moments moment = moments(src,true);
    int centerX = moment.m10 / moment.m00;
    int centerY = moment.m01 / moment.m00;
    Point centerPoint(centerX, centerY);
    Point printPoint(centerX, centerY -10);
    circle(dest,centerPoint, 8, Scalar(255,0,0), CV_FILLED);
    putText(dest,integerToString(centerX) + "," + integerToString(centerY), printPoint,1,1,Scalar(0,255,0),1,8,false);
}

string integerToString( int num ){
	stringstream strings;
	strings << num;
	string s = strings.str();
	return s;
}

void filterSmallContours(vector<vector<Point> > const &contours, vector<vector<Point> > &largeContours, int maxArea){
	int size = contours.size();
	for (int i=0; i < size; i++){
		//if there is a large contour, add it to the vector of large contours
		if (cv::contourArea(contours[i]) > maxArea){
			largeContours.push_back(contours[i]);
		}
	}
}

int main(){
	VideoCapture stream1;
	//default the capture frame size to the certain size
	stream1.set(CV_CAP_PROP_FRAME_WIDTH, Frame_Width);
	stream1.set(CV_CAP_PROP_FRAME_HEIGHT, Frame_Height);
	stream1.open(0);

	if (!stream1.isOpened()) {
		cout << "cannot open camera";
	}

	while(true){

		//initialization of various frames
		Mat cameraFrame, blurFrame, threshold1,threshold2, closedFrame, hsvFrame,colorObjectFrame,thresholdFrame;
		//get image from stream
		stream1.read(cameraFrame);

		//switch the RGB to HSV space
		cv::cvtColor(cameraFrame, hsvFrame,CV_BGR2HSV);

		//select the pixels that may be linked to the part of the color object
		//using the pixelsSelect tool to determine the HSV value
		//**********Testing for the Orange************
		cv::inRange(hsvFrame, Scalar(18,1,193), Scalar(44,179,256), threshold1);
		cv::inRange(hsvFrame, Scalar(193,1,193), Scalar(256,179,256), threshold2);
		cv::bitwise_or(threshold1,threshold2,thresholdFrame);

		//blur image to remove basic imperfections
		medianBlur(thresholdFrame, thresholdFrame, 7);

		//Closing operation
		Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(7, 7), Point(3,3));
		cv::morphologyEx(thresholdFrame, closedFrame, MORPH_CLOSE, element);
	    //find the boundary of the object
		trackObject(closedFrame, cameraFrame);


		//show image
		imshow("ColorBallTracking", cameraFrame);

		if (waitKey(10) >= 0) break;
	}
	return 0;
}


