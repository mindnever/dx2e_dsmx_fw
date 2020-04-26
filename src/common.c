#include "common.h"
#include "mixer.h"
#include "model.h"
#include "dx2e_adc.h"
#include "protospi.h"

struct Model Model;
struct Telemetry Telemetry;
struct Transmitter Transmitter = {
    .txid = 0,
    .calibration = {
        [ADC_CHANNEL_STEERING] = { .max = 3724, .min = 271, .zero = 2009 },
        [ADC_CHANNEL_THROTTLE] = { .max = 3064, .min = 880, .zero = 1724 },
        [ADC_CHANNEL_ST_TRIM] = { .max = 4030, .min = 0, .zero = 1864 },
        [ADC_CHANNEL_TH_TRIM] = { .max = 4030, .min = 0, .zero = 1880 },
        [ADC_CHANNEL_ST_RATE] = { .max = 4030, .min = 0, .zero = 1880 },
        [ADC_CHANNEL_AUX] = { .max = 4030, .min = 0, .zero = 1969 },
    }
};
volatile s32 Channels[NUM_OUT_CHANNELS];

struct CYRF6936_Config CYRF6936_Config = CYRF6936_CONFIG;

void COMMON_Init()
{
    memset(&Model, 0, sizeof(Model));
    memset((void *)&Channels, 0, sizeof(Channels));

    Model.fixed_id = 1;
    Model.num_channels = 7;
    Model.protocol = PROTOCOL_DSMX;
    Model.tx_power = TXPOWER_150mW;
//    Model.proto_opts = 
//    Model.limits =

    memset(&Telemetry, 0, sizeof(Telemetry));

    // also init SPI here
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    
    GPIO_pin_mode(CYRF6936_Config.rst, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    GPIO_pin_mode(CYRF6936_Config.cs, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    GPIO_pin_mode(CYRF6936_Config.tx, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    GPIO_pin_mode(CYRF6936_Config.rx, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    
    GPIO_pin_clear(CYRF6936_Config.rst);
    GPIO_pin_clear(CYRF6936_Config.tx);
    GPIO_pin_clear(CYRF6936_Config.rx);
    GPIO_pin_set(CYRF6936_Config.cs);
    
    GPIO_pin_mode(CYRF6936_Config.sck, GPIO_MODE_AF, GPIO_PUPD_NONE);
    GPIO_pin_mode(CYRF6936_Config.mosi, GPIO_MODE_AF, GPIO_PUPD_NONE);
    GPIO_pin_mode(CYRF6936_Config.miso, GPIO_MODE_AF, GPIO_PUPD_PULLUP);

    // miso, mosi, sck - 
    GPIO_pin_af(CYRF6936_Config.sck, GPIO_AF0);
    GPIO_pin_af(CYRF6936_Config.mosi, GPIO_AF0);
    GPIO_pin_af(CYRF6936_Config.miso, GPIO_AF0);

    rcc_periph_clock_enable(CYRF6936_Config.rcc);

    spi_reset(CYRF6936_Config.spi);

    spi_init_master(CYRF6936_Config.spi,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_8,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_MSBFIRST);
                    
    spi_set_data_size(CYRF6936_Config.spi, SPI_CR2_DS_8BIT);

    spi_enable_software_slave_management(CYRF6936_Config.spi);
    spi_set_nss_high(CYRF6936_Config.spi);
    spi_fifo_reception_threshold_8bit(CYRF6936_Config.spi);
    spi_enable(CYRF6936_Config.spi);    

    priority_ready = 0;
    CLOCK_SetMsecCallback(LOW_PRIORITY, LOW_PRIORITY_MSEC);
    CLOCK_SetMsecCallback(MEDIUM_PRIORITY, MEDIUM_PRIORITY_MSEC);
}

u8 PROTOSPI_xfer(u8 byte)
{
        SPI_DR8(CYRF6936_Config.spi) = byte;

        /* Wait for transfer finished. */
        while (!(SPI_SR(CYRF6936_Config.spi) & SPI_SR_RXNE));

        return SPI_DR8(CYRF6936_Config.spi);
}

#define DEBOUNCE 4

void MIXER_CalcChannels()
{
    static unsigned ch3_button = 0;
    static unsigned ch3_val = 0;
    
    // ADC_CHANNEL_STEERING => Channel[0] [ direction based on ST_SW_PIN ] [ offset by ADC_CHANNEL_ST_TRIM ] [ scaled by ADC_CHANNEL_ST_RATE ]
    // ADC_CHANNEL_THROTTLE => Channel[1] [ direction based on ST_TH_PIN ] [ offset by ADC_CHANNEL_TH_TRIM ]

    // DSM channel ordering TAERGa
    

    if(GPIO_pin_get(BIND_PIN)) {
        ch3_button = 0;
    } else if(ch3_button < DEBOUNCE) {
        ++ch3_button;
        if(ch3_button == DEBOUNCE) { // trigger
            ch3_val = !ch3_val;
        }
    }

    Channels[0] = ADC_NormalizeChannel(ADC_CHANNEL_STEERING) + (ADC_NormalizeChannel(ADC_CHANNEL_ST_TRIM) / 10);
    Channels[1] = ADC_NormalizeChannel(ADC_CHANNEL_THROTTLE) + (ADC_NormalizeChannel(ADC_CHANNEL_TH_TRIM) / 10);
    Channels[2] = ch3_val ? CHAN_MAX_VALUE : CHAN_MIN_VALUE;
    Channels[3] = ADC_NormalizeChannel(ADC_CHANNEL_ST_RATE);
    Channels[4] = ADC_NormalizeChannel(ADC_CHANNEL_AUX);

    if((Channels[5] < 10) || (Channels[5] > -10)) {
        Channels[5] = 0;
    }

    if(GPIO_pin_get(TH_SW_PIN)) {
        Channels[1] = -Channels[1];
    }
    if(GPIO_pin_get(ST_SW_PIN)) {
        Channels[0] = -Channels[0];
    }
}


void TELEMETRY_SetUpdated(int telem)
{
    (void)telem;
}

int PROTOCOL_SticksMoved(int init)
{
    (void)init;
    return 0;
}

void PROTOCOL_SetBindState(u32 msec)
{
    (void)msec;
}


void MCU_SerialNumber(u8 *var, int len)
{
    if(Transmitter.txid) {
        int l = len > 16 ? 16 : len;
        u32 id[4];
        u32 seed = 0x4d3ab5d0ul;
        for(int i = 0; i < 4; i++)
            rand32_r(&seed, Transmitter.txid >> (8*i));
        for(int i = 0; i < 4; i++)
            id[i] = rand32_r(&seed, 0x00);
        memcpy(var, &id[0], l);
        return;
    }
    int l = len > 12 ? 12 : len;
    // Every STM32 should have 12 bytes long unique id at 0x1FFFF7E8
    const u8 *stm32id = (u8*) 0x1FFFF7E8;
    for(int i = 0; i < l; i++) {
        var[i] = *stm32id++;
    }
    while(l < len) {
        var[l++] = 0x00;
    }
}


static u32 rand_seed = 0xb2c54a2ful;
// Linear feedback shift register with 32-bit Xilinx polinomial x^32 + x^22 + x^2 + x + 1
static const uint32_t LFSR_FEEDBACK = 0x80200003ul;
static const uint32_t LFSR_INTAP = 32-1;
static void update_lfsr(uint32_t *lfsr, uint8_t b)
{
    for (int i = 0; i < 8; ++i) {
        *lfsr = (*lfsr >> 1) ^ ((-(*lfsr & 1u) & LFSR_FEEDBACK) ^ ~((uint32_t)(b & 1) << LFSR_INTAP));
        b >>= 1;
    }
}

u32 rand32_r(u32 *seed, u8 update)
{
    if(! seed)
        seed = &rand_seed;
    update_lfsr(seed, update);
    return *seed;
}

u32 rand32()
{
    return rand32_r(0, 0);
}

