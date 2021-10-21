/*
 * IntegerEdit.h
 *
 *  Created on: 2.2.2016
 *      Author: krl
 */

#ifndef INTEGEREDIT_H_
#define INTEGEREDIT_H_

#include "PropertyEdit.h"
#include "display/LiquidCrystal.h"
#include <string>

class IntegerEdit: public PropertyEdit {
public:
	IntegerEdit(LiquidCrystal *lcd_, std::string editTitle, int upper = 100, int lower = 0, int step = 1);
	virtual ~IntegerEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus();
	void display();
	int getValue();
	void setValue(int value);
private:
	void save();
	void displayEditValue();

	LiquidCrystal *lcd;
	std::string title;
	int upper_limit;
	int lower_limit;
	int step_size;
	int value;
	int edit;
	bool focus;
};

#endif /* INTEGEREDIT_H_ */
