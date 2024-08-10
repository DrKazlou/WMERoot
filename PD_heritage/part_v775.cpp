#include <string.h>
#include <stdio.h>

//#include <hadesstd.h>

#include "vmelib.h"

#include "param.h" 
#include "v874.h"

#include "main.h"
#include "readout_class.h"

void Readout_class_v775::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"v775-board", 19); // for screen output
  strncpy(mod_ident,"v775_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_v775::init_module(Param *param)
{
  vme.ww(vme_image, cardBase+V874_VSET, 
     Param_getVal(param, mod_ident_compl, "vset"));  
}
