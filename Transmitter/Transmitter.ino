
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <avr/wdt.h>

#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

#include<stdlib.h>

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, 
ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2);

#define WLAN_SSID       "Umi Network"           // cannot be longer than 32 characters!
#define WLAN_PASS       "uminetwork"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

int buffer_size = 20;

#define IDLE_TIMEOUT_MS  10000

#define WEBSITE      "umi-webapp.herokuapp.com"

#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

void setup(void){
  
  pinMode(7, INPUT); //input from Mega indicating that switch has been flippe
  pinMode(8, OUTPUT); //output to Mega indicating that switch has been flipped
  
  emon1.voltage(4, 120, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(3, 111.1);       // Current: input pin, calibration.
  
  Serial.begin(115200);
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
   
  // Connect to WiFi network
  Serial.print(F("Connecting to WiFi network ..."));
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println(F("done!"));
    
  // Wait for DHCP to complete
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
}

void fill_blanks(char *number_string, float number){
  
  if(number < 100.0){
    number_string[0] = 48;
  }
  if(number < 10.0){
    number_string[1] = 48;
  }
  if(number < 1.0){
    number_string[2]=48;
  }
}

void loop(void)
{
  // Start watchdog 
  wdt_enable(WDTO_8S); 
  
  // Get IP
  uint32_t ip = 0;
  Serial.print(F("umi-webapp.herokuapp.com -> "));
  while  (ip  ==  0)  {
    if  (!  cc3000.getHostByName(WEBSITE, &ip))  {
      Serial.println(F("Couldn't resolve!"));
      while(1){}
    }
    delay(500);
  }  
  cc3000.printIPdotsRev(ip);
  Serial.println(F(""));

  int calc_times;
  for(calc_times = 0; calc_times<25;calc_times++){
    emon1.calcVI(20,2000);         // Calculate all. No.of wavelengths, time-out
  }
  
  emon1.serialprint();

  float realPower       = emon1.realPower;        //extract Real Power into variable
  char realPowerStr[20]="";
  dtostrf(realPower,5,1,realPowerStr);
  //Serial.println(realPowerStr);
  fill_blanks(realPowerStr,realPower);
  //Serial.println(realPowerStr);
  //Serial.println(realPower);
  //Serial.println();
  
  float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
  char apparentPowerStr[20]="";
  dtostrf(apparentPower,5,1,apparentPowerStr);
  //Serial.println(apparentPowerStr);
  fill_blanks(apparentPowerStr,apparentPower);
  //Serial.println(apparentPowerStr);
  //Serial.println(apparentPower);
  //Serial.println();
  
  float powerFactor     = emon1.powerFactor;      //extract Power Factor into Variable
  char powerFactorStr[20]="";
  dtostrf(powerFactor,6,2,powerFactorStr);
  //Serial.println(powerFactorStr);
  fill_blanks(powerFactorStr,powerFactor);
  //Serial.println(powerFactorStr);
  //Serial.println(powerFactor);
  //Serial.println();
  
  float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
  char supplyVoltageStr[20]="";
  dtostrf(supplyVoltage,5,1,supplyVoltageStr);
  //Serial.println(supplyVoltageStr);
  fill_blanks(supplyVoltageStr,supplyVoltage);
  //Serial.println(supplyVoltageStr);
  //Serial.println(supplyVoltage);
  //Serial.println();
  
  float Irms            = emon1.Irms;             //extract Irms into Variable
  char IrmsStr[20]="";
  dtostrf(Irms,5,1,IrmsStr);
  //Serial.println(IrmsStr);
  fill_blanks(IrmsStr,Irms);
  //Serial.println(IrmsStr);
  //Serial.println(Irms);
  //Serial.println();
  

  // Reset watchdog
  wdt_reset();
  
  // Check connection to WiFi
  Serial.print(F("Checking WiFi connection ..."));
  if(!cc3000.checkConnected()){while(1){}}
  Serial.println(F("done."));

  // Send request
  Adafruit_CC3000_Client client = cc3000.connectTCP(ip, 80);
  if (client.connected()) {
    Serial.println(F("Connected to Umi server."));

    
    String webpage_option = "/";

    
    webpage_option = "/submit?";
    webpage_option = webpage_option+"rmscurrent="+IrmsStr;
    webpage_option=webpage_option+"&rmsvolt="+supplyVoltageStr;
    webpage_option=webpage_option+"&apparent="+apparentPowerStr;
    webpage_option=webpage_option+"&real="+realPowerStr;
    webpage_option=webpage_option+"&powerfactor="+powerFactorStr;
    
    Serial.println(webpage_option);
    
    delay(2000);
    
    int length = webpage_option.length();
    
    char option[length];
    
    webpage_option.toCharArray(option, length);
    
    client.fastrprint(F("GET "));
    client.fastrprint(option);
    client.fastrprint(F(" HTTP/1.1\r\n"));
    client.fastrprint(F("Host: ")); client.fastrprint(WEBSITE); client.fastrprint(F("\r\n"));
    client.fastrprint(F("\r\n"));
    client.println();
    
    // Reset watchdog
    
    // Send data
    /*
    Serial.print(F("Sending data ..."));
    client.fastrprintln(F(""));    
    Serial.println("\n"+data);
    sendData(client,data,buffer_size);  
    client.fastrprintln(F(""));
    Serial.println(F("done."));
    */
    // Reset watchdog
    wdt_reset();
    
  } else {
    Serial.println(F("Connection failed"));    
    return;
  }
  
  // Reset watchdog
  wdt_reset();
  
  Serial.println(F("Reading answer ..."));

  while (client.connected()) {
    while (client.available()) {
      char c = client.read();
      if(c == 35){
        char val = client.read();
        if(val == 48){
          Serial.println("light off");
        }else if(val == 49){
          Serial.println("light on");
        }
      }
    }
  }
  
  // Reset watchdog
  wdt_reset();
   
  // Close connection and disconnect
  client.close();
  Serial.println(F("Closing connection"));
  
  // Reset watchdog & disable
  wdt_reset();
  wdt_disable();
  
}

// Send data chunk by chunk
void sendData(Adafruit_CC3000_Client& client, String input, int chunkSize) {
  
  // Get String length
  int length = input.length();
  int max_iteration = (int)(length/chunkSize);
  
  for (int i = 0; i < length; i++) {
    client.print(input.substring(i*chunkSize, (i+1)*chunkSize));
    wdt_reset();
  }  
}

// Wait for a given time using the watchdog
void wait(int total_delay) {
  
  int number_steps = (int)(total_delay/5000);
  wdt_enable(WDTO_8S);
  for (int i = 0; i < number_steps; i++){
    delay(5000);
    wdt_reset();
  }
  wdt_disable();
}
