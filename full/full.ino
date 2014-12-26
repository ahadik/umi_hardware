//includes for Transmitter
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <avr/wdt.h>

//includes for display
#include <inttypes.h>
#include <ctype.h>

// required libraries
#include <SPI.h>
#include <FLASH.h>
#include <EPD.h>
#include <S5813A.h>

//definitions for transmitter
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, 
ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2);

#define WLAN_SSID       "Umi Network"           // cannot be longer than 32 characters!
#define WLAN_PASS       "uminetwork"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

int buffer_size = 20;

#define IDLE_TIMEOUT_MS  10000

#define WEBSITE      "umi-webapp.herokuapp.com"

long randNumber;

//definitions for display
// Change this for different display size
// supported sizes: 144 200 270
#define SCREEN_SIZE 200

// select two images from:  text_image text-hello cat aphrodite venus saturn
#define IMAGE_1  instantpower
#define IMAGE_2  costhour
#define IMAGE_3 solarcharge

// set up images from screen size2
#if (SCREEN_SIZE == 144)
#define EPD_SIZE EPD_1_44
#define FILE_SUFFIX _1_44.xbm
#define NAME_SUFFIX _1_44_bits

#elif (SCREEN_SIZE == 200)
#define EPD_SIZE EPD_2_0
#define FILE_SUFFIX _2_0.xbm
#define NAME_SUFFIX _2_0_bits

#elif (SCREEN_SIZE == 270)
#define EPD_SIZE EPD_2_7
#define FILE_SUFFIX _2_7.xbm
#define NAME_SUFFIX _2_7_bits

#else
#error "Unknown EPB size: Change the #define SCREEN_SIZE to a supported value"
#endif

// Error message for MSP430
#if (SCREEN_SIZE == 270) && defined(__MSP430_CPU__)
#error MSP430: not enough memory
#endif

// no futher changed below this point

// current version number
#define DEMO_VERSION "2"


// pre-processor convert to string
#define MAKE_STRING1(X) #X
#define MAKE_STRING(X) MAKE_STRING1(X)

// other pre-processor magic
// tiken joining and computing the string for #include
#define ID(X) X
#define MAKE_NAME1(X,Y) ID(X##Y)
#define MAKE_NAME(X,Y) MAKE_NAME1(X,Y)
#define MAKE_JOIN(X,Y) MAKE_STRING(MAKE_NAME(X,Y))

// calculate the include name and variable names
#define IMAGE_1_FILE MAKE_JOIN(IMAGE_1,FILE_SUFFIX)
#define IMAGE_1_BITS MAKE_NAME(IMAGE_1,NAME_SUFFIX)
#define IMAGE_2_FILE MAKE_JOIN(IMAGE_2,FILE_SUFFIX)
#define IMAGE_2_BITS MAKE_NAME(IMAGE_2,NAME_SUFFIX)
#define IMAGE_3_FILE MAKE_JOIN(IMAGE_3,FILE_SUFFIX)
#define IMAGE_3_BITS MAKE_NAME(IMAGE_3,NAME_SUFFIX)


// Add Images library to compiler path
#include <Images.h>  // this is just an empty file

// images
PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_1_FILE
#undef char
#undef unsigned

PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_2_FILE
#undef char
#undef unsigned

PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_3_FILE
#undef char
#undef unsigned


#if defined(__MSP430_CPU__)

// TI LaunchPad IO layout
const int Pin_TEMPERATURE = A4;
const int Pin_PANEL_ON = P2_3;
const int Pin_BORDER = P2_5;
const int Pin_DISCHARGE = P2_4;
const int Pin_PWM = P2_1;
const int Pin_RESET = P2_2;
const int Pin_BUSY = P2_0;
const int Pin_EPD_CS = P2_6;
const int Pin_FLASH_CS = P2_7;
const int Pin_SW2 = P1_3;
const int Pin_RED_LED = P1_0;

#else

// Arduino IO layout
/*
const int Pin_TEMPERATURE = A0;
const int Pin_PANEL_ON = 2;
const int Pin_BORDER = 3;
const int Pin_DISCHARGE = 4;
const int Pin_PWM = 5;
const int Pin_RESET = 6;
const int Pin_BUSY = 7;
const int Pin_EPD_CS = 8;
const int Pin_FLASH_CS = 9;
const int Pin_SW2 = 12;
const int Pin_RED_LED = 13;
*/


const int Pin_TEMPERATURE = A8;
const int Pin_PANEL_ON = 48;
const int Pin_BORDER = 3;
const int Pin_DISCHARGE = 46;
const int Pin_PWM = 5;
const int Pin_RESET = 6;
const int Pin_BUSY = 7;
const int Pin_EPD_CS =44;
const int Pin_FLASH_CS = 9;
const int Pin_SW2 = 50;
const int Pin_RED_LED = 52;

#endif


// LED anode through resistor to I/O pin
// LED cathode to Ground
#define LED_ON  HIGH
#define LED_OFF LOW


// define the E-Ink display
EPD_Class EPD(EPD_SIZE, Pin_PANEL_ON, Pin_BORDER, Pin_DISCHARGE, Pin_PWM, Pin_RESET, Pin_BUSY, Pin_EPD_CS);


