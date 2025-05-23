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

    delay(5);

    lcd::defineAntennaBars();
    lcd::command(0x01);

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

void lcd::clean(){
    lcd::command(0x01);
    delay(2);
}



void lcd::defineAntennaBars() {
    // CGRAMアドレス設定
    lcd::command(0x40 | 0x00);
    // antenna_bar 書き込み
    for (int j = 0; j < 8; j++) {
        lcd::data(lcd::antenna_bar[j]);
    }
    // CGRAM 1番
    lcd::command(0x40 | (1 << 3));
    for (int j = 0; j < 8; j++) {
        lcd::data(lcd::antenna_1bar[j]);
    }
    // CGRAM 2番
    lcd::command(0x40 | (2 << 3));
    for (int j = 0; j < 8; j++) {
        lcd::data(lcd::antenna_2bar[j]);
    }
    // CGRAM 3番
    lcd::command(0x40 | (3 << 3));
    for (int j = 0; j < 8; j++) {
        lcd::data(lcd::antenna_3bar[j]);
    }
}

void lcd::printAntennaBars(char bars) {

    lcd::setCursor(0,6);

    switch(bars){

        case 0:
            lcd::data(0x00);
            lcd::data(0xF7); 
        break;

        case 1:
            lcd::data(0x00);
            lcd::data(0x01);
        break;

        case 2:
            lcd::data(0x00);
            lcd::data(0x02);
        break;

        case 3:
            lcd::data(0x00);
            lcd::data(0x03);
        break;

    }

}


// ユーザー定義キャラクターのデータ
const uint8_t lcd::antenna_bar[] = {
    0b11111,
    0b10101,
    0b01110,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00000
};

const uint8_t lcd::antenna_1bar[] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b10000,
    0b10000,
    0b10000,
    0b00000
};

const uint8_t lcd::antenna_2bar[] = {
    0b00000,
    0b00000,
    0b00100,
    0b00100,
    0b10100,
    0b10100,
    0b10100,
    0b00000
};

const uint8_t lcd::antenna_3bar[] = {
    0b00001,
    0b00001,
    0b00101,
    0b00101,
    0b10101,
    0b10101,
    0b10101,
    0b00000
};



extern lcd lcd_control;


void LCD_status_update(){
    


}

char lcd_data_buf1[32] = "";
char lcd_data_buf2[32] = ""; 

char prev_lcd_data_buf1[32] = "";
char prev_lcd_data_buf2[32] = ""; 


char blank[9] = "        ";

void LCD_Timer_Control() {
    static uint8_t scroll_pos1 = 0;
    static uint8_t scroll_pos2 = 0;
    static const uint8_t LCD_WIDTH = 8;
    static const uint8_t GAP = 7;

    size_t len1 = strlen(lcd_data_buf1);
    size_t len2 = strlen(lcd_data_buf2);

    static char prev_disp_buf1[LCD_WIDTH + 1] = "";
    static char prev_disp_buf2[LCD_WIDTH + 1] = "";

    // ======= 1行目 =======
    if (len1 <= LCD_WIDTH) {
        // スクロール不要
        if (strcmp(lcd_data_buf1, prev_lcd_data_buf1) != 0) {
            lcd_control.setCursor(0, 0);
            lcd_control.print("        ");
            lcd_control.setCursor(0, 0);
            lcd_control.print(lcd_data_buf1);
            strcpy(prev_lcd_data_buf1, lcd_data_buf1);
            scroll_pos1 = 0;
        }
    } else {

    // 文字列が変わったときにスクロール位置をリセット
        if (strcmp(lcd_data_buf1, prev_lcd_data_buf1) != 0) {
            scroll_pos1 = 0;
            strcpy(prev_lcd_data_buf1, lcd_data_buf1);
            // 表示バッファも初期化（あえて次のループで再描画されるように）
            prev_disp_buf1[0] = '\0';
        }

        // スクロール用バッファ構築
        static char scroll_buf1[32];
        for (size_t i = 0; i < len1; ++i) scroll_buf1[i] = lcd_data_buf1[i];
        for (size_t i = 0; i < GAP; ++i) scroll_buf1[len1 + i] = ' ';
        scroll_buf1[len1 + GAP] = '\0';

        // 8文字抜き出し
        char disp_buf1[LCD_WIDTH + 1];
        for (size_t i = 0; i < LCD_WIDTH; ++i) {
            disp_buf1[i] = scroll_buf1[scroll_pos1 + i];
        }
        disp_buf1[LCD_WIDTH] = '\0';

        // 表示更新
        if (strcmp(disp_buf1, prev_disp_buf1) != 0) {
            lcd_control.setCursor(0, 0);
            lcd_control.print(disp_buf1);
            strcpy(prev_disp_buf1, disp_buf1);
        }

        // スクロール位置更新
        scroll_pos1++;
        if (scroll_pos1 > len1 + GAP - LCD_WIDTH) scroll_pos1 = 0;
    }

    // ======= 2行目 =======
    if (len2 <= LCD_WIDTH) {
        if (strcmp(lcd_data_buf2, prev_lcd_data_buf2) != 0) {
            lcd_control.setCursor(1, 0);
            lcd_control.print("        ");
            lcd_control.setCursor(1, 0);
            lcd_control.print(lcd_data_buf2);
            strcpy(prev_lcd_data_buf2, lcd_data_buf2);
            scroll_pos2 = 0;
        }
    } else {
        // 文字列が変わったときにスクロール位置をリセット
        if (strcmp(lcd_data_buf2, prev_lcd_data_buf2) != 0) {
            scroll_pos2 = 0;
            strcpy(prev_lcd_data_buf2, lcd_data_buf2);
        }

        static char scroll_buf2[32];
        for (size_t i = 0; i < len2; ++i) scroll_buf2[i] = lcd_data_buf2[i];
        for (size_t i = 0; i < GAP; ++i) scroll_buf2[len2 + i] = ' ';
        scroll_buf2[len2 + GAP] = '\0';

        char disp_buf2[LCD_WIDTH + 1];
        for (size_t i = 0; i < LCD_WIDTH; ++i) {
            disp_buf2[i] = scroll_buf2[scroll_pos2 + i];
        }
        disp_buf2[LCD_WIDTH] = '\0';

        if (strcmp(disp_buf2, prev_disp_buf2) != 0) {
            lcd_control.setCursor(1, 0);
            lcd_control.print(disp_buf2);
            strcpy(prev_disp_buf2, disp_buf2);
        }

        scroll_pos2++;
        if (scroll_pos2 > len2 + GAP - LCD_WIDTH) scroll_pos2 = 0;
    }
}

