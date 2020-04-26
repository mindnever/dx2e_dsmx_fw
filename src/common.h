#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef SUPPORT_XN297DUMP
#define SUPPORT_XN297DUMP 0
#endif

#ifndef SUPPORT_SCANNER
#define SUPPORT_SCANNER 0
#endif

#ifndef HAS_EXTENDED_AUDIO
#define HAS_EXTENDED_AUDIO 0
#endif

#ifndef HAS_MULTIMOD_SUPPORT
#define HAS_MULTIMOD_SUPPORT 0
#endif

#define MAX_POINTS 13

#define _tr_noop(x) x

#define _UNUSED   __attribute__ ((unused))
#define NO_INLINE __attribute__ ((noinline))
#define ctassert(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
#define cterror(MSG) ((MSG)[0] / 0)

#include "dx2e_adc.h"

void COMMON_Init(void);

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


void _msleep(u32 msec);
void _usleep(u32 usec);
#define usleep _usleep

enum Radio {
    CYRF6936,
};

struct mcu_pin {
    u32 port;
    u16 pin;
};

struct tim_config {
    unsigned tim;
    unsigned rcc;
    unsigned rst;
    unsigned irq;
};

struct CYRF6936_Config {
    u32 spi;
    u32 rcc;
    struct mcu_pin rst;
    struct mcu_pin cs;
    struct mcu_pin miso;
    struct mcu_pin mosi;
    struct mcu_pin sck;
    struct mcu_pin irq;
    struct mcu_pin tx;
    struct mcu_pin rx;
};

extern struct CYRF6936_Config CYRF6936_Config;

void MCU_SerialNumber(u8 *var, int len);

#include "hardware.h"
#include "telemetry.h"
#include "interface.h"

enum TxPower {
    TXPOWER_100uW,
    TXPOWER_300uW,
    TXPOWER_1mW,
    TXPOWER_3mW,
    TXPOWER_10mW,
    TXPOWER_30mW,
    TXPOWER_100mW,
    TXPOWER_150mW,
    TXPOWER_LAST,
};

/* Clock functions */
#define LOW_PRIORITY_MSEC 100
#define MEDIUM_PRIORITY_MSEC   5
enum MsecCallback {
    MEDIUM_PRIORITY,
    LOW_PRIORITY,
    TARGET_PRIORITY,
    NUM_MSEC_CALLBACKS,
};

void CLOCK_Init(void);
u32 CLOCK_getms(void);
void CLOCK_StartTimer(unsigned us, u16 (*cb)(void));
void CLOCK_StopTimer(void);
void CLOCK_SetMsecCallback(int cb, u32 msec);
void CLOCK_ClearMsecCallback(int cb);
void CLOCK_StartWatchdog(void);
void CLOCK_ResetWatchdog(void);
void CLOCK_RunMixer(void);
void CLOCK_StartMixer(void);
typedef enum {
    MIX_TIMER,
    MIX_NOT_DONE,
    MIX_DONE
} mixsync_t;
extern volatile mixsync_t mixer_sync;

#define NUM_OUT_CHANNELS 16

extern volatile s32 Channels[NUM_OUT_CHANNELS];

/* Protocol */
#define PROTODEF(proto, module, map, init, name) proto,
enum Protocols {
    PROTOCOL_NONE = 0,
    #include "protocol.h"
    PROTOCOL_COUNT,
};
#undef PROTODEF

u32 rand32_r(u32 *seed, u8 update);  // LFSR based PRNG
u32 rand32(void);  // LFSR based PRNG

int PROTOCOL_SticksMoved(int init);
void PROTOCOL_SetBindState(u32 msec);

#define INP_HAS_CALIBRATION NUM_ADC_CHANNELS

struct StickCalibration {
    u16 max;
    u16 min;
    u16 zero;
};

struct Transmitter {
    u32 txid;
    struct StickCalibration calibration[INP_HAS_CALIBRATION];
};

extern struct Transmitter Transmitter;

/* Mixer functions */
void MIXER_CalcChannels(void);

void ADC_Filter(void);
s32 ADC_NormalizeChannel(int channel);
s32 ADC_ReadRawInput(int channel);

extern volatile u8 priority_ready;

#endif /* _COMMON_H_ */
