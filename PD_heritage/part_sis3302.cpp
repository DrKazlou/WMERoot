#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "sis3302.h"

#include "main.h"
#include "readout_class.h"


Readout_class_sis3302::~Readout_class_sis3302() {
    // printf("Del shortarray\n");
  delete shortarray;
}


void Readout_class_sis3302::init_module_variables(void){
  v874class_module=0;
  max_no_rawdataSMALL= 8;  // not including header/trail
  max_no_rawdataBIG= 8;  // not including header/trail
  adc_range= 0; // reinitialized with real values in init_module this time
  strncpy(modname,"SIS3302-FADC", 12); // for screen output
  strncpy(mod_ident,"sis3302_",12);     // for parameter file readout
  further_parameters[0]=0; // the further parameters are defined later in this 
                           // module, otherwise the number
                           // parameter has 8 bits

}


void Readout_class_sis3302::lam(void){
  unsigned int regv;
  printf("This module does not provide LAM! (sis3302)\n");
  exit(0);
//  vme.rl(vme_image, cardBase+SIS3302_IRQ_CONTROL, &regv); 
}

void Readout_class_sis3302::bsy_on(void){
}

void Readout_class_sis3302::bsy_off(void){

}

int Readout_class_sis3302::get_geo(void){
  return geo; // get geo address
}

void Readout_class_sis3302::set_geo(int int_geo){
}

void Readout_class_sis3302::reset(void){
//  vme.wl(vme_image, cardBase+SIS3302_LATCH_COUNTER_CLEAR, 0xf); //clr counter


}

void Readout_class_sis3302::WaitStop(int cv=1000){
  int counter=0;
  unsigned int retval;
  do{
    vme.rl(vme2_image, 
         cardBase+SIS3302_AcqCntrStat, &retval);
    counter++;
//printf("Increasing counter to %i\n", counter);
    if(counter>cv){ 
      vme.wl(vme2_image, 
           cardBase+SIS3302_StopSampling , 1);  // stop sampling
      printf("Waitstop!\n");
      int_ok=0;
    }
  }while((retval&0x30000));
}


int Readout_class_sis3302::read_one_event(int noe, int & ok){
  unsigned int n;

  number_of_data=0;

  WaitStop();
  
  vme.rl(vme2_image, cardBase+SIS3302_ActualEvCounter, &n);    
  int_noe+=n;
//  printf("Event counter: %i\n", n);

  rawdata[number_of_data++]=0xfadcfadc; // header 
  for(int ch=0;ch<8;ch++) n=GetData(ch, number_of_data);  //GetData
  rawdata[number_of_data++]=0xfadceee; // trail

  number_of_data-=2; // n of d is w/o h/t, but it was counted in 2-4 lines ago
//  printf("SIS: number_of_data: %i\n", number_of_data);
  if(save_event()==0) exit(0);

  vme.wl(vme2_image, 
         cardBase+SIS3302_ArmSampling , 1); // page 28 Arm sampling

  vme.wl(vme2_image, 
         cardBase+SIS3302_Sampling , 1);  // p 28 start sampling
  // printf("Counter SIS sum: %i\n", number_of_data);
  return number_of_data*4; // return saved bytes
} // read_one_event


