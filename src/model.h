#ifndef _MODEL_H_
#define _MODEL_H_

#define NUM_PROTO_OPTS 8

struct Model {
    u32 fixed_id;
    u8 num_channels;
    enum Protocols protocol;
    s16 proto_opts[NUM_PROTO_OPTS];
    enum TxPower tx_power;
    struct Limit limits[NUM_OUT_CHANNELS];
};

extern struct Model Model;

#endif /* _MODEL_H_ */
