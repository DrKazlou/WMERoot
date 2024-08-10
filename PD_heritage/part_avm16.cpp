#include <string.h>
#include <stdio.h>

#include "vmelib.h"

#include "param.h" 
#include "avm16.h"

#include "main.h"
#include "readout_class.h"



Readout_class_avm16::~Readout_class_avm16() {
  if(dbg>0) printf("Del rawhis\n");
  delete rawhis;
}


void Readout_class_avm16::init_module_variables(void){
  v874class_module=0;
  max_no_rawdataSMALL=16;  // not including header/trail
  max_no_rawdataBIG= 16;  // not including header/trail
  adc_range= 0; // reinitialized with real values in init_module this time
  strncpy(modname,"AVM16-FADC", 12); // for screen output
  strncpy(mod_ident,"avm16_",12);     // for parameter file readout
  further_parameters[0]=0; // the further parameters are defined later in this 
                           // module, otherwise the number
                           // parameter has 8 bits

}


void Readout_class_avm16::lam(void){
  unsigned int regv;
  printf("This module does not provide LAM! (avm16)\n");
  exit(0);
//  vme.rl(vme_image, cardBase+SIS3302_IRQ_CONTROL, &regv); 
}

void Readout_class_avm16::bsy_on(void){
}

void Readout_class_avm16::bsy_off(void){

}

int Readout_class_avm16::get_geo(void){
  return geo; // get geo address
}

void Readout_class_avm16::set_geo(int int_geo){
  geo=int_geo;
}

void Readout_class_avm16::reset(void){
//  vme.wl(vme_image, cardBase+SIS3302_LATCH_COUNTER_CLEAR, 0xf); //clr counter
 printf("Master RESET\n");
 vme.wl(vme_image, cardBase+AVM16_ACT, 2); //


}


