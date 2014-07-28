///*
// * hsvTrack.cpp
// * ---Detect the specific HSV value using the this GUI.
// *  Created on: Jul 2, 2014
// *  Author: Yan Liu
// *          Colten Noremore
// *  Refs: "docs.opencv.org"
// *
// */
//
//#include <opencv\highgui.h>
//#include <opencv\cv.h>
//#include <opencv2/video/background_segm.hpp>
//
//using namespace std;
//using namespace cv;
//
////initial min and max HSV filter values. These value will be changed using HSV track GUI
////This GUI will detect the HSG value that using the "Color Ball Tracking" Application
//int Hue_Min = 0;
//int Hue_Max = 256;
//int Sat_Min = 0;
//int Sat_Max = 256;
//int Val_Min = 0;
//int Val_Max = 256;
//
//const int Frame_Width = 640;
//const int Frame_Height = 480;
////const int Max_Dectect = 20; //set up the maximum object can be detected
//
//const string trackingHSVApplication = "Tracking HSV Value";
//
//void createHSVApp() {
//	namedWindow(trackingHSVApplication, 0);
//	char trackingHSV[50];
//	sprintf(trackingHSV, "Hue_Min", Hue_Min);
//	sprintf(trackingHSV, "Hue_Max", Hue_Max);
//	sprintf(trackingHSV, "Sat_Min", Sat_Min);
//	sprintf(trackingHSV, "Sat_Max", Sat_Max);
//	sprintf(trackingHSV, "Val_Min", Val_Min);
//	sprintf(trackingHSV, "Val_Max", Val_Max);
//
//	cv::createTrackbar("Hue_Min", trackingHSVApplication, &Hue_Min, Hue_Max);
//	cv::createTrackbar("Hue_Max", trackingHSVApplication, &Hue_Max, Hue_Max);
//	cv::createTrackbar("Sat_Min", trackingHSVApplication, &Sat_Min, Sat_Max);
//	cv::createTrackbar("Sat_MAX", trackingHSVApplication, &Sat_Max, Sat_Max);
//	cv::createTrackbar("Val_MIN", trackingHSVApplication, &Val_Min, Val_Max);
//	cv::createTrackbar("Val_MAX", trackingHSVApplication, &Val_Max, Val_Max);
//}
//
//void morphologicalImgProc(Mat &frame) {
//	Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(9, 9), Point(5, 5));
//	cv::dilate(frame, frame, element);
//	cv::erode(frame, frame, element);
//	cv::morphologyEx(frame, frame, MORPH_OPEN, element);
//	cv::morphologyEx(frame, frame, MORPH_CLOSE, element);
//}
//
//int main(int argc, char* argv[]) {
//	//initialize the variables
//	Mat cameraFrame, blurFrame, threshold1, threshold2, closedFrame, hsvFrame, colorObjectFrame, thresholdFrame;
//	VideoCapture stream1;
//	Mat grayscale;
//	Mat fgMaskMOG;
//	Mat foreground;
//	Mat background;
//	Ptr <BackgroundSubtractor> pMOG;
//	pMOG = new BackgroundSubtractorMOG();
//
//	Mat cameraOrig;
//	Mat cameraHSV;
//	Mat cameraThreshold;
//	VideoCapture vcapture;
//
//	//create the HSV tracker
//	createHSVApp();
//
//	//default the capture frame size to the certain size
//	vcapture.set(CV_CAP_PROP_FRAME_WIDTH, Frame_Width);
//	vcapture.set(CV_CAP_PROP_FRAME_HEIGHT, Frame_Height);
//	vcapture.open(0);
//
//	while (1) {
//		vcapture.read(cameraOrig);
//		pMOG->operator ()(cameraOrig, foreground);
//		//remove the salt and pepper noise
//		cv::medianBlur(foreground, foreground, 3);
//		//cv::GaussianBlur(cameraOrig, cameraOrig, Size(3, 3), 0);
//		cv::cvtColor(foreground, foreground, CV_GRAY2BGR);
//		//change the color to HSV space
//		cv::cvtColor(foreground, cameraHSV, cv::COLOR_BGR2HSV);
//		cv::imshow("Camera hsv 1st", cameraHSV);
//		//get the threshold frame using the minimum and maximum HSV values
//		cv::inRange(cameraHSV, Scalar(Hue_Min, Sat_Min, Val_Min), Scalar(Hue_Max, Sat_Max, Val_Max), cameraThreshold);
//		cv::imshow("Camera Threshold 1st", cameraThreshold);
//
//		//using the Morphological Image processing to process the image, remove noise
//		morphologicalImgProc(cameraThreshold);
//
//		//show the different kinds of frames
//		cv::imshow("Camera Original", cameraOrig);
//		cv::imshow("Camera Threshold Module", cameraThreshold);
//
//		waitKey(10);
//	}
//	return 0;
//}
