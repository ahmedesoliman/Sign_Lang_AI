#include "app.h"
#include "predict.h"
#include "train.h"

#include <thread>



using namespace cv;
using namespace std;

int main()
{
    App app;                        //App object
    Predict predict;                //predict object
    Train train;                    //train object

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
        app.asl_init();
        predict.predictApp(keyboard);
    }
    if (debug_mode) {

        //predict.train();
        app.asl_init();

        std::thread t1(&App::f1_captureimage, &app);


        std::thread t2(&App::f2_extracthand, &app);


        std::thread t3(&App::f3_extractfeature, &app);


        std::thread t4(&App::f4_identifyletter, &app);


        std::thread t5(&App::f5_displayletter, &app);

        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();

        //predict.train();
    }
}