#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "sis3316.h"

#include "main.h"
#include "readout_class.h"

#define I2C_ACK			8
#define I2C_START		9
#define I2C_REP_START		10
#define I2C_STOP		11
#define I2C_WRITE		12
#define I2C_READ		13
#define I2C_BUSY		31
#define OSC_ADR	0x55


Readout_class_sis3316::~Readout_class_sis3316() {
    // printf("Del shortarray\n");
  delete shortarray;
}


void Readout_class_sis3316::init_module_variables(void){
  v874class_module=0;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 16;  // not including header/trail
  adc_range= 0; // reinitialized with real values in init_module this time
  strncpy(modname,"SIS3316-FADC", 14); // for screen output
  strncpy(mod_ident,"sis3316_",14);     // for parameter file readout
  further_parameters[0]=0; // the further parameters are defined later in this 
                           // module, otherwise the number
                           // parameter has 8 bits

}


void Readout_class_sis3316::lam(void){
  unsigned int regv;
  printf("This module does not provide LAM! (sis3316)\n");
  exit(0);
}

void Readout_class_sis3316::bsy_on(void){
}

void Readout_class_sis3316::bsy_off(void){

}

int Readout_class_sis3316::get_geo(void){
  return geo; // get geo address
}

void Readout_class_sis3316::set_geo(int int_geo){
}

void Readout_class_sis3316::reset(void){
//  vme.wl(vme_image, cardBase+SIS3302_LATCH_COUNTER_CLEAR, 0xf); //clr counter


}

void Readout_class_sis3316::WaitStop(int cv=1000){
  int counter=0;
  unsigned int retval;
  unsigned int calc_retval;

/*  if(bank1_armed_flag==1){
    calc_retval=0x10000;
//    bank1_armed_flag=0;
  }
  else{
    calc_retval=0x30000;
    //  bank1_armed_flag=1;
  }
*/

  do{
    vme.rl(vme2_image, 
         cardBase+SIS3316_ACQUISITION_CONTROL_STATUS, &retval);
    counter++;
//printf("Increasing counter to %i\n", counter);
    if(counter>cv){ 
//      vme.wl(vme2_image, 
//         cardBase+SIS3302_StopSampling , 1);  // stop sampling
      printf("Waitstop! 0x%x\n", retval);
      int_ok=0;
    }
  }while((retval& 0x80000) == 0x0);
//  }while(((retval&0x30000)!=calc_retval));
		
}


int Readout_class_sis3316::read_one_event(int noe, int & ok){
  unsigned int n;

  number_of_data=0;

  WaitStop();

  do {
    vme.rl(vme2_image, cardBase+ SIS3316_ACQUISITION_CONTROL_STATUS, &n);  
	//usleep(500000); //500ms  
    printf("in Loop4:   SIS3316_ACQUISITION_CONTROL_STATUS = 0x%08x     \n", n);
  }while ((n & 0x80000) == 0x0) ; // Address Threshold reached ?

  
//  vme.rl(vme2_image, cardBase+SIS3316_Trigger_Statistic_Counter_Mode_CH1_4, &n);    
//  int_noe=n;
//  printf("Event counter: %i\n", n);



  vme.rl(vme2_image, cardBase+0x1110, &n); 
  printf("-> actual_address_ram 0x%x\n", n);


  if (bank1_armed_flag == 1) {		
	vme.wl(vme2_image, cardBase + SIS3316_KEY_DISARM_AND_ARM_BANK2 , (unsigned int) 0);  //  Arm Bank2
	bank1_armed_flag = 0; // bank 2 is armed
	printf("SIS3316_KEY_DISARM_AND_ARM_BANK2 \n");
  }
   else {
     vme.wl(vme2_image, cardBase+ SIS3316_KEY_DISARM_AND_ARM_BANK1 , (unsigned int) 0);  //  Arm Bank1
     bank1_armed_flag = 1; // bank 1 is armed
     printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");
  }

  // wait here until logic is swapped (max time is trigger_gate_window_length)
  usleep(100); //unsigned int uint_usec  


  if (bank1_armed_flag == 1) {		
    memory_bank_offset_addr = 0x01000000; // ch1 Bank2 has to be readout
  }
   else {
    memory_bank_offset_addr = 0x00000000; // ch1 Bank1 has to be readout
  }

  rawdata[number_of_data++]=0xfadc1616; // header 
  for(int ch=0;ch<16;ch++) n=GetData(ch, number_of_data);  //GetData
  rawdata[number_of_data++]=0xfadceeee; // trail



  number_of_data-=2; // n of d is w/o h/t, but it was counted in 2-4 lines ago
//  printf("SIS: number_of_data: %i\n", number_of_data);
  if(save_event()==0) exit(0);

  // printf("Counter SIS sum: %i\n", number_of_data);
  return number_of_data*4; // return saved bytes
} // read_one_event