int Readout_class_sis3302::GetData(int ch, int & counter){
  unsigned int sto_addr, start_addr, sample_address;
  unsigned int sto_ptr_offset;
  unsigned int data;
  int offset, correction, additional;
  int samplecounter;
  int hisvalue,ped=0, max=0, min=65500;
  unsigned int *dma_ptr;
  int dma_offset;
    //


  if(ch>=0 && ch<8){
    start_addr = RED_ADC_Mem[ch];
    sample_address = RED_Sample_Address_ADC[ch];
  }
   else{
    printf("GetData: channel does not exist\n");
    return 0;
  }
  
  rawdata[counter++] = (ch<<28)+SADC_N_of_samples;
 
   // calculate position in memory
//  vme.rl(vme2_image, cardBase+sample_address, &sto_ptr_offset);
  sto_ptr_offset=*(readpointer+cardBase/4+sample_address/4);

  if(sto_ptr_offset/4 > WrapPageSize_Bytes) printf("Wrong addr offset...\n");
//  printf("ch: %i: Storage pointer position: 0x%x (0x%x, start_addr: 0x%x, sample_addr: 0x%x)\n", 
  //        ch, sto_ptr_offset, WrapPageSize_Bytes, start_addr, sample_address );

//  rawdata[counter++]=sto_ptr_offset;
  switch(sto_ptr_offset&0x3){
    case 3: 
           sto_ptr_offset = (sto_ptr_offset & 0x01fffffc)<<1;
	   correction=-1; //  -1;
           sto_ptr_offset-=4; // read former dataword
           additional=1; 
           break;
    case 0:
           sto_ptr_offset = (sto_ptr_offset & 0x01fffffc)<<1;
           correction=0;
           additional=0; 
           break;
    case 1: 
           sto_ptr_offset = (sto_ptr_offset & 0x01fffffc)<<1;
           correction=+1;
           additional=1; 
           break;
    case 2:
           sto_ptr_offset = (sto_ptr_offset & 0x01fffffc)<<1;
           correction=+2;
           additional=1; 
           break;
  }
//  printf("GetData ch %i, correction: %i\n", ch, correction);

  // ****************1st 2 samples

  if(sto_ptr_offset< (SADC_N_of_samples*2) ){
      //segmented readout
      //     printf("SEGMENTED!\n");
  //1st 2 samples
    sto_addr = start_addr + WrapPageSize_Bytes 
                          - SADC_N_of_samples*2 + sto_ptr_offset;
//    vme.rl(vme2_image, cardBase+sto_addr, &data);
    data=*(readpointer+cardBase/4+sto_addr/4);
    samplecounter=0;

    if(correction==-1){
      shortarray[samplecounter++]=(data>>16);  //only 2nd short is used
    }
    if(correction==0){
      shortarray[samplecounter++]=(data&0xffff);  // 1st short
      shortarray[samplecounter++]=(data>>16);    // 2nd short
    }
    if(correction==1){
      shortarray[samplecounter++]=(data>>16);  //only 2nd short is used
    }
    if(correction==2);  // no short is used with an offset of 2
    int prewrap_addr;
    prewrap_addr=WrapPageSize_Bytes+sto_ptr_offset-SADC_N_of_samples*2;
    for(int i=1,c=0; i<SADC_N_of_samples/2+additional; i++){
      if((i*4+prewrap_addr)<WrapPageSize_Bytes){  // <= ???
          // read out last samples before wrapping
        sto_addr = start_addr+prewrap_addr+ i*4;
// printf("(%i) %x, ", i, sto_addr, sto_ptr_offset);
      }
       else{ // wrap over, start at beginning of buffer
	sto_addr = start_addr + c*4;
//	printf("(%i) c:%x,e:%x ", i, sto_addr, sto_ptr_offset);
        c++;
      }   
/*      if(vme.rl(vme2_image, cardBase+sto_adr, &data)!=0){ //error
	printf("Sta_addr:%i WPS_W: %i SADC_Nos*2: %i sto_ptr_o: %i, i: %i\n", 
                   start_addr, WrapPageSize_Bytes, 
                   SADC_N_of_samples*2, sto_ptr_offset, i);
		   }*/
//      vme.rl(vme2_image, cardBase+sto_addr, &data);
      data=*(readpointer+cardBase/4+sto_addr/4);
      shortarray[samplecounter++]=(data&0xffff);  // 1st short
      shortarray[samplecounter++]=(data>>16);    // 2nd short
    }
//    printf("end segmented: 0x%x\n", sto_ptr_offset);
  }
  else{  //not segmented
  //1st 2 samples
   sto_addr = start_addr + sto_ptr_offset - SADC_N_of_samples*2;
//   vme.rl(vme2_image, cardBase+sto_addr, &data);
   data=*(readpointer+cardBase/4+sto_addr/4);
   samplecounter=0;

   if(correction==-1){
     shortarray[samplecounter++]=(data>>16);  //only 2nd short is used
   }
   if(correction==0){
     shortarray[samplecounter++]=(data&0xffff);  // 1st short
     shortarray[samplecounter++]=(data>>16);    // 2nd short
   }
   if(correction==1){
     shortarray[samplecounter++]=(data>>16);  //only 2nd short is used
   }
   if(correction==2);  // no short is used with an offset of 2
   if(dma_ok==0){ // dma doesn't work, slow...
//   printf("no dma\n");
     for(int i=1; i<SADC_N_of_samples/2+additional; i++){
      //        Bufferaddr + Pointer last DW - go to beginning + 4byte/2evnts
        sto_addr = start_addr + sto_ptr_offset - SADC_N_of_samples*2 + i*4;
/*      if(vme.rl(vme2_image, cardBase+sto_addr, &data)!=0){ //error
	  printf("sto_addr = start_addr + sto_ptr_offset - SADC_N_of_samples*2 + i*4;\n");
	printf("unseg: Sta_addr:0x%x WPS_W: %i SADC_Nos*2: %i sto_ptr_o: %i, i: %i\n", 
                   start_addr, WrapPageSize_Bytes, 
                   SADC_N_of_samples*2, sto_ptr_offset, i);
		   }*/
//      vme.rl(vme2_image, cardBase+sto_addr, &data);
        data=*(readpointer+cardBase/4+sto_addr/4);
        shortarray[samplecounter++]=(data&0xffff);  // 1st short
        shortarray[samplecounter++]=(data>>16);    // 2nd short
      }
    }
     else{  // use dma access
//	 printf("DMA pointer!\n");
      dma_offset = vme.DMAread(cardBase+sto_addr, SADC_N_of_samples*2+additional*4, A32, D32);
      if (dma_offset < 0) {
        printf("DMA error!!!\n");
        vme.releaseDMA();
        exit(0);  
      }
      dma_ptr= (unsigned int *) (dma_base + dma_offset);
      for(int i=1; i<SADC_N_of_samples/2+additional; i++){
	data=*dma_ptr++;
        shortarray[samplecounter++]=(data&0xffff);  // 1st short
        shortarray[samplecounter++]=(data>>16);    // 2nd short
      }
    }
  }
  for(int n=0; n<SADC_N_of_samples; n+=2){
    rawdata[counter++]=(shortarray[n+1]<<16)+shortarray[n];
    //for trace
    his[n+ch*SADC_N_of_samples]=shortarray[n];  // do the histogram stuff
    his[n+1+ch*SADC_N_of_samples]=shortarray[n+1];  // do the histogram stuff

    // for his
    if(shortarray[n]> max) max=shortarray[n];
    if(shortarray[n+1]> max) max=shortarray[n+1];
    if(shortarray[n]< min) min=shortarray[n];
    if(shortarray[n+1]< min) min=shortarray[n+1];
    if(n<ped_start | n>=(SADC_N_of_samples-ped_end)){
	ped+=shortarray[n]+shortarray[n+1];
	}
  }
  ped/=(ped_start+ped_end);
  pedestal[ch]=ped;
  if(max-ped>ped-min){
    hisvalue=max-ped;
    hisarray[ch]=hisvalue;
    value[ch]=max;    
  }
  else{
    hisvalue=ped-min;
    hisarray[ch]=-hisvalue;
    value[ch]=min;    
  }
  his[(8*SADC_N_of_samples)+ch*65536+hisvalue]++;
  // printf("counter: %i\n", counter);
  return counter;
}  //************************ GetData
  

