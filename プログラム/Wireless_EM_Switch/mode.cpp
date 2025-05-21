#include <TWELITE>
#include "mode.hpp"
#include "lcd.hpp"
#include "communication.hpp"
#include "main.hpp"

extern lcd lcd_control;
char current_mode;


void change_mode(char mode_num){

    switch(mode_num){

    case 0: //電源投入時に緊急停止ボタンが押されていた場合、復帰指示とみなして信号を送る
        
        lcd_control.clean();
        current_mode = 0;
        LED_mode = 0;

        sprintf(lcd_data_buf1,"EM_Status Released");

        for(char i=0;i<5;i++){
            vTransmit(0x0, 0x02);
        }


        delay(2000);
        while(digitalRead(EM_SW) == PIN_STATE::HIGH);
        delay(200);

        break;

    case 1:

        lcd_control.clean();
        current_mode = 1;
        LED_mode = 1;
        sprintf(lcd_data_buf1,"Detecting MASTER");
        break;

    case 2:

        lcd_control.clean();
        current_mode = 2;
        LED_mode = 2;
        break;

    case 3:

        lcd_control.clean();
        current_mode = 3;
        LED_mode = 3;
        sprintf(lcd_data_buf2,"EM");
        break;

    case 4:

        lcd_control.clean();
        current_mode = 4;
        LED_mode = 4;
        sprintf(lcd_data_buf2,"EM");
        break;


    }

}