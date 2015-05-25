//We always have to include the library
#include "LedControl.h"
#define ARRAY_SIZE(x) ((sizeof x) / (sizeof *x))


/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc = LedControl(12,11,10,1);

//Mini-ASCII character-set (ASCII-32 'SPC' to ASCII-128 'DEL')
//glyphs borrowed from here:
//http://www.coranac.com/tonc/text/text.htm#sec-bm
const unsigned long glyphs[192] =
{
    0x00000000, 0x00000000, 0x18181818, 0x00180018, 0x00003636, 0x00000000, 0x367F3636, 0x0036367F, 
    0x3C067C18, 0x00183E60, 0x1B356600, 0x0033566C, 0x6E16361C, 0x00DE733B, 0x000C1818, 0x00000000, 
    0x0C0C1830, 0x0030180C, 0x3030180C, 0x000C1830, 0xFF3C6600, 0x0000663C, 0x7E181800, 0x00001818, 
    0x00000000, 0x0C181800, 0x7E000000, 0x00000000, 0x00000000, 0x00181800, 0x183060C0, 0x0003060C, 
    0x7E76663C, 0x003C666E, 0x181E1C18, 0x00181818, 0x3060663C, 0x007E0C18, 0x3860663C, 0x003C6660, 
    0x33363C38, 0x0030307F, 0x603E067E, 0x003C6660, 0x3E060C38, 0x003C6666, 0x3060607E, 0x00181818,
    0x3C66663C, 0x003C6666, 0x7C66663C, 0x001C3060, 0x00181800, 0x00181800, 0x00181800, 0x0C181800, 
    0x06186000, 0x00006018, 0x007E0000, 0x0000007E, 0x60180600, 0x00000618, 0x3060663C, 0x00180018, 

    0x5A5A663C, 0x003C067A, 0x7E66663C, 0x00666666, 0x3E66663E, 0x003E6666, 0x06060C78, 0x00780C06, 
    0x6666361E, 0x001E3666, 0x1E06067E, 0x007E0606, 0x1E06067E, 0x00060606, 0x7606663C, 0x007C6666, 
    0x7E666666, 0x00666666, 0x1818183C, 0x003C1818, 0x60606060, 0x003C6660, 0x0F1B3363, 0x0063331B, 
    0x06060606, 0x007E0606, 0x6B7F7763, 0x00636363, 0x7B6F6763, 0x00636373, 0x6666663C, 0x003C6666, 
    0x3E66663E, 0x00060606, 0x3333331E, 0x007E3B33, 0x3E66663E, 0x00666636, 0x3C0E663C, 0x003C6670, 
    0x1818187E, 0x00181818, 0x66666666, 0x003C6666, 0x66666666, 0x00183C3C, 0x6B636363, 0x0063777F, 
    0x183C66C3, 0x00C3663C, 0x183C66C3, 0x00181818, 0x0C18307F, 0x007F0306, 0x0C0C0C3C, 0x003C0C0C, 
    0x180C0603, 0x00C06030, 0x3030303C, 0x003C3030, 0x00663C18, 0x00000000, 0x00000000, 0x003F0000, 

    0x00301818, 0x00000000, 0x603C0000, 0x007C667C, 0x663E0606, 0x003E6666, 0x063C0000, 0x003C0606, 
    0x667C6060, 0x007C6666, 0x663C0000, 0x003C067E, 0x0C3E0C38, 0x000C0C0C, 0x667C0000, 0x3C607C66, 
    0x663E0606, 0x00666666, 0x18180018, 0x00301818, 0x30300030, 0x1E303030, 0x36660606, 0x0066361E,
    0x18181818, 0x00301818, 0x7F370000, 0x0063636B, 0x663E0000, 0x00666666, 0x663C0000, 0x003C6666, 
    0x663E0000, 0x06063E66, 0x667C0000, 0x60607C66, 0x663E0000, 0x00060606, 0x063C0000, 0x003E603C, 
    0x0C3E0C0C, 0x00380C0C, 0x66660000, 0x007C6666, 0x66660000, 0x00183C66, 0x63630000, 0x00367F6B, 
    0x36630000, 0x0063361C, 0x66660000, 0x0C183C66, 0x307E0000, 0x007E0C18, 0x0C181830, 0x00301818, 
    0x18181818, 0x00181818, 0x3018180C, 0x000C1818, 0x003B6E00, 0x00000000, 0x00000000, 0x00000000, 
};


//custom icons
byte smiley[8] = {B00111100, B01000010, B10100101, B10000001, B10100101, B10011001, B01000010, B00111100};
byte heart1[8] = {B00000000, B01100110, B10011001, B10000001, B01000010, B00100100, B00011000, B00000000};
byte heart2[8] = {B00000000, B01100110, B11111111, B11111111, B01111110, B00111100, B00011000, B00000000};




void setup() 
{  
  Serial.begin(9600);
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,0);
  /* and clear the display */
  lc.clearDisplay(0);
}


void displayIcon(byte* chs)
{
  for(int i = 0; i < 8; ++i)
  {
    lc.setColumn(0, i, chs[i]);
  }
}


void displayGlyph(int index)
{
  if(index < 0 || index >= ARRAY_SIZE(glyphs) - 1) {
    Serial.println("ERROR: Index out of range");
    return;
  }
  
  unsigned long part1 = glyphs[index];
  unsigned long part2 = glyphs[index + 1];
  
  byte ch[8] = 
  { 
    (part1      ) & 0xFF,   
    (part1 >>  8) & 0xFF,   
    (part1 >> 16) & 0xFF,   
    (part1 >> 24) & 0xFF,
    (part2      ) & 0xFF,   
    (part2 >>  8) & 0xFF,   
    (part2 >> 16) & 0xFF,   
    (part2 >> 24) & 0xFF,
  };
  
  for(int i = 0; i < 8; ++i)
    lc.setColumn(0, i, ch[i]);
}


void displayChar(char ch)
{
  int index = 0;
  //difference between ASCII and the mini-ASCII glyph is 32  
  for(int i = 0; i < (ch  - 32); i++)
    index += 2; //traverse to character position within the array
  displayGlyph(index);
}


void displayString(const String& str, const unsigned long delayMs)
{
  for(int i = 0; i < str.length(); ++i)
  {
    displayChar(str[i]);
    delay(delayMs);
  }
}



int x = 1, y = 2;
int dx = -1, dy = 1;

void displayBounce()
{  
  if (x >= 7) dx = -1;
  if (x <= 0) dx = 1;
  if (y >= 7) dy = -1;
  if (y <= 0) dy = 1;
  x += dx;
  y += dy;
  
  lc.clearDisplay(0);
  lc.setLed(0, y, x, true);  
  delay(100);
}



int intensity = 0;
bool forward = true;
int step = 0;

void loop() 
{
  step++;
  lc.setIntensity(0, intensity);

  if(step <= 100)
  {  
    displayIcon(smiley);
  }
  else if(step <= 200)
  {
    displayIcon(heart1);
  }
  else if(step <= 300)
  {
    displayIcon(heart2);
  }
  else if(step <= 350)
  {
    displayBounce();
  }
  else if(step == 351)
  {
    displayString("HELLO WORLD!", 500);
  }
  else
  {
    step = 0;
  }
  
  if(forward)
    intensity++;
  else
    intensity--;
  
  if(intensity >= 15)
    forward = false;
  if(intensity <= 0)
    forward = true;
    
  delay(10);
}

