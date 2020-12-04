// app.cpp
#include "app.h"

//defualt constructor
App::App()
{
}

// default destructor
App::~App()
{

    destroyAllWindows();
    capture.release();
}

// this function loads the data images to learn the algorithm
void App::asl_init()
{
    //*** Preload letter train images starts ***//
    for (int i = 0; i < MAX_LETTERS; i++)
    {
        char buffer[13 * sizeof(char)];

        sprintf_s(buffer, "data/%c.png", ('a' + i));  // foramting

        Mat img1 = imread(buffer, 1);

        if (img1.data)
        {
            Mat img2, threshold_output;;

            cvtColor(img1, img2, COLOR_RGB2GRAY);

            // Detect edges using Threshold
            //The threshold method returns two outputs. The first is the threshold that was used and the second output is the thresholded image.
            threshold(img2, threshold_output, THRESH, 255, THRESH_BINARY);

            //findcontours() function retrieves contours from the binary image using the openCV algorithm[193].
            //The contours are a useful tool for shape analysisand object and detectionand recognition.
            findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

            letters[i] = contours[0];
            //contours returns a vector<vector<point>>
        }
    }
    //***Preload letter train images ends***//

    //*** learn starts ***//

    backGroundMOG2 = createBackgroundSubtractorMOG2(10000, 200, false);

    //***learn ends  ***//
} /* end of asl_init()*/

//this function capture the image
void App::f1_captureimage()
{

    capture = VideoCapture(0);

    while (true)
    {

        //unique_lock<mutex> locker(mtx);
        //cond.wait(locker /*, [this]{return displayReady;}*/);

        cout << "Thread #1: Capture Image\n ";

        // Create the capture object
        if (!capture.isOpened())
        {
            cout << "Cannot Open Webcam !!!" << endl; // Error in opening the video input
            exit(EXIT_FAILURE);
        }
        // Read the current frame
        if (!capture.read(frame))
        {
            cout << "Unable to read next frame." << endl;
            cout << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        Rect myROI(200, 200, 200, 200); // Crop Frame to smaller region : output --> rgb_image

        rgb_image = frame(myROI);

        imshow("th1_captureimage", rgb_image); // output th1--> rgb_imge

        char q = waitKey(33);

        //********************************//
        cout << "\nThread #1 exeuted...\n";

        captureReady = true;
        cond.notify_one();
    }
    capture.release();
} /* end of f1_captureimage()*/

//this function extract the hand
void App::f2_extracthand()
{
    while (true)
    {
        unique_lock<mutex> locker(mtx);
        cond.wait(locker, [this] { return captureReady; });
        cout << "Thread #2: Extract hand\n";

        /*      if (reset <= 10)
                {
                    reset++;
                    backGroundMOG2 = createBackgroundSubtractorMOG2(10000, 16, true);
                }*/

        backGroundMOG2->apply(rgb_image, binary_image, 0);

        threshold(binary_image, threshold_output, THRESH, 255, THRESH_BINARY); // Detect edges using Threshold

        imshow("Binary Image", binary_image);

        imshow("Threshold_output", threshold_output);

        char q = waitKey(33);

        //********************************//
        cout << "\nThread #2 exeuted...\n";

        handReady = true;
        cond.notify_one(); //Unblocks one of the threads currently waiting for this condition. If no threads are waiting, the function does nothing. If more than one, it is unspecified which of the threads is selected.
    }
} /* end of f2_extracthand()*/

//this function extracts the feature
void App::f3_extractfeature()
{
    while (true)
    {
        unique_lock<mutex> locker(mtx);
        cond.wait(locker, [this] { return handReady; });

        cout << "Thread #3: Extract Feature\n";

        findContours(threshold_output, feature_image, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0)); // Find contours

        imshow("feature", feature_image);

        drawing = Mat::zeros(rgb_image.size(), CV_8UC3); // Find largest contour

        double largest_area = 0;

        for (int j = 0; j < feature_image.size(); j++)
        {
            double area = contourArea(feature_image[j], false); // Find the area of contour
            if (area > largest_area)
            {
                largest_area = area;
                maxIndex = j; // Store the index of largest contour
            }
        }

        //printf("%d", maxIndex);  // Draw Largest Contours
        Scalar color = Scalar(0, 0, 255);

        //To draw the contours, cv::drawContours function is used
        drawContours(drawing, feature_image, maxIndex, Scalar(255, 255, 255), FILLED); // filled white

        // Draw Contours
        Mat contourImg = Mat::zeros(rgb_image.size(), CV_8UC3);

        drawContours(contourImg, feature_image, maxIndex, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point(0, 0));

        // Reset if too much noise
        //Scalar sums = sum(drawing);
        //int s = sums[0] + sums[1] + sums[2] + sums[3];
        //if (s >= RESET_THRESH)
        //{
        //    reset = 10;
        //}

        imshow("drwaing", drawing);
        imshow("hierarchy", hierarchy);
        /*     if (contourImg.rows > 0)
                    imshow("th3_extractfeature", contourImg);*/

        char q = waitKey(33);

        //********************************//
        cout << "\nThread #3 executed...\n";

        featureReady = true;
        cond.notify_one();
    }
}  /* end of f3_extractfeature()*/

