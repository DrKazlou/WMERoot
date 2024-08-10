//#include <stdio.h>
//#include <stdlib.h>



#include "WMEFunc.h"

#include <TSystem.h>
#include "TH1D.h"
#include "TROOT.h"
#include "TGMsgBox.h"


extern ReadoutConfig_t Rcfg;
extern Histograms_t Histo;

using namespace std;

int revpos(const char s[], char c){
    for (int i = strlen(s)-1; i>=0; --i)
        if (s[i] == c)
            return i;
    return -1;
}

long get_time()
{
    long time_ms;
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;

    return time_ms;
}

int StartTCP( ){
	
	int sockfd, connfd;
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
    servaddr.sin_addr.s_addr = inet_addr("134.176.17.136");
    servaddr.sin_port = htons(PORT);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))!= 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
 
    	
	char buff[MAX];
	//int msg_len = 0;
	//bool msg_correct = false;
    bzero(buff, sizeof(buff));
	sprintf(buff, "First message to check");
	printf("Try to write via TCP buff: %s\n", buff);
	write(sockfd, buff, sizeof(buff));
	
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	printf("From Server : %s", buff);
		 
	//bzero(buff, sizeof(buff));
	sprintf(buff, "start");
	printf("Sending ack: (%s)\n", buff);
	write(sockfd, buff, sizeof(buff));
	
	return sockfd;
}

void CalcRate(uint64_t &PrevRateTime){
	char CName[100];
	uint64_t CurrentTime, ElapsedTime, DAQTime;
	
	CurrentTime = get_time( );
    ElapsedTime = CurrentTime - PrevRateTime;
	DAQTime = (CurrentTime - Rcfg.StartTime) / 1000;
			
		if (Rcfg.fTimer && DAQTime > Rcfg.timer_val)
			Rcfg.loop = 0;
					        	
		if (ElapsedTime > 1000) { 
			sprintf(CName,"T: %li s",  DAQTime );
			Rcfg.TLabel->SetText(CName);
			gSystem->ProcessEvents(); 
				
           	if (Rcfg.TrgCnt != 0){
				sprintf(CName," TrgCnt. %.2f Hz ", (float)Rcfg.TrgCnt*1000.f/(float)ElapsedTime );
				Rcfg.StatusBar->SetText(CName, 0);
			}
			else{
				sprintf(CName, "No data...");
				Rcfg.StatusBar->SetText(CName, 0);
			}
			
			//for (int i=0; i<4; i++){
			//	sprintf(CName, "Tr[%i] %f", i, Histo.trace[i]->GetBinContent(1366) );
			//	Rcfg.StatusBar->SetText(CName, i+1);
			//}
			
			Rcfg.TrgCnt = 0;
	                       		
        PrevRateTime = CurrentTime;
	   	}
	
}

void SaveAfterError(TCanvas *can){
	
	char FName[50];
	
	struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
	sprintf(FName,"EmergencyFile_%li.root",tv.tv_sec);
	
	//emergency file
	TFile *emfile = new TFile(FName,"RECREATE");
	can->Write("can");
	emfile->Write(); 
    printf("Emergency File saved - %s \n",FName);			 
	
}


void InitReadoutConfig(ReadoutConfig_t *Rcfg){
	
	Rcfg->fPrint = false; // common print flag for debug
	Rcfg->fStoreTraces = false; // flag to store traces
	Rcfg->fTimer = false; // flag for timer usage
	
	Rcfg->loop = -1; // ReadoutLoop flag
	Rcfg->DrawTime = 0.5; // time between histograms drawing in sec
	Rcfg->TrgCnt = 0;
	
}


