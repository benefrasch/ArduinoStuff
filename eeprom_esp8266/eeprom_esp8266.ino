#include <EEPROM.h>


struct {
  char str[4] = "";
} site;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  EEPROM.get(0, data);
  Serial.println();
  Serial.println(data.str);
  strncpy(data.str, "woooooow", 10);
  EEPROM.put(0, data);
  EEPROM.commit(); 
}



void loop() {
  delay(1000);


}
