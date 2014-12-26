/*
UMI LIGHT SWITCH
ARDUINO MEGA
COMPONENTS RUNNING:

*/
#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

int threshold = 20;

int relay = 7;
int fsr = 0;
#define OFF 0
#define ON 1
int switch_status = OFF;
int light_status = OFF;

void setup(){
  Serial.begin(115200);
  pinMode(7, OUTPUT);
  digitalWrite(relay, HIGH);
  emon1.voltage(4, 120, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(3, 111.1);       // Current: input pin, calibration.
}

void loop(){
  int fsr_value = analogRead(fsr);
  if(fsr_value > threshold){

    if(switch_status == OFF){
      if(light_status == OFF){
        switch_status = ON;
        light_status = ON;
        digitalWrite(relay, LOW);
        delay(500);
      }else{
        switch_status = ON;
        light_status = OFF;
        digitalWrite(relay, HIGH);
        delay(500);
      }
    }

  }else{
    switch_status = OFF;
  }
  
  emon1.calcVI(20,2000);         // Calculate all. No.of wavelengths, time-out
  emon1.serialprint();           // Print out all variables
  
  float realPower       = emon1.realPower;        //extract Real Power into variable
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  float Irms            = emon1.Irms;             //extract Irms into Variable
}
