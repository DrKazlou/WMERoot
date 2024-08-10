

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "WMEFrame.h"
#include "WMEFunc.h"


#include "TROOT.h"
#include "TApplication.h"


	
	ReadoutConfig_t Rcfg;    
	DigitizerConfig_t Dcfg;    	
	Histograms_t Histo;    	
	
	

//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
	char CName[100];
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }
   	
	memset(&Rcfg, 0, sizeof(Rcfg));
	memset(&Dcfg, 0, sizeof(Dcfg));
	memset(&Histo, 0, sizeof(Histo));
			
	InitReadoutConfig(&Rcfg);	
	InitHisto(&Histo);
				
	new MainFrame(gClient->GetRoot(), 1700, 850);
		
	DataAcquisition( );
	
	//if(ret){
	//	sprintf(CName, "ERR_DAQ %i \n", ret);
	//	new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
	//}
	
	
	theApp.Run( );

   return 0;
}

