#include <TWELITE>
#include "Interrupt.hpp" // ビヘイビア定義ファイル
#include "lcd.hpp"
#include "LED.hpp"
#include "communication.hpp"
#include "main.hpp"

extern lcd lcd_control;
bool communication_status = false;
LED_color led_color = RED;
LED_flash led_flash = ON;

uint8_t timer3_count = 0;
bool release_flg = false;

//ビヘイビアに関する参考はここ
//https://mwx.twelite.info/api-reference/behavior

/*****************************************************************/
// MUST DEFINE CLASS NAME HERE
#define __MWX_APP_CLASS_NAME INTERRUPT
#include "_mwx_cbs_cpphead.hpp" // 冒頭の定義
/*****************************************************************/


void INTERRUPT::transmit_complete(mwx::packet_ev_tx& evTx) {
    //Serial << "Send_complete" << crlf;
}

MWX_DIO_INT(PIN_DIGITAL::DIO18,uint32_t arg, uint8_t& handled){ //停止ボタンが押されたときに割り込み動作を行う。

    current_mode = EMERGENCY_MAIN;//自身がボタンを押した場合は、EMERGENCY_MAINモードに入る

    Serial << "Set to EM main mode" << crlf;
    detachIntDio(EM_SW);

    handled = false;
}

MWX_DIO_EVENT(PIN_DIGITAL::DIO18,uint32_t arg){

    vTransmit(0x0,0x12,10);
}


MWX_TIMER_INT(0, uint32_t arg, uint8_t& handled){
    //本来は高速で復帰する必要のある割り込み動作にLCDの更新という重い動作を入れるのはよろしくないが、
    // 時間的正確性がそこまで重要視されるものでもないので利便性のためにここで処理する。

    after_release_count ++;


    LCD_Timer_Control();

    if(communication_established_check_flg != true){
        lcd_control.printAntennaBars(0);
        sprintf(lcd_data_buf1,"????");
        return;
    } 

    if(lqi_data > 150){
        lcd_control.printAntennaBars(3);
    }else if(lqi_data > 50){
        lcd_control.printAntennaBars(2);
    }else{
        lcd_control.printAntennaBars(1);
    }


    if(rx_status & 0b00000010){   //SEの状況
        lcd_data_buf1[0] = 'o';
    }else{
        lcd_data_buf1[0] = 0xF7;
    }

    if(rx_status & 0b00000100){   //HEの状況
        lcd_data_buf1[1] = 'o';
    }else{
        lcd_data_buf1[1] = 0xF7;
    }

    if(rx_status & 0b00001000){   //WEの状況
        lcd_data_buf1[2] = 'o';
    }else{
        lcd_data_buf1[2] = 0xF7;
    }

    if(rx_status & 0b00000001){   //OUTPUTの状況
        lcd_data_buf1[3] = 'o';
    }else{
        lcd_data_buf1[3] = 0xF7;
    }

    handled = true;
}


MWX_TIMER_INT(1, uint32_t arg, uint8_t& handled){
    
	//Serial << format("%d",current_mode) << crlf;

    Timer_LED_Control(led_color,led_flash);
    handled = true;
}

MWX_TIMER_INT(2, uint32_t arg, uint8_t& handled){   //疎通確認用のタイマ　2Hzで動作し、疎通確認用フラグを折る。これが一度呼び出されてから次に呼び出されるまでの0.5秒間に一度でも受信があればフラグが立ち、疎通確認が取れる。
    
    if((rx_status & 1<1) && (current_mode == NORMAL)){ //自身がノーマルモードの時に、他の子機によって緊急停止した親機からの信号を受信した場合、EMERGENCY_SUBモードに入る
        current_mode = EMERGENCY_SUB;
    }
    
    
    if(communication_established_check_flg == true){
        communication_status = true;
        led_color = GREEN;

        int volt_1[2];
        int volt_2[2];

        volt_1[0] = int(10*(double(rx_volt1)/1023.0*2.4 *11))/10; //整数部
        volt_1[1] = int(10*(double(rx_volt1)/1023.0*2.4 *11))%10; //小数部
        volt_2[0] = int(10*(double(rx_volt2)/1023.0*2.4 *11))/10; //整数部
        volt_2[1] = int(10*(double(rx_volt2)/1023.0*2.4 *11))%10; //小数部
        sprintf(lcd_data_buf2,"V1: %d.%01dV  V2: %d.%01dV",volt_1[0],volt_1[1],volt_2[0],volt_2[1]);

    }else{
        communication_status = false;
        led_color = RED;
        lqi_data = 0;
        rx_status = 0x00;   //有効なステータスの場合、567ビット目が1なのでこのコマンドはありえない。
        rx_volt1 = 0x0000;
        rx_volt2 = 0x0000;
        sprintf(lcd_data_buf2,"V1: ??V  V2: ??V");

    }
    communication_established_check_flg = false;   
    handled = false;
}

