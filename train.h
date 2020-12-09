/*Copyrights @ Ahmed Soliman www.ahmedesoliman.com*/
//train.h
#pragma once

#ifndef TRAIN_H
#define TRAIN_H
#include "app.h"

class Train : public App
{

private:
public:
	Train(); //default constructor
	~Train(); //default destructor
	void trainApp(char); //this function trains the application and saves an images to the dataset
	virtual void app() {} //this method overrides app pure virtual function
};

#endif // end of TRAIN_H