/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 11;
int readPin = 0; 
int res;
int ledLevel;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  
  res = analogRead(readPin);
  Serial.println(res);
  
  ledLevel = map(res, 0, 1023, 0, 255);
  analogWrite(led, ledLevel);
}
