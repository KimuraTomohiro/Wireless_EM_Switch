#include <TWELITE>
#include "lcd.h"
extern TwoWire Wire; 

#define DEV_ADDR 0x7c

void lcd::command(char data){

    Wire.beginTransmission(DEV_ADDR);
    char send_data[2];
    send_data[0] = DEV_ADDR;
    send_data[1] = data;
    Wire.write(send_data,2);
    delayMicroseconds(50);
    Wire.endTransmission();
    
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

