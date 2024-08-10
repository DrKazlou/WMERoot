#include "vmelib.h"
#include "param.h"
//#include "aio_module_def.h"

//#include "tcpconnection.h"

#ifndef _READOUT_CLASS_H
#define _READOUT_CLASS_H


class Readout_class
{
  protected:
    static int vme_image;          // constr
    static int inh_rotten;
    static VMEBridge vme;
    static FILE * rawout;          // constr
    static int got_writepointer;
    static int dma_base, dma_ok;   // constr
    static int dbg;  // debug message 

    int int_ok;
    unsigned int cardBase;         // constr
    unsigned int * rawdata;        // constr
    unsigned int *readpointer;     // in init_module
    int number_of_data;            // in read_one_event
    unsigned int * decoded_event;  // constr
    int adc_range;      // in init_module_variables
    unsigned int *his;            // constr
    int modulenumber;              // constr
    int modulesize;                // constr
    short unsigned int geo;        // in init_module
    int int_noe;                   // constr
    int max_no_rawdata;
    int max_no_rawdataBIG;  // not including header/trail
    int max_no_rawdataSMALL;  // not including header/trail
    char modname[20];  // for screen output
    char mod_ident[25];     // for parameter file readout
    char mod_ident_compl[27];      // in init_module_variables
    int further_parameters[20];  // in  init_module_variables
    int v874class_module;
  public: 
    Readout_class();
    virtual void init_Readout_class(int module, int size, Param *param, 
                 int inh_rotten, int &geo);
    Readout_class(const Readout_class &);
    ~Readout_class();
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
    virtual int save_histo(FILE * hisout);
    void set_write_pointer(FILE * out);
    int save_further_parameters(void);
    const char * get_name(void); 
  protected:
    virtual int save_event(void);
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void init_v874class_module(Param * param);
    virtual void print_debug_information(void);
};

/*
class System_class
{
  private:
    static int initialized;
    static int file_number;
    Readout_class * readout_modules[AIO_max_no_of_boards];
	
    int do_save_histo; 
    int wie_oft_histo;
    int do_show_data;
    int wie_oft;

    int inh_rotten;
    int Max_Anzahl;
    int Max_Time;
    int do_Max_Anzahl, do_Max_Time;
    unsigned int noe;
    unsigned int saved_bytes, max_save_bytes;
    int no_of_boards, what[AIO_max_no_of_boards], geos[AIO_max_no_of_boards];
    int  LAM;
    time_t anfangszeit, endzeit, jetzt;
    struct timeval now, timerzeit[4];
    int timer_noe[4];
    float noe_persec, noe_persec3, noe_perallsec;
    char gen_starttime[50], starttime[50], stoptime[50];
    long pos_in_file;
    char rawname[100], hisname[100];
    int all_ok;

    FILE * rawout, * hisout;
    Param paramS, *param;
  public:
    System_class();
    ~System_class();
    void init(char * filename, char * tcl_file, unsigned long ext_max_save_bytes,
              int ext_Max_Anzahl, int ext_Max_Time);
    void reset(void);
    void do_readout(void);
  private:
    void init_save_event(void);
    void open_next_save_event(void);
    void write_header_save_event(void);
    void deinit_save_event(void);
    void init_save_histo(void);
    void deinit_save_histo(void);
    void save_histo(void);
    void calc_ev_rate(void);
};
*/

