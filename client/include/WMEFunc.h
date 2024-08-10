#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <vector>
#include <sys/socket.h>

#include <sys/time.h>
#include <sys/types.h>

#include <time.h>
#include <unistd.h> // read(), write(), close()

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGLabel.h"
#include "TTree.h"
#include "TFile.h"
#include "TGStatusBar.h"
#include "TGWindow.h"
#include "TGFrame.h"

#define MAX 16200 // 16080
#define PORT 4000
#define SA struct sockaddr

#define n_sadc 500 // waveform samples
#define b_width 10    // 1 bin = 10 ns due to 100 MS/s sampling rate
#define SADC_CH 8 // channels  per sis3302 board
#define TDC_CH 32 // channels  per v775 board 
//#define QDC_CH 16 // channels  per v792 board 
#define QDC_CH 16 // channels  per v965 board  dual range low/high range for every 16 ch

typedef struct
{	
	bool fInit, fPrint, fTimer, fStoreTraces;
	
	int timer_val;

	int sockfd; // for TCP
		
	int loop;
	TH1D *ampl;
	
	int Nev, TrgCnt;

	uint64_t StartTime;
	double DrawTime;
	
	TGLabel  *TLabel;
	TGStatusBar *StatusBar;
	TCanvas *can;

	TGMainFrame *main;
	TTree *tree;
	TFile *ff;
	
} ReadoutConfig_t;

typedef struct{
		
	uint32_t ns; // number of samples
	//double temp;


}DigitizerConfig_t;

typedef struct{
	
	int xbins, xmin, xmax;
	int ybins, ymin, ymax;
	
}hist_settings_t;


typedef struct{
	
	
	int WF_XMIN, WF_XMAX, WF_YMIN, WF_YMAX;
	int ALBound, ARBound, ILBound, IRBound;
	
	int PSD_BIN;
	int BL_CUT;
	
	char h2Style[10];	
	int FtoDraw_QDC, FtoDraw_TDC;	
	
	int NPad;	
	
	//double k, b, arr_calib[2][10]; // calibration fit parameters
	//char fFormula[40];
	//bool fCalib;
	
	int cAmpl, cInt, cPSD_ampl, cPSD_int, cQsl, cIA, cCharge, cTime, cdT;	
	bool fDraw[SADC_CH], fDrawQDC[SADC_CH], fDrawTDC[TDC_CH], fBL, fTrace, fAmpl, fInt, fdT, fPSD_ampl, fPSD_int, fQsl, fIA, fCharge, fTime;  // flags for every time of histograms;
	bool fQTypeLow, fQTypeHigh;
	int CH_2D; // channel to draw th2d
	
	TH1D *trace[SADC_CH], *ampl[SADC_CH], *integral[SADC_CH], *QDC_L[QDC_CH], *QDC_H[QDC_CH], *TDC[TDC_CH];
	TH2D *int_ampl, *psd_ampl, *psd_int, *qs_ql; 
			
	hist_settings_t ampl_set, integral_set, dt_set, psd_ampl_set;	
	
	std::vector<std::vector<int>> vec;
	
} Histograms_t;


int revpos(const char s[], char c);

long get_time();

void SaveAfterError(TCanvas *can);

int StartTCP( );

void InitReadoutConfig( ReadoutConfig_t *Rcfg );

void InitHisto( Histograms_t *Histo);

void InitVME( ReadoutConfig_t *Rcfg);

void FillHisto( Histograms_t *Histo, char MSG[MAX]);

void DrawHisto( Histograms_t Histo, TCanvas *can);

void ReadoutLoop( );

void DataAcquisition( );





