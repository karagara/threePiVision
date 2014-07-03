/*
 * ObjectTracking.cpp
 * ---Detect the specific HSV value using the this GUI.
 *  Created on: Jul 2, 2014
 *  Author: Yan Liu
 *          Colten Noremore
 *  Refs: "docs.opencv.org"
 *
 */

#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <string>
#include <strstream>
using namespace std;
using namespace cv;

//initial min and max HSV filter values. These value will be changed using HSV track GUI
//This GUI will detect the HSG value that using the "Color Ball Tracking" Application
int Hue_Min = 0;
int Hue_Max = 256;
int Sat_Min = 0;
int Sat_Max = 256;
int Val_Min = 0;
int Val_Max = 256;

const int Frame_Width = 640;
const int Frame_Height = 480;
const int Max_Dectect = 20; //set up the maximum object can be detected
//const int Max_Object_Area = 0;

const string trackingHSVApplication = "Tracking HSV Value";

void createHSVApp() {
	namedWindow(trackingHSVApplication, 0);
	char trackingHSV[50];
	sprintf(trackingHSV, "Hue_Min", Hue_Min);
	sprintf(trackingHSV, "Hue_Max", Hue_Max);
	sprintf(trackingHSV, "Sat_Min", Sat_Min);
	sprintf(trackingHSV, "Sat_Max", Sat_Max);
	sprintf(trackingHSV, "Val_Min", Val_Min);
	sprintf(trackingHSV, "Val_Max", Val_Max);

	cv::createTrackbar("Hue_Min", trackingHSVApplication, &Hue_Min, Hue_Max);
	cv::createTrackbar("Hue_Max", trackingHSVApplication, &Hue_Max, Hue_Max);
	cv::createTrackbar("Sat_Min", trackingHSVApplication, &Sat_Min, Sat_Max);
	cv::createTrackbar("Sat_MAX", trackingHSVApplication, &Sat_Max, Sat_Max);
	cv::createTrackbar("Val_MIN", trackingHSVApplication, &Val_Min, Val_Max);
	cv::createTrackbar("Val_MAX", trackingHSVApplication, &Val_Max, Val_Max);
}

void morphoProcessing(Mat &filterImage) {
	Mat strel1 = cv::getStructuringElement(cv::MORPH_RECT, Size(8, 8));
	Mat strel2 = cv::getStructuringElement(cv::MORPH_RECT, Size(8, 8));
	cv::erode(filterImage, filterImage, strel1);
	cv::dilate(filterImage, filterImage, strel2);
}

String toString(int num) {
	strstream ss;
	string s;
	ss << num;
	ss >> s;
	return s;
}

void trackObjects(Mat src, Mat &dest, int &x, int &y) {
	Mat colorArea;
	bool ObjectFound = false;
	src.copyTo(colorArea);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find the Contours
	cv::findContours(colorArea, contours, hierarchy, CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);

	//find the maximum object boundary
	int index = 0;
	double area = 0, maxArea(0);
	for (int i = 0; i < contours.size(); i++) {
		area = cv::contourArea(Mat(contours[i]));
		if (area > maxArea) {
			maxArea = area;
			index = i;
		}
	}

	Moments moment = moments(colorArea, true);
	x = moment.m10 / moment.m00;
	y = moment.m01 / moment.m00;
	if (x > 0 && y > 0 && area > 10 * 10) {
		drawContours(dest, contours, index, Scalar(0,0, 255), 2, 8, hierarchy);
		cv::circle(dest, Point(x, y), 20, Scalar(255, 0, 0), 3);
		cv::putText(dest, toString(x) + "," + toString(y) + "Object Found", Point(x - 35, y + 35), 1, 1, Scalar(255, 0, 0), 2);
	}
}

int main(int argc, char* argv[]) {
	//initialize the variables
	Mat cameraOrig;
	Mat cameraGray;
	Mat cameraHSV;
	Mat cameraThreshold;
	int x = 0;
	int y = 0;

	//useful for detecting the object's HSV
	createHSVApp();

	VideoCapture vcapture;
	//default the capture frame size to the certain size
	vcapture.set(CV_CAP_PROP_FRAME_WIDTH, Frame_Width);
	vcapture.set(CV_CAP_PROP_FRAME_HEIGHT, Frame_Height);
	vcapture.open(0);

	while (1) {
		vcapture.read(cameraOrig);
		cv::GaussianBlur(cameraOrig, cameraOrig, Size(3, 3), 0);
		cv::cvtColor(cameraOrig, cameraHSV, cv::COLOR_RGB2HSV);
		//This is the example of the "Pink Sticker" HSV value
		//cv::inRange(cameraHSV,Scalar(68, 68, 163), Scalar(237,209,256),cameraThreshold);
		//This can be adjusted during the application run
		cv::inRange(cameraHSV, Scalar(Hue_Min, Sat_Min, Val_Min), Scalar(Hue_Max, Sat_Max, Val_Max), cameraThreshold);
		//using the Morphological Image processing to process the image
		morphoProcessing(cameraThreshold);
		trackObjects(cameraThreshold, cameraOrig, x, y);
		//show the different kinds of frames
		cv::imshow("Camera Original", cameraOrig);
		cv::imshow("Camera HSV Module", cameraHSV);
		cv::imshow("Camera Threshold Module", cameraThreshold);
		waitKey(10);
	}

	return 0;
}
