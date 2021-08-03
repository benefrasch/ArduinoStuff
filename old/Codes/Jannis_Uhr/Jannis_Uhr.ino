/*
   Dieses Script geht davon aus, dass
   das verwendete Potentiometer einen
   Minimalwiderstand von 0Ohm liefert.

   Falls nicht zutreffend können Fehler
   in der Funktionalität auftreten.

   bei allen Variablen bei den im
   Kommentar "EINTRAGEN" steht sind
   auszufüllen.
*/
const int r;         //Pin_Rot                                       EINTRAGEN
const int g;         //Pin_Grün                                      EINTRAGEN
const int b;         //Pin_Blau                                      EINTRAGEN
const int pp ;       //Pin_Poti                                      EINTRAGEN
const int switcha;   //Switch                                        EINTRAGEN
int a;               //Kalibrierungszahl

int p;               //1023 - Aktueller Wert des Potis
int m;         //Niedrigste Zahl aus Serial monitor            

const int x = m / 3;      //maximale Wertedifferenz für eine Schleife
// const int y = 2 * m / 3;
// const int z = m;


void setup() {
  Serial.begin(9600);
  pinMode( r , OUTPUT);
  pinMode( g , OUTPUT);
  pinMode( b , OUTPUT);
  pinMode( switcha , INPUT_PULLUP);

}

void loop() {
  while(switcha == 0){              //Sorgt dafür dass der Microcontroller nix macht wenn der Switch auf aus ist
    delay(100);
    analogWrite(r , 0 );
    analogWrite(g , 0 );
    analogWrite(b , 0 );
  }
  
  p = 1023 - analogRead(pp);
  Serial.println(p);

  if (p > a) {
    m = 1023 - a ;
  }
  
  if (p / m <= 1 / 3) {
    analogWrite(r , 100 / x * p);
    analogWrite(g , 100 - 100 / x * p);
    analogWrite(b , 0 );
  }
  if (p / m > 1 / 3 && p / m <= 2 / 3) {
    analogWrite(r , 0 );
    analogWrite(g , 100 / x * p);
    analogWrite(b , 100 - 100 / x * p);
  }
  if (p / m > 2 / 3) {
    analogWrite(b , 100 / x * p);
    analogWrite(r , 100 - 100 / x * p);
    analogWrite(g , 0 );
  }

}
