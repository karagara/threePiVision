/*
 * handDetection.cpp
 *
 *  Created on: July 16th, 2014
 *      Author: Yan and Colten
 */

#include <opencv\cv.h>
#include <opencv\highgui.h>
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

//filter the small object that appears in the frame for certain area
//void filterSmallContours(vector<vector<Point> > const &contours,
//		vector<vector<Point> > &largeContours, int maxArea) {
//	int size = contours.size();
//	for (int i = 0; i < size; i++) {
//		//if there is a large contour, add it to the vector of large contours
//		if (cv::contourArea(contours[i]) > maxArea) {
//			largeContours.push_back(contours[i]);
//		}
//	}
//}

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
	int largestObj = 0;
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
			handFound = true;
		}
	}
	if( maxArea > 5000){
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
		//int count = 0;
		//bool flag = false;
		for (int j = 0; j < countHullPoint; j++) {
			if (centerP.y >= convexHullPoint[j].y && centerP.y >= convexHullPoint[pos].y) {
				//count++;
				int dist = (centerP.x - convexHullPoint[j].x)^ 2 + (centerP.y - convexHullPoint[j].y) ^ 2;
				if ( dist > maxdist && abs(convexHullPoint[pos].x - convexHullPoint[j].x) < 9){
					maxdist = dist;
					pos = j;
				}
				else if( abs(convexHullPoint[pos].x - convexHullPoint[j].x) >= 9  ){
					fingerPoint.push_back(convexHullPoint[pos]);
					cv::line(dest,centerP, convexHullPoint[pos],Scalar(0, 255, 0), 3, CV_AA, 0);
					circle(dest, convexHullPoint[pos], 8, Scalar(255, 0, 0), CV_FILLED);
					pos = j;
				}
			}
		}

		//**************************************************

//		for( int d = 0; d < defects[largestObj].size(); d++){
//			int temp = defects[largestObj][d][2];
//			if ( centerP.y >= contours[largestObj][temp].y){
//				int dist = (centerP.x - contours[largestObj][temp].x)^ 2 + (centerP.y - contours[largestObj][temp].y) ^ 2;
//				if ( dist > 10 ){
//					defectPoint[largestObj].push_back(contours[largestObj][temp]);
//					circle(dest, defectPoint[largestObj][d], 8, Scalar(0, 255, 0), CV_FILLED);
//				}
//
//			}
//
//		}

		//**************************************************



		int countFinger = fingerPoint.size();
		int angle;
		for ( int x = 0; x < countFinger; x++){
			angle = angle + abs (angleToCenter(fingerPoint[x], centerP) );
		}
		doAction( angle, countFinger );
		//cout << angle << endl;
		putText(dest, integerToString(countFinger), printPoint, 1, 5, Scalar(0, 255, 0), 1, 8, false);
	}
}
}


void doAction(int totalAngleOfFinger, int fingerSize){
	if( totalAngleOfFinger>= 94 && totalAngleOfFinger <= 100 && (fingerSize == 2  ))
		cout << "Go Back Page" << endl;
	else if( totalAngleOfFinger >= 200 && totalAngleOfFinger <= 250 && fingerSize == 5)
		cout << "Maximum the Page" << endl;
	else if( totalAngleOfFinger >= 200 && totalAngleOfFinger <= 250 && fingerSize == 4)
		cout << "minimum the Page" << endl;
	else if( totalAngleOfFinger == 0 )
		cout <<  endl;
	else if( totalAngleOfFinger >= 120 && totalAngleOfFinger <= 135 &&  (fingerSize == 2 ))
		cout << "Reload the Page" << endl;
	else
		cout <<  endl;
}

int main() {
	VideoCapture stream1;
	//default the capture frame size to the certain size
	stream1.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	stream1.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	stream1.open(0);
	if (!stream1.isOpened()) {
		cout << "cannot open camera";
	}
	while (true) {
		//determine that if the hand is detected or not
		//initialization of various frames
		Mat cameraFrame, blurFrame, threshold1, threshold2, closedFrame,
				hsvFrame, colorObjectFrame, thresholdFrame;
		//get image from stream
		stream1.read(cameraFrame);

		//switch the RGB to HSV space
		cv::cvtColor(cameraFrame, hsvFrame, CV_BGR2HSV);

		//using the HSV minimum and maximum value to find the threshold value of the frame
		//we can use these values to detect the hand skin color
//		cv::inRange(hsvFrame, Scalar(0, 0, 195), Scalar(256, 256, 256),threshold1);
//		cv::inRange(hsvFrame, Scalar(195, 0, 195), Scalar(256, 256, 256),threshold2);

		//testing in the blue glove
		cv::inRange(hsvFrame, Scalar(58, 33, 154), Scalar(132, 256, 256),threshold1);
		cv::inRange(hsvFrame, Scalar(154, 33, 154), Scalar(256, 256, 256),threshold2);
		cv::bitwise_or(threshold1, threshold2, thresholdFrame);

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
	}
	return 0;

}
