/* NOTE!! this is currently untested and should be considered a
   work in progress.  treat it accordingly. */

#include "RGB_LED.h"

/* based heavily on the monome series firmware */

/* serial protocol */
/* from device */
#define KEYDOWN     (0x0)
#define KEYUP       (0x1)
#define KNOB        (0xE)

/* to device */
#define LEDON       (0x2)
#define LEDOFF      (0x3)
#define LEDROW      (0x4)
#define CLEAR       (0x9)
#define MODE        (0xB)
#define COLOR       (0xF)



/* pins */
#define PIN_LED0_R      (2)
#define PIN_LED0_G      (4)
#define PIN_LED0_B      (7)

#define PIN_LED1_R      (3)
#define PIN_LED1_G      (5)
#define PIN_LED1_B      (6)

#define PIN_LED2_R      (9)
#define PIN_LED2_G      (10)
#define PIN_LED2_B      (11)

#define PIN_LED3_R      (8)
#define PIN_LED3_G      (12)
#define PIN_LED3_B      (13)

#define PIN_BTN0        (A5)
#define PIN_BTN1        (A3)
#define PIN_BTN2        (A2)
#define PIN_BTN3        (A1)

#define PIN_KNOB0       (A0)
#define PIN_KNOB1       (A4)



void handle_ledOn(int x, int y);
void handle_ledOff(int x, int y);
void handle_ledRow(int y, byte mask);
void handle_color(int x, int y, byte r, byte g, byte b);
void handle_clear(byte flag);
void handle_mode(byte mode);

RGB_LED led[4];

byte rx_count;    /* number of bytes received */
byte rx_length;   /* length of current message */
byte rx_type;     /* type of current message */
byte rx_timeout;  /* timeout counter */
byte rx[16];      /* receive buffer */

/* not all of these are used, but it's more convenient to use the #defines as indices */
byte packet_length[16];


bool opcodeValid(byte opcode)
{
  return ((opcode == KEYDOWN) ||
          (opcode == KEYUP)   ||
          (opcode == KNOB)    ||
          (opcode == LEDON)   ||
          (opcode == LEDOFF)  ||
          (opcode == LEDROW)  ||
          (opcode == CLEAR)   ||
          (opcode == MODE)    ||
          (opcode == COLOR));
}



void initializeHardware()
{
  /* do hardware init stuff TODO */

  pinMode(PIN_BTN0, INPUT);
  pinMode(PIN_BTN1, INPUT);
  pinMode(PIN_BTN2, INPUT);
  pinMode(PIN_BTN3, INPUT);


  led[0].assignPins(PIN_LED0_R, PIN_LED0_G, PIN_LED0_B);
  led[1].assignPins(PIN_LED1_R, PIN_LED1_G, PIN_LED1_B);
  led[2].assignPins(PIN_LED2_R, PIN_LED2_G, PIN_LED2_B);
  led[3].assignPins(PIN_LED3_R, PIN_LED3_G, PIN_LED3_B);

  for (int i = 0; i < 4; ++i)
  {
    led[i].setColor(0);
    led[i].on();
  }

  long colorShifts[12] = {0,0,0,0,0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF, 0, 0, 0, 0};

  for (int i = 4; i < 12; ++i)
  {
    led[0].setColor(colorShifts[i]);
    led[1].setColor(colorShifts[i-1]);
    led[2].setColor(colorShifts[i-2]);
    led[3].setColor(colorShifts[i-3]);
    delay(250);
  }

  for (int i = 0; i < 4; ++i)
  {
    led[i].setColor(0);
    led[i].off();
  }

  Serial.begin(115200);
  

}


void setup()
{
  packet_length[KEYDOWN]  = 2;
  packet_length[KEYUP]    = 2;
  packet_length[LEDON]    = 2;
  packet_length[LEDOFF]   = 2;
  packet_length[LEDROW]   = 2;
  packet_length[CLEAR]    = 1;
  packet_length[MODE]     = 1;
  packet_length[COLOR]    = 5;

  rx_count = rx_type = rx_timeout = 0;
  rx_length = 1;

  initializeHardware();
}


