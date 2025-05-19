#ifndef LCD_H
#define LCD_H

class lcd {
public:
    static void setup();              // LCDの初期化
    static void command(char data);   // コマンド送信
    static void data();               // 未実装だが宣言されている（必要なら実装を追加）
};

#endif