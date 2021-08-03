


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
void setup() {
  Wire.begin(2, 0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.setCursor(0, 0);
  lcd.print("InfiniLight   DMX512"); // Start Printing
  lcd.setCursor(0, 2);
  lcd.print("Status:");
  lcd.setCursor(0, 3);
  lcd.print("last:");

}
void loop() {
  // Nothing Absolutely Nothing!
}
