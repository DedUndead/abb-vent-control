#include "chip.h"
#include "headers/digitaliopin.h"

/**
 * @brief Initialize IO pin
 * @param port   Target port number
 * @param pin    Target pin number
 * @param input  Is input
 * @param pullup Input pullup setting
 * @param invert Invert setting for both input/output
 */
DigitalIoPin::DigitalIoPin(const int& port, const int& pin, const bool& input, const bool& pullup, const bool& invert) :
			       		   io_port(port), io_pin(pin), is_input(input), is_invert(invert)
{
	if (input) init_as_input(pullup);
	else 	   init_as_output();
}

/**
 * @brief Initialize pin as input
 * @param pullup Pullup setting
 */
void DigitalIoPin::init_as_input(const bool& pullup)
{
	// Construct modfunction based on passed parameters
	uint32_t modfunc = IOCON_DIGMODE_EN;
	if (pullup) {
		modfunc = (modfunc | IOCON_MODE_PULLUP);
	}
	else {
		modfunc = (modfunc | IOCON_MODE_PULLDOWN);
	}
	if (is_invert) {
		modfunc = (modfunc | IOCON_INV_EN);
	}
	
	Chip_IOCON_PinMuxSet(LPC_IOCON, io_port, io_pin, modfunc);
    Chip_GPIO_SetPinDIRInput(LPC_GPIO, io_port, io_pin);
}

/**
 * @brief Initialize pin as output
 */
void DigitalIoPin::init_as_output()
{
    Chip_IOCON_PinMuxSet(LPC_IOCON, io_port, io_pin, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, io_port, io_pin);
}

/**
 * @brief Read state of the pin
 * @return Boolean state, depending on inversion settings
 */
bool DigitalIoPin::read()
{
	bool state = Chip_GPIO_GetPinState(LPC_GPIO, io_port, io_pin);
	// Invert output if output set is invert
	if (!is_input && is_invert) state = !state;

	return state;
}

/**
 * @brief Write state of the output pin
 */
void DigitalIoPin::write(bool value)
{
	if (is_input)  return;
	if (is_invert) value = !value;

	Chip_GPIO_SetPinState(LPC_GPIO, io_port, io_pin, value);
}
