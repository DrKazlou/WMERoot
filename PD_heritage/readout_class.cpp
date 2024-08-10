#include "readout_class.h"
#include "param.h" 
#include "v874.h"
#include <errno.h>
//#include "aio_module_def.h"
#include <sys/time.h>

/*
int System_class::initialized=0; 
int System_class::file_number=0; 

System_class::System_class(){
};

System_class::~System_class(){
  if(initialized!=0){
    printf("\n%i events read out\n", noe);

    timestring(stoptime);
    if(all_ok==0 && inh_rotten==0) 
      printf("\a\nSomething is rotten...\a\n"); 

    printf("Start of readout: %s\n", gen_starttime);
    printf("End of readout:   %s\n", stoptime);

    save_histo();
    deinit_save_event();
    deinit_save_histo();
    for(int m=no_of_boards-1; m>=0; m--){
//      printf("Deleting module: %i\n", m);
      delete readout_modules[m];
    }
  }
};

void System_class::init(char * filename, char *tcl_file, 
                        unsigned long ext_max_saved_bytes, 
                        int ext_Max_Anzahl, int ext_Max_Time){
  initialized=1; 

  do_save_histo=1;
  wie_oft_histo=20000;
  do_show_data=1;
  wie_oft=5000;
  Max_Anzahl=Max_Time=0;
  do_Max_Anzahl=do_Max_Time=0;
  noe=0;
  no_of_boards=-1;
  param =&paramS;

  saved_bytes=0;
  max_save_bytes=ext_max_saved_bytes;
  if(max_save_bytes>2093796557) //max filesize> 1.95GB
    max_save_bytes=2093796557;

  if(strlen(filename)<2)
    sprintf(filename, "%s", AIO_default_filename);
  sprintf(rawname, "%s", filename);
  sprintf(hisname, "%s.his", filename);
  if(ext_Max_Anzahl>0){
    Max_Anzahl=ext_Max_Anzahl;
	do_Max_Anzahl=1;
  }
  if(ext_Max_Time>0){
    Max_Time=ext_Max_Time;
	do_Max_Time=1;
  }
  
  printf("\nParameter-File: %s, Max no of counts: %i\n", tcl_file, Max_Anzahl);
  if (0 > conParam(param, tcl_file)) {  // init parameter file readout
    printf("%s:%d:%s\n", __FILE__, __LINE__, strerror(errno));
    exit(0);
  }

//  general parameter initialisation
  no_of_boards = Param_getVal(param, "setup", "no_of_boards");
  printf("\n%i boards to initialize\n", no_of_boards);
  if(no_of_boards==0){
    printf("\nNeed at least 1 board\n");
    exit(0);  
  }
  if(no_of_boards>AIO_max_no_of_boards){
    printf("\nToo many boards\n");
    exit(0);
  }

  LAM = Param_getVal(param, "setup", "LAM");  // which board is the LAM board
  if(LAM>no_of_boards){
    printf("\nLAM out of range\n");
    exit(0);
  }
  
  wie_oft = Param_getVal(param, "setup", "no_show_data"); // show data how often?
  wie_oft_histo = Param_getVal(param, "setup", "no_save_histo"); // save histogram how often?
  inh_rotten = Param_getVal(param, "setup", "inh_rotten", 0);  // ignore rotten information
  printf("\n");
//  END general parameter initialisation

//  initialize boards
  int geo;
  char hlfstr[50];

  for(int n=0, m=0; n<no_of_boards; n++){
    sprintf(hlfstr, "board%i_what", n+1);
    m  = Param_getVal(param, "setup", hlfstr);
    what[n]=m;
    switch(m){
	/*		
      case NTEC_MULTI: // NTEC board on V874 base module
	  readout_modules[n] = new Readout_class_ntec;
          readout_modules[n]->init_Readout_class(n, 1, param,
                                                 inh_rotten, geo);
          break;
      case VETO_MULTI: // VETO board on V874 base module
	  readout_modules[n] = new Readout_class_veto;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
      case V775_TDC: // V775 32 TDC based on V874
	  readout_modules[n] = new Readout_class_v775;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
      case V775N_TDC: // V775 16 TDC based on V874
	  readout_modules[n] = new Readout_class_v775;
          readout_modules[n]->init_Readout_class(n, 0, param, 
                                                 inh_rotten, geo);
          break;
      case V785_PSADC: // V785 32 Peak sensing ADC based on V874
	  readout_modules[n] = new Readout_class_v785;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
      case V785N_PSADC: // V785 16 Peak sensing ADC based on V874
	  readout_modules[n] = new Readout_class_v785;
          readout_modules[n]->init_Readout_class(n, 0, param, 
                                                 inh_rotten, geo);
          break;
      case V792N_QDC: // V792 16 QDC
	  readout_modules[n] = new Readout_class_v792;
          readout_modules[n]->init_Readout_class(n, 0, param, 
                                                 inh_rotten, geo);
          break;
      case V792_QDC: // V792 32 QDC
	  readout_modules[n] = new Readout_class_v792;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
      case V965N_DRQDC: // V965 16 DR-QDC
	  readout_modules[n] = new Readout_class_v965;
          readout_modules[n]->init_Readout_class(n, 0, param, 
                                                 inh_rotten, geo);
          break;
      case V965_DRQDC: // V965 32 DR-QDC
	  readout_modules[n] = new Readout_class_v965;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
	      break;
		  
      case IOL: // iol board on V874 base module
	  readout_modules[n] = new Readout_class_iol;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
			
      case SIS3820_SYNC: // SIS Sync module
	  readout_modules[n] = new Readout_class_sis3820;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
			
			
      case BONN_SYNC: // BONN Sync module
	  readout_modules[n] = new Readout_class_bonnsync;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
	
			
      case V895_LED: // V895 16 LED
	  readout_modules[n] = new Readout_class_v895;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
	case SIS3302_FADC: //
	  readout_modules[n] = new Readout_class_sis3302;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
			
	case BONN_TRACK: //
	  readout_modules[n] = new Readout_class_bonntrack;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
	
			
	case AVM16_FADC: //
	  readout_modules[n] = new Readout_class_avm16;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
	case SIS3316_FADC: //
	  readout_modules[n] = new Readout_class_sis3316;
          readout_modules[n]->init_Readout_class(n, 1, param, 
                                                 inh_rotten, geo);
          break;
			

    }    
    printf("Initialized a %s as board no %i in geo %i\n\n", 
	                readout_modules[n]->get_name(), n+1, geo);
    geos[n]=geo;    
  }
  printf("\n\n");
  desParam(param);
  
  init_save_event();
  readout_modules[0]->set_write_pointer(rawout);

  init_save_histo();

//  END initialize boards
  timestring(gen_starttime); 
  strcat(starttime, gen_starttime); 
  strcat(stoptime, gen_starttime); 
  printf("Start of readout: %s\n", gen_starttime);

  if(do_Max_Anzahl==1) printf("I will read out %i events.\n", Max_Anzahl);
  if(do_Max_Time==1) printf("I will read out for %i seconds.\n", Max_Time);
  if(do_save_histo==1) printf("I will save the histo every %ith event.\n", wie_oft_histo);
  if(do_show_data==1)  printf("I will show the data every %ith event.\n", wie_oft);
  printf("\n");
  printf("hello\n");
  printf("LAM is a %s board (%i)\n", readout_modules[LAM-1]->get_name(), LAM);
  printf("hello2\n");
  write_header_save_event();
}; // END init::System_class(char * filename, char *tcl_file, int Max_Anzahl, int Max_Time);


void System_class::reset(void){
  for(int m=0; m<no_of_boards; m++){
    readout_modules[m]->reset();
  } 
}

void System_class::do_readout(void){
  all_ok=1;
  int terminate_loop=0;

  anfangszeit = time(NULL);
  endzeit =  anfangszeit + Max_Time;

  do{  // while(terminal_loop==0);
    readout_modules[LAM-1]->lam();
    if(noe%wie_oft==0 && do_show_data==1){
      if(all_ok==1 || inh_rotten==1)  printf("\n*** Event %i:\n", noe); 
       else printf("\a\nSomething is rotten before: %i:\n\a", noe); 
    }
    for(int n, m=0; m<no_of_boards; m++){ // read out all modules
      saved_bytes+=readout_modules[m]->read_one_event(noe, all_ok); 
      if(all_ok==0) m=no_of_boards; // terminate readout;
      if(noe%wie_oft==0 && do_show_data==1) 
	readout_modules[m]->printeventdata(noe);
    }
    if(do_Max_Anzahl==1){ // is Max_anzahl reached?
      if(noe+1>=(unsigned int)Max_Anzahl) terminate_loop=1;
      if(noe%wie_oft==0 && do_show_data==1) 
        printf("Events left: %i\n", Max_Anzahl-noe-1);
    }
    if(do_Max_Time==1){ // is Max_Time reached?
	  if(time(NULL)>=endzeit) terminate_loop=1;
      if(noe%wie_oft==0 && do_show_data==1) 
        printf("Time left: %i\n", (int) (endzeit-time(NULL)));
    }
    if(noe%wie_oft_histo==0 || all_ok==0){
      if(all_ok==0 && inh_rotten==0) 
        printf("\aSOMETHING IS ROTTEN IN THE STATE OF DENMARK!\a ");
      printf("File number: %i,  ", file_number);
      save_histo();
      calc_ev_rate();
    }
    if(saved_bytes>max_save_bytes){ // max filesize reached
      open_next_save_event();
    }
    if(noe%wie_oft==0 && do_show_data==1 && noe%wie_oft_histo!=0){
      printf("File number: %i\n", file_number);
      calc_ev_rate();
    }
    if(all_ok==0 && inh_rotten==0){
      terminate_loop=1;
      printf("\n*****************************************************\n");
      printf("\n*****************************************************\n");
      printf("\n*****************************************************\n");
      printf("\n*****************************************************\n");
      printf("\n\aSOMETHING IS ROTTEN IN THE STATE OF DENMARK!\n Stopping readout!\n");
      printf("\n*****************************************************\n");
      printf("\n*****************************************************\n");
      printf("\n*****************************************************\n");
      printf("\n*****************************************************\n");
    }
    noe++;
  }while(terminate_loop==0);
};

void System_class::init_save_event(void){
  char antwort[20];
  char rawname_ext[120];
  char rawname_ext_full[120];

  sprintf(rawname_ext, "%s.000.dat", rawname);
  rawout = fopen(rawname_ext, "r");
  if(rawout!=NULL){ 
    // file exists
    fclose(rawout);
    printf("\n\n%s exists, do you want to replace it? < yes | no > ", 
      rawname);
    scanf("%s", antwort);
    if(strcmp(antwort, "yes")==0 || strcmp(antwort, "YES")==0){
      printf("deleting former raw data\n");
      int n=0;
      do{
	n++;
        sprintf(rawname_ext_full, "%s.%03i.dat", rawname, n);
      }while(remove(rawname_ext_full) ==0);
      printf("former raw data deleted\n");
    }
     else{
      printf("\nFile exists and will not be replaced.\nPlease choose an other filename.\n");
      for(int m=no_of_boards-1; m>=0; m--){
//      printf("Deleting module: %i\n", m);
        delete readout_modules[m];
      }
      exit(0);
    }
  }

  rawout=fopen(rawname_ext, "wb");
  if(rawout==NULL){
    printf("Rawout out error!!\n");
    exit(0);
  }
}

void System_class::open_next_save_event(void){
  char rawname_ext[120];
  
  timestring(stoptime);  // individually per file

  if(pos_in_file!=-1){
    if(fseek(rawout, pos_in_file, 0)==0) //all ok
      fwrite(stoptime, strlen(stoptime), 1, rawout); 
  }
  fclose(rawout);
  file_number++;
  sprintf(rawname_ext, "%s.%03i.dat", rawname, file_number);
  rawout=fopen(rawname_ext, "wb");
  if(rawout==NULL){
    printf("Rawout out error!!\n");
    exit(0);
  }
  timestring(starttime);  // individually per file

  readout_modules[0]->set_write_pointer(rawout);
  write_header_save_event();
  saved_bytes=0; 
}



void System_class::write_header_save_event(void){
  fwrite(AIO_versionsstr, sizeof(AIO_versionsstr), 1, rawout);
  fwrite(gen_starttime, strlen(gen_starttime)+1, 1, rawout);
  fwrite(starttime, strlen(starttime)+1, 1, rawout);
  pos_in_file=ftell(rawout);
  fwrite(stoptime, strlen(stoptime)+1, 1, rawout); 
  fwrite(&no_of_boards, sizeof(&no_of_boards), 1, rawout);
  fwrite(what, sizeof(what), 1, rawout);
  fwrite(geos, sizeof(geos), 1, rawout);
  for(int n=0, m=0; n<no_of_boards; n++){
    readout_modules[n]->save_further_parameters();
  }  
}

void System_class::deinit_save_event(void){
  if(pos_in_file!=-1){
    if(fseek(rawout, pos_in_file, 0)==0) //all ok
      fwrite(stoptime, strlen(stoptime), 1, rawout); 
  }
  fclose(rawout);
  printf("data storage device closed\n");
}


void System_class::init_save_histo(void){
   printf("deleting former histogram data\n");
   hisout=fopen(hisname, "wb");
   printf("former histogram data deleted\n");
   if(hisout==NULL){
     printf("Histogram out error!!\n");
     exit(0);
  }
}

void System_class::deinit_save_histo(void){
   fclose(hisout);
   printf("Histogram closed\n");
}


void System_class::save_histo(void){
  printf("saving histogram: %s\n", hisname);
  fseek(hisout, 0, 0);
  fwrite(AIO_versionsstr, sizeof(AIO_versionsstr), 1, hisout); 
  fwrite(starttime, strlen(starttime)+1, 1, hisout); 
  fwrite(stoptime, strlen(stoptime)+1, 1, hisout); 

  fwrite(&no_of_boards, sizeof(no_of_boards), 1, hisout);
  fwrite(what, sizeof(what), 1, hisout); 
  fwrite(geos, sizeof(geos), 1, hisout); 
  
  for(int m=0; m<no_of_boards; m++){
//      printf("Saving histo for module %i\n", m);
    readout_modules[m]->save_histo(hisout);
  }
}

void System_class::calc_ev_rate(void){
  long seconds, useconds;    
  double timediff;
  gettimeofday(&now, NULL);
  jetzt = time(NULL);


  if(noe!=0){
    timerzeit[3].tv_sec=timerzeit[2].tv_sec;
    timerzeit[3].tv_usec=timerzeit[2].tv_usec;
    timerzeit[2].tv_sec=timerzeit[1].tv_sec;
    timerzeit[2].tv_usec=timerzeit[1].tv_usec;
    timerzeit[1].tv_sec=timerzeit[0].tv_sec;
    timerzeit[1].tv_usec=timerzeit[0].tv_usec;
    timerzeit[0].tv_sec=now.tv_sec;
    timerzeit[0].tv_usec=now.tv_usec;

    timer_noe[3]=timer_noe[2];
    timer_noe[2]=timer_noe[1];
    timer_noe[1]=timer_noe[0];

    seconds  = timerzeit[0].tv_sec  - timerzeit[1].tv_sec;
    useconds = timerzeit[0].tv_usec - timerzeit[1].tv_usec;
    timediff = ((seconds) + useconds/1000000.0);
    noe_persec=(noe-timer_noe[1])/timediff;

    seconds  = timerzeit[0].tv_sec  - timerzeit[3].tv_sec;
    useconds = timerzeit[0].tv_usec - timerzeit[3].tv_usec;
    timediff = ((seconds) + useconds/1000000.0);
    noe_persec3=(noe-timer_noe[3])/timediff;

    timer_noe[0]=noe;
    timediff=(jetzt-anfangszeit);
    if(timediff!=0) noe_perallsec=noe/(timediff);
    else noe_perallsec=0;
    printf("%5.1f Ev/sec (during last 3 hisperiodes: %5.1f), over all: %5.1f in %5.1f minutes\n", 
            noe_persec, noe_persec3, noe_perallsec, timediff/60);
  }
  else{
    timerzeit[0].tv_sec=timerzeit[1].tv_sec=timerzeit[2].tv_sec=now.tv_sec;
    timerzeit[0].tv_usec=timerzeit[1].tv_usec=timerzeit[2].tv_usec=now.tv_usec;
    timer_noe[0]=timer_noe[1]=timer_noe[2]=noe;
    printf("\n");
  }
}
*/