MWX_TIMER_EVENT(2, uint32_t arg){

    if(communication_status == true){   //通信状況に応じてLEDの色を変更
        led_color = GREEN;
    }else{
        led_color = RED;
    }


    if(current_mode == NORMAL){ //モードによって送信する信号を変更
        led_flash = ON;

    }else if(current_mode == EMERGENCY_MAIN){   //自身が緊急停止した場合は、緊急停止信号を出し続ける
        led_flash = BLINK;
        vTransmit(0x0,0x12,10);
    }else if(current_mode == EMERGENCY_SUB){    //緊急停止信号を受信した側の場合は、緊急停止信号は出さない
        led_flash = BLINK_SLOW;

    }else if(current_mode == RELEASE){
        led_flash = BLINK_FAST;
        vTransmit(0x0,0x11,10);
    }


    /*
    子機同士の挙動について

    複数の子機が同時に稼働している状況を想定する。
    ノーマルモードでは、親機の信号は子機にブロードキャストされているため、全ての子機がその信号を受信できる。
    一台の子機が緊急停止ボタンを押すと、その子機はEMERGENCY_MAINモードに入り、定期的に緊急停止信号を送出する。
    親機がそれを受信してrx_statusの1ビット目をfalseにしてワイヤレスで緊急停止が入ったことをブロードキャストする。
    その信号を受信した他の子機は、モードをEMERGENCY_SUBモードに切り替えて緊急停止信号を止めることができるようになる。ただしこのタイミングでEMERGENCY_MAINモードに入ることもできる。

    */
    if(rx_status & (1<<4)){ //4ビット目(復帰中ステータスが送られてきた時)
        led_flash = ON;
        current_mode = NORMAL;
        attachIntDio(EM_SW,PIN_INT_MODE::RISING);   //割り込みを再有効化
        
    }
    //



}




MWX_TIMER_INT(3, uint32_t arg, uint8_t& handled){   //長押し時の解除動作


    if((timer3_count/4)>7){

        current_mode = NORMAL;
        Serial << "else if 1" << crlf;
        if(digitalRead(EM_SW) == PIN_STATE::LOW){
            attachIntDio(EM_SW,PIN_INT_MODE::RISING);
            timer3_count = 0;
            Timer3.end();
        }

    }else if((timer3_count/4)>5){
        Serial << "else if 2" << crlf;
        
        lcd_control.clean();
    //lcd_data_buf1に格納する方法はLCDの更新に割り込みを使っているので、他の割り込み中は使えない。


        release_flg = true;
        Serial << "Set to NORMAL mode" << crlf;

        
    }else if((timer3_count/4) > 3){

        Serial << "else if 3" << crlf;

        current_mode = RELEASE;

    }else{

        Serial << "else" << crlf;

        lcd_control.clean();
        lcd_control.print("RELEASE");

        lcd_control.setCursor(1,0);

        for(char i=0;i<=(timer3_count/4);i++){
            lcd_control.print("##");
        }
    }

    timer3_count ++;
}





/*****************************************************************/
// common procedure (DO NOT REMOVE)
#include "_mwx_cbs_cpptail.cpp"
// MUST UNDEF CLASS NAME HERE
#undef __MWX_APP_CLASS_NAME
/*****************************************************************/