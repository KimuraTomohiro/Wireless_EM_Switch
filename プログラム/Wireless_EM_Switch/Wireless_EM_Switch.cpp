#include <TWELITE>

const uint8_t LED_R = 11;
const uint8_t LED_G = 13;
const uint8_t EM_SW = 18;
const uint8_t RESET_SW = 19;
#define DEV_ADDR 0x3E

class lcd {
    public:
        void setup();              // LCDの初期化
        void command(uint8_t data);   // コマンド送信
        void data(uint8_t data);               // 未実装だが宣言されている（必要なら実装を追加）
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

void setup() {


    delay(50);
    
    bool a = false;
    Wire.begin(WIRE_CONF::WIRE_100KHZ,false);

    lcd lcd_control = lcd();
    lcd_control.setup();

    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(EM_SW,INPUT_PULLDOWN);

    digitalWrite(LED_R, HIGH); // TURN LED_R OFF
    digitalWrite(LED_G, LOW); // TURN LED_G ON

    Serial << "--- Wireless Emergency Switch ---" << crlf;


    lcd_control.data('N');
    lcd_control.data('O');
    lcd_control.data('T');
    lcd_control.data(' ');
    lcd_control.data('O');
    lcd_control.data('K');

}

/*** loop procedure (called every event) */
void loop() {

	if(digitalRead(EM_SW) == PIN_STATE::HIGH){
		digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
	}else{
		digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, LOW);
	}
}


