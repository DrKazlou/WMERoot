
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h> // for future

#include <iostream>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib> 
#include "vmelib.h"
#include <time.h>
//#include "param.h" 
#include "v874.h"
#include "v792.h"
#include "sis3302.h"

#include <setjmp.h>
#include <signal.h>

#include "main.h"

#define PORT 4000
#define MAX 16200 //16080
#define SA struct sockaddr


static jmp_buf terminateJmp;

unsigned int *readpointer_tdc, *readpointer_iol, *readpointer_sis, *readpointer_qdc;
static VMEBridge vme;
ReducedPointers_t RED;

 unsigned EventConfig; //wrap around page until stop, size = 3->256 samples
 unsigned WrapPageSize_Bytes; //16M Samples max * 2Bytes/Sample
 unsigned int page_size;
 bool fDMA;
 int dma_base = 0;
 unsigned int tdc_arr[32], qdc_arr[16], sadc_arr[8];
 char tdc_full[3*32+1], qdc_full[3*32+1], trace_long[8*2000], trace_hex[2000], trace_arr[8][2000];

 bool fPrint;

void handler(int sig);

//void do_readout(int &vme_image, unsigned cardBase){
//}

void lam_iol(int &vme_image, unsigned int cardBase){
    short unsigned int regv;
    vme.ww(vme_image, cardBase+(V874_AUX)+2, 1); // bsy_off | reset ff iol custom

    vme.rw(vme_image, cardBase+V874_STAT_REG_1, &regv);
    regv &= 0x1;
    
    while(regv!=0x1){
    // polling data ready bit
	//printf("Wait a little bit xD ");
	//sleep(1);
	for(int n=0; n<=10;n++)printf("");// play around with that circle; previously 1000
	//printf("\n");
	
	vme.rw(vme_image, cardBase+V874_STAT_REG_1, &regv);
	regv &= 0x1;
    }	
}


void reset_board(int &vme_image, unsigned int cardBase, char* board_name){
    short unsigned int regv;
    
    vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x0004); //Clear data
    vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x0004); //Clear data
    printf("Reset %s at 0x%08X \n", board_name,cardBase);

}

