#include <TWELITE>

const uint8_t LED_R = 11;
const uint8_t LED_G = 13;
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;
#define DEV_ADDR 0x3E

class lcd {
    public:
        void setup();
        void command(uint8_t data);
        void data(uint8_t data);
        void print(const char* str);
        void setCursor(uint8_t row, uint8_t col);
    };

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


lcd lcd_control = lcd();
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



void setup() {
    Wire.begin(WIRE_CONF::WIRE_400KHZ,false);
    lcd_control.setup();

    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(EM_SW,INPUT_PULLDOWN);

    //LEDを制御しているのがPch MOSFETなので、ロジックと出力が逆になる

    digitalWrite(LED_R, HIGH); // TURN LED_R OFF
    digitalWrite(LED_G, LOW); // TURN LED_G ON

    Serial << "--- Wireless Emergency Switch ---" << crlf;

    Timer0.begin(1);
    Timer0.change_hz(0,10);
}




/*** loop procedure (called every event) */
void loop() {

    int vol = 12;
    sprintf(lcd_data_buf2,"Vol:%d",vol);

	if(digitalRead(EM_SW) == PIN_STATE::HIGH){
		digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
        sprintf(lcd_data_buf1,"ON");

	}else{
		digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
        sprintf(lcd_data_buf1,"OFF");
	}

    if (Timer0.available()){
        LCD_Timer_Control();
    }

}