void InitHisto( Histograms_t *Histo){
	char CName[30];
	Histo->PSD_BIN = 2;
	sprintf(Histo->h2Style, "%s", (char*)"");
	//Histo->k = 1;
	//Histo->b = 0;
	//sprintf(Histo->fFormula, "%s", (char*)"            Hr wll b smthng         ");
	//Histo->fCalib = false;
	
	Histo->WF_XMIN = 0; Histo->WF_XMAX = n_sadc * b_width;   // change on some variable, initially 500 samples
	Histo->WF_YMIN = -500; Histo->WF_YMAX = 40000;
	
	Histo->ALBound = 0, Histo->ARBound = n_sadc * b_width; 
	Histo->ILBound = 0, Histo->IRBound = n_sadc * b_width; 
	
	Histo->NPad = 1;
	Histo->BL_CUT = 20;
	Histo->CH_2D = 3;
	//Histo->fBL = true;
	//Histo->fTrace = true;
	Histo->fCharge = true;
	
	Histo->fQTypeHigh = true;
	Histo->fQTypeLow = false;
	
	sprintf(Histo->h2Style, "COLZ");
	
	for (int i = 0; i < QDC_CH; i++){ //16
		Histo->fDrawQDC[i] = true;
		sprintf(CName, "h_QDC_L%i", i);
		Histo->QDC_L[i] = new TH1D(CName, CName, 4096, 0, 4096); // 2^12 bit QDC  4096 as maximum
		sprintf(CName, "h_QDC_H%i", i);
		Histo->QDC_H[i] = new TH1D(CName, CName, 4096, 0, 4096); // 2^12 bit QDC  4096 as maximum
	}
	
	for (int i = 0; i < TDC_CH; i++){ //32
		Histo->fDrawTDC[i] = false;
		sprintf(CName, "h_TDC%i", i);
		Histo->TDC[i] = new TH1D(CName, CName, 4096, 0, 4096); // all 32 will be added for complite code
	}
	
	
	//for (int i = 0; i < SADC_CH; i++){ //8
		//Histo->fDraw[i] = true;
		//sprintf(CName, "h_trace%i", i);
		//Histo->trace[i]= new TH1D(CName, CName, n_sadc, 0, n_sadc * b_width); // ns 500 samples for trace 1 bin = 10 ns, 5 mks whole trace
		
		//sprintf(CName, "h_ampl%i", i);
		//Histo->ampl[i] = new TH1D(CName, CName, 1000, 0, 16384); // 2^16? bit ADC  16384*4 as maximum
		
		//sprintf(CName, "h_integral%i", i);
		//Histo->integral[i] = new TH1D(CName, CName, 1000, 0, 100000); // 1000 0 20000
	//}
	
	//Histo->psd_ampl = new TH2D("h_psd_ampl", "h_psd_ampl", 1000, 0, 16384, 1000, 0, 1);
	//Histo->int_ampl = new TH2D("h_int_ampl", "h_int_ampl", 1000, 0, 16384, 1000, 0, 100000);
	

	//Histo.psd_ampl = new TH2D("h_psd_ampl", "h_psd_ampl", Histo.psd_ampl_set.xbins, Histo.psd_ampl_set.xmin, Histo.psd_ampl_set.xmax, // 1000 0 2000
	//						  							  Histo.psd_ampl_set.ybins, Histo.psd_ampl_set.ymin, Histo.psd_ampl_set.ymax); // 1000 -2 2
	
	//Histo.int_ampl = new TH2D("h_int_ampl", "h_int_ampl", Histo.ampl_set.xbins, Histo.ampl_set.xmin, Histo.ampl_set.xmax, // 1000 0 2000
	//						  							  Histo.integral_set.xbins, Histo.integral_set.xmin, Histo.integral_set.xmax); // 1000 -2 2

}

void InitVME(ReadoutConfig_t *Rcfg ){
	char CName[100];
	
	Rcfg->sockfd = StartTCP( );
	sprintf(CName, "VME rack seems \n to be connected successfully \n");
	new TGMsgBox(gClient->GetRoot(), Rcfg->main, "Info", CName, kMBIconAsterisk, kMBOk);
	
}