void init_board(int &vme_image, unsigned int cardBase, char* board_name){ // , unsigned int *readpointer){
    
    unsigned int data;

    if(board_name =="IoL"){
	int dma_base = 0;
    
	printf("Init VME image \n");
	vme_image = vme.getImage(0x00000000, 0x0FFFFFFF, A32, D32, MASTER);
    
	if (vme_image<0){
	    printf("\n Error [%i]  in allocating an image! \n",vme_image);
	    exit(0);
	}
	else
	    printf("\n Allocation image success [%i]! \n",vme_image);
	
	vme.setOption(vme_image, POST_WRITE_DIS);
    
	printf("Trying to enable DMA !\n");// if enable DMA == 1
	dma_base = vme.requestDMA();
	if (!dma_base){
	    printf("Can`t allocate DMA [%i]! \n", dma_base);
	    fDMA = false;
	}
	else{
	    printf("DMA enabled [%i] ! \n", dma_base);
	    fDMA = true;
	}
	vme.setOption(DMA, BLT_ON);
    }
 
    //printf("Pci Base Addr 0x%08X \n", vme.getPciBaseAddr(vme_image));
    if (board_name == "IoL"){
	readpointer_iol = ((unsigned int*)vme.getPciBaseAddr(vme_image)) + cardBase/4;
        printf("readpointer 0x%08X \n", readpointer_iol);
    }
    if (board_name == "TDC"){
	readpointer_tdc = ((unsigned int*)vme.getPciBaseAddr(vme_image)) + cardBase/4;
        printf("readpointer 0x%08X \n", readpointer_tdc);
    }
    if (board_name == "QDC"){
	readpointer_qdc = ((unsigned int*)vme.getPciBaseAddr(vme_image)) + cardBase/4;
        printf("readpointer 0x%08X \n", readpointer_qdc);
    }
    printf("data 0x%08X \n", data);

    short unsigned int regv;
    vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x2); //ADC offline
    vme.ww(vme_image, cardBase+V874_BIT_SET_1, 0x0080);
    vme.ww(vme_image, cardBase+V874_BIT_CLR_1, 0x0080); //Module clear
    vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x0004); 
    vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x0004); //Clear data
    vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x1800); //Auto increment + Empty Prog
    
    vme.ww(vme_image, cardBase+V874_CRATE_REG, board_name =="QDC"? 0x55 : 1); 
    vme.ww(vme_image, cardBase+V874_FAST_CLR_WIN, 0x010);
    vme.ww(vme_image, cardBase+V874_CLRTIME, 0x40); 
    vme.ww(vme_image, cardBase+V874_CTRL_REG_1, (short unsigned int)0x0);// WTF?! simple 0 doesn't work
    vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x4418); //Peter 0x4000 disable all trg
    if (board_name =="IoL")
	vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x400); 
    if (board_name =="TDC"){
	vme.ww(vme_image, cardBase+V874_VSET, 53); // time range //53
	vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x1000); //0x1400
    }
    if (board_name =="QDC"){
	vme.ww(vme_image, cardBase+V874_BIT_SET_2, 418); // if (bset2 == 0) parameter = 418
	vme.ww(vme_image, cardBase+V792_EVENT_TRIGGER_REGISTER,(short unsigned int)0x0); 
	vme.ww(vme_image, cardBase+V792_IPED, 65);
    }

    vme.rw(vme_image, cardBase+V874_BIT_SET_2, &regv); 
    printf("Bitset2: 0x%x \n", regv);
    
    unsigned int thr;
    for (int i = 0; i<32; i++){
	if (board_name == "IoL")
	    thr = i<28 ? 0x1FF : 0;
	if (board_name == "TDC" || board_name == "QDC")
	    thr = 0x2;
	if (board_name != "QDC")
	    vme.ww(vme_image, cardBase+V874_THRESH + 2*i, thr);
	else{
	    if ( i<16 )
		vme.ww(vme_image, cardBase+V874_THRESH + 2*i, thr);
	}
    }
 


    vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x2); //ADC online
    vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x0004); 
    vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x0004); //Clear data
    
    unsigned short int_geo, act_geo;
    
    if (board_name == "IoL")
	act_geo = 4;
    if (board_name == "TDC")
	act_geo = 8;
    if (board_name == "QDC")
	act_geo = 10;
    
    vme.rw(vme_image, cardBase+V874_GEO_ADDR, &int_geo);
    int_geo &= 0x1F;
    printf("Internal %s geo 0x%04x \n",board_name,int_geo);

    if (int_geo!=act_geo){
	if ((vme.ww(vme_image, cardBase+V874_GEO_ADDR, act_geo))<0){
	    printf("Can't set actual %s geo[%i] != internal geo[%i]\nExit\n",board_name, act_geo, int_geo);
	    exit(0);
	}    
	else{
	    vme.rw(vme_image, cardBase+V874_GEO_ADDR, &int_geo);
	    int_geo &= 0x1F;
	    printf("Now %s geo [%i] \n", board_name, int_geo);
	}    
    }	
}

unsigned int red_calc(unsigned int org){
    //shift down address bits 40:27 by 4 if it reduced address space
    return ( ((org&0xFFF00000)>>4)+(org&0x0FFFFF) );
}

