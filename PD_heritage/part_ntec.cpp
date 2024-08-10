//#include <unistd.h>
//#include <errno.h>
//#include <time.h>
//#include <math.h>
//#include <hadesstd.h>
//#include <cstdlib> 
//#include "hardware.h"

#include <string.h>
#include <stdio.h>

#include "v874.h"
#include "tapspig.h"

#include "vmelib.h"
#include "param.h" 

#include "main.h"
#include "readout_class.h"


void Readout_class_ntec::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"NTEC-board", 19); // for screen output
  strncpy(mod_ident,"ntec_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_ntec::init_module(Param *param)
{
  vme.ww(vme_image, cardBase+V874_VSET, 
     Param_getVal(param, mod_ident_compl, "vset"));  
  vme.ww(vme_image, cardBase+V874_VOFF,
     Param_getVal(param, mod_ident_compl, "voff"));  
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x100); /*set setup mode*/
  pigset(THRESHCFD_1, Param_getVal(param, mod_ident_compl, "threshcfd_1"));
  pigset(THRESHCFD_2, Param_getVal(param, mod_ident_compl, "threshcfd_2"));
  pigset(THRESHCFD_3, Param_getVal(param, mod_ident_compl, "threshcfd_3"));
  pigset(THRESHCFD_4, Param_getVal(param, mod_ident_compl, "threshcfd_4"));
  pigset(THRESHLED1_1, Param_getVal(param, mod_ident_compl, "threshled1_1"));
  pigset(THRESHLED1_2, Param_getVal(param, mod_ident_compl, "threshled1_2"));
  pigset(THRESHLED1_3, Param_getVal(param, mod_ident_compl, "threshled1_3"));
  pigset(THRESHLED1_4, Param_getVal(param, mod_ident_compl, "threshled1_4"));
  pigset(THRESHLED2_1, Param_getVal(param, mod_ident_compl, "threshled2_1"));
  pigset(THRESHLED2_2, Param_getVal(param, mod_ident_compl, "threshled2_2"));
  pigset(THRESHLED2_3, Param_getVal(param, mod_ident_compl, "threshled2_3"));
  pigset(THRESHLED2_4, Param_getVal(param, mod_ident_compl, "threshled2_4"));
  pigset(PEDQAC1_1, Param_getVal(param, mod_ident_compl, "pedqac1_1"));
  pigset(PEDQAC1_2, Param_getVal(param, mod_ident_compl, "pedqac1_2"));
  pigset(PEDQAC1_3, Param_getVal(param, mod_ident_compl, "pedqac1_3"));
  pigset(PEDQAC1_4, Param_getVal(param, mod_ident_compl, "pedqac1_4"));
  pigset(PEDQAC2_1, Param_getVal(param, mod_ident_compl, "pedqac2_1"));
  pigset(PEDQAC2_2, Param_getVal(param, mod_ident_compl, "pedqac2_2"));
  pigset(PEDQAC2_3, Param_getVal(param, mod_ident_compl, "pedqac2_3"));
  pigset(PEDQAC2_4, Param_getVal(param, mod_ident_compl, "pedqac2_4"));
  pigset(PEDQAC3_1, Param_getVal(param, mod_ident_compl, "pedqac3_1"));
  pigset(PEDQAC3_2, Param_getVal(param, mod_ident_compl, "pedqac3_2"));
  pigset(PEDQAC3_3, Param_getVal(param, mod_ident_compl, "pedqac3_3"));
  pigset(PEDQAC3_4, Param_getVal(param, mod_ident_compl, "pedqac3_4"));
  pigset(PEDQAC4_1, Param_getVal(param, mod_ident_compl, "pedqac4_1"));
  pigset(PEDQAC4_2, Param_getVal(param, mod_ident_compl, "pedqac4_2"));
  pigset(PEDQAC4_3, Param_getVal(param, mod_ident_compl, "pedqac4_3"));
  pigset(PEDQAC4_4, Param_getVal(param, mod_ident_compl, "pedqac4_4"));
  pigset(TPSWITCH, Param_getVal(param, mod_ident_compl, "tpswitch"));
  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x100); /*set readout mode*/
}

