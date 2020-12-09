/*Copyrights @ Ahmed Soliman www.ahmedesoliman.com*/
//predict.h
#pragma once

#ifndef PREDICT_H
#define PREDICT_H

#include "app.h"

class Predict : public App
{
private:
public:
	Predict();  //default constructor
	~Predict();	//defualt destructor
	void predictApp(char);	//this function predict the app
	void displayLetter();	//this function displays the letter
	void load_ASL();		//this function loads the letters that was learned for prediction
	virtual void app() {}	//this method overrides app pure virtual function
};

#endif // end of PREDICT_H
