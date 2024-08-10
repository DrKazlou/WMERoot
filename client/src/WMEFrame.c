#include "WMEFrame.h"
#include "WMEOpt.h"

#include "WMEFunc.h"


#include <vector>
#include "TGWindow.h"


extern ReadoutConfig_t Rcfg;
extern DigitizerConfig_t Dcfg;
extern Histograms_t Histo;


using namespace std;	
		


MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{
	
	fMain = new TGMainFrame(p, w, h);
	Rcfg.main = fMain;

   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

   fMain->Connect("CloseWindow()", "MainFrame", this, "CloseWindow()");

   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuDock = new TGDockableFrame(fMain);
   fMain->AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
   fMenuDock->SetWindowName("DTRoot Menu");

   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
	
	fMenuFile = new TGPopupMenu(gClient->GetRoot());
	fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
	fMenuFile->AddEntry("Save h_ampl &txt", M_FILE_SAVE_AMPL_TXT);
	fMenuFile->AddEntry("Save &histo", M_FILE_SAVE_HISTO);
	
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
	
	fMenuFile->DisableEntry(M_FILE_OPEN);
	
   
	fMenuOpt = new TGPopupMenu(gClient->GetRoot());
	fMenuOpt->AddEntry("&Hist options", M_OPT_MENU);
	fMenuOpt->AddEntry("&Digi parameters", M_PARAMS_MENU);
	
   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry("&Manual", M_MANUAL);
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   // Menu button messages are handled by the main frame (i.e. "this")
   // HandleMenu() method.
	fMenuFile->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");
	fMenuOpt->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");
	fMenuHelp->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");

   fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Options", fMenuOpt, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);


	TGHorizontalFrame *hframe1 = new TGHorizontalFrame(fMain,200,40);
	TGVerticalFrame *vframe1 = new TGVerticalFrame(hframe1,200,40);		
    hframe1->AddFrame(vframe1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//
	
	TGGroupFrame *gframe_store = new TGGroupFrame(hframe1, " - ", kVerticalFrame);
	gframe_store->SetTitlePos(TGGroupFrame::kRight); // right aligned
	vframe1->AddFrame(gframe_store, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//
	
	fSTCheck  = new TGCheckButton(gframe_store, new TGHotString("STORE TRACES"), 70);	
	fSTCheck->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
	gframe_store->AddFrame(fSTCheck, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
	fFStore = new TGHorizontalFrame(gframe_store, 200, 40);
	gframe_store->AddFrame(fFStore, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fSTLabel = new TGLabel(fFStore, "File name");
	fFStore->AddFrame(fSTLabel, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
	//fSTTextEntry = new TGTextEntry(fFStore, fSTTextBuffer = new TGTextBuffer(0)) ;
	fSTTextEntry = new TGTextEntry(gframe_store, fSTTextBuffer = new TGTextBuffer(0)) ;
	fSTTextBuffer->AddText(0, "output.root");
	fSTTextEntry->SetEnabled(0);
	fSTTextEntry->Resize(100, fSTTextEntry->GetDefaultHeight());
	gframe_store->AddFrame(fSTTextEntry, new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX, 0, 2, 2, 2));
	
	gframe_store->Resize();
	
	TGGroupFrame *gframe_opt = new TGGroupFrame(vframe1, "Options", kVerticalFrame);
	gframe_opt->SetTitlePos(TGGroupFrame::kRight); // right aligned
	vframe1->AddFrame(gframe_opt, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//

   // 2 column, n rows
   gframe_opt->SetLayoutManager(new TGMatrixLayout(gframe_opt, 0, 1, 10));

const char *numlabel[] = {
   "BLine_cut",
    "DrawTime",
	"CH_2D",
	"Timer"
	};	
	
const Double_t numinit[] = {
   //20, (double)WDcfg.Threshold[0], (double)WDcfg.Threshold[1], Rcfg.DrawTime, 0, 300
   20, Rcfg.DrawTime, 3, 300
};	

	
int iStyle[]	= {0, 2, 0, 0}; 	
	
   for (int i = 0; i < 4; i++) {
		fF[i] = new TGHorizontalFrame(gframe_opt, 200, 30);
		gframe_opt->AddFrame(fF[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
		fNumericEntries[i] = new TGNumberEntry(fF[i], numinit[i], 8, i + 20, (TGNumberFormat::EStyle) iStyle[i]); //numinit[i], 7, i + 20, (TGNumberFormat::EStyle) iStyle[i]
		fNumericEntries[i]->Connect("ValueSet(Long_t)", "MainFrame", this, "DoSetVal()");
		fF[i]->AddFrame(fNumericEntries[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
		fLabel[i] = new TGLabel(fF[i], numlabel[i]);
		fF[i]->AddFrame(fLabel[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
		
		if ( i == 2 )
			fNumericEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 7);
	
		if ( i == 3 ) {
			fNumericEntries[i]->SetState(kFALSE);
			fCTime = new TGCheckButton(fF[i], new TGHotString(""), 66);	
			fCTime->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
			fF[i]->AddFrame(fCTime, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
		} 
		
   }
		
   gframe_opt->Resize();
   
    fTab = new TGTab(vframe1, 300, 300);
	
	TGCompositeFrame *fCF = fTab->AddTab("QDC");
		
	//TGGroupFrame *gframe_ch = new TGGroupFrame(fCF, "Channels", kVerticalFrame);
	//gframe_ch->SetTitlePos(TGGroupFrame::kCenter); 
	//fCF->AddFrame(gframe_ch, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 5, 5, 5));//
     
	
	Pixel_t p_color[8];
	
	gClient->GetColorByName("blue", p_color[0]);
	gClient->GetColorByName("red", p_color[1]);
	//gClient->GetColorByName("green", p_color[3]);
	p_color[2] = 0x8000; //green
	gClient->GetColorByName("magenta", p_color[3]);
	//gClient->GetColorByName("violet", p_color[4]);
	gClient->GetColorByName("orange", p_color[4]);
	//p_color[4] = 0xFFD700; //gold
	gClient->GetColorByName("gray", p_color[5]);
	gClient->GetColorByName("brown", p_color[6]);
	gClient->GetColorByName("black", p_color[7]);
		
	
	for (int i = 0; i < QDC_CH; ++i) {
		sprintf(CName, "CH[%i]",i);
		//fCq[i] = new TGCheckButton(gframe_ch, CName,	i);
		fCq[i] = new TGCheckButton(fCF, CName,	i); // id = [0,16)
		fCq[i]->SetTextColor(p_color[i%8]);
		fCq[i]->SetState(kButtonDown); 
		
		fCq[i]->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
		//gframe_ch->AddFrame(fCa[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
		fCF->AddFrame(fCq[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	}
	 
	
	fQType[0] = new TGCheckButton(fCF, "Low",	80);
	fQType[0]->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
	fQType[0]->SetState(kButtonDown); 
	fCF->AddFrame(fQType[0], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	fQType[1] = new TGCheckButton(fCF, "High",	81);
	fQType[1]->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
	fQType[1]->SetState(kButtonUp); 
	fCF->AddFrame(fQType[1], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
	
	fCF->SetLayoutManager(new TGMatrixLayout(fCF, 0, 4, 8));
	fCF->Resize(); 
	
	fCF = fTab->AddTab("TDC");
	
	for (int i = 0; i < TDC_CH; ++i) {
		sprintf(CName, "CH[%i]",i);
		fCt[i] = new TGCheckButton(fCF, CName,	20+i); //id = [20,52)
		fCt[i]->SetTextColor(p_color[i%8]);
		fCt[i]->SetState(kButtonUp); 
		
		fCt[i]->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
		fCF->AddFrame(fCt[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	}
	 
	fCF->SetLayoutManager(new TGMatrixLayout(fCF, 0, 4, 8));
	fCF->Resize(); 
	
	fCF = fTab->AddTab("dT");
	
	//way to handle Tab disabling
	//TGTabElement *tabel = fTab->GetTabTab("dT");
	//tabel->SetEnabled(kFALSE);
	
	vframe1->AddFrame(fTab, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 5, 5, 5));//
	
   
   // for future , if you want to play with shutters
   /*
   TGShutter *fShutter = new TGShutter(vframe1, kVerticalFrame); //kSunkenFrame
    vframe1->AddFrame(fShutter, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2)); //new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));//
   static int id = 5001;
   TGShutterItem *item = new TGShutterItem(fShutter, new TGHotString("Shutter"), id++);
   TGCompositeFrame *container = (TGCompositeFrame *) item->GetContainer();
   
   TGGroupFrame *gframe_hist = new TGGroupFrame(container, "Hist", kVerticalFrame);
	gframe_hist->SetTitlePos(TGGroupFrame::kRight); 
	container->AddFrame(gframe_hist, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 5, 5, 5));//
	
			 
	gframe_hist->Resize();
	
	fShutter->AddItem(item);
	fShutter->Resize( 200, 80);
   */
   
   
	TGGroupFrame *gframe_hist = new TGGroupFrame(vframe1, "Hist", kVerticalFrame);
	gframe_hist->SetTitlePos(TGGroupFrame::kRight); 
	vframe1->AddFrame(gframe_hist, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 5, 5, 5, 5));//

	const char *cblabel[] = {"BL_CUT", "TRACES", "AMPL_HIST", "INTEGRAL", "Int vs Ampl", "PSD_ampl", "Charge", "Time", "dT"};	
	
	for (int i = 0; i < 9; i++) {
		fC[i] = new TGCheckButton(gframe_hist, new TGHotString(cblabel[i]), 55+i);	
		fC[i]->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
		gframe_hist->AddFrame(fC[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
		fC[i]->SetState(kButtonDisabled);
	}	
	
	gframe_hist->SetLayoutManager(new TGMatrixLayout(gframe_hist, 0, 3, 3));			 
	gframe_hist->Resize();
	
	 
	 
    fInitButton = new TGTextButton(vframe1, " In&it ", 1);
    fInitButton->SetFont(sFont); 
    fInitButton->Resize(60, 30);
    fInitButton->Connect("Clicked()","MainFrame",this,"InitButton()");
    vframe1->AddFrame( fInitButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4));	
	
    fClearButton = new TGTextButton(vframe1, " Cle&ar ", 1);
    fClearButton->SetFont(sFont);
    fClearButton->Resize(60, 30);
    fClearButton->SetState (kButtonDisabled);
    fClearButton->Connect("Clicked()","MainFrame",this,"ClearHisto()");
    vframe1->AddFrame(fClearButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4));	
	
	Rcfg.TLabel = new TGLabel(vframe1, "          Timer          ");
	Rcfg.TLabel->SetTextFont(sFont);
	Rcfg.TLabel->Resize(200, 30);
	
	vframe1->AddFrame(Rcfg.TLabel, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
	vframe1->Resize();
	
	fEcanvas1 = new TRootEmbeddedCanvas("Ecanvas1", hframe1, w - 300, h - 120); // 1600 780
	hframe1->AddFrame(fEcanvas1, new TGLayoutHints(kLHintsCenterX, 10, 5, 25, 0));//kLHintsExpandX |   kLHintsExpandY
	hframe1->Resize();
	
    fMain->AddFrame(hframe1, new TGLayoutHints(kLHintsCenterX, 2, 2 , 2, 2) );
	
	Rcfg.can = fEcanvas1->GetCanvas();
			
	// status bar
	Int_t parts[] = {13, 13, 13, 13, 22, 26};
	Rcfg.StatusBar = new TGStatusBar(fMain, 100, 20, kHorizontalFrame); //kHorizontalFrame //kSunkenFrame
	Rcfg.StatusBar->SetParts(parts,6);
	fMain->AddFrame(Rcfg.StatusBar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 0, 0, 2, 0));
	
	
	TGHorizontalFrame *hframe2 = new TGHorizontalFrame(fMain, 200, 40);
 	fStartButton = new TGTextButton(hframe2," Sta&rt ", 1);
	fStartButton->SetFont(sFont);
    fStartButton->Resize(60, 30);
	fStartButton->SetState (kButtonDisabled);
  	fStartButton->Connect("Clicked()","MainFrame", this, "StartButton()");
  	hframe2->AddFrame(fStartButton, new TGLayoutHints(kLHintsCenterY |  kLHintsExpandX, 4, 4, 4, 4));

	fStopButton = new TGTextButton(hframe2,"  S&top  ", 1);
    fStopButton->SetFont(sFont);
    fStopButton->Resize(60, 30);
	fStopButton->SetState (kButtonDisabled);
	fStopButton->Connect("Clicked()","MainFrame",this,"StopButton()");	
    hframe2->AddFrame(fStopButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX, 4, 4, 4, 4));
	
	hframe2->Resize();
		
   fMain->AddFrame(hframe2, new TGLayoutHints(kLHintsCenterX,       2, 2, 20, 2));
   fMain->SetWindowName("WMERoot");
   fMain->MapSubwindows( );

	fMain->Resize( );
	fMain->MapWindow( );
   //fMain->Print();
   Connect("Created()", "MainFrame", this, "Welcome()");
   Created( );
}

MainFrame::~MainFrame()
{
   // Delete all created widgets.

   delete fMenuFile;

   delete fMain;
}

void MainFrame::CloseWindow()
{
    	
	gApplication->Terminate();
}

void MainFrame::DoCheckBox(){
	
	TGCheckButton *cbtn = (TGCheckButton *) gTQSender;
	Int_t id = cbtn->WidgetId();
	//printf("checkbox changed %i  \n", id);
	
	//Store traces checkbox
	if (id == 70){ 
		fSTTextEntry->SetEnabled(fSTCheck->GetState() == kButtonDown ? 1 : 0);
		Rcfg.fStoreTraces = fSTCheck->GetState( ) == kButtonDown ? true : false;
	}	
	
	 //Timer checkbox
	if (id == 66 ) {
	   fNumericEntries[3]->SetState( fCTime->GetState() == kButtonDown ? kTRUE : kFALSE );
	   Rcfg.fTimer = fCTime->GetState( ) == kButtonDown ? true : false;
	}  
	
	if (id < QDC_CH ) {
		Histo.fDrawQDC[id] = fCq[id]->GetState() == kButtonDown ? true : false;
		
		int n = QDC_CH;
		for (int i = 0; i<QDC_CH; i++)
			if (Histo.fDrawQDC[i] && i<n) 
				n = i;
		Histo.FtoDraw_QDC = n;
	}
	
	if (id >= 20 && id < (20 + TDC_CH) ) {
		Histo.fDrawTDC[id-20] = fCt[id-20]->GetState() == kButtonDown ? true : false;
		
		int n = TDC_CH;
		for (int i = 0; i<TDC_CH; i++)
			if (Histo.fDrawTDC[i] && i<n) 
				n = i;
		Histo.FtoDraw_TDC = n;
	}
		
	if (id == 55)
		Histo.fBL = fC[0]->GetState() == kButtonDown ? true : false;
	
	//if (id == 63){
	//	TGTabElement *tabel = fTab->GetTabTab("dT");
	//	tabel->SetEnabled(fC[8]->GetState() == kButtonDown ? kTRUE : kFALSE);
	//}
	
	if (id == 80)
		Histo.fQTypeLow = fQType[0]->GetState() == kButtonDown ? true : false;
	
	if (id == 81)
		Histo.fQTypeHigh = fQType[1]->GetState() == kButtonDown ? true : false;
	
   	if (id == 61 || id == 62 || id == 63 ){ //
		//Histo.fTrace = fC[1]->GetState() == kButtonDown ? true : false;
		//Histo.fAmpl = fC[2]->GetState() == kButtonDown ? true : false;
		//Histo.fInt = fC[3]->GetState() == kButtonDown ? true : false;
		//Histo.fIA = fC[4]->GetState() == kButtonDown ? true : false;
		//Histo.fPSD_ampl = fC[5]->GetState() == kButtonDown ? true : false;
		
		Histo.fCharge = fC[6]->GetState() == kButtonDown ? true : false;
		Histo.fTime = fC[7]->GetState() == kButtonDown ? true : false;
		Histo.fdT = fC[8]->GetState() == kButtonDown ? true : false;
		
		fC[id-55]->GetState() == kButtonDown ? Histo.NPad++ : Histo.NPad--;
		
		//if (Histo.fAmpl)
		//	Histo.cAmpl = 1 + (Histo.fTrace ? 1 : 0);
						
		//if (Histo.fInt)
		//	Histo.cInt = 1 + (Histo.fTrace ? 1 : 0) + (Histo.fAmpl ? 1 : 0);
				
		//if (Histo.fIA)	
		//	Histo.cIA = 1 + (Histo.fTrace ? 1 : 0) + (Histo.fAmpl ? 1 : 0) + (Histo.fInt ? 1 : 0);
		
		//if (Histo.fPSD_ampl)	
		//	Histo.cPSD_ampl = 1 + (Histo.fTrace ? 1 : 0) + (Histo.fAmpl ? 1 : 0) + (Histo.fInt ? 1 : 0) + (Histo.fIA ? 1 : 0);
	
		if (Histo.fCharge)	
			Histo.cCharge = 1;
		
		if (Histo.fTime)	
			Histo.cTime = 1 + (Histo.fCharge ? 1 : 0);
		
		if (Histo.fdT)	
			Histo.cdT = 1 + (Histo.fCharge ? 1 : 0) + (Histo.fTime ? 1 : 0);
						
		Rcfg.can->Clear();
		Rcfg.can->SetGrid( );
		
		
		if (Histo.NPad == 1)
			Rcfg.can->Divide(1, 1, 0.001, 0.001);
		if (Histo.NPad == 2)
			Rcfg.can->Divide(2, 1, 0.001, 0.001);
		if (Histo.NPad > 2 && Histo.NPad < 5)
			Rcfg.can->Divide(2, 2, 0.001, 0.001);
		if (Histo.NPad > 4 && Histo.NPad < 7)
			Rcfg.can->Divide(3, 2, 0.001, 0.001);
		
		Rcfg.can->Modified( );
		
		//printf("NPad %i \n", Histo.NPad);		
	}

}

void MainFrame::DoSetVal(){
		
	Histo.BL_CUT = fNumericEntries[0]->GetNumber();
	Rcfg.DrawTime = fNumericEntries[1]->GetNumber();
	Histo.CH_2D = fNumericEntries[2]->GetNumber();
	Rcfg.timer_val = fNumericEntries[3]->GetNumber();

}


void MainFrame::InitButton()
{
	InitVME( &Rcfg);
				
	//InitHisto( );
		
	//enable buttons after DIGI initialisation
	fClearButton->SetState (kButtonUp);
	fStartButton->SetState (kButtonUp);
	fStopButton->SetState (kButtonUp);
		
	for (int i=0; i<9; i++)	
		fC[i]->SetState(i > 5 ? kButtonUp : kButtonDisabled);
	
	
		//fC[0]->SetState(kButtonDown); //BL_CUT ON
		//fC[1]->SetState(kButtonDown); //TRACES ON	
	fC[6]->SetState(kButtonDown); //Charge ON	
		
	fInitButton->SetState(kButtonDisabled);
}

void MainFrame::ClearHisto()
{
	for (int i = 0; i < QDC_CH; i++)	{	
		Histo.QDC_L[i]->Reset("ICESM");
		Histo.QDC_H[i]->Reset("ICESM");
	}	
	
	for (int i = 0; i < TDC_CH; i++)	
		Histo.TDC[i]->Reset("ICESM");
	
	
	
	//for (int i = 0; i < SADC_CH; i++){		
	//	Histo.trace[i]->Reset("ICESM");
	//	Histo.ampl[i]->Reset("ICESM");
	//	Histo.integral[i]->Reset("ICESM");
	//}
	
	//Histo.psd_ampl->Reset("ICESM");
	//Histo.int_ampl->Reset("ICESM");
	
	printf("ClearHisto \n");
	Rcfg.Nev = 0;
	Rcfg.TrgCnt = 0;
	Rcfg.can->Update( );
	
	Rcfg.StartTime = get_time( );
	
}

void MainFrame::StartButton()
{	
	
 	bool fStart = true;
	Rcfg.StartTime = get_time( );
	printf("Start button \n");

	/*
	//Store traces if choosen
	if (Rcfg.fStoreTraces){
				
		int retval;
		// check if such file exist
		
		if( !gSystem->AccessPathName( fSTTextBuffer->GetString( ) ) ){ //such file exist
			sprintf(CName, "File  %s exist. \n It will be overwritten. \n Continue?", fSTTextBuffer->GetString( ));
			new TGMsgBox(gClient->GetRoot(), fMain, "Warning", CName, kMBIconExclamation, kMBNo | kMBYes, &retval); //1 - Yes, No -2 // strange logic
			printf("retval %d \n", retval);
			retval == 2 ? fStart = false: fStart = true; 
			
			if (fStart){
				sprintf(CName, "Traces will be saved in  \n %s", fSTTextBuffer->GetString( ));
				new TGMsgBox(gClient->GetRoot(), fMain, "Info", CName, kMBIconAsterisk, kMBOk);
				Rcfg.ff = TFile::Open(fSTTextBuffer->GetString(),"RECREATE");		
				//ff = new TFile(fSTTextBuffer->GetString(),"WRITE");		
				Rcfg.tree = new TTree("vtree", "vtree");		
				Rcfg.tree->Branch("EventCounter", &Histo.EC);
				Rcfg.tree->Branch("TimeStamp", &Histo.TTT);
				Rcfg.tree->Branch("Trace", &Histo.v_out);	 
			}
		} 
		else{
			sprintf(CName, "Traces will be saved in  \n %s", fSTTextBuffer->GetString( ));
			new TGMsgBox(gClient->GetRoot(), fMain, "Info", CName, kMBIconAsterisk, kMBOk);
			Rcfg.ff = TFile::Open(fSTTextBuffer->GetString(),"RECREATE");		
				//ff = new TFile(fSTTextBuffer->GetString(),"WRITE");		
			Rcfg.tree = new TTree("vtree", "vtree");		
			Rcfg.tree->Branch("EventCounter", &Histo.EC);
			Rcfg.tree->Branch("TimeStamp", &Histo.TTT);
			Rcfg.tree->Branch("Trace", &Histo.v_out);	 
		}
	}
	*/
	
	Rcfg.loop = 1;

}

void MainFrame::StopButton()
{	
	printf("Stop button \n");
	
	Rcfg.loop = 0;

	if (Rcfg.fStoreTraces){
		Rcfg.tree->Write();
		Rcfg.ff->Write();
		printf("Data saved as \"%s\"\n", Rcfg.ff->GetName());
	}
}




void MainFrame::HandleMenu(Int_t id)
{
   
	const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            "Text files",    "*.[tT][xX][tT]",
                            0,               0 };	
	switch (id) {
		
		case M_FILE_OPEN:
        	/*{
          	static TString dir(".");
          	TGFileInfo fi;
          	fi.fFileTypes = filetypes;
          	fi.fIniDir    = StrDup(dir);
          	//printf("fIniDir = %s\n", fi.fIniDir);
          	new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
          	printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
         	dir = fi.fIniDir;
        	}*/
			new TGMsgBox(gClient->GetRoot(), fMain, "Info", "It'll be added it nearest future.", kMBIconAsterisk, kMBOk);
        	break;
			
      	case M_FILE_SAVE_AMPL_TXT:
        	{
           	static TString dir(".");
        	TGFileInfo fi;
            fi.fFileTypes = filetypes;
           	fi.fIniDir    = StrDup(dir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
			
			FILE *outtxt = fopen(fi.fFilename,"RECREATE");	
			for (int i = 0; i<Histo.ampl[0]->GetNbinsX( ); i++)
				fprintf(outtxt, "%i %f \n", i, Histo.ampl[0]->GetBinContent(i) );
				
            printf("File saved as: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
            }
         	break;

   		case M_FILE_SAVE_HISTO:
		{			 
		 	static TString dir(".");
            TGFileInfo fi;
            fi.fFileTypes = filetypes;
            fi.fIniDir    = StrDup(dir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
                      
		 	TFile *outfile = new TFile(fi.fFilename,"RECREATE");
			Rcfg.can->Write("can");
			
			for (int i=0; i<QDC_CH; i++){
				Histo.QDC_L[i]->Write(Histo.QDC_L[i]->GetTitle());
				Histo.QDC_H[i]->Write(Histo.QDC_H[i]->GetTitle());
			}	
			
			for (int i=0; i<TDC_CH; i++)
				Histo.TDC[i]->Write(Histo.TDC[i]->GetTitle());
				
			//for (int i=0; i<SADC_CH; i++){
			//	Histo.ampl[i]->Write(Histo.ampl[i]->GetTitle());
			//	Histo.integral[i]->Write(Histo.integral[i]->GetTitle());
			//}
			//Histo.int_ampl->Write(Histo.int_ampl->GetTitle());
			//Histo.psd_ampl->Write(Histo.psd_ampl->GetTitle());
						
			outfile->Write(); 
         	printf("File saved - %s \n",fi.fFilename);			 
		 }
         break;
	
   	case M_FILE_EXIT:
        CloseWindow();   
        break;
	 
	case M_OPT_MENU:
        new OptMenu(gClient->GetRoot(), fMain, 400, 200);
		break; 	 
		
	//case M_PARAMS_MENU:
    //   new ParamsMenu(gClient->GetRoot(), fMain, 400, 400);
	//	break; 	

	//case M_CALIB_MENU:
    //    new CalibMenu(gClient->GetRoot(), fMain, 400, 400);
	//	break; 			
	
	case M_MANUAL:
		new TGMsgBox(gClient->GetRoot(), fMain, "Manual", "After a while it will be added \n but noone knows value of while \n because it's a loop", kMBIconAsterisk, kMBOk);
        break;
	
	case M_HELP_ABOUT:
		new TGMsgBox(gClient->GetRoot(), fMain, "About program", "Handmade spectra and waveform reader \n for CAEN DT5770", kMBIconAsterisk, kMBOk);
        break;
 
      default:
         printf("Menu item %d selected\n", id);
         break;
   }
}