void Readout_class_ntec::pigset(int device, int l_value){ 
  int n;
  short unsigned int value = l_value;

  printf("Device PB: 0x%0x, Value: %x\n", device, value);
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x100); // setup mode
  vme.ww(vme_image, cardBase+(V874_AUX), value & 0xFF);    // transfer the lower 8bits
  vme.ww(vme_image, cardBase+(V874_AUX+(0x38<<1)), ((value >> 8) & 0x1F)); // transfer the higher 5 bits
  vme.ww(vme_image, cardBase+(V874_AUX+((0x40 | device)<<1)), (short unsigned int) 0);  //data internal PLD->external pins
  vme.ww(vme_image, cardBase+(V874_AUX+((0x40 | device)<<1)), (short unsigned int) 0);   //Writes DAC with correct #
  vme.ww(vme_image, cardBase+(V874_AUX+((0x40 | device)<<1)), (short unsigned int) 0); //Writes DAC with correct #
  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x100); //readout mode
}/* Diese Funktion setzt den Wert an dem angegebenen Device (zB THRESHCFD_1) */

void Readout_class_ntec::printeventdata(unsigned int noe){
  if(modulenumber==0) printf("\n");	 
  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);

  if(decoded_event[9]!=0) printf("  SGS1: %4i,", decoded_event[9]); else printf("            ,");
  if(decoded_event[11]!=0) printf("  SG1: %4i,", decoded_event[11]);  else printf("           ,");
  if(decoded_event[8]!=0) printf("  LGS1: %4i,", decoded_event[8]); else printf("            ,");
  if(decoded_event[10]!=0) printf("  LG1: %4i,", decoded_event[10]);  else printf("           ,");
  if(decoded_event[16]!=0) printf("  TAC1: %4i\n", decoded_event[16]); else printf("           \n");

  if(decoded_event[13]!=0) printf("  SGS2: %4i,", decoded_event[13]); else printf("            ,");
  if(decoded_event[15]!=0) printf("  SG2: %4i,", decoded_event[15]);  else printf("           ,");
  if(decoded_event[12]!=0) printf("  LGS2: %4i,", decoded_event[12]); else printf("            ,");
  if(decoded_event[14]!=0) printf("  LG2: %4i,", decoded_event[14]);  else printf("           ,");
  if(decoded_event[17]!=0) printf("  TAC2: %4i\n", decoded_event[17]); else printf("           \n");

  if(decoded_event[21]!=0) printf("  SGS3: %4i,", decoded_event[21]); else printf("            ,");
  if(decoded_event[23]!=0) printf("  SG3: %4i,", decoded_event[23]);  else printf("           ,");
  if(decoded_event[20]!=0) printf("  LGS3: %4i,", decoded_event[20]); else printf("            ,");
  if(decoded_event[22]!=0) printf("  LG3: %4i,", decoded_event[22]);  else printf("           ,");
  if(decoded_event[18]!=0) printf("  TAC3: %4i\n", decoded_event[18]); else printf("           \n");

  if(decoded_event[25]!=0) printf("  SGS4: %4i,", decoded_event[25]); else printf("            ,");
  if(decoded_event[27]!=0) printf("  SG4: %4i,", decoded_event[27]);  else printf("           ,");
  if(decoded_event[24]!=0) printf("  LGS4: %4i,", decoded_event[24]); else printf("            ,");
  if(decoded_event[26]!=0) printf("  LG4: %4i,", decoded_event[26]);  else printf("           ,");
  if(decoded_event[19]!=0) printf("  TAC4: %4i\n", decoded_event[19]); else printf("           \n");	 
  printf("  BP28: 0x%02x", decoded_event[28]);
  printf("  BP29: 0x%02x\n", decoded_event[29]);
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}

