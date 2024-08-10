//#include <unistd.h>
//#include <errno.h>
//#include <time.h>
#include <string.h>
#include <stdio.h>
//#include <math.h>
#include "vmelib.h"
//#include <cstdlib> 

#include "param.h" 
#include "v874.h"
//#include "hardware.h"

#include "main.h"
#include "readout_class.h"

void Readout_class_iol::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 32;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"iol-board", 19); // for screen output
  strncpy(mod_ident,"iol_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class_iol::printeventdata(unsigned int noe){
  if(modulenumber==0) printf("\n"); 
  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}

void Readout_class_iol::lam(void){
  short unsigned int regv;
	
  bsy_off(); // iol custom, remove for base class
  vme.rw(vme_image, cardBase+V874_STAT_REG_1, &regv); 
  regv = regv &0x1;
    // polling data_ready_bit
  while(regv!=0x1){ //no event &0x3
    for(int n=0; n<=1000; n++) printf(""); // wait a little bit
    vme.rw(vme_image, cardBase+V874_STAT_REG_1, &regv); 
    regv = regv &0x1;
      // polling data_ready_bit
  }
}

void Readout_class_iol::bsy_on(void){
    //  ww(vme_image, cardBase+V874_BIT_SET_2, 0x1); 
}

void Readout_class_iol::bsy_off(void){
    // ww(vme_image, cardBase+V874_BIT_CLR_2, 0x1); 
  vme.ww(vme_image, cardBase+(V874_AUX)+2, 1);  //reset ff iol custom
}