int Readout_class::vme_image=0; 
int Readout_class::dma_base=0; 
int Readout_class::dma_ok=0;    
int Readout_class::inh_rotten=0; 
int Readout_class::got_writepointer=0; 
int Readout_class::dbg=0; 
FILE * Readout_class::rawout; 

//static VMEBridge vme; 
VMEBridge Readout_class::vme;

void Readout_class::init_module_variables(void){
  v874class_module=1;
  max_no_rawdataSMALL= 16;  // not including header/trail
  max_no_rawdataBIG= 32;  // not including header/trail
  adc_range= 4001; 
  strncpy(modname,"wrong_identifier", 19); // for screen output
  strncpy(mod_ident,"wrong_identifier",12);     // for parameter file readout
  further_parameters[0]=0; // no further parameter, otherwise the number
                           // parameter has 4 bits
}

void Readout_class::init_module(Param * param){
 //  short unsigned int regv;
}

Readout_class::Readout_class(){
}


void Readout_class::set_write_pointer(FILE * out){
  rawout= out; // file pointer raw file
  got_writepointer=1;
}

void Readout_class::init_Readout_class(int module, int size, Param *param, 
                                       int ext_inh_rotten, int &ret_geo){
  int len;

  int_ok=1;
  if (module==0){ // init VME image in first module
    printf("Init VME image\n");  // was   0x0fffffff 
    vme_image = vme.getImage(0x00000000,  0x0fffffff, A32, D32, MASTER);
    if(vme_image<0){             //         87654321       
      printf("\nError in allocating an image!\n");
      exit(0);
    }
    vme.setOption(vme_image, POST_WRITE_DIS);
    if(Param_getVal(param, "setup", "enable_dma")==1){
      printf("Trying to enable DMA !\n");
      dma_base = vme.requestDMA();
      if (!dma_base) {
        printf("Can't allocate DMA !\n");
        dma_ok=0;
      }
      printf("DMA enabled!\n\n");
      dma_ok=1;
      vme.setOption(DMA, BLT_ON);
    }
     else dma_ok=0;
  }

  // ************board dependend software initialisation  
  init_module_variables(); // Boardnames, size of event...

  modulesize=size;
  if(modulesize==0){ // small variation of board
    max_no_rawdata=max_no_rawdataSMALL;
  }                  // or BIG one...
   else max_no_rawdata=max_no_rawdataBIG; 
  modulenumber=module; // which one am I?
  sprintf(mod_ident_compl, "%s%i", mod_ident, module+1); // for param.tcl
  inh_rotten=ext_inh_rotten;  
  rawdata = new unsigned int[max_no_rawdata+2]; // raw incl header/trail
  decoded_event = new unsigned int[max_no_rawdata]; // without header/trail

  his = new unsigned int [adc_range*max_no_rawdata]; 
  for(int n=0; n<adc_range*max_no_rawdata; n++){
    his[n] = 0; 
  }  // reset histogram


  int_noe=0;                 // reset eventcounter
  // ************END board dependend software initialisation  
  if(v874class_module>=0){  // >=0 real board, <0 tcp or so...
    cardBase = Param_getVal(param, mod_ident_compl, "cardbase");
    readpointer=((unsigned int *) vme.getPciBaseAddr(vme_image))+cardBase/4; 
    geo=(Param_getVal(param, mod_ident_compl, "geo", 0));
  }

  // ************v874 class module hardware initialisation  
  if(v874class_module==1){
    init_v874class_module(param);
  }

  // ************board dependend hardware initialisation  
  init_module(param);
  ret_geo=geo;
}

