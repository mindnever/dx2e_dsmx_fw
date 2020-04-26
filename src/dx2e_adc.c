#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>

#include "dx2e_adc.h"
#include "common.h"
#include "mixer.h"

#define OVERSAMPLE 32

volatile uint16_t dx2e_adc_raw[NUM_ADC_CHANNELS * OVERSAMPLE];
volatile uint16_t dx2e_adc_filtered[NUM_ADC_CHANNELS];

//static uint32_t dx2e_adc_ztmp[NUM_ADC_CHANNELS];
static unsigned oversample = 0;

void dma1_channel1_isr(void)
{
	if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL1, DMA_TCIF)) {
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL1, DMA_TCIF);
		dma_disable_channel(DMA1, DMA_CHANNEL1);
		
		uint32_t tmp[NUM_ADC_CHANNELS] = { 0 };
		volatile uint16_t *raw = dx2e_adc_raw;
		
		for(unsigned i = 0; i < OVERSAMPLE; ++i) {
			for(unsigned j = 0; j < NUM_ADC_CHANNELS; ++j) {
				tmp[j] += *raw++;
			}
		}
		
		for(unsigned j = 0; j < NUM_ADC_CHANNELS; ++j) {
			dx2e_adc_filtered[j] = tmp[j] / OVERSAMPLE;
		}

		++oversample;
		if(oversample >= OVERSAMPLE) {
			oversample = 0;
		}
		
		dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) &dx2e_adc_raw[NUM_ADC_CHANNELS * oversample]);
		dma_enable_channel(DMA1, DMA_CHANNEL1);
		adc_start_conversion_regular(ADC1);
#if 0
		// lpf
		for(unsigned i = 0; i < NUM_ADC_CHANNELS; ++i) {
		    // float would be like this
		    // float f = 0.1;
		    // dx2e_adc_filtered[i] = dx2e_adc_filtered[i] * (1 - f) + dx2e_adc_raw[i] * f;
		    int m = 3;
		    
		    
		    dx2e_adc_ztmp[i] = dx2e_adc_ztmp[i] - (dx2e_adc_ztmp[i] >> m) + dx2e_adc_raw[i];
		    dx2e_adc_filtered[i] = dx2e_adc_ztmp[i] >> m;
		}
#endif
	}
}

void dx2e_adc_init()
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_periph_clock_enable(RCC_DMA);

	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0|GPIO1|GPIO2|GPIO3|GPIO4|GPIO5|GPIO6|GPIO7);

        nvic_set_priority(NVIC_DMA1_CHANNEL1_IRQ, 0);
        nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

	adc_power_off(ADC1);
	
	adc_set_single_conversion_mode(ADC1);
	adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);

	// adc_set_regular_sequence()
        ADC_CHSELR(ADC1) = ADC_CHSELR_CHSEL(0) | ADC_CHSELR_CHSEL(1) | ADC_CHSELR_CHSEL(2) | ADC_CHSELR_CHSEL(3) 
                         | ADC_CHSELR_CHSEL(4) | ADC_CHSELR_CHSEL(5) | ADC_CHSELR_CHSEL(6) | ADC_CHSELR_CHSEL(7);
        ADC_CFGR1(ADC1) &= ~ADC_CFGR1_SCANDIR; // step up

	dma_channel_reset(DMA1, DMA_CHANNEL1);
	dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_LOW);
	dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
	dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t) &ADC1_DR);
	/* The array adc_values[] is filled with the waveform data to be output */
	dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) dx2e_adc_raw);
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, NUM_ADC_CHANNELS);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);
	dma_enable_channel(DMA1, DMA_CHANNEL1);


	adc_enable_dma(ADC1);

	adc_power_on(ADC1);
	
	adc_start_conversion_regular(ADC1);
}

void ADC_Filter()
{
}


s32 ADC_NormalizeChannel(int channel)
{
    s32 value = dx2e_adc_filtered[channel];
    s32 max = Transmitter.calibration[channel].max;
    s32 min = Transmitter.calibration[channel].min;
    s32 zero = Transmitter.calibration[channel].zero;
    if (!zero) {
        // If this input doesn't have a zero, calculate from max/min
        zero = ((u32)max + min) / 2;
    }
    // Derate min and max by 1% to ensure we can get all the way to 100%
    max = (max - zero) * 99 / 100;
    min = (min - zero) * 99 / 100;
    if (value >= zero) {
        value = (value - zero) * CHAN_MAX_VALUE / max;
    } else {
        value = (value - zero) * CHAN_MIN_VALUE / min;
    }

    // Bound output
    if (value > CHAN_MAX_VALUE)
        value = CHAN_MAX_VALUE;
    if (value < CHAN_MIN_VALUE)
        value = CHAN_MIN_VALUE;

//    #define ADC_CHAN(x, y, z) (z)
//    const s8 chan_inverted[NUM_ADC_CHANNELS] = ADC_CHANNELS;
//    #undef ADC_CHAN
//    value = value * chan_inverted[channel - 1];
    return value;
}

s32 ADC_ReadRawInput(int channel)
{
	return dx2e_adc_filtered[channel];
}
