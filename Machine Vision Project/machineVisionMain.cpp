/*
 * machineVisionMain.cpp
 *
 *  Created on: July 16th, 2014
 *      Author: Yan and Colten
 */

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "opencv2/core/core.hpp"
#include <opencv2/video/background_segm.hpp>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
#define PI 3.14159265

//declare the functions
//void filterSmallContours(vector<vector<Point> > const &contours, vector<vector<Point> > &largeContours, int maxArea);
void morphologicalImgProc(Mat &frame);
string integerToString(int num);
int angleToCenter(const Point &v1, const Point &v2);
void doAction(int totalAngleOfFinger, int fingerSize);

//calculate the angle between two points
int angleToCenter(const Point &finger, const Point &center) {
	float y_angle = center.y - finger.y; //center = 1;
	float x_angle = finger.x - center.x;// tip =2;
	float theta = atan(y_angle/ x_angle);
	int angleFinger = (int) round( theta * 180 / PI);
	return angleFinger;
}

//the function that convert the integer to string
//return a string value
string integerToString(int num) {
	stringstream strings;
	strings << num;
	string s = strings.str();
	return s;
}

//morphological Image processing
//Erosion -> dilation -> closing the frame ensure to get the better performance
void morphologicalImgProc(Mat &frame) {
	Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(9, 9), Point(5, 5));
	Mat element1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(7, 7), Point(5, 5));
	cv::dilate(frame, frame, element);
	cv::erode(frame, frame, element);
	cv::morphologyEx(frame, frame, MORPH_OPEN, element);
	cv::morphologyEx(frame, frame, MORPH_CLOSE, element);
}