//this fucntion identifies the letter
void App::f4_identifyletter()
{
    while (true)
    {
        unique_lock<mutex> locker(mtx);
        cond.wait(locker, [this] { return featureReady; });
        cout << "\nThread #4: Identify letter\n";

        // Compare to reference train images
        if (feature_image.size() > 0 && frames++ > SAMPLE_RATE && feature_image[maxIndex].size() >= 5)
        {
            RotatedRect testRect = fitEllipse(feature_image[maxIndex]);

            frames = 0;

            double lowestDiff = HUGE_VAL;

            for (int i = 0; i < MAX_LETTERS; i++)
            {
                if (letters[i].size() == 0)
                    continue;

                double difference = distance(letters[i], feature_image[maxIndex]);

                if (difference < lowestDiff)
                {
                    lowestDiff = difference;
                    asl_letter = 'a' + i;
                }
            }

            if (lowestDiff > DIFF_THRESH)
            { // Dust
                asl_letter = 0;
            }

            cout << "The letter is: " << asl_letter << " | difference: " << lowestDiff << endl;
        }

        //********************************//
        cout << "\nThread #4 exeuted... \n";

        signReady = true;
        cond.notify_one();
    }
} /* end of f4_identifyletter()*/

//this fucntion displays the letter
void App::f5_displayletter()
{

    unique_lock<mutex> locker(mtx);
    cond.wait(locker, [this]() { return signReady; });

    int letterCount = 0; // number of letters captured since last display
    char lastLetters[NUM_LAST_LETTERS] = { 0 };

    //creates a Mat object filled with zeros
    Mat letter_image = Mat::zeros(200, 200, CV_8UC3);
    char lastExecLetter = 0; // last letter sent to doSystemCalls()

    while (true)
    {

        cout << "\nThread #5: Display output\n";

        letterCount %= NUM_LAST_LETTERS;         // Show majority of last letters captured
        lastLetters[letterCount++] = asl_letter; // input from f4
        letter_image = Mat::zeros(200, 200, CV_8UC3);

        int counts[MAX_LETTERS + 1] = { 0 };

        for (int i = 0; i < NUM_LAST_LETTERS; i++)
            counts[lastLetters[i] + 1 - 'a']++;

        int maxCount = 0;
        char maxChar = 0;
        for (int i = 0; i < MAX_LETTERS + 1; i++)
        {
            if (counts[i] > maxCount)
            {
                maxCount = counts[i];
                maxChar = i;
            }
        }

        if (maxChar && maxCount >= MIN_FREQ)
        {
            maxChar = maxChar - 1 + 'a';
            char buffer[2 * sizeof(char)];
            sprintf_s(buffer, "%c", maxChar);

            putText(letter_image, buffer, Point(10, 75), FONT_HERSHEY_SIMPLEX, 12, Scalar(255, 255, 255), 1, 1);

            vector<vector<Point>> dummy;

            dummy.push_back(letters[maxChar - 'a']);

            drawContours(letter_image, dummy, 0, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point(0, 0));
            if (maxChar != lastExecLetter)
            {
                lastExecLetter = maxChar;
            }
        }

        imshow("Letter", letter_image); // output f5--> letter_image
        char q = waitKey(33);

        //********************************//
        cout << "\nThread #5 exeuted...\n";

        displayReady = true;
        cond.notify_one();
    }
} /* end of f5_displayletter()*/

// this function returns max distance between two vector points a and b
int App::distance_2(vector<Point> a, vector<Point> b)
{
    int maxDist = 0;
    for (int i = 0; i < a.size(); i++)
    {
        int min = 1000000;
        for (int j = 0; j < b.size(); j++)
        {
            int dx = (a[i].x - b[j].x);

            int dy = (a[i].y - b[j].y);

            int tmpDist = dx * dx + dy * dy;

            if (tmpDist < min)
            {
                min = tmpDist;
            }

            if (tmpDist == 0)
            {
                break; // can't get better than equal.
            }
        }
        maxDist += min;
    }
    return maxDist;
} /* end of distance_2()*/