// I/O setup
void setup() {
  
        //setup for Transmitter
        randomSeed(analogRead(0));
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
  
	pinMode(Pin_RED_LED, OUTPUT);
	pinMode(Pin_SW2, INPUT);
	pinMode(Pin_TEMPERATURE, INPUT);
	pinMode(Pin_PWM, OUTPUT);
	pinMode(Pin_BUSY, INPUT);
	pinMode(Pin_RESET, OUTPUT);
	pinMode(Pin_PANEL_ON, OUTPUT);
	pinMode(Pin_DISCHARGE, OUTPUT);
	pinMode(Pin_BORDER, OUTPUT);
	pinMode(Pin_EPD_CS, OUTPUT);
	pinMode(Pin_FLASH_CS, OUTPUT);

	digitalWrite(Pin_RED_LED, LOW);
	digitalWrite(Pin_PWM, LOW);
	digitalWrite(Pin_RESET, LOW);
	digitalWrite(Pin_PANEL_ON, LOW);
	digitalWrite(Pin_DISCHARGE, LOW);
	digitalWrite(Pin_BORDER, LOW);
	digitalWrite(Pin_EPD_CS, LOW);
	digitalWrite(Pin_FLASH_CS, HIGH);

#if defined(__AVR__)
	// wait for USB CDC serial port to connect.  Arduino Leonardo only
	while (!Serial) {
	}
	delay(20);  // allows terminal time to sync
#endif
	Serial.println();
	Serial.println();
	Serial.println("Demo version: " DEMO_VERSION);
	Serial.println("Display: " MAKE_STRING(EPD_SIZE));
	Serial.println();

	FLASH.begin(Pin_FLASH_CS);
	if (FLASH.available()) {
		Serial.println("FLASH chip detected OK");
	} else {
		uint8_t maufacturer;
		uint16_t device;
		FLASH.info(&maufacturer, &device);
		Serial.print("unsupported FLASH chip: MFG: 0x");
		Serial.print(maufacturer, HEX);
		Serial.print("  device: 0x");
		Serial.print(device, HEX);
		Serial.println();
	}

	// configure temperature sensor
	S5813A.begin(Pin_TEMPERATURE);

}


static int state = 0;


// main loop
void loop() {
	int temperature = S5813A.read();
	Serial.print("Temperature = ");
	Serial.print(temperature);
	Serial.println(" Celcius");

	EPD.begin(); // power up the EPD panel
	EPD.setFactor(temperature); // adjust for current temperature

	int delay_counts = 50;
	switch(state) {
	default:
	case 0:         // clear the screen
		EPD.clear();
		state = 1;
		delay_counts = 5;  // reduce delay so first image come up quickly
		break;

	case 1:         // clear -> power
		EPD.image(IMAGE_1_BITS);
		++state;
		break;

	case 2:         // power -> cost
		EPD.image(IMAGE_1_BITS, IMAGE_2_BITS);
		++state;
		break;
        
        case 3:        // cost -> solar
                EPD.image(IMAGE_2_BITS, IMAGE_3_BITS);
		++state;
		break;

	case 4:        // solar -> power
		EPD.image(IMAGE_3_BITS, IMAGE_1_BITS);
		state = 2;  // backe to picture nex time
		break;
	}
	EPD.end();   // power down the EPD panel

	// flash LED for 5 seconds
	for (int x = 0; x < delay_counts; ++x) {
		digitalWrite(Pin_RED_LED, LED_ON);
		delay(50);
		digitalWrite(Pin_RED_LED, LED_OFF);
		delay(50);
	}

        //start connection and transmition
        
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
        
      
      
        // Prepare JSON for Xively & get length
        int length = 0;
        int rmscurrent = random(300);
        int rmsvolt = random(300);
        int apparent = random(300);
        int real = random(300);
        int powerfactor = random(300);
      
        // JSON data
        String data = "";
        data = data + "\n{\"rmscurrent\":"+String(rmscurrent)
        +", \"rmsvolt\":"+String(rmsvolt)
        +",\"apparent\":"+String(apparent)
        +",\"real\":"+String(real)
        +",\"powerfactor\":"+String(powerfactor)+"}";
        
        // Get length
        length = data.length();
       
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
          randNumber = random(300);
          webpage_option = "/?";
          webpage_option = webpage_option+"rmscurrent="+randNumber;
          randNumber = random(300);
          webpage_option=webpage_option+"&rmsvolt="+randNumber;
          randNumber = random(300);
          webpage_option=webpage_option+"&apparent="+randNumber;
          randNumber = random(300);
          webpage_option=webpage_option+"&real="+randNumber;
          randNumber = random(300);
          webpage_option=webpage_option+"&powerfactor="+randNumber;
          
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
          Serial.print(F("Sending data ..."));
          client.fastrprintln(F(""));    
          Serial.println("\n"+data);
          sendData(client,data,buffer_size);  
          client.fastrprintln(F(""));
          Serial.println(F("done."));
          
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
            Serial.print(c);
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
