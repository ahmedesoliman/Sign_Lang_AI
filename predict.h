#ifndef PREDICT_H
#define PREDICT_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <iostream>
#include <vector>

#include <opencv2/video/background_segm.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>

#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>
#include <mutex>

#include <condition_variable>



using namespace cv;
using namespace std;

// Global data (macro definitions)
#define DIFF_THRESH 230					// Maximum difference threshold to accept an input
#define KEY_ESC (27)					//defining the escape button;
#define MAX_LETTERS 26					// Number of letters 26 
#define NUM_LAST_LETTERS 3			    // Number of letters to store;
#define MIN_FREQ 2					    // Minimum frequency of last letters;
#define THRESH 200
#define RESET_THRESH 25000000
#define SAMPLE_RATE 1


class Predict
{
private:
	std::mutex mtx;
	std::condition_variable cond;

	bool captureReady;
	bool handReady;
	bool featureReady;
	bool signReady;
	bool displayReady;


	int frames = 0;                         // frames varaible to count how many frames processed
	int maxIndex = 0;
	int reset = 0;
	
	// Private members 
	Mat frame;                              // current frame
	Mat rgb_image;                          // output f1--> rgb_imge
	Mat binary_image;
	Mat drawing;							// output f2--> binary_image
	Mat threshold_output;					// Generate the tresholdoutput

	//this is 2D vector like matrix on OpenCV
	vector<vector<Point>> feature_image;	// output f3--> feature_image
	vector<vector<Point>> contours;			// returns a vector<vector<Point>>

	char asl_letter;						// output f4--> asl_letter

	VideoCapture capture;					// openCV Class for video capturing from video files, image sequences or cameras

	Ptr<BackgroundSubtractor> backGroundMOG2;  //create Background Subtractor objects

	vector<Point> letters[MAX_LETTERS];		//A vector of point in OpenCV is just a standard C++ STL vector containing OpenCV Point objects 

	vector<Vec4i> hierarchy;				// Vec4i is a structure for representing a vector with 4 dimensions, with each value an int


public:
	Predict();
	~Predict();
	// member functions declarations
	void f1_captureimage();				// this method should capture the image
	void f2_extracthand();				//this method should extract the hand
	void f3_extractfeature();			// this method should extract the 
	void f4_identifyletter();			// this method should indentify the letter
	void f5_displayletter();			// this method should display the letter
	void asl_init();									// this method iniallized & preload letter images when started
	int distance_2(vector<Point>, vector<Point>);		//this method take two vectors of points a and b
	double distance(vector<Point>, vector<Point>);		//this method find the distance of 
	void run(char);
	//void train();
};

#endif // PREDICT_H