void Readout_class_sis3302::init_module(Param * param){
  char buf[16];
  unsigned int regval;

  printf("init sis! 0x%x\n", cardBase);

  hex =  Param_getVal(param, mod_ident_compl, "hex");  // print out in hex

  sprintf(buf, "red_addr_space");
  RED =  Param_getVal(param, mod_ident_compl, buf);
  
  copy_RED_pointers();  // depending on  wether it is a reduced address space or not

  if(RED==0){   // get image for normal address space
    printf("Allocating new image...\n");
    vme2_image = vme.getImage(cardBase, 0xfffffff, A32, D32, MASTER);
    if(vme2_image<0){
      printf("\nError in allocating an image for sis3302 board %i!\n",
                                         modulenumber);
      exit(0);
    }
    vme.setOption(vme2_image, POST_WRITE_DIS);
  }
  else vme2_image=vme_image;  // reduced address space, no further image needed
//  printf("Testing reduced address space jumper... ");
  vme.rl(vme2_image, cardBase+SIS3302_ContrStat, &regval);
  if(((regval>>30)&0x1)!=RED){
    printf("\n\nReduced address space jumper is not set correctly!\n");
    exit(0);
  }
//  printf("OK!\n");

  readpointer=((unsigned int *) vme.getPciBaseAddr(vme2_image));
//  printf("Readpointer: 0x%x\n",readpointer);

  vme.rl(vme2_image, cardBase+SIS3302_ModuleID_Firm, &regval); 
//  printf("ModID: 0x%x\n", regval);
 

//  printf("General reset\n");
  vme.wl(vme2_image, 
         cardBase+SIS3302_GeneralReset, 1);
//  printf("Reset DACs\n");
  vme.wl(vme2_image, 
         cardBase+SIS3302_DAC_CntrStat, 0x3); // reset all DACs
  
//  printf("Switch on user LED\n");
  vme.wl(vme2_image, 
         cardBase+SIS3302_ContrStat, 1); //switch on user LED

  sprintf(buf, "clock");
  clock =  Param_getVal(param, mod_ident_compl, buf);
  switch(clock){
    case 100:
      printf("100MHz internal, switch on Start/Stop logic\n");
      vme.wl(vme2_image,  // 0x0100 100MHz, enable front panel Start/Stop logic
         cardBase+SIS3302_AcqCntrStat, 0x0100);
      break;
    case 50:
      printf("50MHz internal, switch on Start/Stop logic\n");
      vme.wl(vme2_image,  // 0x0101 50MHz, enable front panel Start/Stop logic
         cardBase+SIS3302_AcqCntrStat, 0x0101);
      break;
    case 25:
      printf("25MHz internal, switch on Start/Stop logic\n");
      vme.wl(vme2_image,  // 0x0102 25MHz, enable front panel Start/Stop logic
         cardBase+SIS3302_AcqCntrStat, 0x0102);
      break;
    case 10:
      printf("10MHz internal, switch on Start/Stop logic\n");
      vme.wl(vme2_image,  // 0x0103 100MHz, enable front panel Start/Stop logic
         cardBase+SIS3302_AcqCntrStat, 0x0103);
      break;
    default:
	printf("%i as clock frequency is not yet supported\n", clock);
        exit(0);
      break;
  }
//  printf("Set TriggerFlagClear\n");
  vme.wl(vme2_image,
         cardBase+RED_TriggerFlagClear[0], 2000); // ch 1+2
  vme.wl(vme2_image,
         cardBase+RED_TriggerFlagClear[1], 2000); // ch 3+4
  vme.wl(vme2_image,
         cardBase+RED_TriggerFlagClear[2], 2000); // ch 5+6
  vme.wl(vme2_image,
         cardBase+RED_TriggerFlagClear[3], 2000); // ch 7+8

//  printf("Event config\n");
  if(RED==0){  //normal addr space 
    EventConfig=0x11; // wrap around page until stop, size=1 -> 4M samples
    WrapPageSize_Bytes = 16777216*2; // 16M Samples max * 2Byte/Sample
  }
  else{  // reduced address space
    EventConfig=0x13; // wrap around page until stop, size=3 -> 256k samples
    WrapPageSize_Bytes = 16777216*2; // 16M Samples max * 2Byte/Sample
  }
  page_size = EventConfig&0xF;
  for(int i=0;i<page_size;i++) WrapPageSize_Bytes /= 4;

  vme.wl(vme2_image, 
         cardBase+RED_Event_Config_ALL_ADC, EventConfig);

  for(int ch=0; ch<8; ch++){
    sprintf(buf, "threshold%02d", ch);
    SetThresh(ch, Param_getVal(param, mod_ident_compl, buf));
    sprintf(buf, "trigger%02d", ch);
    SetTrigSetup(ch, Param_getVal(param, mod_ident_compl, buf));
    sprintf(buf, "dac%02d", ch);
    SetDAC(ch, Param_getVal(param, mod_ident_compl, buf));
  }

  
  vme.wl(vme2_image, 
         cardBase+SIS3302_ExtStopDelay, 
         Param_getVal(param, mod_ident_compl, "stopdelay"));

  vme.wl(vme2_image, 
         cardBase+SIS3302_ArmSampling , 1); // page 28 Arm sampling

  vme.wl(vme2_image, 
         cardBase+SIS3302_Sampling , 1);  // p 28 start sampling


  SADC_N_of_samples=Param_getVal(param, mod_ident_compl, "no_of_samples");
  if(SADC_N_of_samples%2==1) SADC_N_of_samples++; // only even values 
  shortarray =new unsigned short[SADC_N_of_samples+4];

  ped_start=Param_getVal(param, mod_ident_compl, "ped_range_beginning");
  ped_end=Param_getVal(param, mod_ident_compl, "ped_range_end");

  further_parameters[0]=5; // otherwise the number
                           // parameter has 8 bits
  further_parameters[1]=page_size; // 
  further_parameters[2]=SADC_N_of_samples; // 
  further_parameters[3]=clock;
  further_parameters[4]=ped_start; // 
  further_parameters[5]=ped_end; // 
  // further_parameters[1]=max_hits_per_event; // max hits per event

//********* buffersizes have to be calculated differently
  delete rawdata;   
  delete decoded_event;
  delete his;

  rawdata = new unsigned int[SADC_N_of_samples/2*8+8+2]; // raw head/trai/ch
  decoded_event = new unsigned int[max_no_rawdata]; // without header/trail

  his = new unsigned int [8*SADC_N_of_samples+8*65536];
  for(int n=0; n<(8*SADC_N_of_samples+8*65536); n++){
    his[n] = 0; 
  }  // reset histogram
//********* END buffersizes have to be calculated differently
  geo=Param_getVal(param, mod_ident_compl, "geo");
  int_noe=-1;
} //********** Readout_class_sis3302::init_module


