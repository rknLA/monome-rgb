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

class RGB_LED;

class RGB_LED_nopwm;


void handle_ledOn(int x, int y);
void handle_ledOff(int x, int y);
void handle_ledRow(int y, byte mask);
void handle_color(int x, int y, byte r, byte g, byte b);
void handle_clear(byte flag);
void handle_mode(byte mode);


byte rx_count;    /* number of bytes received */
byte rx_length;   /* length of current message */
byte rx_type;     /* type of current message */
byte rx_timeout;  /* timeout counter */
byte rx_buf[16];  /* receive buffer */

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
  packet_length[COLOL]    = 5;

  rx_count = rx_type = rx_timeout = 0;
  rx_length = 1;

  initializeHardware();
}

void loop()
{

  /* handle input from computer */

  if (rx_timeout > 40) {
    rx_count = 0;
  }
  
  if (Serial.available())
  {
    //read the serial byte
    rx[rx_count] = Serial.read(1);

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
        handle_ledColor(x, y, red, green, blue);
        break;
      default:
        //not really anything to do, huh?
        break;
      }
    }

  }


  /* handle button presses */
  for (int i = 0; i < 4; ++i)
  {
    btn_state[i] = digitalRead(btn_pin[i]);

  }

    
}
  

  
