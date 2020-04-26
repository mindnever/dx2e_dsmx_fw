#include "miniprintf.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "uart_util.h"


void uart_init(void)
{
        rcc_periph_clock_enable(RCC_GPIOA);

        /* Enable clocks for USART1. */
        rcc_periph_clock_enable(RCC_USART1);
        
        /* Setup GPIO pins for USART1 transmit. */
        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

        /* Setup USART1 TX pin as alternate function. */
        gpio_set_af(GPIOA, GPIO_AF1, GPIO9);

        usart_set_baudrate(USART1, 115200);
        usart_set_databits(USART1, 8);
        usart_set_parity(USART1, USART_PARITY_NONE);
        usart_set_stopbits(USART1, USART_CR2_STOPBITS_1);
        usart_set_mode(USART1, USART_MODE_TX_RX);
        usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

        /* Finally enable the USART. */
        usart_enable(USART1);
}

static inline void uart_putc(char ch)
{
	usart_send_blocking(USART1,ch);
}

int uart_printf(const char *format,...)
{
	va_list args;
	int rc;

	va_start(args,format);
	rc = mini_vprintf_cooked(uart_putc,format,args);
	va_end(args);
	return rc;
}

