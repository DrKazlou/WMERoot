set setup(no_of_boards) 3

set setup(enable_dma) 0
set setup(no_save_histo) 1
set setup(no_show_data) 1
set setup(LAM) 1

;# 11=NTEC, 21=Veto, 30=775TDC_N, 31=775TDC, 40=785ADC_N, 41=785ADC
;# 50=792QDC_N, 51=792QDC, 60=965_DRQDC_A, 61=965_DRQDC, 89=IOL, 90=SIS3820, 110=ledv895 
;# 160=sis3302 
set setup(board1_what) 89
set setup(board2_what) 31
set setup(board3_what) 160

set iol_1(cardbase) 0x01000000 
set iol_1(geo) 4 
set iol_1(crate) 1 

set iol_1(ctrl) 0x0 
set iol_1(bset2) 0x400 
set iol_1(clrtime) 0x40 
set iol_1(fclrwin) 0x010 

set v775_2(cardbase) 0x03000000 
set v775_2(geo) 8 
set v775_2(vset) 53 
set v775_2(ctrl) 0x0 
set v775_2(bset2) 0x1000 
set v775_2(clrtime) 0x40 
set v775_2(fclrwin) 0x10 
set v775_2(crate) 1 
set v775_2(threshold00) 0x02 
set v775_2(threshold01) 0x02 
set v775_2(threshold02) 0x02 
set v775_2(threshold03) 0x02 
set v775_2(threshold04) 0x02 
set v775_2(threshold05) 0x02 
set v775_2(threshold06) 0x02 
set v775_2(threshold07) 0x02 
set v775_2(threshold08) 0x02 
set v775_2(threshold09) 0x02 
set v775_2(threshold10) 0x02 
set v775_2(threshold11) 0x02 
set v775_2(threshold12) 0x02 
set v775_2(threshold13) 0x02 
set v775_2(threshold14) 0x02 
set v775_2(threshold15) 0x02 
set v775_2(threshold16) 0x02 
set v775_2(threshold17) 0x02 
set v775_2(threshold18) 0x02 
set v775_2(threshold19) 0x02 
set v775_2(threshold20) 0x02 
set v775_2(threshold21) 0x02 
set v775_2(threshold22) 0x02 
set v775_2(threshold23) 0x02 
set v775_2(threshold24) 0x02 
set v775_2(threshold25) 0x02 
set v775_2(threshold26) 0x02 
set v775_2(threshold27) 0x02 
set v775_2(threshold28) 0x02 
set v775_2(threshold29) 0x02 
set v775_2(threshold30) 0x02 
set v775_2(threshold31) 0x02

set sis3302_3(cardbase) 0x06000000 
set sis3302_3(red_addr_space) 1 
set sis3302_3(clock) 100 
set sis3302_3(geo) 16 
set sis3302_3(crate) 1 
set sis3302_3(stopdelay) 300 
set sis3302_3(no_of_samples) 500 
set sis3302_3(ped_range_beginning) 10 
set sis3302_3(ped_range_end) 0 
set sis3302_3(hex) 0 
set sis3302_3(threshold00) 0x06004E20
set sis3302_3(threshold01) 0x060061a8
set sis3302_3(threshold02) 0x060061a8
set sis3302_3(threshold03) 0x060060e0
set sis3302_3(threshold04) 0x060062d4
set sis3302_3(threshold05) 0x0600623e
set sis3302_3(threshold06) 0x0600620c
set sis3302_3(threshold07) 0x06006270
set sis3302_3(trigger00) 0x000f0000 
set sis3302_3(trigger01) 0x000f0000 
set sis3302_3(trigger02) 0x000f0000 
set sis3302_3(trigger03) 0x000f0000 
set sis3302_3(trigger04) 0x000f0000 
set sis3302_3(trigger05) 0x000f0000 
set sis3302_3(trigger06) 0x000f0000 
set sis3302_3(trigger07) 0x000f0000 
set sis3302_3(dac00) 0x7fff 
set sis3302_3(dac01) 0x7fff 
set sis3302_3(dac02) 0x7fff 
set sis3302_3(dac03) 0x7fff 
set sis3302_3(dac04) 0x7fff 
set sis3302_3(dac05) 0x7fff 
set sis3302_3(dac06) 0x7fff 
set sis3302_3(dac07) 0x7fff 
