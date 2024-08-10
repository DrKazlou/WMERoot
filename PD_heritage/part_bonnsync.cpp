#include <string.h>
#include <stdio.h>

#include "vmelib.h"
#include "param.h"
 
#include "v895.h"

#include "main.h"
#include "readout_class.h"


void Readout_class_bonnsync::init_module_variables(void){
  v874class_module=0;
  max_no_rawdataSMALL= 1;  // not including header/trail
  max_no_rawdataBIG= 1;  // not including header/trail
  adc_range= 1; 
  number_of_data=1;
  strncpy(modname,"Bonn sync", 19); // for screen output
  strncpy(mod_ident,"bonnsync_",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

int Readout_class_bonnsync::read_one_event(int noe, int & ok){
//  printf("Reading out bonn sync now\n");
  rawdata[0]=0xbbaa;  // header
  rawdata[1]=noe;  // noe
  rawdata[2]=0xaabb;  // trail

  if(save_event()==0) exit(0);

  vme.wl(vme_image2, cardBase+0x4, noe);   // write eventnumber
  vme.wl(vme_image2, cardBase, 1);   // sygnal to CB: crate is ready for next event   

  return ((1+2)*4); // return # saved bytes: header, noe, trail
}

void Readout_class_bonnsync::lam(void){
    printf("Bonnsync module is not allowed as LAM!\n");
    exit(0);
}

void Readout_class_bonnsync::reset(void){
}

void Readout_class_bonnsync::bsy_on(void){
}

void Readout_class_bonnsync::bsy_off(void){
}

int Readout_class_bonnsync::get_geo(void){
   return geo;
}

void Readout_class_bonnsync::set_geo(int int_geo){
  geo=int_geo;
}

void Readout_class_bonnsync::printeventdata(unsigned int noe){
}


void Readout_class_bonnsync::init_module(Param *param){
  printf("Allocating new image for bonn module...\n");
  vme_image2 = vme.getImage(cardBase, 0x2000, A32, D32, MASTER);
  if(vme_image2<0){
    printf("\nError in allocating an image for sis3302 board %i!\n",
                                       modulenumber);
    exit(0);
  }
  vme.setOption(vme_image2, POST_WRITE_DIS);

}























