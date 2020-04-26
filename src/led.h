#ifndef _LED_H_
#define _LED_H_

typedef enum
{
    blink_off,
    blink_slow,
    blink_fast,
    blink_on,
    blink_pulse
} blink_t;

extern volatile uint8_t led_a;
extern volatile uint8_t led_b;

void LED_Handle(void);
void LED_Init(void);

#endif /* _LED_H_ */