Readout_class::Readout_class(const Readout_class &){
  printf("A module has to be initialized individually, no copying allowed!\n");
  exit(0);
}

Readout_class::~Readout_class() {
//  printf("Destructor module %i\n\n", modulenumber);
  if(modulenumber==0){
    vme.releaseImage(vme_image);
    vme.releaseDMA();
  }
//  printf("Del rawdata\n");
  delete rawdata;
//  printf("Del decoded_event\n");
  delete decoded_event;
//  printf("Del his\n");
  delete his;
//  printf("end Del\n");
}



int Readout_class::save_further_parameters(void){
  int param[20];
  param[0]=0xfade0000+((modulenumber&0xff)<<8)+(further_parameters[0]&0xff);
  fwrite(&param[0], sizeof(param[0]), 1, rawout);
//  printf("param 0: 0x%x\n", param[0]);
  for(int n=1; n<=further_parameters[0]; n++){
    param[n]=further_parameters[n];
    fwrite(&param[n], sizeof(param[0]), 1, rawout);  
//  printf("param %i: 0x%x\n", n, param[n]);
  }

}



void Readout_class::init_v874class_module(Param * param){
  // END Board dependend initialisation
  short unsigned int regv;
  unsigned int parameter;

  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x2); // ADC offline
  vme.ww(vme_image, cardBase+V874_BIT_SET_1, 0x0080);
  vme.ww(vme_image, cardBase+V874_BIT_CLR_1, 0x0080); // Module Clear
  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x0004);
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x0004); // Clear Data
  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x1800); // Auto Increment +Empty Prog
   
  vme.ww(vme_image, cardBase+V874_CRATE_REG, 
         Param_getVal(param, mod_ident_compl, "crate"));
  parameter=Param_getVal(param, mod_ident_compl, "fclrwin", 0);
  if(parameter==0) parameter=0x10;
  vme.ww(vme_image, cardBase+V874_FAST_CLR_WIN, parameter);
  parameter=Param_getVal(param, mod_ident_compl, "clrtime", 0);
  if(parameter==0) parameter=0x40;
  vme.ww(vme_image, cardBase+V874_CLRTIME, parameter);   
  vme.ww(vme_image, cardBase+V874_CTRL_REG_1,
                Param_getVal(param, mod_ident_compl, "ctrl", 0));
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x4418); // Peter 0x4000 disable all trg
  parameter=Param_getVal(param, mod_ident_compl, "bset2", 0);
  if(parameter==0) parameter=418;
  vme.ww(vme_image, cardBase+V874_BIT_SET_2, parameter);
                
  vme.rw(vme_image, cardBase+V874_BIT_SET_2, &regv);
  printf("Bitset2: 0x%x\n", regv);

  for (int i = 0; i < 32; i++) {
    char buf[16];
    sprintf(buf, "threshold%02d", i);
    parameter=Param_getVal(param, mod_ident_compl, buf, 0);
    if(parameter==0 && i<28) parameter=0x1ff;
    if(parameter==0 && i>=28) parameter=0;
    vme.ww(vme_image, cardBase+V874_THRESH + 2 * i, parameter);
  };

  // end ini


  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x2); // ADC online

  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x0004);
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x0004);   // Clear Data

  if(geo!=get_geo()){
    set_geo(geo);
  }

  // END Board dependend initialisation
}


