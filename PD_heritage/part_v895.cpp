#define _POSIX_C_SOURCE 199309L
#include <string.h>
#include <stdio.h>

#include "vmelib.h"
#include "param.h"
 
#include "v895.h"

#include "main.h"
#include "readout_class.h"


void Readout_class_v895::init_module_variables(void){
  v874class_module=0;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 16;  // not including header/trail
  adc_range= 0; 
  strncpy(modname,"v895-LED", 19); // for screen output
  strncpy(mod_ident,"v895_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

int Readout_class_v895::read_one_event(int noe, int & ok){
    return 0;
}

void Readout_class_v895::lam(void){
}

void Readout_class_v895::reset(void){
}

void Readout_class_v895::bsy_on(void){
}

void Readout_class_v895::bsy_off(void){
}

int Readout_class_v895::get_geo(void){
   return geo;
}

void Readout_class_v895::set_geo(int int_geo){
  geo=int_geo;
}

void Readout_class_v895::printeventdata(unsigned int noe){
}

int Readout_class_v895::save_histo(FILE * hisout){
    fwrite(&adc_range, sizeof(adc_range), 1, hisout);
    fwrite(&max_no_rawdata, sizeof(max_no_rawdata), 1, hisout);
    return 0;
}

int Readout_class_v895::save_event(void){
}


void Readout_class_v895::init_module(Param *param)
{
  for (int i = 0; i < 16; i++) {
    char buf[16];
    sprintf(buf, "threshold%02d", i);
    vme.ww(vme_image, cardBase+V895_BASE_THRSH_REG + 2 * i, 
      Param_getVal(param, mod_ident_compl, buf));
  };
  vme.ww(vme_image, cardBase+V895_OUTP_W_0_7, 
    Param_getVal(param, mod_ident_compl, "width_1"));
  vme.ww(vme_image, cardBase+V895_OUTP_W_8_15, 
    Param_getVal(param, mod_ident_compl, "width_2"));
  vme.ww(vme_image, cardBase+V895_MAJORITY_THRSH , 
    Param_getVal(param, mod_ident_compl, "major_thrsh"));
  vme.ww(vme_image, cardBase+V895_PATTERN_INH, 
    Param_getVal(param, mod_ident_compl, "pattern_inh"));
  geo=Param_getVal(param, mod_ident_compl, "geo");
}