class Readout_class_iol : public Readout_class
{
  public: 
    virtual void printeventdata(unsigned int noe);
    virtual void lam(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
  protected:
    virtual void init_module_variables(void);
};

class Readout_class_ntec : public Readout_class
{
  public:
    virtual void printeventdata(unsigned int noe);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void pigset(int device, int l_value);
};

class Readout_class_veto : public Readout_class
{
  public:
    virtual void printeventdata(unsigned int noe);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void pigset(int device, int l_value);
};

class Readout_class_v775 : public Readout_class
{
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
};

class Readout_class_v895 : public Readout_class
{
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
    virtual int save_histo(FILE * hisout);
  protected:
    virtual int save_event(void);
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void init_v874class_module(Param * param);
};

class Readout_class_v785 : public Readout_class
{
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
};

class Readout_class_v792 : public Readout_class
{
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
};

class Readout_class_v965 : public Readout_class
{
  public:
    virtual void printeventdata(unsigned int noe);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
};

class Readout_class_sis3820 : public Readout_class
{
  public:
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void init_v874class_module(Param * param);
};

class Readout_class_sis3302 : public Readout_class
{  
  public:
    int max_hits_per_event;
    ~Readout_class_sis3302();
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
    virtual int save_histo(FILE * hisout);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void WaitStop(int cv);
    void copy_RED_pointers(void);
    unsigned int calc_RED_pointers(unsigned int org);
    int GetData(int ch, int & counter);
    void SetThresh(int ch, unsigned long int thresh);
    void SetTrigSetup(int ch, unsigned long int thresh);
    void SetDAC(int ch,unsigned long int val);
    int vme2_image;          // constr
    unsigned int page_size;
    unsigned EventConfig;
    unsigned WrapPageSize_Bytes;
    unsigned int SADC_N_of_samples;    
    unsigned short *shortarray;
    int pedestal[8], value[8], hisarray[8]; 
    int ped_start, ped_end;
    int RED;   
    int clock;
    unsigned int RED_ADC_Mem[8], RED_Event_Config_ALL_ADC;
    unsigned int RED_Sample_Address_ADC[8], RED_Trigger_Setup_ADC[8];
    unsigned int RED_Trigger_Threshold_ADC[8], RED_TriggerFlagClear[4];
    unsigned int RED_EventDirectory_ADC[8];
    int hex;
    int threshold[8];
};

/*
class Readout_class_bonnsync : public Readout_class
{
  public:
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    int vme_image2;          // constr
};
*/
/*
class Readout_class_bonntrack : public Readout_class
{
  public:
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                   
    virtual void set_geo(int int_geo);            
    virtual void printeventdata(unsigned int noe);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    char ip[150];          // init_module
    TcpConnection client;
    CTSEvent event;
};

*/

class Readout_class_avm16 : public Readout_class
{  
  public:
    ~Readout_class_avm16();
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
    virtual int save_histo(FILE * hisout);
  protected:
    int mod_type, cr, cha_inh, cha_raw;
    int trg_level, anal_ctrl;
    int iw_start, iw_length, sw_start, sw_length, sw_intlength;
    int no_of_raw_ch, raw_ch_ena[16];
    int samples_to_readout;

    int card_address, channel, data_ident;
    int integral[16];
    short int win_start_time[16], mean_lvl[16];
    short int min_time[16], min_lvl[16], max_time[16], max_lvl[16];
    short int zero_cross[16];
    short int inst_mean_line_lvl[16], mean_val_before[16];
    short int int_of_iw[16], mean_val_after[16];
    int own_ph[16];
    int *rawhis;
    int pedestal[16], value[16]; 
    int ped_start, ped_end;

    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
};

class Readout_class_sis3316 : public Readout_class
{  
  public:
    int max_hits_per_event;
    ~Readout_class_sis3316();
    virtual int read_one_event(int noe, int & ok);
    virtual void lam(void);
    virtual void reset(void);
    virtual void bsy_on(void);   
    virtual void bsy_off(void);
    virtual int  get_geo(void);                    // OK
    virtual void set_geo(int int_geo);                    // OK
    virtual void printeventdata(unsigned int noe);
    virtual int save_histo(FILE * hisout);
  protected:
    virtual void init_module_variables(void);
    virtual void init_module(Param * param);
    void WaitStop(int cv);
    unsigned int calc_RED_pointers(unsigned int org);
    int GetData(int ch, int & counter);
    void SetThresh(int ch, unsigned long int thresh);
    void SetTrigSetup(int ch, unsigned long int thresh);
    void SetDAC(int ch,unsigned long int val);
    int set_frequency(int osc, unsigned char *values);
    int I2cStart(int osc);
    int I2cStop(int osc);
    int I2cWriteByte(int osc, unsigned char data, char *ack);
    int I2cReadByte(int osc, unsigned char *data, char ack);
    int Si570FreezeDCO(int osc);
    int Si570Divider(int osc, unsigned char *data);
    int Si570UnfreezeDCO(int osc);
    int Si570NewFreq(int osc);

    int vme2_image;          // constr
//    unsigned int page_size;
//    unsigned EventConfig;
    unsigned int SADC_N_of_samples;    
    unsigned int trigger_delay;
    unsigned short *shortarray;
    int pedestal[16], value[16], hisarray[16]; 
    int bank1_armed_flag;
    unsigned int memory_bank_offset_addr;
    unsigned int memory_channel_offset_addr;
    int ped_start, ped_end;
    int clock;
    int hex;
    int threshold[16];
    unsigned char freqPreset250MHz[6];
};

#endif