void calc_red_pointers( ){
    printf("Calculating Reduced pointers \n");
    memset(&RED, 0, sizeof(RED));

    RED.ADC_Mem[0] = red_calc(SIS3302_ADC_Mem_1);
    RED.ADC_Mem[1] = red_calc(SIS3302_ADC_Mem_2);
    RED.ADC_Mem[2] = red_calc(SIS3302_ADC_Mem_3);
    RED.ADC_Mem[3] = red_calc(SIS3302_ADC_Mem_4);
    RED.ADC_Mem[4] = red_calc(SIS3302_ADC_Mem_5);
    RED.ADC_Mem[5] = red_calc(SIS3302_ADC_Mem_6);
    RED.ADC_Mem[6] = red_calc(SIS3302_ADC_Mem_7);
    RED.ADC_Mem[7] = red_calc(SIS3302_ADC_Mem_8);

    RED.Event_Config_ALL_ADC = red_calc(SIS3302_Event_Config_ALL_ADC);
    
    RED.Sample_Address_ADC[0] = red_calc(SIS3302_Sample_Address_ADC1);
    RED.Sample_Address_ADC[1] = red_calc(SIS3302_Sample_Address_ADC2);
    RED.Sample_Address_ADC[2] = red_calc(SIS3302_Sample_Address_ADC3);
    RED.Sample_Address_ADC[3] = red_calc(SIS3302_Sample_Address_ADC4);
    RED.Sample_Address_ADC[4] = red_calc(SIS3302_Sample_Address_ADC5);
    RED.Sample_Address_ADC[5] = red_calc(SIS3302_Sample_Address_ADC6);
    RED.Sample_Address_ADC[6] = red_calc(SIS3302_Sample_Address_ADC7);
    RED.Sample_Address_ADC[7] = red_calc(SIS3302_Sample_Address_ADC8);

    RED.Trigger_Setup_ADC[0] = red_calc(SIS3302_Trigger_Setup_ADC1);
    RED.Trigger_Setup_ADC[1] = red_calc(SIS3302_Trigger_Setup_ADC2);
    RED.Trigger_Setup_ADC[2] = red_calc(SIS3302_Trigger_Setup_ADC3);
    RED.Trigger_Setup_ADC[3] = red_calc(SIS3302_Trigger_Setup_ADC4);
    RED.Trigger_Setup_ADC[4] = red_calc(SIS3302_Trigger_Setup_ADC5);
    RED.Trigger_Setup_ADC[5] = red_calc(SIS3302_Trigger_Setup_ADC6);
    RED.Trigger_Setup_ADC[6] = red_calc(SIS3302_Trigger_Setup_ADC7);
    RED.Trigger_Setup_ADC[7] = red_calc(SIS3302_Trigger_Setup_ADC8);

    RED.Trigger_Threshold_ADC[0] = red_calc(SIS3302_Trigger_Threshold_ADC1);
    RED.Trigger_Threshold_ADC[1] = red_calc(SIS3302_Trigger_Threshold_ADC2);
    RED.Trigger_Threshold_ADC[2] = red_calc(SIS3302_Trigger_Threshold_ADC3);
    RED.Trigger_Threshold_ADC[3] = red_calc(SIS3302_Trigger_Threshold_ADC4);
    RED.Trigger_Threshold_ADC[4] = red_calc(SIS3302_Trigger_Threshold_ADC5);
    RED.Trigger_Threshold_ADC[5] = red_calc(SIS3302_Trigger_Threshold_ADC6);
    RED.Trigger_Threshold_ADC[6] = red_calc(SIS3302_Trigger_Threshold_ADC7);
    RED.Trigger_Threshold_ADC[7] = red_calc(SIS3302_Trigger_Threshold_ADC8);

    RED.Trigger_Flag_Clear[0] = red_calc(SIS3302_TriggerFlagClear_ADC12);
    RED.Trigger_Flag_Clear[1] = red_calc(SIS3302_TriggerFlagClear_ADC34);
    RED.Trigger_Flag_Clear[2] = red_calc(SIS3302_TriggerFlagClear_ADC56);
    RED.Trigger_Flag_Clear[3] = red_calc(SIS3302_TriggerFlagClear_ADC78);

    RED.Event_Directory_ADC[0] = red_calc(SIS3302_EventDirectory_ADC1);
    RED.Event_Directory_ADC[1] = red_calc(SIS3302_EventDirectory_ADC2);
    RED.Event_Directory_ADC[2] = red_calc(SIS3302_EventDirectory_ADC3);
    RED.Event_Directory_ADC[3] = red_calc(SIS3302_EventDirectory_ADC4);
    RED.Event_Directory_ADC[4] = red_calc(SIS3302_EventDirectory_ADC5);
    RED.Event_Directory_ADC[5] = red_calc(SIS3302_EventDirectory_ADC6);
    RED.Event_Directory_ADC[6] = red_calc(SIS3302_EventDirectory_ADC7);
    RED.Event_Directory_ADC[7] = red_calc(SIS3302_EventDirectory_ADC8);

}

void SetDAC(int &vme_image,unsigned int cardBase, int ch,unsigned long int val){
    unsigned int regval;
    unsigned long int val_ch = ch<<4, max_timeout = 5000, timeout_cnt = 0;
    
    vme.wl(vme_image, cardBase+SIS3302_DAC_Data, val);
    vme.wl(vme_image, cardBase+SIS3302_DAC_CntrStat, val_ch+1);

    do{
	vme.rl(vme_image, cardBase+SIS3302_DAC_CntrStat, &regval);
	timeout_cnt++;
    }while(((regval & 0x80000)==0x8000)&&(timeout_cnt < max_timeout));
    
    vme.wl(vme_image, cardBase+SIS3302_DAC_CntrStat, val_ch+2);
    timeout_cnt = 0;
    
    do{
	vme.rl(vme_image, cardBase+SIS3302_DAC_CntrStat, &regval);
	timeout_cnt++;
    }while(((regval & 0x80000)==0x8000)&&(timeout_cnt < max_timeout));

    //if(timeout_cnt < max_timeout)
//printf("Success! Set DAC[%i] to 0x%04x \n", ch, val);

    if(timeout_cnt >= max_timeout)	
	printf("Loading DAC Failed [%i]\n", ch);

}