void FillHisto(Histograms_t *Histo, char MSG[MAX]){
	
	char sadc_str[5], trace_hex[16000], sub_tdc[10], sub_qdc[10], evt_str[10], tdc_str[200], qdc_str[200];
	int ch_start;
	
	Double_t BL_mean, integral, psd_val, Qs, Ql;
	int m_stamp, ampl; 
	
	Int_t p = -1; //: -1; //POLARITY for future use -1 - NEGATIVE; +1 - POSITIVE
	
	//vector <vector <int>> vec; 
	//vector <int> v_sub{}, v_raw{};
	
		
	bzero(evt_str, sizeof(evt_str)); //Event number
	bzero(tdc_str, sizeof(tdc_str)); // TDC
	bzero(qdc_str, sizeof(qdc_str)); // QDC 
	//bzero(sadc_str, sizeof(sadc_str)); // SADC ampl val
		
	strncpy(evt_str, MSG+strcspn(MSG,":") + 1, strcspn(MSG,";") - strcspn(MSG,":") - 1); // Event number
	strncpy(tdc_str, MSG+strcspn(MSG,"(") + 1, strcspn(MSG,")") - strcspn(MSG,"(") - 1); // TDC(FFA...ABC) return number between brackets
	strncpy(qdc_str, MSG+strcspn(MSG,"[") + 1, strcspn(MSG,"]") - strcspn(MSG,"[") - 1); // QDC[FFA...ABC] return number between brackets
	
	//printf("Evt %s TDC(%li)  evt_len(%li) qdc_len(%li) \n", evt_str, strtol(sub_tdc, 0, 16), strlen(evt_str), strlen(tdc_str));		
			
	int s_pos = 3 * Histo->CH_2D; //3 * ch
	
	sprintf(sub_tdc,"%c%c%c", tdc_str[s_pos], tdc_str[s_pos + 1], tdc_str[s_pos + 2]);
	sprintf(sub_qdc,"%c%c%c", qdc_str[s_pos], qdc_str[s_pos + 1], qdc_str[s_pos + 2]);

		
	printf("\rServer : Evt %s TDC (%li) QDC[%li]", evt_str, strtol(sub_tdc, 0, 16), strtol(sub_qdc, 0, 16));
	fflush(stdout);
			
		
	for (int ch =0; ch<QDC_CH; ch++){
		s_pos = 3 * ch; //3 * ch low range first half
		sprintf(sub_qdc,"%c%c%c", qdc_str[s_pos], qdc_str[s_pos + 1], qdc_str[s_pos + 2]);
		Histo->QDC_L[ch]->Fill(strtol(sub_qdc, 0, 16));
		
		s_pos = 3 * ch + 48; //3 * ch high range second half
		sprintf(sub_qdc,"%c%c%c", qdc_str[s_pos], qdc_str[s_pos + 1], qdc_str[s_pos + 2]);
		Histo->QDC_H[ch]->Fill(strtol(sub_qdc, 0, 16));
	}	
	
	for (int ch =0; ch<TDC_CH; ch++){
		s_pos = 3 * ch; //3 * ch 
		sprintf(sub_tdc,"%c%c%c", tdc_str[s_pos], tdc_str[s_pos + 1], tdc_str[s_pos + 2]);
		Histo->TDC[ch]->Fill(strtol(sub_tdc, 0, 16));
	}
	
	
	//part for SADC boards
	//bzero(trace_hex, sizeof(trace_hex));
	//strncpy(trace_hex, MSG+strcspn(MSG,"{") + 1, strcspn(MSG,"}") - strcspn(MSG,"{") - 1); // W{2000hexsimbols}  return number between brackets
	/*			
	for (int ch =0; ch<SADC_CH; ch++){
		Histo->trace[ch]->Reset("ICESM");
		ch_start = ch * n_sadc * 4; // 4 char simbols for every sample [500 of them]
		
		BL_mean = 0; ampl = 0; integral = 0; psd_val = 0; Qs = 0; Ql = 0;
		
		for(int i=0;i<n_sadc; i++){
			sprintf(sub,"%c%c%c%c",trace_hex[4*i+ch_start], trace_hex[4*i+1+ch_start], trace_hex[4*i+2+ch_start], trace_hex[4*i+3+ch_start]);
			v_raw.push_back(strtol(sub, 0, 16));
			//Histo->trace[ch]->Fill(i * b_width, strtol(sub, 0, 16));
		}
		
		for ( int j=0; j<Histo->BL_CUT; j++)
			BL_mean += v_raw[j];	
		BL_mean /= Histo->BL_CUT;	
		
		for ( int j=0; j<v_raw.size( ); j++){
			v_sub.push_back(v_raw[j] - BL_mean);
						
			if (v_sub[j] * p > ampl && j * b_width > Histo->ALBound && j * b_width < Histo->ARBound){
				ampl = v_sub[j] * p;
				m_stamp = j;
			}	
			
			if (j * b_width > Histo->ILBound && j * b_width < Histo->IRBound)
				integral += v_sub[j] * p;
						
			Histo->trace[ch]->Fill(j * b_width, Histo->fBL ? v_sub[j] : v_raw[j]);
		}
		
		Histo->integral[ch]->Fill(integral);
		Histo->ampl[ch]->Fill(ampl);
		
		//PSD
		if ( ( (Histo->fPSD_ampl) || (Histo->fPSD_int) )  && (ch == Histo->CH_2D) ){
			for (int j=m_stamp; j<v_sub.size( ); j++){
				if (j<(m_stamp + Histo->PSD_BIN) )
					Qs = Qs + p * v_sub[j];
				Ql = Ql + p * v_sub[j];
			}
			psd_val = 1 - ( Qs/Ql );
			
			if (Histo->fPSD_ampl) 
				Histo->psd_ampl->Fill(ampl, psd_val);
			//if (Histo->fPSD_int) 
			//	Histo->psd_int->Fill(integral, psd_val);	
			
			//if (Histo->fQsl) 
			//	Histo->qs_ql->Fill(Ql, Qs);	
		}
		//PSD
		
		if (Histo->fIA && ch == Histo->CH_2D)
			Histo->int_ampl->Fill(ampl, integral);
		
		//printf("CH[%i]: %f", ch, Histo->trace[ch]->GetBinContent(250));
		
		//Histo->vec.push_back(v_raw); // for future use in trace storage
		v_raw.clear( ); v_sub.clear( );
	} // channels circle
	*/
			
}


