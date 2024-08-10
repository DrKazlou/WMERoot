set setup(no_of_boards) 2

set setup(enable_dma) 0
set setup(no_save_histo) 1
set setup(no_show_data) 1
set setup(LAM) 1

;# 11=NTEC, 21=Veto, 30=775TDC_N, 31=775TDC, 40=785ADC_N, 41=785ADC
;# 50=792QDC_N, 51=792QDC, 60=965_DRQDC_A, 61=965_DRQDC, 89=IOL, 90=SIS3820, 110=ledv895 
;# 160=sis3302 
set setup(board1_what) 89
set setup(board2_what) 160

set iol_1(cardbase) 0x01000000 
set iol_1(geo) 4 
set iol_1(crate) 1 

set iol_1(ctrl) 0x0 
set iol_1(bset2) 0x400 
set iol_1(clrtime) 0x40 
set iol_1(fclrwin) 0x010 

set sis3302_2(cardbase) 0x06000000 
set sis3302_2(red_addr_space) 1 
set sis3302_2(clock) 100 
set sis3302_2(geo) 16 
set sis3302_2(crate) 1 
set sis3302_2(stopdelay) 300 
set sis3302_2(no_of_samples) 500 
set sis3302_2(ped_range_beginning) 10 
set sis3302_2(ped_range_end) 0 
set sis3302_2(hex) 0 
set sis3302_2(threshold00) 0x06004E20
set sis3302_2(threshold01) 0x060061a8
set sis3302_2(threshold02) 0x060061a8
set sis3302_2(threshold03) 0x060060e0
set sis3302_2(threshold04) 0x060062d4
set sis3302_2(threshold05) 0x0600623e
set sis3302_2(threshold06) 0x0600620c
set sis3302_2(threshold07) 0x06006270
set sis3302_2(trigger00) 0x000f0000 
set sis3302_2(trigger01) 0x000f0000 
set sis3302_2(trigger02) 0x000f0000 
set sis3302_2(trigger03) 0x000f0000 
set sis3302_2(trigger04) 0x000f0000 
set sis3302_2(trigger05) 0x000f0000 
set sis3302_2(trigger06) 0x000f0000 
set sis3302_2(trigger07) 0x000f0000 
set sis3302_2(dac00) 0x7fff 
set sis3302_2(dac01) 0x7fff 
set sis3302_2(dac02) 0x7fff 
set sis3302_2(dac03) 0x7fff 
set sis3302_2(dac04) 0x7fff 
set sis3302_2(dac05) 0x7fff 
set sis3302_2(dac06) 0x7fff 
set sis3302_2(dac07) 0x7fff 
