#ifndef _DX2E_ADC_H_
#define _DX2E_ADC_H_

enum {
    ADC_CHANNEL_STEERING = 0,
    ADC_CHANNEL_THROTTLE = 1,
    ADC_CHANNEL_ST_TRIM = 2,
    ADC_CHANNEL_TH_TRIM = 3,
    ADC_CHANNEL_RF = 4,
    ADC_CHANNEL_ST_RATE = 5,
    ADC_CHANNEL_BATTERY = 6,
    ADC_CHANNEL_AUX = 7,
};

#define NUM_ADC_CHANNELS 8

void dx2e_adc_init(void);

#endif /* _DX2E_ADC_H_ */
