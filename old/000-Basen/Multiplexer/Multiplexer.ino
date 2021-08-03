int mps_1 = 3;
int mps_2 = 4;
int mps_3 = 5;
int out;

void setup() {
  pinMode(mps_1 , OUTPUT);
  pinMode(mps_2 , OUTPUT);
  pinMode(mps_3 , OUTPUT);
  digitalWrite(mps_1, LOW);
  digitalWrite(mps_2, LOW);
  digitalWrite(mps_3, LOW);
  Serial.begin(9600);
}

void loop() {
  Serial.print("1: X: ");
  abfrage(1 , 0);
  Serial.print( out );
  Serial.print("   Y: ");
  abfrage(1 , 1);
  Serial.print( out );
  
  Serial.print("     2: X: ");
  abfrage(1 , 2);
  Serial.print( out );
  Serial.print("   Y: ");
  abfrage(1 , 3);
  Serial.print( out );
  Serial.println("");


}


void abfrage(int analogpin , int multiplexerpin) {
  digitalWrite(mps_1 , bitRead(multiplexerpin , 0 ) );
  digitalWrite(mps_2 , bitRead(multiplexerpin , 1 ) );
  digitalWrite(mps_3 , bitRead(multiplexerpin , 3 ) );
  out = analogRead(analogpin);
}