int Readout_class_sis3302::save_histo(FILE * hisout){    
//  printf("Saving histo, %i, %i\n", SADC_N_of_samples, max_no_rawdata);
  fwrite(&SADC_N_of_samples, sizeof(SADC_N_of_samples), 1, hisout);
  fwrite(&max_no_rawdata, sizeof(max_no_rawdata), 1, hisout);
  return fwrite(his, sizeof(his)*(8*SADC_N_of_samples+(8*65536)), 1, hisout);
}

void Readout_class_sis3302::printeventdata(unsigned int noe){
  int t;

  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);
  for(int n=0; n<8;n++){
    t=5;
    if((threshold[n]>>24)&1==1){ // Less then
      if(value[n] < (threshold[n]&0xffff)) t=1;
       else t=0;
    }
    if((threshold[n]>>25)&1==1){ // greater then
     if(value[n] >= (threshold[n]&0xffff)) t=1;
       else t=0;
    }
    if(hex==0){
      printf("ch: %i, ped: %4i, value: %4i (%i), histovalue: %4i  \t",
              n, pedestal[n], value[n], t, hisarray[n]);
    } 
    else{  // hex==1
      printf("ch: %i, ped: 0x%4x, value: 0x%4x (%i), histovalue: %4i  \t",
              n, pedestal[n], value[n], t, hisarray[n]);
    }
    if(n%2==1) printf("\n"); 
  }
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}

