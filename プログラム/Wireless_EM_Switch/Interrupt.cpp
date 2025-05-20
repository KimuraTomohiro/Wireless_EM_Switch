#include <TWELITE>
#include "Interrupt.hpp" // ビヘイビア定義ファイル

#define DEV_ADDR 0x3E

/*****************************************************************/
// MUST DEFINE CLASS NAME HERE
#define __MWX_APP_CLASS_NAME TIMER_INTERRUPT
#include "_mwx_cbs_cpphead.hpp" // 冒頭の定義
/*****************************************************************/

void lcd::command(uint8_t data){

        Wire.beginTransmission(DEV_ADDR);
        uint8_t send_data[2];
        send_data[0] = 0x00;
        send_data[1] = data;
        Wire.write(send_data,2);
        delayMicroseconds(50);
        Wire.endTransmission();
}

void lcd::data(uint8_t data){
        Wire.beginTransmission(DEV_ADDR);
        uint8_t send_data[2];
        send_data[0] = 0x40;
        send_data[1] = data;
        Wire.write(send_data,2);
        delayMicroseconds(50);
        Wire.endTransmission();
}

void lcd::print(const char* str) {
        while (*str) {
            data(*str++);
        }

}

void lcd::setCursor(uint8_t row, uint8_t col) {
        uint8_t row_offsets[] = {0x00, 0x40};
        if (row > 1) row = 1; // 最大2行まで対応
        lcd::command(0x80 | (col + row_offsets[row]));
}


void lcd::setup(){
    delay(50);
    lcd::command(0x38);
    lcd::command(0x39);
    lcd::command(0x14);
    lcd::command(0x70);
    lcd::command(0x56);
    lcd::command(0x6C);
    delay(300);
    lcd::command(0x38);
    lcd::command(0x0C);
    lcd::command(0x01);
    delay(5);
}


extern lcd lcd_control;

char lcd_data_buf1[10] = "";
char lcd_data_buf2[10] = ""; 

char prev_lcd_data_buf1[10] = "";
char prev_lcd_data_buf2[10] = ""; 

void LCD_Timer_Control(){

    if(strcmp(lcd_data_buf1, prev_lcd_data_buf1) != 0 || strcmp(lcd_data_buf2, prev_lcd_data_buf2) != 0){
        lcd_control.command(0x01);
        delay(1);
        lcd_control.setCursor(0, 0);
        lcd_control.print(lcd_data_buf1);
        strcpy(prev_lcd_data_buf1, lcd_data_buf1);
        lcd_control.setCursor(1, 0);
        lcd_control.print(lcd_data_buf2);
        strcpy(prev_lcd_data_buf2, lcd_data_buf2);
    }


}