#include <TWELITE>
#include "mode.hpp"
#include "lcd.hpp"
#include "communication.hpp"
#include "main.hpp"

extern lcd lcd_control;
char current_mode;
char next_mode;
bool release_signal_send_flg = false;

void change_mode(char mode_num){

    switch(mode_num){
    case 0:

        LED_mode = 4;
        next_mode = current_mode;
        current_mode = 0;
        Timer1.change_hz(10);    //赤の高速点滅
        sprintf(lcd_data_buf1,"Releasing...");
        release_signal_send_flg = true;
        Timer4.begin(1,true,false);
        break;

    case 1:

        LED_mode = 1;
        lcd_control.clean();

        while(digitalRead(EM_SW) == PIN_STATE::HIGH){
            delay(100); //緊急停止からの復帰など、ボタンが押された状態の時に放されるまで待つ
        }
        delay(300);
        attachIntDio(EM_SW,PIN_INT_MODE::RISING);
        current_mode = 1;

        sprintf(lcd_data_buf1,"Waiting for connection");
        break;

    case 2:
        LED_mode = 2;
        lcd_control.clean();

        sprintf(lcd_data_buf1,"V1: %dV ,V2: %dV",rx_volt1,rx_volt2);

        while(digitalRead(EM_SW) == PIN_STATE::HIGH){
            delay(100); //緊急停止からの復帰など、ボタンが押された状態の時に放されるまで待つ
        }
        delay(300);
        attachIntDio(EM_SW,PIN_INT_MODE::RISING);
        current_mode = 2;

        break;

    case 3:
        detachIntDio(EM_SW);  
        lcd_control.clean();
        current_mode = 3;
        LED_mode = 3;
        sprintf(lcd_data_buf1,"Waiting for connection");
        lcd_control.setCursor(1,0);
        lcd_control.print("EM");
        break;


    case 4:
        detachIntDio(EM_SW);  
        lcd_control.clean();
        sprintf(lcd_data_buf1,"V1: %dV ,V2: %dV",rx_volt1,rx_volt2);
        current_mode = 4;
        LED_mode = 4;
        lcd_control.setCursor(1,0);
        lcd_control.print("EM");
        break;

    }

}