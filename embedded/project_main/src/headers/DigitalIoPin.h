#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_


class DigitalIoPin {
public:
	DigitalIoPin(const int& port, const int& pin, const bool& input = false,
			     const bool& pullup = false, const bool& invert = false);
	DigitalIoPin(const DigitalIoPin &) = delete;
	virtual ~DigitalIoPin() {};
	bool read();
	void write(bool value);
private:
	const int io_port;
	const int io_pin;
	const bool is_input;
	const bool is_invert;
	void init_as_input(const bool& pullup);
	void init_as_output();

};


#endif /* DIGITALIOPIN_H_ */
