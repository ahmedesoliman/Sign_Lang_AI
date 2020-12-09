//train.h
#pragma once

#ifndef TRAIN_H
#define TRAIN_H
#include "app.h"

class Train : public App
{

private:
public:
	Train();
	~Train();
	void trainApp(char);
	virtual void app() {}
};

#endif // end of TRAIN_H