#define _POSIX_C_SOURCE 199309L
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib> 
//#include <hadesstd.h>
#include "vmelib.h"
#include "param.h" 
#include "v785.h"

#include "main.h"

void Readout_class_v785::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"V785-PSADC", 19); // for screen output
  strncpy(mod_ident,"v785_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_v785::init_module(Param *param)
{
  vme.ww(vme_image, cardBase+V785_EVENT_TRIGGER_REGISTER, Param_getVal(param, mod_ident_compl, "evnt_trg_reg"));
}
