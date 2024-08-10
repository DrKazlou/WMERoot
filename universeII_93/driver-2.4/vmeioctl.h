/*
    Some defines for ioctl calls used by universeII.c and vmelib.cpp
    Copyright (C) 2002 Andreas Ehmanns <ehmanns@iskp.uni-bonn.de>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef VMEIOCTL_H
#define VMEIOCTL_H

#define IOCTL_SET_CTL      0xF001
#define IOCTL_SET_IMAGE    0xF002
// 0xF003 and 0xF004 undefined
#define IOCTL_PCI_SIZE     0xF005
#define IOCTL_SET_WINT     0xF007
#define IOCTL_GET_ADDR     0xF008
#define IOCTL_GET_IMAGE    0xF009
#define IOCTL_GEN_VME_IRQ  0xF00A
#define IOCTL_SET_IRQ      0xF00B
#define IOCTL_WAIT_IRQ     0xF00C
#define IOCTL_SET_OPT      0xF00D
#define IOCTL_TEST_ADDR    0xF00E
#define IOCTL_TEST_BERR    0xF00F
#define IOCTL_NEW_DCP      0xF010
#define IOCTL_ADD_DCP      0xF011
#define IOCTL_EXEC_DCP     0xF012
#define IOCTL_DEL_DCL      0xF013
#define IOCTL_REQUEST_DMA  0xF020
#define IOCTL_RELEASE_DMA  0xF021

#define IOCTL_RESET_ALL    0xF023
#define IOCTL_VMESYSRST    0xF024

#define IOCTL_SET_MBX      0xF100
#define IOCTL_WAIT_MBX     0xF101
#define IOCTL_RELEASE_MBX  0xF102


typedef struct
{
    int ms;
    unsigned int base;
    unsigned int size;
} image_regs_t;


typedef struct
{
    unsigned int addr;
    unsigned int count;
    int vas;
    int vdw;
    int dma_ctl;
    int bufNr;
} dma_param_t;


typedef struct
{
    unsigned int dctl;
    unsigned int dtbc;
    unsigned int dva;
    int list;
} list_packet_t;


typedef struct
{
    int vmeIrq;
    int vmeStatus;
    int vmeAddrSt;
    int vmeValSt;
    int vmeAddrCl;
    int vmeValCl;
} irq_setup_t;


typedef struct
{
    unsigned int addr;
    unsigned int mode;
} there_data_t;

#endif
