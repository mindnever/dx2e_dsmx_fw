#include "common.h"
#include "hardware.h"
#include "led.h"

#define TICK_FAST (100/LOW_PRIORITY_MSEC)
#define TICK_SLOW (500/LOW_PRIORITY_MSEC)

volatile uint8_t led_a = blink_off;
volatile uint8_t led_b = blink_off;

#define LED_ON_G GPIO_pin_set(LED_G_PIN)
#define LED_OFF_G GPIO_pin_clear(LED_G_PIN)
#define LED_ON_R GPIO_pin_clear(LED_R_PIN)
#define LED_OFF_R GPIO_pin_set(LED_R_PIN)

static uint8_t led_state(blink_t b, uint8_t slow, uint8_t fast)
{
    if(b == blink_fast) { b = fast ? blink_on : blink_off; }
    if(b == blink_slow) { b = slow ? blink_on : blink_off; }
    
    return b == blink_on;
}

void LED_Handle(void)
{
    static uint8_t fast = 0;
    static uint8_t slow = 0;
    static uint16_t pulse_a = 0;
    static uint16_t pulse_b = 0;
    static uint16_t tick_f = 0, tick_s = 0;

    if(led_a == blink_pulse) {
      pulse_a = TICK_SLOW;
      led_a = blink_off;
    }
    
    if(led_b == blink_pulse) {
      pulse_b = TICK_SLOW;
      led_b = blink_off;
    }
    
    if(pulse_a > 0) {
      --pulse_a;
    }
    if(pulse_b > 0) {
      --pulse_b;
    }

    if(++tick_f > TICK_FAST) {
        fast = !fast;
        tick_f = 0;
    }
    
    if(++tick_s > TICK_SLOW) {
        slow = !slow;
        tick_s = 0;
    }
#ifdef LED_G_PIN    
    if(led_state( (pulse_a > 0) ? blink_on : led_a, slow, fast)) {
        LED_ON_G;
    } else {
        LED_OFF_G;
    }
#endif
#ifdef LED_R_PIN    
    if(led_state( (pulse_b > 0) ? blink_on : led_b, slow, fast)) {
        LED_ON_R;
    } else {
        LED_OFF_R;
    }
#endif
}

void LED_Init()
{
	GPIO_pin_mode(LED_G_PIN, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	GPIO_pin_mode(LED_R_PIN, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	LED_OFF_G;
	LED_OFF_R;
}