void init_sis(int &vme_image, unsigned int cardBase){ //, unsigned int *readpointer){
    unsigned int regval;
    unsigned int red_val = 1;
    unsigned int data;

    for (int i =0; i<8; i++)
	sadc_arr[i] = 0;

    printf("Init SIS 0x%08X\n", cardBase);
    
    //Check reduced addresses
    vme.rl(vme_image, cardBase+SIS3302_ContrStat, &regval);
    if ( ((regval>>30)&0x1)!=red_val ){
	printf("Reduced address space jumper is not set correctly RED should be 0x%X\n",(regval>>30)&0x1);
	exit(0);
    }
    
    readpointer_sis = (unsigned int*)vme.getPciBaseAddr(vme_image);
    //printf("readpointer_sis 0x%08X \n", readpointer_sis);
           
    vme.rl(vme_image, cardBase+SIS3302_ModuleID_Firm, &regval);
    //printf("ModuleID: 0x%x \n", regval);

    vme.wl(vme_image, cardBase+SIS3302_GeneralReset, 1); //General reset
    vme.wl(vme_image, cardBase+SIS3302_DAC_CntrStat, 0x3); //DACs reset
    vme.wl(vme_image, cardBase+SIS3302_ContrStat, 1); //switch on user LED
    vme.wl(vme_image, cardBase+SIS3302_AcqCntrStat, 0x0100); // 0x0100 100 MHz, enable Start/Stop logic
    
    calc_red_pointers();
    //printf("Reduced pointers calculated RED_ADC_Mem[2]: x%08x\n", RED.ADC_Mem[2]);

    for(int i=0; i<4; i++) //4 pairs of channels = 8 channels of sis3302
	vme.wl(vme_image, cardBase+RED.Trigger_Flag_Clear[i], 2000);

    //considering RED switched on
    //unsigned EventConfig = 0x13; //wrap around page until stop, size = 3->256 samples
    //unsigned WrapPageSize_Bytes = 16777216*2; //16M Samples max * 2Bytes/Sample
    //unsigned int page_size = EventConfig & 0xF;

    EventConfig = 0x13; //wrap around page until stop, size = 3->256 samples
    WrapPageSize_Bytes = 16777216*2; //16M Samples max * 2Bytes/Sample
    page_size = EventConfig & 0xF;

    for (int i=0; i<page_size;i++) WrapPageSize_Bytes /= 4;

    vme.wl(vme_image, cardBase+RED.Event_Config_ALL_ADC, EventConfig); // EventConfig

    for (int i=0; i<8; i++){
	vme.wl(vme_image, cardBase+RED.Trigger_Threshold_ADC[i],0x06004E20);//Set thresholds
	vme.wl(vme_image, cardBase+RED.Trigger_Setup_ADC[i],0x000F0000);//Set trigger setup | what is it?
	SetDAC(vme_image, cardBase, i, 0x7FFF); //Set DAC
    }

    vme.wl(vme_image, cardBase+SIS3302_ExtStopDelay,300);//Set stopdelay
    vme.wl(vme_image, cardBase+SIS3302_ArmSampling,1);//page 28 Arm sampling | comment from Peter code
    vme.wl(vme_image, cardBase+SIS3302_Sampling,1);//page 28 Start sampling | comment from Peter code
}


