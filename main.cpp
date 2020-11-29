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

   printf("Press (T) to train or (D) to Debug: ");
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
        predict.asl_init();

        std::thread t1(&Predict::f1_captureimage, &predict);
   
        std::thread t2(&Predict::f2_extracthand, &predict);

        std::thread t3(&Predict::f3_extractfeature, &predict);

        std::thread t4(&Predict::f4_identifyletter, &predict);
        
        std::thread t5(&Predict::f5_displayletter, &predict);
        
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
    }

	//predict.train();
}