void Readout_class_sis3302::SetTrigSetup(int ch,
	unsigned long int thresh){
//  printf("Setting trigger setup for channel %i to 0x%x\n", ch, thresh);
  if(ch>=0 && ch<8){
    vme.wl(vme2_image, cardBase+RED_Trigger_Setup_ADC[ch], thresh);
  }
   else
    printf("channel %i does not exist\n", ch);
}

//===================================================================
void Readout_class_sis3302::SetThresh(int ch,
	unsigned long int thresh){
//  printf("Setting threshold for trigger forchannel %i to 0x%x\n", ch, thresh);
  if(ch>=0 && ch<8){
    vme.wl(vme2_image, cardBase+RED_Trigger_Threshold_ADC[ch], thresh);
    threshold[ch]=thresh;
  }
   else
    printf("channel %i does not exist\n", ch);
}

void Readout_class_sis3302::SetDAC(int ch,unsigned long int val){
  unsigned long int val_ch;
  unsigned int regval;
  unsigned long int max_timeout,timeout_cnt;

  if(ch<0 || ch > 7){
    printf("Error setting DAC ch %i\n", ch);
    return;
  }
    
  val_ch = ch << 4;
  timeout_cnt = 0;
  max_timeout = 5000;
    
  vme.wl(vme2_image, cardBase+SIS3302_DAC_Data, val);
  vme.wl(vme2_image, cardBase+SIS3302_DAC_CntrStat,
                        1+val_ch);// write to DAC Register
  do{
    vme.rl(vme2_image, cardBase+SIS3302_DAC_CntrStat, &regval);
    timeout_cnt ++;
  }while(((regval & 0x8000) == 0x8000) && (timeout_cnt < max_timeout));
  vme.wl(vme2_image, cardBase+SIS3302_DAC_CntrStat, 2+val_ch); 
  timeout_cnt = 0;
  do{
    vme.rl(vme2_image, cardBase+SIS3302_DAC_CntrStat, &regval);
    timeout_cnt ++;
  }while(((regval & 0x8000) == 0x8000) &&
         (timeout_cnt < max_timeout));
  if(timeout_cnt<max_timeout);
      //printf("Success! Set Dac #%i to %x\n",ch,val);
  else printf("Loading DAC Failed\n");
}

