#ifndef BACKGROUNDREMOVER_H
#define BACKGROUNDREMOVER_H

#include<opencv2/opencv.hpp>


using namespace cv;
using namespace std;

class Backgroundremover {
public:
	Backgroundremover(void);
	void calibrate(Mat input);
	Mat getForeground(Mat input);

private:
	Mat background;
	bool calibrated = false;

	Mat getForegroundMask(Mat input);
	void removeBackground(Mat input, Mat background);
};

#endif