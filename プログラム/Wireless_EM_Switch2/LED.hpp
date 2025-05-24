#ifndef LED_HPP
#define LED_HPP


extern char LED_mode;

enum LED_flash{
    OFF,
    ON,
    BLINK_SLOW,
    BLINK,
    BLINK_FAST,
};

enum LED_color{
    RED,
    GREEN
};


void Timer_LED_Control(LED_color color,LED_flash flash);

#endif // LED_HPP