#ifndef AVM16_H
#define AVM16_H

#define AVM16_IDENT        0x000
#define AVM16_SERIAL       0x004
#define AVM16_COM_IDS      0x008
#define AVM16_MOD_TYPE     0x00c
#define AVM16_STATE        0x010
#define AVM16_DLENGTH      0x014
#define AVM16_TP_DAQ       0x01c
#define AVM16_OFFSET_DAC   0x020
#define AVM16_JTAG_CSR     0x030
#define AVM16_JTAG_DATA    0x034
#define AVM16_CR           0x100
#define AVM16_ACT          0x104
#define AVM16_CHA_INH      0x108
#define AVM16_CHA_RAW      0x10c
#define AVM16_TRG_LEVEL    0x110
#define AVM16_ANAL_CTRL    0x114
#define AVM16_IW_START     0x118   
#define AVM16_IW_LENGTH    0x11c
#define AVM16_SW_START     0x120
#define AVM16_SW_LENGTH    0x124
#define AVM16_SW_INTLENGTH 0x128
#define AVM16_ACLCK_SHIFT  0x12c
#define AVM16_LB_TEST      0x130
#define AVM16_BASE_LINE    0x200
#define AVM16_NOISE_LEVEL  0x240
#define AVM16_Q_THRESHOLD  0x280
#define AVM16_DATA_RANGE   0x400

#endif
