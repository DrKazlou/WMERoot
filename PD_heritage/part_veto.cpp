#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "v874.h"
#include "veto.h"

#include "main.h"
#include "readout_class.h"

void Readout_class_veto::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"VETO-board", 19); // for screen output
  strncpy(mod_ident,"veto_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_veto::init_module(Param *param)
{
  vme.ww(vme_image, cardBase+V874_VSET, 
     Param_getVal(param, mod_ident_compl, "vset"));  
  vme.ww(vme_image, cardBase+V874_VOFF,
     Param_getVal(param, mod_ident_compl, "voff"));  
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x100); /*set setup mode*/
  pigset(THRESH_1, Param_getVal(param, mod_ident_compl, "thresh_1"));
  pigset(THRESH_2, Param_getVal(param, mod_ident_compl, "thresh_2"));
  pigset(THRESH_3, Param_getVal(param, mod_ident_compl, "thresh_3"));
  pigset(THRESH_4, Param_getVal(param, mod_ident_compl, "thresh_4"));
  pigset(THRESH_5, Param_getVal(param, mod_ident_compl, "thresh_5"));
  pigset(THRESH_6, Param_getVal(param, mod_ident_compl, "thresh_6"));
  pigset(THRESH_7, Param_getVal(param, mod_ident_compl, "thresh_7"));
  pigset(THRESH_8, Param_getVal(param, mod_ident_compl, "thresh_8"));
  pigset(PEDQAC_1, Param_getVal(param, mod_ident_compl, "pedqac_1"));
  pigset(PEDQAC_2, Param_getVal(param, mod_ident_compl, "pedqac_2"));
  pigset(PEDQAC_3, Param_getVal(param, mod_ident_compl, "pedqac_3"));
  pigset(PEDQAC_4, Param_getVal(param, mod_ident_compl, "pedqac_4"));
  pigset(PEDQAC_5, Param_getVal(param, mod_ident_compl, "pedqac_5"));
  pigset(PEDQAC_6, Param_getVal(param, mod_ident_compl, "pedqac_6"));
  pigset(PEDQAC_7, Param_getVal(param, mod_ident_compl, "pedqac_7"));
  pigset(PEDQAC_8, Param_getVal(param, mod_ident_compl, "pedqac_8"));

  pigset(TPSWITCH, Param_getVal(param, mod_ident_compl, "tpswitch"));
  
  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x100); /*set readout mode*/
}

void Readout_class_veto::pigset(int device, int l_value){ 
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


void Readout_class_veto::printeventdata(unsigned int noe){
  if(modulenumber==0) printf("\n");	 
  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);

  if(decoded_event[20]!=0) printf("  QAC1: %4i,", decoded_event[20]); else printf("            ,");
  if(decoded_event[12]!=0) printf("  tim1: %4i,", decoded_event[12]); else printf("            ,");
  if(decoded_event[21]!=0) printf("  QAC2: %4i,", decoded_event[21]); else printf("            ,");
  if(decoded_event[16]!=0) printf("  tim2: %4i\n", decoded_event[16]); else printf("            \n");
  if(decoded_event[22]!=0) printf("  QAC3: %4i,", decoded_event[22]); else printf("            ,");
  if(decoded_event[13]!=0) printf("  tim3: %4i,", decoded_event[13]); else printf("            ,");
  if(decoded_event[23]!=0) printf("  QAC4: %4i,", decoded_event[23]); else printf("            ,");
  if(decoded_event[17]!=0) printf("  tim4: %4i\n", decoded_event[17]); else printf("            \n");
  if(decoded_event[24]!=0) printf("  QAC5: %4i,", decoded_event[24]); else printf("            ,");
  if(decoded_event[14]!=0) printf("  tim5: %4i,", decoded_event[14]); else printf("            ,");
  if(decoded_event[25]!=0) printf("  QAC6: %4i,", decoded_event[25]); else printf("            ,");
  if(decoded_event[18]!=0) printf("  tim6: %4i\n", decoded_event[18]); else printf("            \n");
  if(decoded_event[26]!=0) printf("  QAC7: %4i,", decoded_event[26]); else printf("            ,");
  if(decoded_event[15]!=0) printf("  tim7: %4i,", decoded_event[15]); else printf("            ,");
  if(decoded_event[27]!=0) printf("  QAC8: %4i,", decoded_event[27]); else printf("            ,");
  if(decoded_event[19]!=0) printf("  tim8: %4i\n", decoded_event[19]); else printf("            \n");
  printf("  BP28: 0x%02x", decoded_event[28]);
  printf("  BP29: 0x%02x\n", decoded_event[29]);
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}
