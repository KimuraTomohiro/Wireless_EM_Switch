class TIMER_INTERRUPT: MWX_APPDEFS_CRTP(TIMER_INTERRUPT)
{
public:
    static const uint8_t TYPE_ID = 0x01;

    // load common definition for handlers
    #define __MWX_APP_CLASS_NAME TIMER_INTERRUPT
    #include "_mwx_cbs_hpphead.hpp"
    #undef __MWX_APP_CLASS_NAME

public:
    // constructor
    TIMER_INTERRUPT() {}

    void _setup() {}    
    void _begin() {}

public:
    // TWENET callback handler (mandate)
    void loop() {}
    void on_sleep(uint32_t & val) {}
    void warmboot(uint32_t & val) {}
    void wakeup(uint32_t & val) {}

    void on_create(uint32_t& val) { _setup();  }
    void on_begin(uint32_t& val) { _begin(); }
    void on_message(uint32_t& val) { }

public:
    void network_event(mwx::packet_ev_nwk& pEvNwk) {}
    void receive(mwx::packet_rx& rx) {}
    void transmit_complete(mwx::packet_ev_tx& evTx) {}
};


class lcd {
    public:
        void setup();
        void command(uint8_t data);
        void data(uint8_t data);
        void print(const char* str);
        void setCursor(uint8_t row, uint8_t col);
    };

void LCD_Timer_Control();