void Readout_class::print_debug_information(void){
  int geo, crate, cnt, channel, data;
  printf("inh_rotten: %i\n", inh_rotten);
  for(int n=0; n<(number_of_data+2); n++){
      printf("DEBUGING %s: Data no %i: 0x%x\n", mod_ident_compl, n, rawdata[n]);
      switch((rawdata[n]>>24)&0x7){
	case 0:
	  geo=((rawdata[n]>>27)&0x1f);
	  if(modulesize==1) channel=((rawdata[n]>>16)&0x1f);
	   else channel=((rawdata[n]>>17)&0xf);
	  rawdata[n]=((rawdata[n]>>0)&0xfff); 
	  printf("  Rawdata[N] word: Geo: %i, channel: %i, rawdata[n]: %i\n", 
                            geo, channel, rawdata[n]);
          break;
	case 2:
	  geo=((rawdata[n]>>27)&0x1f);
	  crate=((rawdata[n]>>16)&0xff);
	  cnt=((rawdata[n]>>8)&0x3f); 
	  printf("  Header: Geo: %i, crate: %i, count: %i\n", geo, crate, cnt);
          break;
	case 4:
	  geo=((rawdata[n]>>27)&0x1f);
	  cnt=((rawdata[n])&0xffffff); 
	  printf("  Tail: Geo: %i, counter: %i\n", geo, cnt);
          break;
	case 6:
	  printf("  Invalid Datum!\n");
          break;
        default:
	  printf("WTF?\n");
      }
  }
}

