extern char lcd_data_buf1[32];
extern char lcd_data_buf2[32];


class lcd {
    public:
        void command(uint8_t data);
        void data(uint8_t data);

        void setup();
        void print(const char* str);
        void setCursor(uint8_t row, uint8_t col);
        void clean();
    };

    
void LCD_Timer_Control();