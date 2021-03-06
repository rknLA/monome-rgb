/* rgb led abstraction */

#include "WProgram.h"

#ifndef __RGB_LED_H__
#define __RGB_LED_H__

class RGB_LED
{
private:
    byte red;
    byte green;
    byte blue;

    byte pin_r;
    byte pin_g;
    byte pin_b;

    byte pwm;

    byte state;

public:
    void setColor(long rgb);

    void setColor(byte r, byte g, byte b);

    void assignPins(int rp, int gp, int bp);

    void on(void);

    void off(void);

    void setState(int s);
};

#endif /* __RGB_LED_H__ */