unsigned int Readout_class_sis3302::calc_RED_pointers(unsigned int org){
  if(RED==0) return org;
  else{                  // reduced reduced address space
    return( ((org&0xfff00000)>>4) + (org&0x0fffff) );
  }           // shift down address bits 27:40 by 4 if it is reduced address space
}

void Readout_class_sis3302::copy_RED_pointers(void){
   if(RED!=0) printf("Calculating RED pointers\n");
   RED_ADC_Mem[0]=calc_RED_pointers(SIS3302_ADC_Mem_1);
   RED_ADC_Mem[1]=calc_RED_pointers(SIS3302_ADC_Mem_2);
   RED_ADC_Mem[2]=calc_RED_pointers(SIS3302_ADC_Mem_3);
   RED_ADC_Mem[3]=calc_RED_pointers(SIS3302_ADC_Mem_4);
   RED_ADC_Mem[4]=calc_RED_pointers(SIS3302_ADC_Mem_5);
   RED_ADC_Mem[5]=calc_RED_pointers(SIS3302_ADC_Mem_6);
   RED_ADC_Mem[6]=calc_RED_pointers(SIS3302_ADC_Mem_7);
   RED_ADC_Mem[7]=calc_RED_pointers(SIS3302_ADC_Mem_8);

   RED_Event_Config_ALL_ADC=calc_RED_pointers(SIS3302_Event_Config_ALL_ADC);

   RED_Sample_Address_ADC[0]=calc_RED_pointers(SIS3302_Sample_Address_ADC1);
   RED_Sample_Address_ADC[1]=calc_RED_pointers(SIS3302_Sample_Address_ADC2);
   RED_Sample_Address_ADC[2]=calc_RED_pointers(SIS3302_Sample_Address_ADC3);
   RED_Sample_Address_ADC[3]=calc_RED_pointers(SIS3302_Sample_Address_ADC4);
   RED_Sample_Address_ADC[4]=calc_RED_pointers(SIS3302_Sample_Address_ADC5);
   RED_Sample_Address_ADC[5]=calc_RED_pointers(SIS3302_Sample_Address_ADC6);
   RED_Sample_Address_ADC[6]=calc_RED_pointers(SIS3302_Sample_Address_ADC7);
   RED_Sample_Address_ADC[7]=calc_RED_pointers(SIS3302_Sample_Address_ADC8);

   RED_Trigger_Setup_ADC[0]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC1);
   RED_Trigger_Setup_ADC[1]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC2);
   RED_Trigger_Setup_ADC[2]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC3);
   RED_Trigger_Setup_ADC[3]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC4);
   RED_Trigger_Setup_ADC[4]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC5);
   RED_Trigger_Setup_ADC[5]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC6);
   RED_Trigger_Setup_ADC[6]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC7);
   RED_Trigger_Setup_ADC[7]=calc_RED_pointers(SIS3302_Trigger_Setup_ADC8);

   RED_Trigger_Threshold_ADC[0]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC1);
   RED_Trigger_Threshold_ADC[1]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC2);
   RED_Trigger_Threshold_ADC[2]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC3);
   RED_Trigger_Threshold_ADC[3]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC4);
   RED_Trigger_Threshold_ADC[4]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC5);
   RED_Trigger_Threshold_ADC[5]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC6);
   RED_Trigger_Threshold_ADC[6]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC7);
   RED_Trigger_Threshold_ADC[7]=calc_RED_pointers(SIS3302_Trigger_Threshold_ADC8);

   RED_TriggerFlagClear[0]=calc_RED_pointers(SIS3302_TriggerFlagClear_ADC12);
   RED_TriggerFlagClear[1]=calc_RED_pointers(SIS3302_TriggerFlagClear_ADC34);
   RED_TriggerFlagClear[2]=calc_RED_pointers(SIS3302_TriggerFlagClear_ADC56);
   RED_TriggerFlagClear[3]=calc_RED_pointers(SIS3302_TriggerFlagClear_ADC78);

   RED_EventDirectory_ADC[0]=calc_RED_pointers(SIS3302_EventDirectory_ADC1);
   RED_EventDirectory_ADC[1]=calc_RED_pointers(SIS3302_EventDirectory_ADC2);
   RED_EventDirectory_ADC[2]=calc_RED_pointers(SIS3302_EventDirectory_ADC3);
   RED_EventDirectory_ADC[3]=calc_RED_pointers(SIS3302_EventDirectory_ADC4);
   RED_EventDirectory_ADC[4]=calc_RED_pointers(SIS3302_EventDirectory_ADC5);
   RED_EventDirectory_ADC[5]=calc_RED_pointers(SIS3302_EventDirectory_ADC6);
   RED_EventDirectory_ADC[6]=calc_RED_pointers(SIS3302_EventDirectory_ADC7);
   RED_EventDirectory_ADC[7]=calc_RED_pointers(SIS3302_EventDirectory_ADC8);
} // void Readout_class_sis3302::copy_RED_pointers(void)
