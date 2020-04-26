#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static inline void GPIO_pin_af(struct mcu_pin pin, uint8_t af)
{
    gpio_set_af(pin.port, af, pin.pin);
}

static inline void GPIO_pin_mode(struct mcu_pin pin, uint8_t mode, uint8_t pull_up_down)
{
    gpio_mode_setup(pin.port, mode, pull_up_down, pin.pin);
}

static inline void GPIO_pin_set(struct mcu_pin pin)
{
    gpio_set(pin.port, pin.pin);
}

static inline void GPIO_pin_clear(struct mcu_pin pin)
{
    gpio_clear(pin.port, pin.pin);
}

static inline uint16_t GPIO_pin_get(struct mcu_pin pin)
{
    return gpio_get(pin.port, pin.pin);
}

#define LED_G_PIN ((struct mcu_pin) { \
        .port = GPIOB, \
        .pin = GPIO6 \
    })

#define LED_R_PIN ((struct mcu_pin) { \
        .port = GPIOA, \
        .pin = GPIO8 \
    })

#define POWER_PIN ((struct mcu_pin) { \
        .port = GPIOB, \
        .pin = GPIO1 \
    })

#define BIND_PIN ((struct mcu_pin) { \
        .port = GPIOB, \
        .pin = GPIO2 \
    })

#define TH_SW_PIN ((struct mcu_pin) { \
        .port = GPIOB, \
        .pin = GPIO7 \
    })

#define ST_SW_PIN ((struct mcu_pin) { \
        .port = GPIOB, \
        .pin = GPIO8 \
    })

#define TP3_PIN ((struct mcu_pin) { \
        .port = GPIOF, \
        .pin = GPIO1 \
    })

#define SYSCLK_TIM ((struct tim_config) { \
    .tim = TIM14,   \
    .rcc = RCC_TIM14, \
    .rst = RST_TIM14, \
    .irq = NVIC_TIM14_IRQ \
    })
#define SYSCLK_TIMER_ISR tim14_isr

#define CYRF6936_CONFIG ((struct CYRF6936_Config) { \
        .spi = SPI1, \
        .rcc = RCC_SPI1, \
        .rst = { .port = GPIOB, .pin = GPIO0 }, \
        .cs = { .port = GPIOA, .pin = GPIO15 }, \
        .miso = { .port = GPIOB, .pin = GPIO4 }, \
        .mosi = { .port = GPIOB, .pin = GPIO5 }, \
        .sck = { .port = GPIOB, .pin = GPIO3 }, \
        .tx = { .port = GPIOA, .pin = GPIO11 }, \
        .rx = { .port = GPIOA, .pin = GPIO12 }, \
    })

#endif /* _HARDWARE_H_ */