int Readout_class_sis3316::GetData(int ch, int & counter){
  unsigned int address_prev_bank;
  unsigned int address_ram_reached; // ^⁼how many samples+1 read out
  unsigned int data;
  int subch, subrange;
  int no_of_samples;
  int samplecounter=0;
  int hisvalue,ped=0, max=0, min=65500;
  unsigned int *dma_ptr;
  int dma_offset;

  subch=(ch)%4;  // act like ch 1-4 (count 0-4)
  subrange=(ch)/4; // 1-4 (0), 5-8(1), 9-12(2), 13-16(3)  

  if (subch == 0) { memory_channel_offset_addr = 0x00000000 ; } // channel 1
  if (subch == 1) { memory_channel_offset_addr = 0x02000000 ; } // channel 2			
  if (subch == 2) { memory_channel_offset_addr = 0x10000000 ; } // channel 3			
  if (subch == 3) { memory_channel_offset_addr = 0x12000000 ; } // channel 4	

  address_prev_bank=SIS3316_ADC_CH1_PREVIOUS_BANK_SAMPLE_ADDRESS_REG + 0x1000*subrange + 0x4*subch; 
  printf("Subch: %i, Subrange: %i -> address_prev_bank 0x%x\n", subch, subrange, address_prev_bank);

  vme.rl(vme2_image, cardBase+address_prev_bank, &address_ram_reached); 
  printf("-> address_ram_reached 0x%x\n", address_ram_reached);

  rawdata[counter++] = (ch<<28)+SADC_N_of_samples; // just save some more info

  printf("read data from 0x%x to internal fifo\n", 0x80000000 + memory_channel_offset_addr + memory_bank_offset_addr);
  vme.wl(vme2_image,                                    // read from fadc to intern to fifo
    cardBase + SIS3316_DATA_TRANSFER_CH1_4_CTRL_REG + (subrange*4) , 0x80000000 + memory_channel_offset_addr + memory_bank_offset_addr);

// todo: get data!
vme.rl(vme2_image,    // get fifo
	 cardBase+ SIS3316_FPGA_ADC1_MEM_BASE+(subrange*SIS3316_FPGA_ADC_MEM_OFFSET), &data);
  printf("No 2: Timestamp id format bits: 0x%x\n", data);


//  address_
// todo!  no_of_samples=rawdata[counter-1]&0x3ffffff;
  if(no_of_samples!=SADC_N_of_samples){
      printf("no_of_samples!=SADC_N_of_samples (%i / %i)\n", no_of_samples, SADC_N_of_samples);
      exit(0);
  }
  if(dma_ok==0){ // dma doesn't work, slow...
    for(int i=0; i< SADC_N_of_samples; i++){
	// wrong!    data= *(readpointer+cardBase/4+address_ram/4);  // read samples

      shortarray[samplecounter++]=(data&0xffff);  // 1st short
      shortarray[samplecounter++]=(data>>16);    // 2nd short
    }
  }
   else{  // use dma access
     printf("DMA pointer!\n");
/*      dma_offset = vme.DMAread(cardBase+address_vme, SADC_N_of_samples*2, A32, D32);
      if (dma_offset < 0) {
        printf("DMA error!!!\n");
        vme.releaseDMA();
        exit(0);  
      }
      dma_ptr= (unsigned int *) (dma_base + dma_offset);
      for(int i=1; i<SADC_N_of_samples/2; i++){
	data=*dma_ptr++;
        shortarray[samplecounter++]=(data&0xffff);  // 1st short
        shortarray[samplecounter++]=(data>>16);    // 2nd short
      }
    }
*/
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
  his[(16*SADC_N_of_samples)+ch*65536+hisvalue]++;
  // printf("counter: %i\n", counter);

  return counter;
}  //************************ GetData
  

void Readout_class_sis3316::init_module(Param * param){
  char buf[16];
  unsigned int regval;

  printf("init sis! 0x%x\n", cardBase);

  hex =  Param_getVal(param, mod_ident_compl, "hex");  // print out in hex

// *** bigger image for FLASH-ADC readout
  printf("Allocating new image...\n");
  vme2_image = vme.getImage(cardBase, 0x04fffff, A32, D32, MASTER);
  if(vme2_image<0){
    printf("\nError in allocating an image for sis3316 board %i!\n",
                                       modulenumber);
      exit(0);
  }
  vme.setOption(vme2_image, POST_WRITE_DIS);
// *** bigger image for FLASH-ADC readout

// readpointer for faster readout
  readpointer=((unsigned int *) vme.getPciBaseAddr(vme2_image));
  printf("Readpointer: 0x%x\n",readpointer);


  vme.rl(vme2_image, cardBase+SIS3316_MODID, &regval);   // give out ModID
  printf("ModID:    0x%8x\n", regval);                              

  for(int i=0; i<4; i++){ // P14, 5a Group 1-4
  vme.rl(vme2_image,     
         cardBase+SIS3316_ADC_CH1_4_FIRMWARE_REG+(0x1000*i), &regval);  // and FPGA version
    printf("FPGAID %i: 0x%08x\n", i, regval);
  }
 

  vme.wl(vme2_image,     
         cardBase+ SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
  vme.wl(vme2_image,     
         cardBase+ SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
  vme.wl(vme2_image,     
         cardBase+ SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
  vme.wl(vme2_image,     
         cardBase+ SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
  vme.wl(vme2_image,
         cardBase+ SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, 0xE0E0E0E0);  // clear error Latch bits 

  int return_code;
  vme.rl(vme2_image,     
         cardBase+SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, &regval);
  printf("SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS: 0x%08x  return_code = 0x%08x\n", regval, return_code);
		if (regval != 0x18181818) { exit(0); }


  vme.rl(vme2_image,     
         cardBase+ SIS3316_ADC_CH1_4_STATUS_REG, &regval);  
		printf("SIS3316_ADC_CH1_4_STATUS_REG     = 0x%08x \n", regval);
		if (regval != 0x130118) { exit(0); }

  vme.rl(vme2_image,     
         cardBase+ SIS3316_ADC_CH5_8_STATUS_REG, &regval);  
		printf("SIS3316_ADC_CH5_8_STATUS_REG     = 0x%08x \n", regval);
		if (regval != 0x130118) { exit(0); }

  vme.rl(vme2_image,     
         cardBase+ SIS3316_ADC_CH9_12_STATUS_REG, &regval);  
		printf("SIS3316_ADC_CH9_12_STATUS_REG    = 0x%08x \n", regval);
		if (regval != 0x130118) { exit(0); }

  vme.rl(vme2_image,     
         cardBase+ SIS3316_ADC_CH13_16_STATUS_REG, &regval);  
		printf("SIS3316_ADC_CH13_16_STATUS_REG   = 0x%08x \n\n", regval);
		if (regval != 0x130118) { exit(0); }




// ******** Sample Clock configuration sequence
  freqPreset250MHz[0] = 0x20; freqPreset250MHz[1] = 0xC2;
  freqPreset250MHz[2] = 0xBC; freqPreset250MHz[3] = 0x33;
  freqPreset250MHz[4] = 0xE4; freqPreset250MHz[5] = 0xF2;
  clock=250;

  printf("General reset\n");
  vme.wl(vme2_image, 
       cardBase+SIS3316_KEY_RESET, (unsigned int) 0x0);   //Page 14, 1
  vme.wl(vme2_image, 
       cardBase+SIS3316_KEY_DISARM, (unsigned int) 0x0);  //23 p 131  Disarm first

  printf("Set frequency\n");
  set_frequency(0, freqPreset250MHz);

// needed? not in example  vme.wl(vme2_image, 
//    cardBase+SIS3316_SAMPLE_CLOCK_DISTRIBUTION_CONTROL, (unsigned int)0); 
// P14, 2+3   Onboard progr Oscillator

// done in set_frequency   vme.wl(vme2_image, 
//         cardBase+SIS3316_KEY_ADC_CLOCK_DCM_RESET, 1); // P14, 4

  for(int i=0; i<4; i++){ // P14, 5a Group 1-4
    vme.wl(vme2_image,     
         cardBase+SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG+(0x1000*i), 0xf00);
  }
  sleep(1);
  for(int i=0; i<4; i++){ // P14, 5b Group 1-4
    vme.wl(vme2_image,                          
	   cardBase+SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG+(0x1000*i), 0x300+ 0x1008); // new value (old 0x48) for and 150MHz
  }
  printf("Set frequency done\n");
  sleep(1);
// ******** Sample Clock configuration sequence

  for(int ch=0; ch<16; ch++){
/*    sprintf(buf, "threshold%02d", ch);
    SetThresh(ch, Param_getVal(param, mod_ident_compl, buf));
    sprintf(buf, "trigger%02d", ch);
    SetTrigSetup(ch, Param_getVal(param, mod_ident_compl, buf));
    sprintf(buf, "dac%02d", ch);
    SetDAC(ch, Param_getVal(param, mod_ident_compl, buf));*/
  }

//  regval = vme.wrl(vme2_image,                          
//	     cardBase+SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, &data);  
//  printf("SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS: data = 0x%08x     return_code = 0x%08x\n", data, regval);


  vme.wl(vme2_image,                          
    cardBase+SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, 0xE0E0E0E0);  // clear error Latch bits 




// set ADC chips via SPI
	for(int i=0; i<4; i++) {
          vme.wl(vme2_image,                          
	    cardBase+(i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x81001444 ); // SPI (OE)  set binary
	  usleep(10); //unsigned int uint_usec  
          vme.wl(vme2_image,                          
	    cardBase+(i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x81401444 ); // SPI (OE)  set binary
	  usleep(10); //unsigned int uint_usec  
          vme.wl(vme2_image,                          
	    cardBase+(i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x8100ff01 ); // SPI (OE)  update
	  usleep(10); //unsigned int uint_usec  
          vme.wl(vme2_image,                     
	    cardBase+(i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x8140ff01 ); // SPI (OE)  update
	  usleep(10); //unsigned int uint_usec  
	}	


  SADC_N_of_samples=Param_getVal(param, mod_ident_compl, "no_of_samples");
  if(SADC_N_of_samples%2==1) SADC_N_of_samples++; // only even values 

// setting up trace len
  for(int i=0; i<4; i++){
    vme.wl(vme2_image, 
	   cardBase+SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG+(0x1000*i), SADC_N_of_samples<<16);
  }
// trigger delay
  trigger_delay=Param_getVal(param, mod_ident_compl, "pre_trg_delay");
  for(int i=0; i<4; i++){
    vme.wl(vme2_image, 
         cardBase+SIS3316_ADC_CH1_4_PRE_TRIGGER_DELAY_REG+(0x1000*i), trigger_delay);
  }
//	trigger_gate_window_length 	
  unsigned int trigger_gate_window_length;
  trigger_gate_window_length=SADC_N_of_samples*5;
  for(int i=0; i<4; i++){
    vme.wl(vme2_image, 
         cardBase+ SIS3316_ADC_CH1_4_TRIGGER_GATE_WINDOW_LENGTH_REG+(0x1000*i), (trigger_gate_window_length -2 & 0xffff) ); // 
  }

// now setting up external trigger:

//  vme.wl(vme2_image, 
  //       cardBase+SIS3316_KEY_DISARM, 1);  //23 p 131  Disarm first
  


  for(int i=0; i<4; i++){
    vme.wl(vme2_image, 
         cardBase+ SIS3316_ADC_CH1_4_EVENT_CONFIG_REG+(0x1000*i), 0x08080808 );  //external trigger
  }



// Enable LEMO Input "TI" as Trigger External Trigger
  vme.wl(vme2_image, 
          cardBase+ SIS3316_NIM_INPUT_CONTROL_REG , (unsigned int) 0x50);  //  

// enable "external Trigger function" (NIM In, if enabled and VME key write)
 // enable "external Timestamp clear function" (NIM In, if enabled and VME key write)
  vme.wl(vme2_image, 
         cardBase+SIS3316_ACQUISITION_CONTROL_STATUS, 0x500); 


  vme.wl(vme2_image, 
          cardBase+ SIS3316_KEY_DISARM_AND_ARM_BANK1 , (unsigned int) 0);  //  Arm Bank1
  bank1_armed_flag=1;  // start condition
// that's it, external trigger set.

// Enable LEMO Output "TO"   // Select all triggers
  vme.wl(vme2_image, 
         cardBase+ SIS3316_LEMO_OUT_TO_SELECT_REG, 0xffff ); //  

// Enable LEMO Output "CO" as sample clock
  vme.wl(vme2_image, 
         cardBase+ SIS3316_LEMO_OUT_CO_SELECT_REG, 0x1 ); //  

  for(int i=0; i<4; i++){
    vme.wl(vme2_image, 
         cardBase+ SIS3316_DATA_TRANSFER_CH1_4_CTRL_REG+0x4*i  ,  0xC0000000);   // start Write FSM
  }

  shortarray =new unsigned short[SADC_N_of_samples+4];

  ped_start=Param_getVal(param, mod_ident_compl, "ped_range_beginning");
  ped_end=Param_getVal(param, mod_ident_compl, "ped_range_end");

  further_parameters[0]=4; // otherwise the number
                           // parameter has 8 bits
  further_parameters[1]=SADC_N_of_samples; // 
  further_parameters[2]=clock;
  further_parameters[3]=ped_start; // 
  further_parameters[4]=ped_end; // 
  // further_parameters[1]=max_hits_per_event; // max hits per event

//********* buffersizes have to be calculated differently
  delete rawdata;   
  delete decoded_event;
  delete his;

  rawdata = new unsigned int[SADC_N_of_samples/2*16+16+2]; // raw head/trai/ch
  decoded_event = new unsigned int[max_no_rawdata]; // without header/trail

  his = new unsigned int [16*SADC_N_of_samples+16*65536];
  for(int n=0; n<(16*SADC_N_of_samples+16*65536); n++){
    his[n] = 0; 
  }  // reset histogram
//********* END buffersizes have to be calculated differently
  geo=Param_getVal(param, mod_ident_compl, "geo");
  int_noe=-1;
} //********** Readout_class_sis3302::init_module


int Readout_class_sis3316::save_histo(FILE * hisout){    
//  printf("Saving histo, %i, %i\n", SADC_N_of_samples, max_no_rawdata);
  fwrite(&SADC_N_of_samples, sizeof(SADC_N_of_samples), 1, hisout);
  fwrite(&max_no_rawdata, sizeof(max_no_rawdata), 1, hisout);
  return fwrite(his, sizeof(his)*(16*SADC_N_of_samples+(16*65536)), 1, hisout);
}

void Readout_class_sis3316::printeventdata(unsigned int noe){
  int t;

  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);
  for(int n=0; n<16;n++){
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

void Readout_class_sis3316::SetTrigSetup(int ch,
	unsigned long int thresh){
//  printf("Setting trigger setup for channel %i to 0x%x\n", ch, thresh);
/*
  if(ch>=0 && ch<16){
    vme.wl(vme2_image, cardBase+RED_Trigger_Setup_ADC[ch], thresh);
  }
   else
    printf("channel %i does not exist\n", ch);
*/
}

//===================================================================
void Readout_class_sis3316::SetThresh(int ch,
	unsigned long int thresh){
//  printf("Setting threshold for trigger forchannel %i to 0x%x\n", ch, thresh);
/*  if(ch>=0 && ch<16){
    vme.wl(vme2_image, cardBase+RED_Trigger_Threshold_ADC[ch], thresh);
    threshold[ch]=thresh;
  }
   else
    printf("channel %i does not exist\n", ch);
*/
}

void Readout_class_sis3316::SetDAC(int ch,unsigned long int val){
/*
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

*/
}


int Readout_class_sis3316::I2cStart(int osc){
	int rc;
	int i;
	unsigned int tmp;

	if(osc > 3){
		return -101;
	}

	// start
        rc=vme.wl(vme2_image, 
          cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), 1<<I2C_START);

	if(rc){
		return rc;
	}

	i = 0;
	do{
		// poll i2c fsm busy
           rc=vme.rl(vme2_image, 
              cardBase+  SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
		if(rc){
			return rc;
		}
		i++;
	}while((tmp & (1<<I2C_BUSY)) && (i < 1000));

	// register access problem
	if(i == 1000){
		return -100;
	}

	return 0;
}

int Readout_class_sis3316::I2cStop(int osc){
	int rc;
	int i;
	unsigned int tmp;

	if(osc > 3){
		return -101;
	}

	// stop 
        rc=vme.wl(vme2_image, 
          cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), 1<<I2C_STOP);
	if(rc){
		return rc;
	}
	
	i = 0;
	do{
		// poll i2c fsm busy
	       
          rc=vme.rl(vme2_image, 
            cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
		if(rc){
			return rc;
		}
		i++;
	}while((tmp & (1<<I2C_BUSY)) && (i < 1000));

	// register access problem
	if(i == 1000){
		return -100;
	}

	return 0;
}



int Readout_class_sis3316::set_frequency(int osc, unsigned char *values){
	int rc;

	if(values == NULL){
		return -100;
	}
	if(osc > 3 || osc < 0){
		return -100;
	}

	rc = this->Si570FreezeDCO(osc);
	if(rc){
		return rc;
	}

	rc = this->Si570Divider(osc, values);
	if(rc){
		return rc;
	}

	rc = this->Si570UnfreezeDCO(osc);
	if(rc){
		return rc;
	}

	rc = this->Si570NewFreq(osc);
	if(rc){
		return rc;
	}

// min. 10ms wait
	sleep(1);

	// DCM Reset
        rc=vme.wl(vme2_image, 
          cardBase+ 0x438, (unsigned int) 0);
	if(rc){
		return rc;
	}


	return 0;
}

//        rc=vme.wl(vme2_image, 
//          cardBase+ 

int Readout_class_sis3316::Si570FreezeDCO(int osc){
	int rc;
	char ack;

	// start
	rc = this->I2cStart(osc);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	// address
	rc = this->I2cWriteByte(osc, OSC_ADR<<1, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// register offset
	rc = this->I2cWriteByte(osc, 0x89, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// write data
	rc = this->I2cWriteByte(osc, 0x10, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// stop
	rc = this->I2cStop(osc);
	if(rc){
		return rc;
	}

	return 0;
}

int Readout_class_sis3316::Si570Divider(int osc, unsigned char *data){
	int rc;
	char ack;
	int i;

	// start
	rc = this->I2cStart(osc);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	// address
	rc = this->I2cWriteByte(osc, OSC_ADR<<1, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// register offset
	rc = this->I2cWriteByte(osc, 0x0D, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// write data
	for(i = 0;i < 2;i++){
		rc = this->I2cWriteByte(osc, data[i], &ack);
		if(rc){
			this->I2cStop(osc);
			return rc;
		}

		if(!ack){
			this->I2cStop(osc);
			return -101;
		}
	}

	// stop
	rc = this->I2cStop(osc);
	if(rc){
		return rc;
	}

	return 0;
}

int Readout_class_sis3316::Si570UnfreezeDCO(int osc){
	int rc;
	char ack;

	// start
	rc = this->I2cStart(osc);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	// address
	rc = this->I2cWriteByte(osc, OSC_ADR<<1, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// register offset
	rc = this->I2cWriteByte(osc, 0x89, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// write data
	rc = this->I2cWriteByte(osc, 0x00, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// stop
	rc = this->I2cStop(osc);
	if(rc){
		return rc;
	}

	return 0;
}

int Readout_class_sis3316::Si570NewFreq(int osc){
	int rc;
	char ack;

	// start
	rc = this->I2cStart(osc);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	// address
	rc = this->I2cWriteByte(osc, OSC_ADR<<1, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// register offset
	rc = this->I2cWriteByte(osc, 0x87, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// write data
	rc = this->I2cWriteByte(osc, 0x40, &ack);
	if(rc){
		this->I2cStop(osc);
		return rc;
	}

	if(!ack){
		this->I2cStop(osc);
		return -101;
	}

	// stop
	rc = this->I2cStop(osc);
	if(rc){
		return rc;
	}

	return 0;
}



int Readout_class_sis3316::I2cWriteByte(int osc, unsigned char data, char *ack){
	int rc;
	int i;
	unsigned int tmp;
	
	if(osc > 3){
		return -101;
	}

	// write byte, receive ack
        rc=vme.wl(vme2_image, 
          cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), 1<<I2C_WRITE ^ data);
	if(rc){
		return rc;
	}
	
	i = 0;
	do{
		// poll i2c fsm busy
        rc=vme.rl(vme2_image, 
          cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
		if(rc){
			return rc;
		}
		i++;
	}while((tmp & (1<<I2C_BUSY)) && (i < 1000));

	// register access problem
	if(i == 1000){
		return -100;
	}

	// return ack value?
	if(ack){
		// yup
		*ack = tmp & 1<<I2C_ACK ? 1 : 0;
	}

	return 0;
}

int Readout_class_sis3316::I2cReadByte(int osc, unsigned char *data, char ack){
	int rc;
	int i;
	unsigned int tmp;
	
	if(osc > 3){
		return -101;
	}

	// read byte, put ack
	tmp = 1<<I2C_READ;
	tmp |= ack ? 1<<I2C_ACK : 0;
        rc=vme.wl(vme2_image, 
          cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), tmp);
	if(rc){
		return rc;
	}
	
	i = 0;
	do{
		// poll i2c fsm busy
        rc=vme.rl(vme2_image, 
          cardBase+ SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
		if(rc){
			return rc;
		}
		i++;
	}while((tmp & (1<<I2C_BUSY)) && (i < 1000));

	// register access problem
	if(i == 1000){
		return -100;
	}

	return 0;
}