void DrawHisto( Histograms_t Histo, TCanvas *can){
	//Color_t color[SADC_CH] = {kBlue, kRed, kGreen, kMagenta, kOrange - 3, kGray, kYellow + 3, kBlack}; 	
	Color_t color[TDC_CH] = {kBlue, kRed, kGreen, kMagenta, kOrange - 3, kGray, kYellow + 3, kBlack, 
												kBlue, kRed, kGreen, kMagenta, kOrange - 3, kGray, kYellow + 3, kBlack,
												kBlue, kRed, kGreen, kMagenta, kOrange - 3, kGray, kYellow + 3, kBlack,
												kBlue, kRed, kGreen, kMagenta, kOrange - 3, kGray, kYellow + 3, kBlack}; 	
	
	//int FirstToDraw = 0;
	//printf("DrawHisto\n");
	
	
	if (Histo.fCharge){
		can->cd(Histo.cCharge);
		for (int j = 0; j<QDC_CH; j++){
			if (Histo.fDrawQDC[j]){
				
				if (Histo.fQTypeLow && !Histo.fQTypeHigh){
					Histo.QDC_L[j]->SetLineColor(color[j]);
					Histo.QDC_L[j]->Draw(j == Histo.FtoDraw_QDC ? "HIST" : "HIST SAME");
				}	
				if (Histo.fQTypeHigh && !Histo.fQTypeLow){
					Histo.QDC_H[j]->SetLineColor(color[j]);
					Histo.QDC_H[j]->Draw(j == Histo.FtoDraw_QDC ? "HIST" : "HIST SAME");
				}	
				
				if (Histo.fQTypeLow && Histo.fQTypeHigh){
					Histo.QDC_L[j]->SetLineColor(color[j]);
					Histo.QDC_H[j]->SetLineColor(color[j]);
					Histo.QDC_L[j]->Draw(j == Histo.FtoDraw_QDC ? "HIST" : "HIST SAME");
					Histo.QDC_H[j]->Draw("HIST SAME");
				}
			}
		}
	}
	
	if (Histo.fTime){
		can->cd(Histo.cTime);
		for (int j = 0; j<TDC_CH; j++){
			if (Histo.fDrawTDC[j]){
				Histo.TDC[j]->SetLineColor(color[j]);
				Histo.TDC[j]->Draw(j == Histo.FtoDraw_TDC ? "HIST" : "HIST SAME");
			}	
		}
	}
	
	
	//Part for SADC 
	
	/*
	if (Histo.fTrace){
		can->cd(1);
		for (int j = 0; j<SADC_CH; j++){
			if (Histo.fDraw[j]){
				Histo.trace[j]->SetLineColor(color[j]);
				if (j == Histo.FirstToDraw){
					Histo.trace[j]->Draw("HIST");
					Histo.trace[j]->GetYaxis()->SetRangeUser(Histo.WF_YMIN, Histo.WF_YMAX);
					Histo.trace[j]->GetYaxis()->SetTitleOffset(1.1);
					Histo.trace[j]->GetYaxis()->SetTitle("ADC, lsb");
					Histo.trace[j]->GetXaxis()->SetRangeUser(Histo.WF_XMIN, Histo.WF_XMAX);
					Histo.trace[j]->GetXaxis()->SetTitle(b_width == 1 ? " Bins, b_width = 10 ns" : " Time, ns"); // Time, ns
				}	
				else 
					Histo.trace[j]->Draw("HIST SAME");
			}
		}
	}
	
	
	if (Histo.fAmpl){
		can->cd(Histo.cAmpl);
		for (int j = 0; j<SADC_CH; j++){
			if (Histo.fDraw[j]){
				Histo.ampl[j]->SetLineColor(color[j]);
				Histo.ampl[j]->Draw(j == Histo.FirstToDraw ? "HIST" : "HIST SAME");
			}
		}
	}
		
		
	if (Histo.fInt){		
		can->cd(Histo.cInt);
		for (int j = 0; j<SADC_CH; j++){
			if (Histo.fDraw[j]){
				Histo.integral[j]->SetLineColor(color[j]);
				Histo.integral[j]->Draw(j == Histo.FirstToDraw ? "HIST" : "HIST SAME");
			}
		}
	}
	
	
	if (Histo.fIA){
		can->cd(Histo.cIA);
		Histo.int_ampl->SetMarkerStyle(21);
		Histo.int_ampl->SetMarkerSize(0.4);
		Histo.int_ampl->SetMarkerColor(kBlue);
		Histo.int_ampl->Draw(Histo.h2Style);
	}		
	
	if (Histo.fPSD_ampl){
		can->cd(Histo.cPSD_ampl);
		Histo.psd_ampl->SetMarkerStyle(21);
		Histo.psd_ampl->SetMarkerSize(0.4);
		Histo.psd_ampl->SetMarkerColor(kBlue);
		Histo.psd_ampl->Draw(Histo.h2Style); //COLZ
	}		
*/
	
	can->Update();
	
}