int Readout_class::read_one_event(int noe, int & ok){
  unsigned short readybit;
  time_t starttime=0;
  unsigned int data;
  int channel, channel_counter;
 
  for(int n=0;n<max_no_rawdata+2;n++){ 
    rawdata[n]=0;           // clear event data stream
  }
  for(int n=0;n<max_no_rawdata;n++){ 
    decoded_event[n]=0;   // clear printdata/hisdata
  } 

  do{ // wait for data in buffer
    vme.rw(vme_image, cardBase+V874_STAT_REG_1, &readybit); 
    if((readybit&0x1)!=1){
      if(starttime==0) starttime=time(NULL);
      if((time(NULL)-starttime)>2){
	 printf("Board %i, a %s isn't ready for %i seconds\a\n", 
                modulenumber+1, modname, int (time(NULL)-starttime));
      }
    }
  }while((readybit&0x1)!=1);  // exit when data in buffer

  data=*readpointer;  // read out header
  if(((data>>24)&0x7) == 6){ // it isnt header! invalid datum
     printf("\ninvalid datum (board %i) at event %i: 0x%x\n", 
             modulenumber+1, noe, data);
     return 0;
  }
  if(((data>>24)&0x7) != 2){ // it isnt header!
     printf("\nwrong header (%s) at event %i: 0x%x\n", 
             mod_ident_compl, noe, data); 
     ok=0;
  }
  rawdata[0]=data;        // save header in data stream

  number_of_data=(data>>8)&0x3f; // decode max_no_of_ch in header
  if(number_of_data>max_no_rawdata) {   // something is fishy
    printf("Number of channels too big!! %i!\n", number_of_data);
    printf("0x%x\n", data); number_of_data=max_no_rawdata;
  }

  for(channel_counter=1; channel_counter<=number_of_data; channel_counter++){ // data readout
    data=*readpointer;   // read data word
    rawdata[channel_counter]=data;                  // save word in data stream
    if(((data>>24)&0x7) == 6){ // is invalid datum!
      printf("\ninvalid internal datum (board %i) at event %i: 0x%x\n", 
              modulenumber+1, noe, data);
      ok=0;
    }
    if(modulesize==0){ // small module
      channel=(data>>17)&0xf; // decode channel
    }
    else{  // BIG module
      channel=(data>>16)&0x1f; // decode channel
    }
    data=data&0x1fff;        // decode 12bit data

    decoded_event[channel]=data;  // decoded event
   
    if(((data>>12)&0x3) == 0){ // no overflow/underthreshold bit
	his[data+channel*adc_range]++;  // do the histogram stuff...
    }
    else{
      printf("\nOVERFLOW or UNDERTHRESHOLD on ch %i!!%i!\n", 
                  channel, ((data>>12)&0X3));
      printf("\nData was 0x%x", rawdata[channel_counter]);
    }
  }
  rawdata[channel_counter]=data; // save trail in data stream
  data=*readpointer;  // read out trail

  rawdata[channel_counter]=data; // save trail in data stream
  int_noe = (data&0xffffff);  // noe is in trail

  if(((data>>24)&0x7) !=4){  // is it trail?
    if(strcmp(mod_ident, "iol_")==0){ // sometimes there is a glitch...
      printf("IOL Bug! recovering...\n");
      data=noe+(rawdata[0]&0xf8000000)+0x04000000; // calculate trail
      rawdata[channel_counter]=data; // save trail in data stream
      int_noe=noe;
    }
    else{
      printf("\nwrong trail (%s) at event %i: 0x%x\n", 
                  mod_ident_compl, noe, data); 
      ok=0;
    }
  }

  if(save_event()==0) exit(0);
  
  if(int_noe!=noe){
    printf("%s: Internal NoE (%i) != NoE (%i)\n", 
               mod_ident_compl, int_noe, noe);
    int_ok=0;
    ok=0;
  }
  if(ok==0){
      print_debug_information();
  }
  vme.rw(vme_image, cardBase+V874_STAT_REG_1, &readybit);
  if(readybit&0x5!=0 && inh_rotten==0){  // checking readybit
    printf("Something is rotten in the state of %s.(Address 0x%x, Reg 1: 0x%x)\b\n", 
            modname, cardBase, readybit);
    number_of_data=-3;
    
  }
  return((number_of_data+2)*4);  // return saved bytes
} // Readout_class::read_one_event(int & ok){


