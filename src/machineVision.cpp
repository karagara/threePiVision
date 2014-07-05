//============================================================================
// Name        : machineVision.cpp
// Author      : Yan Liu and Colten Normore
// Version     : 0.0.1
// Copyright   : Share everywhere
// Description : A (theoretically) simple program to count fingers
//============================================================================

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <string>
#include <iostream>
using namespace cv;
using namespace std;

const int Frame_Width = 640;
const int Frame_Height = 480;
#define PI 3.14159265

bool R1(int R, int G, int B);
void getSkin(Mat const &src, Mat &dest);
void getContours(Mat const &closedFrame, Mat const &bwFrame, Mat &contourFrame, vector<vector<Point> > &contours);
void filterSmallContours(vector<vector<Point> > const &contours, vector<vector<Point> > &largeContours, int maxArea);
void takeDerivative(vector<int> const &samples, vector<int> &dsamples, int &fingerCount);

int main() {
	VideoCapture stream1;
	//default the capture frame size to the certain size
	stream1.set(CV_CAP_PROP_FRAME_WIDTH, Frame_Width);
	stream1.set(CV_CAP_PROP_FRAME_HEIGHT, Frame_Height);
	stream1.open(0);

	if (!stream1.isOpened()) {
		cout << "cannot open camera";
	}

	//unconditional loop
	while (true) {
		//initialization of various frames
		Mat cameraFrame, skinFrame, blurFrame, closedFrame, bwFrame, contourFrame;
		vector<vector<Point> > contours, largeContours;

		//get image from stream
		stream1.read(cameraFrame);

		//blur image to remove basic imperfections
	    medianBlur(cameraFrame, blurFrame, 7);

		//find skin
		getSkin(blurFrame, skinFrame);

		//Closing operation
		Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(7, 7), Point(3,3));
		cv::morphologyEx(skinFrame, closedFrame, MORPH_CLOSE, element);

		//find contours
		cv::cvtColor(closedFrame, bwFrame, CV_RGB2GRAY);
		contourFrame=cameraFrame.clone();
		getContours(closedFrame, bwFrame, contourFrame, contours);

		//filter out small contours
		filterSmallContours(contours, largeContours, 2000);

		//get bounding box and moments
		cv::Rect bBox;
		cv::Moments moment;
		vector<int> samples(360);
		if (largeContours.size() > 0){
			bBox = cv::boundingRect(largeContours[0]);
			cv::rectangle(contourFrame,bBox,Scalar(0, 255, 0));
			moment = cv::moments(largeContours[0]);
			int com_x = moment.m10 / moment.m00;
			int com_y = moment.m01 / moment.m00;


			//Create samples at a set ratio away from COM in each direction
			int sampleDistance = (bBox.y-com_y)*.90;
			cv::circle(contourFrame, Point(com_x, com_y), abs(sampleDistance), Scalar(255, 0, 0), 2);

			getSamples(closedFrame,samples, sampleDistance, com_x, com_y);
		}

		//take derivative of signal
		vector<int> dsamples(359);
		int fingerCount = -1;
		takeDerivative(samples, dsamples, fingerCount);

		//display number of fingers detected
		std::string s;
		std::stringstream out;
		out << fingerCount;
		s = out.str();
		std::string fingerString = "There are: " + s +" finger(s)";
		cv::putText(contourFrame, fingerString ,Point(35, 35), 1, 1, Scalar(255, 0, 0), 2);


	    //setup outputs
	    namedWindow("closed");
	    namedWindow("contours");

		//show image
		imshow("closed", closedFrame);
		imshow("contours", contourFrame);

		if (waitKey(10) >= 0) break;
	}
	return 0;
}

void getSamples(Mat const &closedFrame, vector<int> &samples, int sampleDistance, int com_x, int com_y){
	for (int i=0; i < 360 ; i++){
		int sample_x = sampleDistance * cos(i*PI/180);
		sample_x += com_x;
		int sample_y = sampleDistance * sin(i*PI/180);
		sample_y += com_y;

		if(sample_x < 0) sample_x = 0;
		else if(sample_x > Frame_Width) sample_x = Frame_Width-1;

		if(sample_y < 0) sample_y = 0;
		else if(sample_y > Frame_Height) sample_y = Frame_Height-1;

		if (closedFrame.ptr<Vec3b>(sample_y)[sample_x] == Vec3b::all(255)){
			samples[i] = 1;
		}else{
			samples[i] = 0;
		}
	}

}

void takeDerivative(vector<int> const &samples, vector<int> &dsamples, int &fingerCount){
	for(int i = 1; i < 360; i++){
		dsamples[i] = samples[i] - samples[i-1];
		if (dsamples[i] < 0){
			dsamples[i] = 0;
		}
		if (dsamples[i] == 1){
			fingerCount++;
		}
	}
}


void getContours(Mat const &closedFrame, Mat const &bwFrame, Mat &contourFrame, vector<vector<Point> > &contours ){
	vector<Vec4i> hierarchy;
	cv::findContours(bwFrame,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	cv::drawContours(contourFrame,contours,-1,Scalar(0, 0, 255), 3, 8, hierarchy);
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

void getSkin(Mat const &src, Mat &dest) {
    // allocate the result matrix
    dest = src.clone();

    Vec3b cwhite = Vec3b::all(255);
    Vec3b cblack = Vec3b::all(0);
    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {
            Vec3b pix_bgr = src.ptr<Vec3b>(i)[j];
            int B = pix_bgr.val[0];
            int G = pix_bgr.val[1];
            int R = pix_bgr.val[2];
            // apply rgb rule
            bool a = R1(R,G,B);

            //fill based on rule
            if(a)
            	dest.ptr<Vec3b>(i)[j] = cwhite;
            else
            	dest.ptr<Vec3b>(i)[j] = cblack;
        }
    }
}

bool R1(int R, int G, int B){

	double rgRatio = (double)R/(double)G;
	bool isSkin = false;
	if ( rgRatio > 1.05 && rgRatio < 4.0){
		isSkin = true;
	}
	return isSkin;

}