void ReadoutLoop( ){ 
	
	uint64_t PrevRateTime = get_time( ), PrevDrawTime;
		
	Rcfg.Nev = 0;
	Rcfg.TrgCnt = 0;
	
	char buff[MAX], MSG[MAX]; 
	int msg_len = 0, min_msg = 213, max_msg = 223; // 223 for 10 num Evt
	bool msg_correct = false;

	while(Rcfg.loop == 1)	{
				
		CalcRate(PrevRateTime);		
				
		bzero(buff, sizeof(buff));
		read(Rcfg.sockfd, buff, sizeof(buff));
		
		msg_correct = false;
		msg_len += strlen(buff);
		
		//printf("Got MSG (%li) [%i]\n", strlen(buff), msg_len);
		
		//if (revpos(MSG,'}')!=-1)
		//	printf("END inside pos(%i) \n",revpos(MSG,'}'));
		
		if (msg_len > max_msg) // helps crashes after stop/start due to stacked of  previous MSG "in TCP"
		{
			//printf("WTF?! \n");
			msg_len = 0;
			bzero(MSG, MAX); //16080
		}
		else{
			strcat(MSG, buff);
		
			if(msg_len > min_msg){ // 160 | 16000
				//printf("MSG last smbl  %c\n", MSG[msg_len-1]);
				
				if (revpos(MSG,'}') == msg_len-1){
					//printf("\t MSG finished \n");	
					msg_correct = true;
				}	
				else{
					//printf("Pos } = %i \n FULLMSG: (%i) \n", revpos(MSG,'}'), msg_len);
					msg_correct = false;
					bzero(MSG, MAX); //16080
				}	
					
				msg_len = 0;
			}
		}
	//	
		if (msg_correct){
			//printf("\rReady to draw...");
			//fflush(stdout);
			Rcfg.TrgCnt++;
				
			FillHisto(&Histo, MSG);
	
			bzero(MSG, MAX); //16080
			
			if ( (get_time( ) - PrevDrawTime) >= Rcfg.DrawTime*1000){ // sec*1000 = ms  
				DrawHisto(Histo, Rcfg.can);
				PrevDrawTime = get_time( );
			}
		}
				
		gSystem->ProcessEvents( ); 
	} // inf loop


}



void DataAcquisition( ){
		
	while(1) {
		gSystem->ProcessEvents(); 
		if (Rcfg.loop == 1){
						
			ReadoutLoop( );
		}
		
		if (Rcfg.loop == 0){
			printf("Acquisition stopped \n");
			Rcfg.loop = -1;
		}
	}
	
}