byte lastBtnState[4] = {0};
byte currBtnState[4] = {0};
byte newBtnState[4] = {0};

byte lastADC[2] = {0};
byte currADC[2] = {0};

void loop()
{

  /* handle input from computer */

  if (rx_timeout > 40) {
    rx_count = 0;
  }
  
  if (Serial.available())
  {
    //read the serial byte
    rx[rx_count] = Serial.read();

    //do start-of-message checking
    if (rx_count == 0)
    {
      rx_type = rx[0] >> 4;

      if (opcodeValid(rx_type))
      {
        rx_length = packet_length[rx_type];
        rx_count++;
        rx_timeout = 0;
      }
    } else {
      rx_count++;
    }
  
    //check for end of message
    if (rx_count == rx_length)
    {
      rx_count = 0;

      int x, y;
      int mask;
      byte red, green, blue;

      // TODO add serial message handlers!
      switch(rx_type)
      {
      case LEDON:
        x = rx[1] >> 4;
        y = rx[1] & 0x0F;   //kinda only here for scalability?
        handle_ledOn(x,y);
        break;
      case LEDOFF:
        x = rx[1] >> 4;
        y = rx[1] & 0x0F;   //kinda only here for scalability?
        handle_ledOff(x,y);
        break;
      case LEDROW:
        y = rx[0] & 0x0F;
        mask = rx[1];
        handle_ledRow(y, mask);
        break;
      case CLEAR:
        mask = rx[0] & 0x01;
        handle_clear(mask);
        break;
      case MODE:
        mask = rx[0] & 0x03;
        handle_mode(mask);
        break;
      case COLOR:
        x = rx[1] >> 4;
        y = rx[1] & 0x0F;
        red = rx[2];
        green = rx[3];
        blue = rx[4];
        handle_Color(x, y, red, green, blue);
        break;
      default:
        //not really anything to do, huh?
        break;
      }
    }

  }


  /* handle button presses */

  newBtnState[0] = digitalRead(PIN_BTN0);
  newBtnState[1] = digitalRead(PIN_BTN1);
  newBtnState[2] = digitalRead(PIN_BTN2);
  newBtnState[3] = digitalRead(PIN_BTN3);

  for (int i = 0; i < 4; ++i)
  {
    if (newBtnState[i] == currBtnState[i] &&
        currBtnState[i] != lastBtnState[i])
    {
      if (newBtnState[i])
      {
        //press
        byte coord = i << 4;
        Serial.print((byte)KEYDOWN);
        Serial.print(coord);
      } else {
        //release
        byte coord = i << 4;
        Serial.print((byte)KEYUP);
        Serial.print(coord);
      }

    }

  }

  /* handle adcs */
  currADC[0] = (analogRead(PIN_KNOB0) >> 2);
  currADC[1] = (analogRead(PIN_KNOB1) >> 2);

  for (int i = 0; i < 2; ++i)
  {
    if (currADC[i] != lastADC[i])
    {
      //send ADC message
      byte first = KNOB | i;
      byte second = currADC[i];
      Serial.print(first);
      Serial.print(second);
    }
  }

    
}
  

  


void handle_ledOn(int x, int y)
{
  //ignore y since we only gots 1 row!
  led[x].on();
}

void handle_ledOff(int x, int y)
{
  //ignore y since we only gots 1 row!
  led[x].off();
}

void handle_ledRow(int y, byte mask)
{
  //ignoring y...
  for (int i = 0; i < 4; ++i)
  {
    if ((mask >> i) & 0x01)
    {
      led[i].on();
    } else {
      led[i].off();
    }
  }
}

void handle_color(int x, int y, byte r, byte g, byte b)
{
  //ignore y
  led[x].setColor(r,g,b);
}

void handle_clear(byte flag)
{
  if (flag)
  {
    for(int i = 0; i < 4; ++i)
    {
      led[i].on();
    }
  } else {
    for(int i = 0; i < 4; ++i)
    {
      led[i].off();
    }
  }

}


void handle_mode(byte mode)
{

}

