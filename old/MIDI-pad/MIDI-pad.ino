#include <MIDIUSB.h>



int fader[3];
bool buttons[12];
bool buttons_toggle[12];
int var = 2;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(9, INPUT);

  pinMode(15, OUTPUT); //R
  pinMode(14, OUTPUT); //G1
  pinMode(16, OUTPUT); //G2
  pinMode(10, OUTPUT); //G3

}

void loop() {
  checkButtons();
  checkSliders();
  
}

//----------------------------------------------------------------------------------------------------- Sliders
void checkSliders() {
  if (!((fader[0] - var < map(analogRead(0), 0, 1027, 0, 127)) && (fader[0] + var > map(analogRead(0), 0, 1027, 0, 127)))) {
    fader[0] = map(analogRead(0), 0, 1027, 0, 127);

    Serial.println("Fader: 1  ----  " + String(fader[0]));
    controlChange(1, 1, fader[0]);

    if (fader[0] > 105) digitalWrite(14, HIGH);
    else digitalWrite(14, LOW);
  }
  if (!((fader[1] - var < map(analogRead(1), 0, 1027, 0, 127)) && (fader[1] + var > map(analogRead(1), 0, 1027, 0, 127)))) {
    fader[1] = map(analogRead(1), 0, 1027, 0, 127);
    Serial.println("Fader: 2  ----  " + String(fader[1]));

    controlChange(1, 2, fader[1]);

    if (fader[1] > 105) digitalWrite(16, HIGH);
    else digitalWrite(16, LOW);
  }
  if (!((fader[2] - var < map(analogRead(3), 0, 1027, 0, 127)) && (fader[2] + var > map(analogRead(3), 0, 1027, 0, 127)))) {
    fader[2] = map(analogRead(3), 0, 1027, 0, 127);

    Serial.println("Fader: 3  ----  " + String(fader[2]));
    controlChange(1, 3, fader[2]);

    if (fader[2] > 105) digitalWrite(10, HIGH);
    else digitalWrite(10, LOW);
  }
} 
//----------------------------------------------------------------------------------------------------- Buttons
void checkButtons() {
  digitalWrite(2, HIGH);
  if (digitalRead(5) != buttons[0]) {
    button(0, 5, 2);
  }
  if (digitalRead(6) != buttons[1]) {
    button(1, 6, 0);
  }
  if (digitalRead(7) != buttons[2]) {
    button(2, 7, 0);
  }
  if (digitalRead(9) != buttons[3]) {
    button(3, 9, 0);
  }
  digitalWrite(2, LOW);


  digitalWrite(3, HIGH);
  if (digitalRead(5) != buttons[4]) {
    button(4, 5, 0);
  }
  if (digitalRead(6) != buttons[5]) {
    button(5, 6, 0);
  }
  if (digitalRead(7) != buttons[6]) {
    button(6, 7, 0);
  }
  if (digitalRead(9) != buttons[7]) {
    button(7, 9, 0);
  }
  digitalWrite(3, LOW);


  digitalWrite(4, HIGH);
  if (digitalRead(5) != buttons[8]) {
    button(8, 5, 0);
  }
  if (digitalRead(6) != buttons[9]) {
    button(9, 6, 0);
  }
    if (digitalRead(7) != buttons[10]) {
    button(10, 7, 0);
    }
  if (digitalRead(9) != buttons[11]) {
    button(11, 9, 0);
  }
  digitalWrite(4, LOW);
}
//-----------------------------------------------------------------------------------------------------


void button(int button, int input, int mode) { //if mode == 0, normal  | if mode == 1, inverted | if mode == 2, toggle
  buttons[button] = digitalRead(input);
  Serial.println("Button: " + String(button) + " ---  " + String(buttons[button]));
  if (mode == 0)controlChange(2, button, buttons[button]);
  else if (mode == 1) controlChange(2, button, !buttons[button]);
  else if (mode == 2) {
    if (buttons[button] == true) {
      buttons_toggle[button] = !buttons_toggle[button];
      controlChange(2, button, buttons_toggle[button]);
    }
  }

  //Mutebutton LED:
  if (button == 0) {
    digitalWrite(15, !buttons_toggle[button]);
  }
}


// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