void trackHand(Mat src, Mat &dest) {
	//variables init
	//bool handFound;
	Rect boundRect;
	int largestObj;
	vector<vector<Point> > contours; //store all the contours
	vector<vector<Point> > contoursSet(contours.size());//store large contours
	vector<Vec4i> hierarchy;
	vector<Point> convexHullPoint;

	vector<Point> fingerPoint;
	Point centerP;
	int numObjects = 0;
	bool flag = false;
	double area = 0;
	double maxArea = 0;
	bool handFound = false;

	findContours(src, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	vector<vector<int> > hull( contours.size());
	vector<vector<Vec4i> > defects ( contours.size());
	vector<vector<Point> > defectPoint( contours.size());

	numObjects = hierarchy.size();
	for (unsigned int i = 0; i < contours.size(); i++) {
		Mat tempContour = Mat(contours[i]);
		area = contourArea(tempContour);
		if (area > maxArea) {
			maxArea = area;
			largestObj = i;
			//handFound = true;
		}
	}
	if( maxArea > 4000){
	handFound = true;
	boundRect = boundingRect(contours[largestObj]);
	//filterSmallContours(contours, contoursSet, maxArea);

	//draw the boundary of the object

	drawContours(dest, contours, largestObj, Scalar(0, 0, 255), 3, 8,hierarchy);

	convexHull(contours[largestObj], convexHullPoint, true, true);
	//approxPolyDP( Mat(contoursSet[largestObj]), contoursSet[largestObj], 3, true );
	convexHull(contours[largestObj], hull[largestObj], false );
    convexityDefects(contours[largestObj], hull[largestObj], defects[largestObj]);
    //cout << defects[largestObj].size() <<endl;
	Moments moment = moments(Mat(contours[largestObj]), true);
	int centerX = moment.m10 / moment.m00;
	int centerY = moment.m01 / moment.m00;
	Point centerPoint(centerX, centerY);
	centerP = centerPoint;
	Point printPoint(centerX, centerY + 15);
	Point printPoint1(boundRect.x, boundRect.y);
	circle(dest, centerPoint, 8, Scalar(255, 0, 0), CV_FILLED);
	//put the BoundingBox in the contour region
	rectangle(dest, boundRect, Scalar(0, 0, 255), 2, 8, 0);
	int rectHeight = boundRect.height;
	int rectWidth = boundRect.width;
	int maxXpoint = boundRect.x + boundRect.width;
	int maxYpoint = boundRect.y + boundRect.height;
	//print the center point position in the CoG(center of Grativity)
	//putText(dest, integerToString(temp1) + "," + integerToString(temp2), printPoint, 1, 1, Scalar(0, 255, 0), 1, 8, false);

	//working without show the figure tips
	if (handFound) {

		int countHullPoint = convexHullPoint.size();
		int maxdist = 0;
		int pos = 0;
	    int tempDist;
		bool flag;
		for (int j = 1; j < countHullPoint; j++) {
			pos = j;
			if (centerP.y >= convexHullPoint[j].y && centerP.y >= convexHullPoint[pos].y) {
				pos = j;
				int dist = (centerP.x - convexHullPoint[j].x)^ 2 + (centerP.y - convexHullPoint[j].y) ^ 2;
				if (  abs(convexHullPoint[j-1].x - convexHullPoint[j].x) < 12){
					if ( dist > maxdist){
						maxdist = dist;
						pos = j;
					}
				}
				else if( j == 0 || abs(convexHullPoint[j-1].x - convexHullPoint[j].x) >= 12 ){
					fingerPoint.push_back(convexHullPoint[pos]);
					cv::line(dest,centerP, convexHullPoint[pos],Scalar(0, 255, 0), 3, CV_AA, 0);
					circle(dest, convexHullPoint[pos], 8, Scalar(255, 0, 0), CV_FILLED);
					pos = j;
				}
			}
		}

		int countFinger = fingerPoint.size();
		int angle;
		if( countFinger <= 5){
			for ( int x = 0; x < countFinger; x++){
				angle = angle + abs (angleToCenter(fingerPoint[x], centerP) );
			}
		}

		doAction( angle, countFinger );
		//cout << angle << endl;
		putText(dest, integerToString(countFinger), printPoint, 1, 5, Scalar(0, 255, 0), 1, 5, false);

	}
}
}

//action performed based on the number of fingers and the total angle
//1.  5 fingers && total angle: 270 - 285
//2.  4 fingers && total angle: 240 - 255
//3.  3 fingers && total angle: 190 - 210
//4.  2 fingers && total angle: 120 - 130
//5.  1 finger && total angle:  65 - 75
void doAction(int totalAngleOfFinger, int fingerSize){
	if( totalAngleOfFinger>= 270 && totalAngleOfFinger <= 285 && (fingerSize == 5  ))
		cout << "Maximum the Page" << endl;
	else if( totalAngleOfFinger >= 240 && totalAngleOfFinger <= 255 && fingerSize == 4)
		cout << "Minimum the Page" << endl;
	else if( totalAngleOfFinger >= 190 && totalAngleOfFinger <= 210 && fingerSize == 3)
		cout << "Go back the page" << endl;
	else if( totalAngleOfFinger >= 120 && totalAngleOfFinger <= 130 &&  (fingerSize == 2 ))
		cout << "Reload the Page" << endl;
	else if( totalAngleOfFinger >= 65 && totalAngleOfFinger <= 75 &&  (fingerSize == 1 ) )
		cout << "Closing the Page" << endl;
	else if( totalAngleOfFinger == 0 )
		cout << endl;
	else
		cout << endl;
}


int main() {
	//initialization of various frames
	Mat cameraFrame, blurFrame, threshold1, threshold2, closedFrame,
			hsvFrame, colorObjectFrame, thresholdFrame;
	VideoCapture stream1;

	//default the capture frame size to the certain size
	stream1.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	stream1.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	stream1.open(0);
	if (!stream1.isOpened()) {
		cout << "cannot open camera";
	}

	while (true) {
		//get image from stream
		stream1.read(cameraFrame);

		//switch the RGB to HSV space
		cv::cvtColor(cameraFrame, hsvFrame, CV_BGR2HSV);

		//testing in the blue glove on hand
		//need to adjust before the live demo
		//cv::inRange(hsvFrame, Scalar(58, 58, 95), Scalar(133, 154, 256),thresholdFrame);

		//Tracking actual hand in restricted background
		cv::inRange(hsvFrame, Scalar(96, 23, 123), Scalar(256, 100, 256),thresholdFrame);

		//blur image to remove basic imperfections
		medianBlur(thresholdFrame, thresholdFrame, 5);

		//do the morphological image processing
		//closing the frame
		morphologicalImgProc(thresholdFrame);

		//track the hand, put the bounding box around the hand
		//calculate the center point of the hand
		trackHand(thresholdFrame, cameraFrame);

		namedWindow("Hand_Detection");
		imshow("Hand_Detection", cameraFrame);
		if (waitKey(10) >= 0)
			break;
		cameraFrame.release();

	}
	return 0;

}
