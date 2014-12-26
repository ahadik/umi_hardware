#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instanc



volatile int stat = 0;
volatile int light = 0;
#define RELAY1  7
int fsrAnalogPin = 0;
int fsrReading;

void setup(){

  
  Serial.begin(115200);
  stat = 0;
  pinMode(RELAY1, OUTPUT);
  stat = 0;
  /*
  
  emon1.voltage(2, 120, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(1, 111.1);       // Current: input pin, calibration.
  */
  InitialiseIO();
  InitialiseInterrupt();
  digitalWrite(RELAY1, HIGH);
}

void loop(){
//  fsrReading = analogRead(fsrAnalogPin);

//  stat = 0;
  /*
  emon1.calcVI(20,2000);         // Calculate all. No.of wavelengths, time-out
  emon1.serialprint();           // Print out all variables
  float realPower       = emon1.realPower;        //extract Real Power into variable
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable
  */
}


void InitialiseIO(){
   pinMode(A0, INPUT);	   // Pin A0 is input to which a switch is connected
  digitalWrite(A0, HIGH);   // Configure internal pull-up resistor
  pinMode(A1, INPUT);	   // Pin A1 is input to which a switch is connected
  digitalWrite(A1, HIGH);   // Configure internal pull-up resistor
  pinMode(A2, INPUT);	   // Pin A2 is input to which a switch is connected
  digitalWrite(A2, HIGH);   // Configure internal pull-up resistor
}

void InitialiseInterrupt(){
  cli();		// switch interrupts off while messing with their settings  
  PCICR =0x02;          // Enable PCINT1 interrupt
  PCMSK1 = 0b00000111;
  sei();		// turn interrupts back on
}

ISR(PCINT1_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
            // will generate an interrupt: but this will always be the same interrupt routine
Serial.println("Here");
  if (digitalRead(A0)>0){
//    Serial.println("test");
  
    if(!stat){
      stat = 1;
      if(light){
        light = 0;
        digitalWrite(RELAY1,HIGH);
      }else{
        light = 1;
        digitalWrite(RELAY1, LOW);
      }
      delay(1000);
      stat = 0;
    }
  }
}
