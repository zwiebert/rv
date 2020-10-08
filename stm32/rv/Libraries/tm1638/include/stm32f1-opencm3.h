/**
 * \file stm32f1-opencm3.h
 * \brief Wrappers around some opencm3 functions.
 * \author: bertw
 */

#ifndef TM1638_SRC_STM32F1_OPENCM3_H_
#define TM1638_SRC_STM32F1_OPENCM3_H_

typedef struct  {
	uint32_t port;
	uint16_t pin;
} gpio_pin_T;

#define my_set_pin(Pin) gpio_set(Pin.port, Pin.pin)
#define my_clear_pin(Pin) gpio_clear(Pin.port, Pin.pin)
#define my_put_pin(Pin, value) (value) ? my_set_pin(Pin) : my_clear_pin(Pin)
#define my_get_pin(Pin) gpio_get(Pin.port, Pin.pin)

#define my_pin_input_float(Pin)  	gpio_set_mode(Pin.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, Pin.pin)
#define my_pin_input_pull_down(Pin)  	(gpio_set_mode(Pin.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, Pin.pin), my_clear_pin(Pin))
#define my_pin_input_pull_up(Pin)  	(gpio_set_mode(Pin.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, Pin.pin), my_set_pin(Pin))
#define my_pin_output_open(Pin)     gpio_set_mode(Pin.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, Pin.pin)

#endif /* TM1638_SRC_STM32F1_OPENCM3_H_ */