int Readout_class_avm16::read_one_event(int noe, int & ok){
  unsigned state, no_of_bytes, filler;
  unsigned int *dma_ptr;
  int dma_offset;
  int hisvalue,ped=0, max=0, min=65500;
  int ch_done[16];

  rawdata[0]=0xfadcfafa;  // internal header
  
//  for(int n=0; n<16*samples_to_readout; n++) rawhis[n]=0; // delete rawhis
  memset(rawhis, 0, (16*samples_to_readout)*4);    // set all rawhis to 0 

  do{
    vme.rl(vme_image, cardBase+AVM16_STATE, &state);
    // read state if there is data in fifo
    if((state&0x200)==0x200){
      printf("ERROR. Fifo buffer full\n");
      exit(0);
    }
//   printf("AVM16 STATE: 0x%x\n\n", state);
  }while((state&0x1)==0); // wait till there is data valid

  do{
    vme.rl(vme_image, cardBase+AVM16_DLENGTH, &no_of_bytes); 
    // no of datawords to read out should be>0
//    printf("AVM16: %i datawords to read out\n", no_of_bytes/4);   
  }while(no_of_bytes==0);


  if(dma_ok==0){ // dma doesn't work, slow...
    printf("no dma, normal readout not implemented yet...\n");
    exit(0);
  }
  else{
      //  printf("DMA  readout!\n");
    if(no_of_bytes%8!=0) filler=4; // fillerbytes for dma readout
    else filler=0;
//    printf("AVM16: %i BYTES to read out\n\n", no_of_bytes);   

    dma_offset = vme.DMAread(cardBase+AVM16_DATA_RANGE, 
                             no_of_bytes+filler, A32, D32);
    if (dma_offset < 0) {
      printf("DMA error!!!\n");
      vme.releaseDMA();
      exit(0);
    }
    dma_ptr= (unsigned int *) (dma_base + dma_offset);
    for(int n=0; n< no_of_bytes/4; n++){
	rawdata[n+1]=*dma_ptr++;
    }
    rawdata[(no_of_bytes/4)+1]=0xcdafcdaf;  // internal trail
  }

  if((rawdata[1]&0x7fffffff)!=no_of_bytes){
    printf("ERROR! %u bytes due to header, %u due to register in %s\n",
           rawdata[1]&0x7fffffff, no_of_bytes, mod_ident_compl);
    exit(0);
  }
  if(dbg>2) printf("Length of block in BYTE: %u in %s \n", 
                    rawdata[1]&0x7fffffff, mod_ident_compl);
//  printf("Trigger time: %u (0x%x) ", rawdata[2], rawdata[2]);
//  printf("Eventnumber: %u\n", rawdata[3]);

  int rawdatanumber[16], min_channel, old_ch;

  for(int n=0; n<16;n++){
    rawdatanumber[n]=0;
    ch_done[n]=0;
    integral[n]=0; win_start_time[n]=0; mean_lvl[n]=0;
    min_time[n]=0; min_lvl[n]=0; max_time[n]=0; max_lvl[n]=0;
    zero_cross[n]=0; inst_mean_line_lvl[n]=0; mean_val_before[n]=0;
    int_of_iw[n]=0; mean_val_after[n]=0; own_ph[n]=0;
  }
  min_channel=0;
  old_ch=42;
  
  for(int n=4; n<((no_of_bytes/4)+4); n++){ // cycle over the remaining data
    card_address = ((rawdata[n]>>28)&0xf);
    channel = ((rawdata[n]>>22)&0xf);
    data_ident = ((rawdata[n]>>16)&0x3f);

    if(channel<0 || channel >15){
      printf("Error! Channel number too big (%i on module %s)\n", 
                 channel, mod_ident_compl);
      exit(0);
    }
    if(((rawdata[n]>>12)&0x3ff)==0x0){  // raw data
/*      if(channel!=old_ch){
        printf("channel: %i\n", channel);
        old_ch=channel;
	} */
      if(channel<min_channel && dbg>0){
        printf("Jump back channel from %i to %i\n", min_channel, channel);
      }
      min_channel=channel;

/*      if(rawdatanumber[channel]==(samples_to_readout-1)){
	printf("Estimated end rawdatapoints in ch %i: %i\n", 
                channel, rawdatanumber[channel]);
		} */
      if(rawdatanumber[channel]<samples_to_readout){
        // copy rawdata to his
        rawhis[channel*samples_to_readout+rawdatanumber[channel]]=(rawdata[n]&0xfff);
        his[channel*samples_to_readout+rawdatanumber[channel]]=(rawdata[n]&0xfff);
      }
      else{
	if(dbg>0) printf("Too many rawdatapoints in ch %i: %i\n", 
                channel, rawdatanumber[channel]);
      }
      rawdatanumber[channel]++;   
    }
    else{
      if(((data_ident>>4)&0x3)==0x2){ // Integral
	if(ch_done[channel]==0){ // first come, only serve
	  integral[channel]= rawdata[n]&0xffff;  // really 20bit?
//          printf("integral ch %i: %i\n", channel, integral[channel]);
/*          if(rawdata[n]&0x80000==0x80000){ 
            integral[channel]*=-1; // signed 20bit
	    }  */
          ch_done[channel]=0;
	}
      }
      else switch(data_ident){
	case 0x30: // window start time
          win_start_time[channel]=rawdata[n]&0xffff;
          break;
        case 0x31: // mean level
          mean_lvl[channel]=rawdata[n]&0xffff;
          break;
	case 0x32: // minimum time
          min_time[channel]=rawdata[n]&0xffff;
          break;
	case 0x33: // minimum level
          min_lvl[channel]=rawdata[n]&0xffff;
          break;
	case 0x34: // maximum time
          max_time[channel]=rawdata[n]&0xffff;
          break;
	case 0x35: // maximum level
	case 0x3d: // maximum level overflow bit
          max_lvl[channel]=rawdata[n]&0xffff;
          break;
	case 0x36:  // zero crossing
          zero_cross[channel]=rawdata[n]&0xffff;
          break;
        case 0x37: // data integral window     
        case 0x20: // data integral window     
	    // fucked up in module sometimes, ignore...
  /*        inst_mean_line_lvl=rawdata[n++]&0xffff;
          if(((rawdata[n]>>16)&0x37)!=0x37){
	    printf("Error in integral window block 2 (0x%x): data_ident: 0x%x\n",
                  rawdata[n], ((rawdata[n]>>16)&0x37));
            exit(0);
	  }
          mean_val_before==rawdata[n++]&0xffff;
          if(((rawdata[n]>>16)&0x20)!=0x20){
	    printf("Error in integral window block 3 (0x%x): data_ident: 0x%x\n",
                  rawdata[n], ((rawdata[n]>>16)&0x37));

	    exit(0);
	  }
          int_of_iw=rawdata[n++]&0xffff;
          if(((rawdata[n]>>16)&0x37)!=0x37){
	    printf("Error in integral window block 4 (0x%x): data_ident: 0x%x\n",
                  rawdata[n], ((rawdata[n]>>16)&0x37));

	    exit(0);
	  }
           mean_val_after=rawdata[n]&0xffff;
  */
          break;
        default:
	  printf("Error: Unknown data_ident: 0x%x (%s)\n",
                  data_ident, mod_ident_compl);
	  exit(0);
      } 
    }
  }
  //  create own pulseheigth

  for(int ch=0; ch<16;ch++){
    if(raw_ch_ena[ch]==1){
      ped=0;
      max=0;
      min=65500;

      for(int n=0; n<samples_to_readout; n++){
        // for his
        hisvalue=rawhis[ch*samples_to_readout+n];
        if(hisvalue> max) max=hisvalue;
        if(hisvalue< min) min=hisvalue;
        if(n<ped_start | n>=(samples_to_readout-ped_end)){
	  ped+=hisvalue;
	}
      }
      ped/=(ped_start+ped_end);
      pedestal[ch]=ped;
      if(max-ped>ped-min){
        own_ph[ch]=max-ped;
        value[ch]=max;    
      }
       else{
        own_ph[ch]=ped-min;
        value[ch]=min;    
      }
    }
    else{  // no rawdata in there
      for(int n=0; n<samples_to_readout; n++){
        his[ch*samples_to_readout+n]=0;
      }
      pedestal[ch]=0;
      value[ch]=0;
      own_ph[ch]=0;     
    }
//  fill his
    his[samples_to_readout*16 + (ch)*65536+win_start_time[ch]+32000]++;
    his[samples_to_readout*16 + (ch+16)*65536+(integral[ch]/16)+32000]++;
                                            // otherwise 20 bit :-/
    his[samples_to_readout*16 + (ch+32)*65536+own_ph[ch]]++;
 
  }

  if(dbg>2){
    for(int n=0; n<16; n++){
      printf("Number of rawdata in ch %i: %i\n", n, rawdatanumber[n]);
    }
  }
//  printf("\n");

  number_of_data=(rawdata[1]&0x7fffffff)/4;  // (Length of block in BYTE)/4
  if(save_event()==0) exit(0);
  int_noe=rawdata[3];  

  if(int_noe!=noe){
    printf("%s: Internal NoE (%i) != NoE (%i)\n", 
               mod_ident_compl, int_noe, noe);
    int_ok=0;
    ok=0;
  }

//  for(int n=0; n<16;n++)
//          printf("end integral ch %i: %i\n", n, integral[n]);

  return (number_of_data+2)*4; // return saved bytes
} // read_one_event


