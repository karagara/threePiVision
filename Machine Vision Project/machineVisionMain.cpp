/*********************************************************************
 *  machineVisionMain.cpp
 *  Machine Vision Project - Hand Gesture control web browser
 *
 *  Created on: June 16th, 2014
 *  Submitted by:  Yan Liu
 *                 Colten Normore
 *  Version:       1.0.0
 *  This project can be Shared Everywhere
 *********************************************************************/

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;
//initialize the frame size
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
#define PI 3.14159265

//declare the local functions
void morphologicalImgProc(Mat &frame);
string integerToString(int num);
int angleToCenter(const Point &v1, const Point &v2);
String doAction(int totalAngleOfFinger, int fingerSize);
void sendResult(String msg);


//calculate the angle between two points
int angleToCenter(const Point &finger, const Point &center) {
	float y_angle = center.y - finger.y; //center = 1;
	float x_angle = finger.x - center.x;// tip =2;
	float theta = atan(y_angle/ x_angle);
	int angleFinger = (int) round( theta * 180 / PI);
	return angleFinger;
}

//convert the integer to string
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

//the important function to track the hand, the algorithm is described in the report
void trackHand(Mat src, Mat &dest) {
	//initialization local variables
	Rect boundRect;
	int largestObj;
	vector<vector<Point> > contours; //store all the contours
	vector<vector<Point> > contoursSet(contours.size());//store large contours
	vector<Vec4i> hierarchy;
	vector<Point> convexHullPoint;
	vector<Point> fingerPoint;
	Point centerP;
	int numObjects = 0;
	double area = 0;
	double maxArea = 0;
	bool handFound = false;
	//find all the contours in the threshold Frame
	findContours(src, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	numObjects = hierarchy.size();
	for (unsigned int i = 0; i < contours.size(); i++) {
		Mat tempContour = Mat(contours[i]);
		area = contourArea(tempContour);
		if (area > maxArea) {
			maxArea = area;
			largestObj = i;
		}
	}
	if( maxArea > 4000){
	handFound = true;
	boundRect = boundingRect(contours[largestObj]);
	//draw the boundary of the object
	drawContours(dest, contours, largestObj, Scalar(0, 0, 255), 3, 8,hierarchy);
	//find the convex points for the largest object which is hand
	convexHull(contours[largestObj], convexHullPoint, true, true);
	//use moment method to find the center point
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
	if (handFound) {
		int countHullPoint = convexHullPoint.size();
		int maxdist = 0;
		int pos = 0;
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

		//get the size the fingers, and calculate the total angle of these fingers
		int countFinger = fingerPoint.size();
		int angle = 0;
		String resultMsg;
		if( countFinger <= 5){
			for ( int x = 0; x < countFinger; x++){
				angle = angle + abs (angleToCenter(fingerPoint[x], centerP) );
			}
		}
		//cout << angle << endl;
		resultMsg = doAction( angle, countFinger);
		sendResult(resultMsg);
		putText(dest, integerToString(countFinger), printPoint, 1, 5, Scalar(0, 255, 0), 1, 5, false);
	}
}
}

////send out the result signal
void sendResult(String msg){
	cout << "Command: " << msg << endl;
}

//action performed based on the number of fingers and the total angle
//1.  5 fingers && total angle: 270 - 285
//2.  4 fingers && total angle: 240 - 255
//3.  3 fingers && total angle: 190 - 210
//4.  2 fingers && total angle: 120 - 130
//5.  1 finger && total angle:  65 - 75
String doAction(const int totalAngleOfFinger, const int fingerSize){
	String result = "";
	if( totalAngleOfFinger>= 270 && totalAngleOfFinger <= 285 && (fingerSize == 5  ))
		result = "Maximum the Page";
	else if( totalAngleOfFinger >= 240 && totalAngleOfFinger <= 255 && fingerSize == 4)
		result = "Minimum the Page";
	else if( totalAngleOfFinger >= 190 && totalAngleOfFinger <= 210 && fingerSize == 3)
		result = "Go back the page";
	else if( totalAngleOfFinger >= 120 && totalAngleOfFinger <= 130 &&  (fingerSize == 2 ))
		result = "Reload the Page";
	else if( totalAngleOfFinger >= 65 && totalAngleOfFinger <= 75 &&  (fingerSize == 1 ) )
		result = "Closing the Page";
	return result;
}


int main() {
	//initialization of local frames
	Mat cameraFrame, blurFrame, threshold1, threshold2, closedFrame, hsvFrame, colorObjectFrame, thresholdFrame;
	VideoCapture stream1;

	//default the capture frame size to the certain size & open the camera
	stream1.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	stream1.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	stream1.open(0);

	//report the error if the camera not connected properly
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
		cv::inRange(hsvFrame, Scalar(58, 58, 95), Scalar(133, 154, 256),thresholdFrame);

		//Tracking actual hand in restricted background
		//cv::inRange(hsvFrame, Scalar(96, 23, 123), Scalar(256, 100, 256),thresholdFrame);

		//blur image to remove basic imperfections
		medianBlur(thresholdFrame, thresholdFrame, 5);

		//do the morphological image processing
		//closing the frame
		morphologicalImgProc(thresholdFrame);

		//track the hand, put the bounding box around the hand
		//calculate the center point of the hand
		trackHand(thresholdFrame, cameraFrame);

		namedWindow("Hand_Gesture_Detection");
		imshow("Hand_Gesture_Detection", cameraFrame);

		if (waitKey(10) >= 0)
			break;
		//release the memory
		cameraFrame.release();
	}
	return 0;

}
