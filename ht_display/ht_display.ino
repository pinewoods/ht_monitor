
#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd;

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(0, 128, 255);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  delay(1000);
}

int nivel = 0;

void loop()
{
  nivel += 1;
  if(nivel>=99) nivel=0;

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print("Nivel: ");
  lcd.print(nivel);

  //lcd.setRGB(random(255), random(255), random(255));
  delay(990);
}
