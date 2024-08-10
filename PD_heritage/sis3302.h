#ifndef SIS_3302_H
#define SIS_3302_H

#define SIS3302_NCHANNELS 8

#define SIS3302_ContrStat      0x000
#define SIS3302_ModuleID_Firm  0x004
#define SIS3302_Interrupt_Conf 0x008
#define SIS3302_Interrupt_Cntr 0x00C
//
#define SIS3302_AcqCntrStat    0x010
#define SIS3302_ExtStartDelay  0x014
#define SIS3302_ExtStopDelay   0x018

#define SIS3302_MaxNumbOfEvents 0x20
#define SIS3302_ActualEvCounter 0x24

#define SIS3302_CBLT_Setup     0x030
#define SIS3302_ADC_MemPage    0x034

#define SIS3302_DAC_CntrStat   0x050
#define SIS3302_DAC_Data       0x054
//
#define SIS3302_JTAG_Test_DataIn 0x60
#define SIS3302_JTAG_Cntr      0x064
//
#define SIS3302_GeneralReset    0x400
//

#define SIS3302_ArmSampling     0x410
#define SIS3302_DisarmSampling  0x414
#define SIS3302_Sampling        0x418
#define SIS3302_StopSampling    0x41C

#define SIS3302_TimestampClear  0x42C
//
//...................................................................
//
#define SIS3302_ADC_Mem_1 0x04000000
#define SIS3302_ADC_Mem_2 0x04800000
#define SIS3302_ADC_Mem_3 0x05000000
#define SIS3302_ADC_Mem_4 0x05800000
#define SIS3302_ADC_Mem_5 0x06000000
#define SIS3302_ADC_Mem_6 0x06800000
#define SIS3302_ADC_Mem_7 0x07000000
#define SIS3302_ADC_Mem_8 0x07800000
//
#define SIS3302_Event_Config_ALL_ADC 0x01000000
//-------------------------------------------------------
#define SIS3302_Sample_Address_ADC1 0x02000010
#define SIS3302_Sample_Address_ADC2 0x02000014
#define SIS3302_Sample_Address_ADC3 0x02800010
#define SIS3302_Sample_Address_ADC4 0x02800014
#define SIS3302_Sample_Address_ADC5 0x03000010
#define SIS3302_Sample_Address_ADC6 0x03000014
#define SIS3302_Sample_Address_ADC7 0x03800010
#define SIS3302_Sample_Address_ADC8 0x03800014
//
//
#define SIS3302_Trigger_Setup_ADC1 0x02000030 
#define SIS3302_Trigger_Setup_ADC2 0x02000038
#define SIS3302_Trigger_Setup_ADC3 0x02800030
#define SIS3302_Trigger_Setup_ADC4 0x02800038
#define SIS3302_Trigger_Setup_ADC5 0x03000030
#define SIS3302_Trigger_Setup_ADC6 0x03000038
#define SIS3302_Trigger_Setup_ADC7 0x03800030
#define SIS3302_Trigger_Setup_ADC8 0x03800038
//
#define SIS3302_Trigger_Threshold_ADC1 0x02000034
#define SIS3302_Trigger_Threshold_ADC2 0x0200003C
#define SIS3302_Trigger_Threshold_ADC3 0x02800034
#define SIS3302_Trigger_Threshold_ADC4 0x0280003C
#define SIS3302_Trigger_Threshold_ADC5 0x03000034
#define SIS3302_Trigger_Threshold_ADC6 0x0300003C
#define SIS3302_Trigger_Threshold_ADC7 0x03800034
#define SIS3302_Trigger_Threshold_ADC8 0x0380003C
//
#define SIS3302_TriggerFlagClear_ADC12 0x0200002C
#define SIS3302_TriggerFlagClear_ADC34 0x0280002C
#define SIS3302_TriggerFlagClear_ADC56 0x0300002C
#define SIS3302_TriggerFlagClear_ADC78 0x0380002C
//
#define SIS3302_EventDirectory_ADC1 0x02010000
#define SIS3302_EventDirectory_ADC2 0x02018000
#define SIS3302_EventDirectory_ADC3 0x02810000
#define SIS3302_EventDirectory_ADC4 0x02818000
#define SIS3302_EventDirectory_ADC5 0x03010000
#define SIS3302_EventDirectory_ADC6 0x03018000
#define SIS3302_EventDirectory_ADC7 0x03810000
#define SIS3302_EventDirectory_ADC8 0x03818000

#endif
