#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "v792.h"

#include "main.h"
#include "readout_class.h"

void Readout_class_v792::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"V792-QDC", 19); // for screen output
  strncpy(mod_ident,"v792_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_v792::init_module(Param *param)
{
  int i;

  vme.ww(vme_image, cardBase+V792_EVENT_TRIGGER_REGISTER, 
     Param_getVal(param, mod_ident_compl, "evnt_trg_reg"));

  i= Param_getVal(param, mod_ident_compl, "iped");
  if(i<60){
    i=60;
    printf("IPed should be >=60, set to 60.\n");
  }
  vme.ww(vme_image, cardBase+V792_IPED, i);
}

