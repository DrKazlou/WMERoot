#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "v965.h"

#include "main.h"
#include "readout_class.h"


void Readout_class_v965::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"V965_DRQDC", 19); // for screen output
  strncpy(mod_ident,"v965_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_v965::init_module(Param *param)
{
  int i;

  vme.ww(vme_image, cardBase+V965_EVENT_TRIGGER_REGISTER, 
      Param_getVal(param, mod_ident_compl, "evnt_trg_reg"));

  i= Param_getVal(param, mod_ident_compl, "iped");
  if(i<60){
    i=60;
    printf("IPed should be >=60, set to 60.\n");
  }
  vme.ww(vme_image, cardBase+V965_IPED, i);
}


void Readout_class_v965::printeventdata(unsigned int noe){
  if(modulenumber==0) printf("\n");	 
  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);
  for(int n=0; n<max_no_rawdata; n++){
    if(n%5==0 &&n!=0) printf("\n");
    if(decoded_event[n]!=0){
      if(n%2==0) printf("  %2iH: %4i,", n/2, decoded_event[n]);
       else printf("  %2iL: %4i,", n/2, decoded_event[n]);
    }
     else printf("           ,");
  }
  printf("\n");
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}
