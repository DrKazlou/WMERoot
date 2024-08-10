//#include "readout_class.h"

typedef struct{
    unsigned int ADC_Mem[8];
    unsigned int Event_Config_ALL_ADC;
    unsigned int Sample_Address_ADC[8];
    unsigned int Trigger_Setup_ADC[8];
    unsigned int Trigger_Threshold_ADC[8];
    unsigned int Trigger_Flag_Clear[4];
    unsigned int Event_Directory_ADC[8];

}ReducedPointers_t;

void fnExit (void);