int read_event(int &vme_image, unsigned int cardBase, char* board_name, time_t start_time, unsigned int evt){
    unsigned short readybit;
    int number_of_data;
    unsigned int data;
    unsigned int *rawdata, *decoded_event;
    int ch, cc;
    unsigned int_noe;
    
    //int MAX_CH = board_name == "QDC" ? 16 : 32;
    int MAX_CH = 32;
    // unsigned int rawdata[MAX_CH + 2], decoded_event[MAX_CH];
    static char t_str[4], q_str[4];
    int range[MAX_CH];
   

    rawdata = new unsigned int[32 + 2];
    decoded_event = new unsigned int[MAX_CH];
   

    for (int n=0;n<32+2; n++ )
	rawdata[n] = 0;
    
    for (int n=0;n<MAX_CH; n++ ){
	decoded_event[n] = 0;
	range[n] = 0; // low or high range, used only for QDC
    }	
            	
        //read_one_evt

	do{
	    vme.rw(vme_image, cardBase+V874_STAT_REG_1, &readybit);
	    if((readybit&0x1)!=1){
		if (start_time==0) start_time = time(NULL);
		if ( (time(NULL)-start_time) > 2 )
		    printf("IoL Board isn\t ready for %i seconds \n", (int)(time(NULL)-start_time));
	    }
	}while((readybit&0x1)!=1);
    
	if (board_name == "IoL"){
	    data=*readpointer_iol;
	    //data =*(unsigned int*)0x41258000;
	    //printf("readpointer_iol 0x%08x \n", readpointer_iol);
	}
	if (board_name == "TDC"){
	    data=*readpointer_tdc;
	    //printf("readpointer_tdc 0x%08x \n", readpointer_tdc);
	}
	if (board_name == "QDC"){
	    data=*readpointer_qdc;
	    //printf("readpointer_tdc 0x%08x \n", readpointer_tdc);
	}
	
	if (((data>>24)&0x7)==6){
	    printf("%s invalid datum Evt %i 0x%x\n",board_name, evt, data);
	    return(0);
	}

	if (((data>>24)&0x7)!=2){
	    printf("%s wrong header Evt %i 0x%x\n",board_name, evt, data);
	    exit(0);
	}
	
	rawdata[0] = data; // save header in data stream
	number_of_data = (data>>8)&0x3F; // decode max_no_of_ch in header

	if (fPrint)
	    printf("Reading %s: number of data %i data 0x%08x \n",board_name, number_of_data, data);
	
	if (number_of_data > MAX_CH)
	    printf("\nNumber of channels to big [%i] > MAX_CH (%i)\n",number_of_data, MAX_CH);
	
	//data readout
	for (cc=1; cc<=number_of_data; cc++){
	    if (board_name == "IoL")
		data=*readpointer_iol;
	    if (board_name == "TDC")
		data=*readpointer_tdc;
	    if (board_name == "QDC")
		data=*readpointer_qdc;

	    rawdata[cc]=data;
	    
	    if (((data>>24)&0x7)==6){
		printf("%s invalid datum Evt %i 0x%x\n",board_name, evt, data);
		return(0);
	    }
	    if (board_name != "QDC")//if (MAX_CH > 16)
		ch = (data>>16)&0x1F; //decode channel | for BIG module
	    else
		ch = (data>>17)&0xF; // decode channel | for small channel
	    
	    if (board_name == "QDC")
		range[ch] = (data>>16)&0x1; // range bit for QDC board

	    data = data&0x1FFF;
	    
	    board_name != "QDC"? decoded_event[ch]=data : decoded_event[ch + 16*range[ch]] = data;

	    //if (board_name == "TDC")
	    //	tdc_arr[ch] = data;
	    //if (board_name == "QDC")
	    //	qdc_arr[ch] = data;

	    //if (((data>>12)&0x3)==0)  // no overflow/underthreshold bit[b13:b12]
	    //	printf("his values ch %i data %08x \n", ch, data);
	    //else 
	    if (((data>>12)&0x3)!=0)
	    	printf("\n Overflow or Underthreshold on ch[%i] bit[%i]\n", ch, ((data>>12)&0x3));
	}
	
	//data readout

	rawdata[cc] = data; // save trail in data stream
	
	if (board_name == "IoL")
	    data=*readpointer_iol;
	if (board_name == "TDC")
	    data=*readpointer_tdc;
	if (board_name == "QDC")
	    data=*readpointer_qdc;
		
	int_noe = (data&0xFFFFFF); // noe is in trail

	if(((data>>24)&0x7)!=4) { // is it trail?
	    if(board_name == "IoL"){
		printf("IoL bug! recovering... \n");
		data = evt+(rawdata[0]&0xF8000000)+0x040000000; // calculate trail
		rawdata[cc] = data;
		int_noe = evt;
	    }
	    else{
		printf("\n Wrong trail %s at Evt %i : 0x%08x \n",board_name, evt, data);
		return(0);
	    }
	    
	}

	
	// create unified char array for future

	if (board_name == "TDC"){
	    bzero(tdc_full, sizeof(tdc_full));
	    bzero(t_str,sizeof(t_str));
	    for (int n=0; n<MAX_CH; n++){
		sprintf(t_str,"%03x",decoded_event[n]);
		strcat(tdc_full, t_str);
		if (fPrint){
		    if(n%5==0 && n!=0) 
			printf("\n");
		    if(decoded_event[n]!=0) 
			printf(" %2i: %4i,",n,decoded_event[n]);
		    else
			printf("     ,");
		}    
	    }
	}
	
	if (board_name == "QDC"){
	    bzero(qdc_full, sizeof(qdc_full));
	    bzero(q_str,sizeof(q_str));
	    for (int n=0; n<MAX_CH; n++){
		sprintf(q_str,"%03x",decoded_event[n]);
		strcat(qdc_full, q_str);
		if (fPrint){
		    if(n%5==0 && n!=0) 
			printf("\n");
		    printf(" %2i: %4i [%i],",n,decoded_event[n],range[n]);
		}
		//if(decoded_event[n]!=0) 
		//    printf(" %2i: %4i [%i],",n,decoded_event[n],range[n]);
		//else
		//    printf("     ,");
	    }
	}

	// print event info
	
	if(int_noe!=evt)
	    printf("Internal NoE(%i) != Evt(%i) \n", int_noe, evt); 
	
	vme.rw(vme_image, cardBase+V874_STAT_REG_1, &readybit);
	if(readybit&0x5!=0)
	    printf("Something is rotten Reg 1: 0x%08x \n", readybit);

	return number_of_data;
}

