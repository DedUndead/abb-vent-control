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
 * No effect for input.
 */
void DigitalIoPin::write(bool value)
{
	if (is_input)  return;
	if (is_invert) value = !value;

	Chip_GPIO_SetPinState(LPC_GPIO, io_port, io_pin, value);
}

/**
 * @brief Configure interrupt channel for the GPIO pin in INMUX block
 * Has no effect on output
 * @param pin_irq_index PININT index used for GPIO mapping
 */
void DigitalIoPin::enable_interrupt(const int& pin_irq_index)
{
	if (!is_input) return;
	if (pin_irq_index < 0 || pin_irq_index > 7) return;

	Chip_INMUX_PinIntSel(pin_irq_index, io_port, io_pin);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(pin_irq_index));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(pin_irq_index));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(pin_irq_index));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ((IRQn_Type)(PIN_INT0_IRQn + pin_irq_index));
	NVIC_EnableIRQ((IRQn_Type)(PIN_INT0_IRQn + pin_irq_index));
}

/**
 * @brief Initialize pin interrupt
 * Static, effects entire system
 */
void DigitalIoPin::init_gpio_interrupts()
{
	/* Initialize PININT driver */
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
	Chip_SYSCTL_PeriphReset(RESET_PININT);
}
