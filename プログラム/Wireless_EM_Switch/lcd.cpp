#include <TWELITE>
#include "lcd.hpp"

#define DEV_ADDR 0x3E

void lcd::command(uint8_t data){    //AQM0802Aのコマンド送信
    Wire.beginTransmission(DEV_ADDR);
    uint8_t send_data[2];
    send_data[0] = 0x00;//コマンド用1バイト
    send_data[1] = data;
    Wire.write(send_data,2);
    delayMicroseconds(50);
    Wire.endTransmission();
}

void lcd::data(uint8_t data){   //AQM0802Aのデータ送信
    Wire.beginTransmission(DEV_ADDR);
    uint8_t send_data[2];
    send_data[0] = 0x40;//データ用1バイト
    send_data[1] = data;
    Wire.write(send_data,2);
    delayMicroseconds(50);
    Wire.endTransmission();
}

void lcd::setup(){  //AQM0802Aのセットアップ用。最初に呼ぶ。
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

void lcd::print(const char* str) {  //文字列送信用
    while (*str) {
        data(*str++);
    }

}

void lcd::setCursor(uint8_t row, uint8_t col) { //AQM0802Aのカーソル移動
    uint8_t row_offsets[] = {0x00, 0x40};
    if (row > 1) row = 1; // 2行まで対応
    lcd::command(0x80 | (col + row_offsets[row]));
}




extern lcd lcd_control;

char lcd_data_buf1[10] = "";
char lcd_data_buf2[10] = ""; 

char prev_lcd_data_buf1[10] = "";
char prev_lcd_data_buf2[10] = ""; 

void LCD_Timer_Control(){   //タイマで規定時間ごとにLCDの表示をバッファ内の文字列に更新する。バッファ内の文字列に変更がなければなにもしない。
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

