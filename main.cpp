#include "predict.h"
#include <thread>



using namespace cv;
using namespace std;

int main()
{
    Predict predict;             //predict object
   ///* predict.run();  */       //run app

    char keyboard = 0;           // last key pressed

    int training_mode = 0;       // 0 = no training; 1 = training

    int debug_mode = 0;         // 0= no debug; 1= debug

    cout << "Press (T) to train or (D) to Debug: ";

    cin >> keyboard;

    if (keyboard == 'T' || keyboard == 't')
    {
        training_mode = 1;
    }

    if (keyboard == 'D' || keyboard == 'd') {
        debug_mode = 1;
    }

    if (training_mode)
    {
        predict.run(keyboard);

    }

    if (debug_mode) {

        //predict.train();
        predict.asl_init();

        predict.f1_captureimage();

        //predict.train();
    }
}