#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "sis3820.h"

#include "main.h"
#include "readout_class.h"


void Readout_class_sis3820::init_module_variables(void){
  v874class_module=0;
  max_no_rawdataSMALL= 5;  // not including header/trail
  max_no_rawdataBIG= 5;  // not including header/trail
  adc_range= 1;
  number_of_data=5;
  strncpy(modname,"SIS-sync-module", 19); // for screen output
  strncpy(mod_ident,"sis3820_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}


void Readout_class_sis3820::lam(void){
  unsigned int regv;

  bsy_off();   // reset FF
  vme.rl(vme_image, cardBase+SIS3820_IRQ_CONTROL, &regv); 
  regv = (regv>>16) &0x1;
  while(regv!=0x1){ //no event &0x1
    for(int n=0; n<=1000; n++) printf(""); // wait a little bit
    vme.rl(vme_image, cardBase+SIS3820_IRQ_CONTROL, &regv); 
    regv = (regv>>16) &0x1;
  }
}

void Readout_class_sis3820::bsy_on(void){
  printf("There is no BUSY ON for a SYS 3820 module (0x%x)\n", cardBase);
}

void Readout_class_sis3820::bsy_off(void){
  vme.wl(vme_image, cardBase+SIS3820_IRQ_CONTROL, 0x10000); 
//  printf("Busy off (0x%x)\n", address);
}

int Readout_class_sis3820::get_geo(void){
  return geo; // get geo address
}
void Readout_class_sis3820::set_geo(int int_geo){
  geo=int_geo;
}

void Readout_class_sis3820::reset(void){
  vme.wl(vme_image, cardBase+SIS3820_LATCH_COUNTER_CLEAR, 0xf); //clr counter

  printf("RESET! 0x%x\n", cardBase);
  printf("  Reseted a %s as board no %i\n", modname, modulenumber+1);

}

int Readout_class_sis3820::read_one_event(int noe, int & ok){
  unsigned int data;

  for(int n=0;n<max_no_rawdata+2;n++){ 
    rawdata[n]=0;           // clear event data stream
  }
  for(int n=0;n<max_no_rawdata;n++){ 
    decoded_event[n]=0;   // clear printdata
  } 

  rawdata[0]=0xaaaa;  // header
  vme.rl(vme_image, cardBase+SIS3820_LATCH_COUNTER1, &data); 
  decoded_event[0]=rawdata[1]=data;  // counter 1
  vme.rl(vme_image, cardBase+SIS3820_LATCH_COUNTER2, &data); 
  decoded_event[1]=rawdata[2]=data;  // counter 2
  vme.rl(vme_image, cardBase+SIS3820_LATCH_COUNTER3, &data); 
  decoded_event[2]=rawdata[3]=data;  // counter 3
  vme.rl(vme_image, cardBase+SIS3820_LATCH_COUNTER4, &data); 
  decoded_event[3]=rawdata[4]=data;  // counter 4

  vme.rl(vme_image, cardBase+SIS3820_DATA_IN, &data); 
  decoded_event[4]=rawdata[5]=data;  // direct data in

  rawdata[6]=0x5555;  // trail

  if(save_event()==0) exit(0);

  int_noe = rawdata[1];
  if((int_noe-1)!=noe) int_ok=0;
  return ((5+2)*4); // return # saved bytes
} // ntec_read_one_event


void Readout_class_sis3820::init_module(Param * param){
  unsigned int constat;

  printf("Base address: 0x%08x\n", cardBase);

  vme.rl(vme_image, cardBase+SIS3820_MODID, &constat); 
  printf("ModID: 0x%x\n", constat);
  // start ini
  vme.wl(vme_image, cardBase+SIS3820_CONTROL_STATUS, 0x1012); 

  vme.rl(vme_image, cardBase+SIS3820_CONTROL_STATUS, &constat);
  printf("Control Status: 0x%x\n", constat);
   /*enable FF1, set invert output 5, enable input counters*/
}


void Readout_class_sis3820::printeventdata(unsigned int noe){
  if(modulenumber==0) printf("\n");	 
  printf("%s: %i, No. of channels: %2i, Noe (SIS corrected): %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe-1, noe);

  printf("Counter 1: %i\n", decoded_event[0]);
  printf("Counter 2: %i\n", decoded_event[1]);
  printf("Counter 3: %i\n", decoded_event[2]);
  printf("Counter 4: %i\n", decoded_event[3]);
  printf("Input register: 0x%x\n", decoded_event[4]);
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}

