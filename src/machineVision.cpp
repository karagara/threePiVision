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
void filterSmallContours(vector<vector<Point> > const &contours,
		vector<vector<Point> > &largeContours, int maxArea);
void morphologicalImgProc(Mat &frame);
string integerToString(int num);
float angleBetween(const Point &v1, const Point &v2);

//calculate the angle between two points
//need to be renamed variables!!!!!!
float angleBetween(const Point &v1, const Point &v2) {
	float len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
	float len2 = sqrt(v2.x * v2.x + v2.y * v2.y);

	float dot = v1.x * v2.x + v1.y * v2.y;

	float a = dot / (len1 * len2);

	if (a >= 1.0)
		return 0.0;
	else if (a <= -1.0)
		return PI;
	else
		return acos(a); // 0..PI
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
void filterSmallContours(vector<vector<Point> > const &contours,
		vector<vector<Point> > &largeContours, int maxArea) {
	int size = contours.size();
	for (int i = 0; i < size; i++) {
		//if there is a large contour, add it to the vector of large contours
		if (cv::contourArea(contours[i]) > maxArea) {
			largeContours.push_back(contours[i]);
		}
	}
}

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
	vector<int> convexPoint;
	Point centerP;
	int numObjects = 0;
	bool flag = false;
	double area = 0;
	double maxArea = 0;
	bool handFound = false;

	findContours(src, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	//findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CLOCKWISE,Point(0, 0));
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
	boundRect = boundingRect(contours[largestObj]);
	//filterSmallContours(contours, contoursSet, maxArea);

	//draw the boundary of the object

	drawContours(dest, contours, largestObj, Scalar(0, 0, 255), 3, 8,hierarchy);

	convexHull(contours[largestObj], convexHullPoint, true, true);
	//approxPolyDP( Mat(contoursSet[largestObj]), contoursSet[largestObj], 3, true );
	Moments moment = moments(Mat(contours[largestObj]), true);
	int centerX = moment.m10 / moment.m00;
	int centerY = moment.m01 / moment.m00;
	Point centerPoint(centerX, centerY);
	centerP = centerPoint;
	Point printPoint(centerX, centerY - 10);
	circle(dest, centerPoint, 8, Scalar(255, 0, 0), CV_FILLED);
	//put the BoundingBox in the contour region
	rectangle(dest, boundRect, Scalar(0, 0, 255), 2, 8, 0);

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
			if (centerP.y >= convexHullPoint[j].y && centerP.y > convexHullPoint[pos].y) {
				//count++;
				int dist = (centerP.x - convexHullPoint[j].x)^ 2 + (centerP.y - convexHullPoint[j].y) ^ 2;
				if ( dist > maxdist && abs(convexHullPoint[pos].x - convexHullPoint[j].x) < 5 ){
					maxdist = dist;
					pos = j;
				}
				else if( abs(convexHullPoint[pos].x - convexHullPoint[j].x) >= 9){
					fingerPoint.push_back(convexHullPoint[pos]);
					cv::line(dest,centerP, convexHullPoint[pos],Scalar(0, 255, 0), 3, CV_AA, 0);
					circle(dest, convexHullPoint[pos], 8, Scalar(255, 0, 0), CV_FILLED);
					pos = j;
				}
			}
		}

		putText(dest, integerToString(fingerPoint.size()), printPoint, 1, 5, Scalar(0, 255, 0), 1, 8, false);
	}
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
		cv::inRange(hsvFrame, Scalar(0, 0, 195), Scalar(256, 256, 256),threshold1);
		cv::inRange(hsvFrame, Scalar(195, 0, 195), Scalar(256, 256, 256),threshold2);
		cv::bitwise_or(threshold1, threshold2, thresholdFrame);

		//blur image to remove basic imperfections
		medianBlur(thresholdFrame, thresholdFrame, 3);

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
