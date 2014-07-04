/*
 * visionTest.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: Colten
 *
 *  Moved the old test methods out from the main file to here. Nice
 *  place to start and see if all the opencv libraries are linked
 *  properly
 */


/*
 * Module one, good for testing without a webcam
 */

//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//using namespace std;
//using namespace cv;
//
//int main()
//{
//
//Mat img = imread("d:/lenna.png", CV_LOAD_IMAGE_COLOR);
//
//namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
//imshow("MyWindow", img);
//
//waitKey(0);
//return 0;
//}

/*
 * module 2, testing with a webcam
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int MAX_KERNEL_LENGTH = 5;
int DELAY_BLUR = 100;

int main() {
	VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialised
		cout << "cannot open camera";
	}

	//unconditional loop
	while (true) {
		//get image from stream
		Mat cameraFrame;
		Mat blurFrame;
		stream1.read(cameraFrame);

		//do a basic blur
	    medianBlur ( cameraFrame, blurFrame, MAX_KERNEL_LENGTH );

		//show image
		imshow("cam", blurFrame);
		if (waitKey(30) >= 0) break;
	}
	return 0;
}