void Readout_class::lam(void){
  short unsigned int regv;
	
  vme.rw(vme_image, cardBase+V874_STAT_REG_1, &regv); 
  regv = regv &0x1;
    // polling data_ready_bit
  while(regv!=0x3){ //no event &0x3
    for(int n=0; n<=1000; n++) printf(""); // wait a little bit
    vme.rw(vme_image, cardBase+V874_STAT_REG_1, &regv); 
    regv = regv &0x3;
      // polling data_ready_bit
  }
}

void Readout_class::reset(void){
  short unsigned int regv;

  vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x0004);   // Clear Data 
  vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x0004);   // Clear Data 
  printf("RESET! 0x%x\n", cardBase);
  printf("  Reseted a %s as board no %i\n", modname, modulenumber+1);

}

void Readout_class::bsy_on(void){
    vme.ww(vme_image, cardBase+V874_BIT_SET_2, 0x1); 
}

void Readout_class::bsy_off(void){
    vme.ww(vme_image, cardBase+V874_BIT_CLR_2, 0x1); 
}

int Readout_class::get_geo(void){
  unsigned short int_geo;
  vme.rw(vme_image, cardBase+V874_GEO_ADDR, &int_geo);
  int_geo= int_geo &0x1F;
  return int_geo;
}

void Readout_class::set_geo(int int_geo){
  if((vme.ww(vme_image, cardBase+V874_GEO_ADDR, int_geo))<0){
    printf("Can't set the geo address (geo from init file (%i)!= geo from board (%i).\n", int_geo, get_geo());
    printf("EXIT\n");
    exit(0);
  }

}