void wait_stop(int &vme_image, unsigned int cardBase){
    int counter = 0, cv = 1000;
    unsigned int retval;
    do{
	vme.rl(vme_image, cardBase + SIS3302_AcqCntrStat, &retval);
	counter++;
	if(counter>cv){
	    vme.wl(vme_image, cardBase + SIS3302_StopSampling, 1); // stop sampling
	    printf("WaitStop! \n");
	}    
    }while((retval&0x30000));// [b17:b16]
}

int get_data(int ch, int &counter, unsigned int cardBase){
    unsigned int sto_addr, start_addr, sample_addr, sto_ptr_offset, data;
    int offset, correction, additional, sample_counter;
    int value, ped = 0, max = 0, min = 65500, dma_offset;
    unsigned int *dma_ptr;

    const int ped_start = 10, ped_end = 0, n_sadc = 500;
    unsigned int rawdata[n_sadc/2*8+8+2]; // is it correct to write such way?
    unsigned int decoded_event[8]; 
    unsigned short sh_array[n_sadc+4];
    static char substr[8]; // 2 samples by 4 byte for every

    start_addr = RED.ADC_Mem[ch];
    sample_addr = RED.Sample_Address_ADC[ch];

    rawdata[counter++] = (ch<<28)+n_sadc;
    //calculate position in memory
    sto_ptr_offset = *(readpointer_sis + cardBase/4 + sample_addr/4);

    if(sto_ptr_offset/4 > WrapPageSize_Bytes) printf("Wrong addr offset ..\n");

    switch(sto_ptr_offset&0x3){
	case 3:
	    sto_ptr_offset = (sto_ptr_offset & 0x01FFFFFC)<<1;
	    correction = -1;
	    additional = 1;
	    break;
   	case 0:
	    sto_ptr_offset = (sto_ptr_offset & 0x01FFFFFC)<<1;
	    correction = 0;
	    additional = 0;
	    break;
    	case 1:
	    sto_ptr_offset = (sto_ptr_offset & 0x01FFFFFC)<<1;
	    correction = +1;
	    additional = 1;
	    break;
	case 2:
	    sto_ptr_offset = (sto_ptr_offset & 0x01FFFFFC)<<1;
	    correction = +2;
	    additional = 1;
	    break;
    }
    
    // 1st 2 samples
    if (sto_ptr_offset <2*n_sadc){
	//segmented readout
	printf("Segmented!! \n");
	sto_addr = start_addr + WrapPageSize_Bytes - 2*n_sadc + sto_ptr_offset;
	data = *(readpointer_sis + cardBase/4 + sto_addr/4);
	sample_counter = 0;
	
	if (correction == -1) sh_array[sample_counter++] = (data>>16); // only 2nd short is used
	if (correction == 0){
	    sh_array[sample_counter++] = (data&0xFFFF); // 1st short
	    sh_array[sample_counter++] = (data>>16); // 2nd short
	}
	if (correction == 1) sh_array[sample_counter++] = (data>>16); // only 2nd short is used
	//if(correction ==2); // no short is used with an offset of 2
	
	int prewrap_addr = WrapPageSize_Bytes + sto_ptr_offset - 2*n_sadc;
	
	for (int i=1, c=0; i<n_sadc/2+additional; i++){
	    if ((4*i + prewrap_addr)<WrapPageSize_Bytes)
		sto_addr = start_addr + prewrap_addr + 4*i; // readout last samples before wrapping
	    else{
		sto_addr = start_addr + 4*c; // wrap over, start at the beginning of buffer
		c++;
	    }
	    data = *(readpointer_sis + cardBase/4 + sto_addr/4);
	    sh_array[sample_counter++] = (data&0xFFFF); // 1st short
	    sh_array[sample_counter++] = (data>>16); // 1st short
	}
	
    }
    else {
// not segmented readout
//1st 2 samples
	sto_addr = start_addr + sto_ptr_offset - 2*n_sadc;
	data = *(readpointer_sis + cardBase/4 + sto_addr/4);
	sample_counter = 0;
	
	if (correction == -1) sh_array[sample_counter++] = (data>>16); // only 2nd short is used
	if (correction == 0){
	    sh_array[sample_counter++] = (data&0xFFFF); // 1st short
	    sh_array[sample_counter++] = (data>>16); // 2nd short
	}
	if (correction == 1) sh_array[sample_counter++] = (data>>16); // only 2nd short is used
	
	if(!fDMA){
	    for (int i=1; i<n_sadc/2+additional; i++){
		sto_addr = start_addr + sto_ptr_offset -2*n_sadc + 4*i; // Bufferaddr + pointer last DW - go to the beginning + 4 byte/2events
		data = *(readpointer_sis + cardBase/4 + sto_addr/4);
		sh_array[sample_counter++] = (data&0xFFFF); // 1st short
		sh_array[sample_counter++] = (data>>16); // 1st short
	    }
	}
	else{
	    dma_offset = vme.DMAread(cardBase + sto_addr, 2*n_sadc + 4*additional, A32, D32);
	    if (dma_offset <0){
		printf("DMA error!!! \n");
		vme.releaseDMA();
		exit(0);
	    }
	    dma_ptr = (unsigned int*)(dma_base + dma_offset);
	    for(int i=1; i<n_sadc/2+additional; i++){
		data = *dma_ptr++;
		sh_array[sample_counter++] = (data&0xFFFF); // 1st short
		sh_array[sample_counter++] = (data>>16); // 2nd short
	    }
	}
    }
    
    // fill rawdata and decode trace
    if (ch==3){
	//bzero(trace_hex, sizeof(trace_hex));
	bzero(trace_arr[ch], sizeof(trace_arr[ch]));
    //bzero(substr, sizeof(substr));
    }
    if(ch==0)
	bzero(trace_long, sizeof(trace_long));
   
    bzero(substr, sizeof(substr));

    for (int n=0; n<n_sadc; n+=2){ // rewrite this part as soon as you don't need such rawdata type
	rawdata[counter++] = (sh_array[n+1]<<16) + sh_array[n]; 
	//if (ch==3){
	//    sprintf(substr,"%04x%04x",sh_array[n],sh_array[n+1]);
	//    //strcat(trace_hex, substr);
	//    strcat(trace_arr[ch], substr);
	//}
	sprintf(substr,"%04x%04x",sh_array[n],sh_array[n+1]);
	strcat(trace_long, substr);

	if(sh_array[n]>max) max = sh_array[n];
	if(sh_array[n+1]>max) max = sh_array[n+1];
	if(sh_array[n]<min) min = sh_array[n];
	if(sh_array[n+1]<min) min = sh_array[n+1];
	
	if(n<ped_start | n>=(n_sadc-ped_end)) ped += sh_array[n] + sh_array[n+1]; //BL calculation
    }
    ped /= (ped_start + ped_end);
    
    value = (max-ped)>(ped-min) ? (max-ped) : (ped-min);
   
    sadc_arr[ch] = value;
    //printf(" ch[%i] ped(%i) value(%i) len(%lu)", ch, ped ,value, strlen(trace_long));
    //printf(" \n ch[%i] W{%s}\n", ch, trace_long);

    return counter;
  
}