void Readout_class_avm16::init_module(Param * param){
  char buf[16];
  unsigned int regval;
  int address;

  printf("init avm16! Baseaddress: 0x%x\n", cardBase);

  readpointer=((unsigned int *) vme.getPciBaseAddr(vme_image));
//  printf("Readpointer: 0x%x\n",readpointer);

  vme.rl(vme_image, cardBase+AVM16_IDENT, &regval); 
  printf("Ident: 0x%x\n", regval);
 
  printf("General reset (0x%x): 0x%x\n",AVM16_CR, (unsigned int) 0x80);
  vme.wl(vme_image, cardBase+AVM16_CR, (unsigned int) 0x80);

  regval = 0x2;
  printf("Sync timer RESET (0x%x): 0x%x\n", AVM16_ACT, regval);
  vme.wl(vme_image, cardBase+AVM16_ACT, regval); //

  regval = 0x1;
  printf("Master RESET (0x%x): 0x%x\n", AVM16_ACT, regval);
  vme.wl(vme_image, cardBase+AVM16_ACT, regval); //1

//  printf("Set back RESET\n");
//  vme.wl(vme_image, cardBase+AVM16_ACT, (unsigned int) 0x0); //

  sprintf(buf, "mod_type");
  mod_type=Param_getVal(param, mod_ident_compl, buf);
  printf("Mod Type (0x%x): 0x%x\n", AVM16_MOD_TYPE, mod_type);
  vme.wl(vme_image, cardBase+AVM16_MOD_TYPE, mod_type);


  vme.rl(vme_image, cardBase+AVM16_MOD_TYPE, &regval); 
  printf("MOD TYPE read out: 0x%x\n\n", regval);  


  printf("COM IDs (0x%x): 0x%x\n", AVM16_COM_IDS, 0x0);
  vme.wl(vme_image, cardBase+AVM16_COM_IDS, regval);

 
  sprintf(buf, "cr");
  cr=Param_getVal(param, mod_ident_compl, buf);
  regval = cr & 0xfe;
  printf("General Settings, CR (0x%x): 0x%x\n", AVM16_CR, regval);
  vme.wl(vme_image, cardBase+AVM16_CR, regval);

  
  sprintf(buf, "cha_inh");
  cha_inh=Param_getVal(param, mod_ident_compl, buf);
  printf("Channel inhibit mask (0x%x): 0x%x\n", AVM16_CHA_INH, cha_inh);
  vme.wl(vme_image, cardBase+AVM16_CHA_INH, cha_inh);


  sprintf(buf, "cha_raw");
  cha_raw=Param_getVal(param, mod_ident_compl, buf);
  printf("Enable channel raw mask (0x%x): 0x%x\n", AVM16_CHA_RAW, cha_raw);
  vme.wl(vme_image, cardBase+AVM16_CHA_RAW, cha_raw);
  no_of_raw_ch=0;
  for(int n=0; n<16; n++){
      if(dbg>2) printf("n: %i (%i) of 0x%x\n",n, ((cha_raw>>n)&0x1), cha_raw);
    if(((cha_raw>>n)&0x1)==0x1){  // if particular channel is enabled
      raw_ch_ena[n]=1; // this one is ena (for his)
      no_of_raw_ch++;
      if (dbg>2) printf("Enable rawdata: Bit %i of 0x%x is used\n",
                         n, cha_raw);
    }
    else raw_ch_ena[n]=0;
  }
  printf("%i channels will be read out raw\n", no_of_raw_ch);


  printf("Baseline offset\n");
  for(int n=0;n<16;n++){
    sprintf(buf, "offset_dac%02i", n);
    regval =  Param_getVal(param, mod_ident_compl, buf);
    regval =  regval&0xfff;  // pure data
    regval += 0x2000;     // update DACs and not powered down
    regval += (n%4)<<14;  // 4 channels per address at bit 14&15

    address=AVM16_OFFSET_DAC+(n/4)*0x4;

    printf("%i address: 0x%x, data 0x%x\n", n, address, regval);
    vme.wl(vme_image, cardBase+address, regval);
  }

  sprintf(buf, "trg_level");
  trg_level=Param_getVal(param, mod_ident_compl, buf);
  printf("Trigger level (0x%x): %i\n", AVM16_TRG_LEVEL, trg_level);
  vme.wl(vme_image, cardBase+AVM16_TRG_LEVEL, trg_level);

  sprintf(buf, "anal_ctrl");
  anal_ctrl=Param_getVal(param, mod_ident_compl, buf);
  printf("Analysis control (0x%x): 0x%x\n", AVM16_ANAL_CTRL, anal_ctrl);
  vme.wl(vme_image, cardBase+AVM16_ANAL_CTRL, anal_ctrl);

  sprintf(buf, "iw_start");
  iw_start=Param_getVal(param, mod_ident_compl, buf);
  printf("Integral window start (0x%x): %i\n", AVM16_IW_START, iw_start);
  vme.wl(vme_image, cardBase+AVM16_IW_START, iw_start);

  sprintf(buf, "iw_length");
  iw_length=Param_getVal(param, mod_ident_compl, buf);
  printf("Integral window length (0x%x): %i\n", AVM16_IW_LENGTH, iw_length);
  vme.wl(vme_image, cardBase+AVM16_IW_LENGTH, iw_length);

  sprintf(buf, "sw_start");
  sw_start=Param_getVal(param, mod_ident_compl, buf);
  printf("Trigger latency (0x%x): %i\n", AVM16_SW_START, sw_start);
  vme.wl(vme_image, cardBase+AVM16_SW_START, sw_start);

  sprintf(buf, "sw_length");
  sw_length=Param_getVal(param, mod_ident_compl, buf);
  printf("length of time window (0x%x): %i\n", AVM16_SW_LENGTH, sw_length);
  vme.wl(vme_image, cardBase+AVM16_SW_LENGTH, sw_length);
  samples_to_readout=((sw_length+1)*2);

  sprintf(buf, "sw_intlength");
  sw_intlength=Param_getVal(param, mod_ident_compl, buf);
  printf("Integral length of pulse integrals (0x%x): %i\n",
                                    AVM16_SW_INTLENGTH, sw_intlength);
  vme.wl(vme_image, cardBase+AVM16_SW_INTLENGTH, sw_intlength);


  regval=0;
  printf("ACLCK_SHIFT (0x%x): %i\n",
                              AVM16_ACLCK_SHIFT, regval);
  vme.wl(vme_image, cardBase+AVM16_ACLCK_SHIFT, regval);

  
  for(int n=0;n<16;n++){
    sprintf(buf, "q_threshold%02i", n);
    regval =  Param_getVal(param, mod_ident_compl, buf);
    address=AVM16_Q_THRESHOLD+(n*0x4);

    printf("%i Q_threshold address: 0x%x, data: %i\n", n, address, regval);
    vme.wl(vme_image, cardBase+address, regval);
  }


  vme.rl(vme_image, cardBase+AVM16_CR, &regval); 
  regval &= 0xff;
  regval |= 0x1;
  printf("Enable with CR (0x%x): 0x%x\n", AVM16_CR, regval);
  vme.wl(vme_image, cardBase+AVM16_CR, regval);

  ped_start=Param_getVal(param, mod_ident_compl, "ped_range_beginning");
  ped_end=Param_getVal(param, mod_ident_compl, "ped_range_end");


  rawhis =new int[16*samples_to_readout];


/* old from sis3302

//********* buffersizes have to be calculated differently
  delete decoded_event;

  decoded_event = new unsigned int[max_no_rawdata]; // without header/trail

//********* END buffersizes have to be calculated differently

*/


//********* buffersizes have to be calculated differently
  delete his;
  his = new unsigned int [16*samples_to_readout+48*65536];
  for(int n=0; n<(16*samples_to_readout+48*65536); n++){
    his[n] = 0; 
  }  // reset histogram



  further_parameters[0] = 13; // otherwise the number
                           // parameter has 8 bits
  further_parameters[ 1] = mod_type; 
  further_parameters[ 2] = cr; 
  further_parameters[ 3] = cha_inh; 
  further_parameters[ 4] = cha_raw;
  further_parameters[ 5] = trg_level;
  further_parameters[ 6] = anal_ctrl;
  further_parameters[ 7]= iw_start;
  further_parameters[ 8]= iw_length;
  further_parameters[ 9]= sw_start;
  further_parameters[10]= sw_length;
  further_parameters[11]= sw_intlength;
  further_parameters[12]= ped_start;
  further_parameters[13]= ped_end;

  delete rawdata;   
  rawdata = new unsigned int[68*1024+2*4]; // 68k max buffer+ raw head/trai/ch

  geo=Param_getVal(param, mod_ident_compl, "geo");
  int_noe=-1;
} //********** Readout_class_avm16::init_module


int Readout_class_avm16::save_histo(FILE * hisout){    
  if(dbg>0) printf("Saving histo, %i, %i\n", samples_to_readout, max_no_rawdata);
  fwrite(&samples_to_readout, sizeof(samples_to_readout), 1, hisout);
  fwrite(&max_no_rawdata, sizeof(max_no_rawdata), 1, hisout);
  return fwrite(his, sizeof(his)*(16*samples_to_readout+(48*65536)), 1, hisout);

}

void Readout_class_avm16::printeventdata(unsigned int noe){
  int t;

  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);
  for(int n=0; n<16;n++){
    printf("ch: %2i, ped: %5i, value: %5i, own_ph: %5i  \t",
              n, pedestal[n], value[n], own_ph[n], integral[n]);
    if(n%2==1) printf("\n"); 
  }

  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}