void Readout_class::printeventdata(unsigned int noe){
  if(modulenumber==0) printf("\n");	 
  printf("%s: %i, No. of channels: %2i, Noe: %i (%i)  \n",
         modname, modulenumber, max_no_rawdata, int_noe, noe);
  for(int n=0; n<max_no_rawdata; n++){
    if(n%5==0 &&n!=0) printf("\n");
    if(decoded_event[n]!=0) printf("  %2i: %4i,", n, decoded_event[n]);
     else printf("          ,");
  }
  printf("\n"); 
  if(int_ok==0) printf("Something is rotten in module %i\n", modulenumber);
}

int Readout_class::save_histo(FILE * hisout){
//   printf("Saving histo, %i, %i\n", adc_range, max_no_rawdata);
   fwrite(&adc_range, sizeof(adc_range), 1, hisout);
   fwrite(&max_no_rawdata, sizeof(max_no_rawdata), 1, hisout);
   return fwrite(his, sizeof(his)*adc_range*max_no_rawdata, 1, hisout);
}

const  char* Readout_class::get_name(void){
    return modname;
}
int Readout_class::save_event(void){
/*   printf("SavingHeader: 0x%x\n", rawdata[0]);
   for(int n=0; n<number_of_data+2; n++){
     printf("Saving %i: 0x%x\n", n, rawdata[n]);
     }*/
   return fwrite(rawdata, sizeof(rawdata)*(number_of_data+2), 1, rawout);
}
