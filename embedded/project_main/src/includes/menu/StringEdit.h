#ifndef INCLUDES_MENU_STRINGEDIT_H_
#define INCLUDES_MENU_STRINGEDIT_H_

#include "PropertyEdit.h"
#include "display/LiquidCrystal.h"
#include <string>

class StringEdit: public PropertyEdit {
public:
	StringEdit(LiquidCrystal *lcd_, std::string editTitle, std::string* items_, int n);
	virtual ~StringEdit();
	void increment();
	void decrement();
	void accept();
	void cancel();
	void setFocus(bool focus);
	bool getFocus();
	void display();
	int getValue();
	void setValue(int index);
	void toggle_adjust() {};
private:
	void save();
	void displayEditValue();

	LiquidCrystal *lcd;
	std::string title;
	std::string* items;
	int value;
	int length;
	int edit;
	bool focus;
};



#endif /* INCLUDES_MENU_STRINGEDIT_H_ */
