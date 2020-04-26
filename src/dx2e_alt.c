/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2013 Chuck McManis <cmcmanis@mcmanis.com>
 * Copyright (C) 2013 Onno Kortmann <onno@gmx.net>
 * Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz> (merge)
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>

#include "uart_util.h"
#include "dx2e_adc.h"
#include "led.h"
#include "common.h"

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	GPIO_pin_mode(POWER_PIN, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	GPIO_pin_mode(BIND_PIN, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	GPIO_pin_mode(TH_SW_PIN, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	GPIO_pin_mode(ST_SW_PIN, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
}

static void clock_setup_hse(void)
{
         rcc_set_hpre(RCC_CFGR_HPRE_NODIV);
         rcc_set_ppre(RCC_CFGR_PPRE_NODIV);

         flash_prefetch_enable();
         flash_set_ws(FLASH_ACR_LATENCY_024_048MHZ);

	 rcc_osc_bypass_enable(RCC_HSE);
         rcc_osc_on(RCC_HSE);
         rcc_wait_for_osc_ready(RCC_HSE);

         rcc_set_sysclk_source(RCC_HSE);

         while(rcc_system_clock_source() != RCC_HSE);

         rcc_apb1_frequency = 12000000;
         rcc_ahb_frequency = 12000000;

         // rcc_osc_off(RCC_PLL); // this is NO-OP in libopencm3
         RCC_CR &= ~RCC_CR_PLLON;
         
         /* PLL: 12MHz * 4 = 48MHz */
         rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_MUL4);
         rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);
         rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

         rcc_osc_on(RCC_PLL);
         rcc_wait_for_osc_ready(RCC_PLL);
         rcc_set_sysclk_source(RCC_PLL);
         

         rcc_apb1_frequency = 24000000;
         rcc_ahb_frequency = 24000000;

#if 0
         gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
         gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);
         gpio_set_af(GPIOA, 0, GPIO8);
         rcc_set_mco(RCC_CFGR_MCO_SYSCLK);
#endif
}

int main(void)
{
	// initial clock

	rcc_clock_setup_in_hsi_out_48mhz();

	gpio_setup();
	
//	gpio_set(GPIOB, GPIO1); // delay poweroff? 
	
	uart_init();

	uart_printf("Booting: ADC");
	
	dx2e_adc_init();

	uart_printf(" CLOCK");

	CLOCK_Init();

	LED_Init();

	led_a = blink_on;
	led_b = blink_off;

	uart_printf(" COMMON");

	COMMON_Init();
	
	uart_printf(" DSM2-");
	
	if(GPIO_pin_get(BIND_PIN)) {
		uart_printf("INIT");
		DSM2_Cmds(PROTOCMD_INIT);
	} else {
		uart_printf("BIND");
		DSM2_Cmds(PROTOCMD_BIND);
		led_a = blink_fast;
	}

	clock_setup_hse();	
	uart_printf("Entering main loop\r\n");
	
	/* Do nothing in main loop */
	while (1) {
//		for(unsigned i = 0; i < NUM_ADC_CHANNELS; ++i) {
//			uart_printf("ADC[%d] = %-6d ", i, ADC_ReadRawInput(i));
//		}

		s32 v = ADC_ReadRawInput(ADC_CHANNEL_BATTERY);
		
		if(v < 1000) {
			led_b = blink_slow;
		} else if(v > 1100) {
			led_b = blink_off;
		}

		uart_printf("V = %5d ", ADC_ReadRawInput(ADC_CHANNEL_BATTERY));

		for(unsigned i = 0; i < 7; ++i) {
			uart_printf("CH[%d] = %-5d ", i, Channels[i]);
		}
		
		uart_printf("\r\n");
	}
}
