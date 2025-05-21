#include <TWELITE>
#include "mode.hpp"
#include "lcd.hpp"


extern char mode;
extern char LED_mode;
extern char lcd_data_buf1[10];
extern char lcd_data_buf2[10];


void change_mode(char mode_num){

    switch(mode_num){

    case 0:
        LED_mode = 0;
        break;

    case 1:
        LED_mode = 1;
        sprintf(lcd_data_buf1,"Search");
        sprintf(lcd_data_buf2,"MASTER");
        break;


    case 2:
        LED_mode = 2;
        break;

    case 3:
        LED_mode = 3;
        break;

    case 4:
        LED_mode = 4;
        break;


    }

}