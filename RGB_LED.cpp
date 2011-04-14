#include "RGB_LED.h"

void RGB_LED::setColor(long rgb)
{
    red     = (rgb & 0x00FF0000) >> 16; 
    green   = (rgb & 0x0000FF00) >> 8;
    blue    = (rgb & 0x000000FF);

    //if led is on, write the color value
    if (state)
    {
      on();
    }
}

void RGB_LED::setColor(byte r, byte g, byte b)
{
    red     = r; 
    green   = g;
    blue    = b;

    //if led is on, write the color value
    if (state)
    {
      on();
    }
}

void RGB_LED::assignPins(int rp, int gp, int bp)
{
    pin_r = rp;
    pin_g = gp;
    pin_b = bp;

    pinMode(pin_r, OUTPUT);
    pinMode(pin_g, OUTPUT);
    pinMode(pin_b, OUTPUT);
}

void RGB_LED::on(void)
{
  if (pwm)
  {
      analogWrite(pin_r, ~red);
      analogWrite(pin_g, ~green);
      analogWrite(pin_b, ~blue);
  } else {
      //timer-based software pwm TODO
      //temporary 8 color only until timer implemented
      if (red > 127)
      {
        digitalWrite(pin_r, LOW);
      } else {
        digitalWrite(pin_r, HIGH);
      }

      if (green > 127)
      {
        digitalWrite(pin_g, LOW);
      } else {
        digitalWrite(pin_g, HIGH);
      }

        if (blue > 127)
        {
          digitalWrite(pin_b, LOW);
        } else {
          digitalWrite(pin_b, HIGH);
        }
    }

    state = 1;
}

void RGB_LED::off(void)
{
    if (pwm)
    {
        analogWrite(pin_r, 0);
        analogWrite(pin_g, 0);
        analogWrite(pin_b, 0);
    } else {
        //timer-based software pwm TODO
        //temporary 8 color only until timer implemented
        digitalWrite(pin_r, HIGH);
        digitalWrite(pin_g, HIGH);
        digitalWrite(pin_b, HIGH);
    }
    state = 0;
}

void RGB_LED::setState(int s)
{
    if (s)
    {
        on();
    } else {
        off();
    }
}