int read_event_sis(int &vme_image, unsigned int cardBase, unsigned int evt){
    unsigned int n;
    int value;
    int number_of_data = 0;

    wait_stop(vme_image, cardBase);

    vme.rl(vme_image, cardBase+SIS3302_ActualEvCounter, &n);
    
    printf("Internal SIS event %i \n", n);
    

    for(int ch = 0; ch<8; ch++) n = get_data(ch, number_of_data, cardBase);
    //for(int ch = 0; ch<8; ch++) value = get_data(ch, number_of_data, cardBase);
    

    vme.wl(vme_image, cardBase+SIS3302_ArmSampling,1); // page 28 Arm sampling
    vme.wl(vme_image, cardBase+SIS3302_Sampling,1); // page 28 start sampling
    
    return number_of_data*4;
    //return value;
}

int main(int argc, char *argv[]){
   
    ///tcp preparation

	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len); 
	if (connfd < 0) { 
		printf("server accept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server accept the client...\n"); 
			
	//int MAX = 80;

char buff[MAX], ack[80], task[10];
bzero(buff, sizeof(buff));
read(connfd, buff, sizeof(buff));
printf("F: %s [%lu]\n", buff,sizeof(buff));
	
    //tcp preparation
 
  atexit (fnExit);

  time_t t_start, t_now, start_time;
  int s_elapsed = 0;
  
  unsigned int cardBase_iol = 0x01000000;
  unsigned int cardBase_tdc = 0x03000000;
  unsigned int cardBase_sis = 0x05000000;
  unsigned int cardBase_qdc = 0x0A000000;

  // static VMEBridge vme;
    int vme_image = 0,vme_image_iol = 0, vme_image_tdc = 0, vme_image_sis = 0;
        
    init_board(vme_image, cardBase_iol, "IoL");
    init_board(vme_image, cardBase_tdc, "TDC");
    init_board(vme_image, cardBase_qdc, "QDC");
    // init_sis(vme_image, cardBase_sis);
    
    reset_board(vme_image, cardBase_iol, "IoL");
    reset_board(vme_image, cardBase_tdc, "TDC");
    reset_board(vme_image, cardBase_qdc, "QDC");
    //reset sis?
  
    bzero(buff, sizeof(buff));
    sprintf(buff,"readpointer_qdc after reset 0x%08x \n", readpointer_qdc);
    write(connfd, buff, sizeof(buff));
    
    //sleep(1);
    //bzero(ack, sizeof(ack));
    //read(connfd, ack, sizeof(ack));
    //printf("S: %s \n", ack);
    //bzero(buff, sizeof(buff));
    read(connfd, buff, sizeof(buff));
    printf("S: %s [%lu]\n", buff, sizeof(buff));

    bool fStart = true;

  if(strncmp("start",buff,5) == 0 || fStart){
      printf("GO!\n");
      t_start = time(NULL);
      signal(SIGINT, handler); // save exit via <CTRL-C>
      

      if (setjmp(terminateJmp) == 0) {

      // READOUT LOOP!!!!
	  unsigned int evt = 0;
	  int NoD = 0; // number of data
	  start_time = 0;
	
	  
	  /*
	  for (int i =0; i<2000; i++){
	      bzero(buff, MAX);
	      sprintf(buff,"From server with love [%i]", i);
	      printf("Sending MSG[%i]: (%s)\n",i, buff);
	      write(connfd, buff, sizeof(buff));
	      
	      //sleep(1);

	      bzero(buff, MAX);
	      read(connfd, buff, sizeof(buff));
	      printf("Got from client[%i] '%s' \n",i, buff);
	      
	      //sleep(1);
	  }
	  */

	  while(1){
	      lam_iol(vme_image, cardBase_iol);
	      if (evt==0)
		  sleep(1);//help a lot with synchronisation
	      s_elapsed  = (int)(time(NULL) - t_start);

	      if (fPrint)
		  printf("\nEvent %i : time %i Evt/sec: %02f \n", evt,s_elapsed,s_elapsed>0 ? (double)(evt/s_elapsed):0 );

	      	
	      //each board require such function, 2 QDC -> 2 read_event with different cardBase_qdc1/2
	      NoD = read_event(vme_image, cardBase_iol, "IoL", start_time, evt);
	      NoD = read_event(vme_image, cardBase_tdc, "TDC", start_time, evt);
	      NoD = read_event(vme_image, cardBase_qdc, "QDC", start_time, evt);

	      //NoD = read_event_sis(vme_image, cardBase_sis, evt);
	      
	      
	      //printf("\n Event:%i: TDC(%s) QDC[%s]\n",evt, tdc_full, qdc_full);
	      

	      bzero(buff, sizeof(buff));
	      sprintf(buff,"Event:%i;TDC(%s)QDC[%s]WF{}", evt, tdc_full, qdc_full);
	      // sprintf(buff,"Event:%i;TDC(%s)QDC[%i]WF{%s}", evt, tdc_full, qdc_full, trace_long);
	     	      
	      
	      printf("\rEvent:%i",evt);
	      fflush(stdout);
	      
	      write(connfd, buff, sizeof(buff));

	      evt++;
	     	     
	      //confirmation from client
	      //  bzero(task, sizeof(task));
	      //  read(connfd, task, sizeof(task));
	      //  printf("\n From client [%s] (%i)",task, strlen(task));

	      //if(strncmp(buff,"ACK",3)!=0){
	      //  printf("Doesn't confirmed by client \n");
	      //  exit(0);
	      //} 
	      
	  } // infinite while(1)
	  
      }
      // END READOUT LOOP!!!!
  } // if started by client
  else
      printf("Smth else %s \n",ack);


  printf("\nFinished!\n ");   

}   

void handler(int sig)
{
  longjmp(terminateJmp, sig);
}


void fnExit(void){

}


