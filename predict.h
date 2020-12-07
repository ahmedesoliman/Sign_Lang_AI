//predict.h
#pragma once

#ifndef PREDICT_H
#define PREDICT_H

#include "app.h"

class Predict : public App {
private:

public:
	Predict();
	~Predict();
	void predictApp(char);		//this function predict the app
	void displayLetter();		//this function displays the letter
	void load_ASL();			//
	virtual void print() {}		

};

#endif // end of PREDICT_H
