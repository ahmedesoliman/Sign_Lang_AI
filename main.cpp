/************* 

Final Project
Name: Ahmed Soliman
ID# : 24033795
Class : Fall 2020, CSC 211H 0500[64041] - (Borough of Manhattan CC)
Date : 12 / 15 / 2020
Time : 05 : 30PM
Instructor Name : Dr.Azhar

*************/

#include "app.h"
#include "predict.h"
#include "train.h"

#include <thread>



using namespace cv;
using namespace std;



int main(int argc, char** argv)
{
    App app;                        //App object
    Predict predict;                //predict object
    Train train;                    //train object

    try {
        char keyboard = 0;           // last key pressed
        int training_mode = 0;       // 0 = no training; 1 = training

        int debug_mode = 0;         // 0= no debug; 1= debug

        int predict_mode = 0;        // 0= no predict; 1=predict

        cout << "Press: (T) to train - (P) to predict - (D) to Debug: ";

        cin >> keyboard;

        if (keyboard == 'T' || keyboard == 't')
        {
            training_mode = 1;
        }

        if (keyboard == 'D' || keyboard == 'd') {
            debug_mode = 1;
        }

        if (keyboard == 'P' || keyboard == 'p') {

            predict_mode = 1;
        }
        if (training_mode)
        {
            app.asl_init();
            train.trainApp(keyboard);

        }

        if (predict_mode) {
            predict.load_ASL();
            predict.predictApp(keyboard);
        }
        if (debug_mode) {

            //predict.train();
            app.asl_init();
            app.run(keyboard);

            //std::thread t1(&App::f1_captureimage, &app);


            //std::thread t2(&App::f2_extracthand, &app);


            //std::thread t3(&App::f3_extractfeature, &app);


            //std::thread t4(&App::f4_identifyletter, &app);


            //std::thread t5(&App::f5_displayletter, &app);

            //t1.join();
            //t2.join();
            //t3.join();
            //t4.join();
            //t5.join();

            //predict.train();
        }
        //else if (keyboard != "p" && keyboard != "P" && keyboard != "T" && keyboard != "t") {
        //    throw "invalid input please try again!";
        //}
    } 
    catch (string str) {
        cout << str;
    }
}