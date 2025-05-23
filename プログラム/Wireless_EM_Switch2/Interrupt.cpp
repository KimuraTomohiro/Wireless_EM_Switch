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

//ビヘイビアに関する参考はここ
//https://mwx.twelite.info/api-reference/behavior

/*****************************************************************/
// MUST DEFINE CLASS NAME HERE
#define __MWX_APP_CLASS_NAME INTERRUPT
#include "_mwx_cbs_cpphead.hpp" // 冒頭の定義
/*****************************************************************/


void INTERRUPT::transmit_complete(mwx::packet_ev_tx& evTx) {

    //Serial << "送信完了" << crlf;
}

MWX_DIO_INT(PIN_DIGITAL::DIO18,uint32_t arg, uint8_t& handled){ //停止ボタンが押されたときに割り込み動作を行う。
    handled = false;
}

MWX_DIO_EVENT(PIN_DIGITAL::DIO18,uint32_t arg){
    Serial <<"Pressed"<< crlf;

    if(communication_status == false){
        vTransmit(0x0,0x10,5);
        led_flash = BLINK;
    }

    while(digitalRead(EM_SW) == PIN_STATE::HIGH){
        delay(50);
    }
    delay(100);
    while(digitalRead(EM_SW) == PIN_STATE::LOW){
        delay(50);
    }
    delay(100);
}


MWX_TIMER_INT(0, uint32_t arg, uint8_t& handled){
    //本来は高速で復帰する必要のある割り込み動作にLCDの更新という重い動作を入れるのはよろしくないが、
    // 時間的正確性がそこまで重要視されるものでもないので利便性のためにここで処理する。

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
    Timer_LED_Control(led_color,led_flash);
    handled = true;
}

MWX_TIMER_INT(2, uint32_t arg, uint8_t& handled){   //疎通確認用のタイマ　2Hzで動作し、疎通確認用フラグを折る。これが一度呼び出されてから次に呼び出されるまでの0.5秒間に一度でも受信があればフラグが立ち、疎通確認が取れる。
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
        rx_status = 0xFF;
        rx_volt1 = 0xFFFF;
        rx_volt2 = 0xFFFF;
        sprintf(lcd_data_buf2,"V1: ??V  V2: ??V");

    }
    communication_established_check_flg = false;   
    handled = true;
}

MWX_TIMER_EVENT(2, uint32_t arg){



}



/*****************************************************************/
// common procedure (DO NOT REMOVE)
#include "_mwx_cbs_cpptail.cpp"
// MUST UNDEF CLASS NAME HERE
#undef __MWX_APP_CLASS_NAME
/*****************************************************************/