extern char lcd_data_buf1[10];
extern char lcd_data_buf2[10];

extern char prev_lcd_data_buf1[10];
extern char prev_lcd_data_buf2[10];

class lcd {
    public:
        void setup();
        void command(uint8_t data);
        void data(uint8_t data);
        void print(const char* str);
        void setCursor(uint8_t row, uint8_t col);
    };

    
void LCD_Timer_Control();