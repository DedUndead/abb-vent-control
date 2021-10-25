#include "StringEdit.h"
#include <cstdio>
#include <string>

StringEdit::StringEdit(LiquidCrystal *lcd_, std::string editTitle, std::string* items_, int n):
lcd(lcd_), title(editTitle), items(items_), length(n)
{
	value = 0;
	edit = 0;
	focus = false;
}

StringEdit::~StringEdit() {
}

void StringEdit::increment() {
	if (edit + 1 > length - 1) return;
	edit++;
}

void StringEdit::decrement() {
	if (edit - 1 < 0) return;
	edit--;
}

void StringEdit::accept() {
	save();
}

void StringEdit::cancel() {
	edit = value;
}


void StringEdit::setFocus(bool focus) {
	this->focus = focus;
}

bool StringEdit::getFocus() {
	return this->focus;
}

void StringEdit::display() {
	lcd->clear();
	lcd->print(title);
	char s[17];
	if(focus) {
		snprintf(s, 17, "  [%9s]   ", &(items[edit][0]));
	}
	else {
		snprintf(s, 17, "   %9s    ", &(items[edit][0]));
	}
	lcd->print(s, 16);
}


void StringEdit::save() {
	// set current value to be same as edit value
	value = edit;
	// todo: save current value for example to EEPROM for permanent storage
}

int StringEdit::getValue() {
	return value;
}

void StringEdit::setValue(int value) {
	edit = value;
	save();
}
