#ifndef SIS_3820_H
#define SIS_3820_H

#define SIS3820_NCHANNELS 6

#define SIS3820_CONTROL_STATUS 0x0
#define SIS3820_MODID 0x4
#define SIS3820_IRQ_CONFIG 0x8
#define SIS3820_IRQ_CONTROL 0xc

#define SIS3820_DATA_D_OUT 0x10
#define SIS3820_DATA_JK_OUT 0x14

#define SIS3820_DATA_IN 0x20
#define SIS3820_DATA_LATCH_IN 0x24

#define SIS3820_LATCH_COUNTER1 0x30
#define SIS3820_LATCH_COUNTER2 0x34
#define SIS3820_LATCH_COUNTER3 0x38
#define SIS3820_LATCH_COUNTER4 0x3c
#define SIS3820_LATCH_COUNTER_CLEAR 0x40

#define SIS3820_KEY_RST 0x60

#endif