//
double App::distance(vector<Point> a, vector<Point> b)
{
    int maxDistAB = distance_2(a, b);
    int maxDistBA = distance_2(b, a);
    int maxDist = max(maxDistAB, maxDistBA);

    return sqrt((double)maxDist);
} /* end of distance()*/


void App::run(char key) {
    capture = VideoCapture(0);

    //Creates MOG2 Background Subtractor.
    backGroundMOG2 = createBackgroundSubtractorMOG2(10000, 200, false);

    while (key != KEY_ESC)
    {
        /*    cout << "inside training \n";*/
        if (!capture.isOpened())
        {
            // Error in opening the video input
            cout << "Cannot Open Webcam... " << endl;
            exit(EXIT_FAILURE);
        }

        Mat fgMaskMOG2; // foreground mask foreground mask generated by MOG2 method

        if (!capture.read(frame))
        {
            cout << "Unable to read next frame." << endl;
            cout << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }

        // Crop Frame to smaller region using the rectangle of interest method
        Rect myROI(200, 200, 200, 200);

        Mat cropFrame = frame(myROI);

        // Update the background model
        backGroundMOG2->apply(cropFrame, fgMaskMOG2, 0);

        // Detect edges using Threshold
        /*adaptiveThreshold(fgMaskMOG2, threshold_output, THRESH, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 0);  -- adabtiveThreshhold can be another method to be used*/
        threshold(fgMaskMOG2, threshold_output, THRESH, 255, THRESH_BINARY);

        // Find contours
        findContours(threshold_output, feature_image, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // Find largest contour

        Mat drawing1 = Mat::zeros(cropFrame.size(), CV_8UC3); // empty matrix to draw the contours on it - intialize it to Mat::zeros means all black
        double largest_area = 0;

        for (int j = 0; j < feature_image.size(); j++)
        {
            double area = contourArea(feature_image[j], false); // Find the area of the contour
            if (area > largest_area)
            {
                largest_area = area;
                maxIndex = j; // Store the index of largest contour
            }
        }

        // Draw Largest Contours
        Scalar color = Scalar(0, 0, 255);
        drawContours(drawing1, feature_image, maxIndex, Scalar(255, 255, 255), FILLED); // fill white

        // Draw Contours
        Mat contourImg = Mat::zeros(cropFrame.size(), CV_8UC3);
        drawContours(contourImg, feature_image, maxIndex, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point(0, 0));

        //Reset if too much noise
        /*           Scalar sums = sum(drawing1);
               int s = sums[0] + sums[1] + sums[2] + sums[3];
               if (s >= RESET_THRESH)
               {
                   backGroundMOG2 = createBackgroundSubtractorMOG2(10000, 200, false);
                   continue;
               }*/

               // Show the current frame and the foreground masks

        imshow("Crop Frame", cropFrame);
        imshow("Mask", drawing1);
        imshow("Foregound Mask", fgMaskMOG2);
        imshow("Contour image", contourImg);


        //if (contourImg.rows > 0)
        //    imshow("Contour", contourImg);

        key = waitKey(1);

        // Manual reset the keyboard
        if (key == ' ')
            backGroundMOG2 = createBackgroundSubtractorMOG2(10000, 200, false);

        //f3_identify_letter
        if (feature_image.size() > 0 && frames++ > SAMPLE_RATE && feature_image[maxIndex].size() >= 5)
        {
            RotatedRect testRect = fitEllipse(feature_image[maxIndex]);
            //fits an ellipse around a set of 2d points. The function calculates the ellipse that fits(in a least-sense) a set of 2D points best of all.
            //it returns the rotated rectangle in which the ellipse is inscribed. the first algorithm - Param(points input 2d point set, stored in std::vector<> or Mat)

            frames = 0;

            double lowestDiff = HUGE_VAL;

            for (int i = 0; i < MAX_LETTERS; i++)
            {
                if (letters[i].size() == 0)
                    continue;

                double difference = distance(letters[i], feature_image[maxIndex]);

                if (difference < lowestDiff)
                {
                    lowestDiff = difference;
                    asl_letter = 'a' + i;
                }
            }

            if (lowestDiff > DIFF_THRESH)
            { // Dust
                asl_letter = 0;
            }

            cout << "The letter is: " << asl_letter << " | difference: " << lowestDiff << endl;
        }
    }

}