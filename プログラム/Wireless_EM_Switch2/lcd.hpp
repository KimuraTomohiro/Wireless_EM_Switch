extern char lcd_data_buf1[32];
extern char lcd_data_buf2[32];


class lcd {
    public:
        void command(uint8_t data);
        void data(uint8_t data);
        void defineAntennaBars();
        void setup();
        void print(const char* str);
        void setCursor(uint8_t row, uint8_t col);
        void clean();
        void printAntennaBars(char bars);

    private:
        static const uint8_t antenna_bar[8];
        static const uint8_t antenna_1bar[8];
        static const uint8_t antenna_2bar[8];
        static const uint8_t antenna_3bar[8];

    };

    
void LCD_Timer_Control();
void LCD_Antenna_update();