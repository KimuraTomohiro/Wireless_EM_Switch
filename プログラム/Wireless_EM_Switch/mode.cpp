#include <TWELITE>
#include "mode.hpp"
#include "lcd.hpp"

extern lcd lcd_control;
char current_mode;


void change_mode(char mode_num){

    switch(mode_num){

    case 0:
        
        lcd_control.clean();
        current_mode = 0;
        LED_mode = 0;
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
        break;

    case 4:
    
        lcd_control.clean();
        current_mode = 4;
        LED_mode = 4;
        break;


    }

}