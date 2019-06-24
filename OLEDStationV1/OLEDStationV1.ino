#include <WiFi.h>
#include <SPI.h>
#include <SSD_13XX.h>
WiFiServer server(80);                // Set a object server as a WiFiServer class
IPAddress IP(192,168,4,15);           // Select ip and mask
IPAddress mask = (255, 255, 255, 0);

// The devices that want to connect to our Access Point will need to use the next credentials.
char ssid[] = "WeatherStation";           // SSID for the Access Point 
char pass[] = "ilikeweather";         // password for the Access Point

byte EspLed = 2;                      // Built in lED on the ESP32

// Screen Pin define
#define _cs   17  // goes to TFT CS
#define _dc   16  // goes to TFT DC
#define _mosi 23  // goes to TFT MOSI
#define _sclk 18  // goes to TFT SCK/CLK
#define _rst  5   // ESP RST to TFT RESET
#define _miso     // Not connected
     
uint8_t errorCode = 0;

SSD_13XX tft = SSD_13XX(_cs, _dc);

//Variable for printing to the screen
//Creates Global Vairables for the Screen
  String SleepCheck;
  String IBMPTempC;
  String IBMPPres;
  String IDHTTempC;
  String IDHTHum;
  String IRainVal;
    
  double BMPTempC;
  double BMPPresPa;
  double DHTTempC;
  double DHTHum;
  double RainVal;

  //C to F conversions
  double TempC;
  double TempF;

  //Pressure conversion
  double PresATM;

  
void setup() {
Serial.begin(115200);
long unsigned debug_start = millis();
while (!Serial && ((millis() - debug_start) <= 5000));
  tft.begin(false);
  delay(30);
  
  //the following it's mainly for Teensy
  //it will help you to understand if you have choosed the
  //wrong combination of pins!
  errorCode = tft.getErrorCode();
  if (errorCode != 0) {
	  Serial.print("Init error! ");
	  if (bitRead(errorCode, 0)) Serial.print("MOSI or SCLK pin mismach!\n");
	  if (bitRead(errorCode, 1)) Serial.print("CS or DC pin mismach!\n");
  }
  else {
	  Serial.println(F("Benchmark Sketch V1.1"));
	  tft.setTextColor(YELLOW);
	  tft.setTextScale(2);
  }

  //Wifi declaration
  
  WiFi.mode(WIFI_AP);                     // Set Wi-Fi as access point/server
  WiFi.softAP(ssid, pass);                // SSID and Password for the AP
  WiFi.softAPConfig(IP, IP, mask);        // Set our own desired IP address
  server.begin();                         // Begin the server
  
  pinMode(EspLed, OUTPUT);                // Set Built-in LED as output

  Serial.println("Server started.");
  Serial.print("IP: ");     
  Serial.println(WiFi.softAPIP());        // .softAPIP calls for the IP of the access point which we set earlier
  Serial.print("MAC:");     
  Serial.println(WiFi.softAPmacAddress());  // Calls for the mac address
}


void loop(void) {
  
  WiFiClient client = server.available(); // Return a client object to the class if there is a client available
  if (!client) {return;}                  // Return cuts the function (loop) if client class is not connected
  
  digitalWrite(EspLed, LOW); 
 
    
  IBMPTempC = client.readStringUntil('\r');
  IBMPPres = client.readStringUntil('\r');
  IDHTTempC = client.readStringUntil('\r');
  IDHTHum = client.readStringUntil('\r');
  IRainVal = client.readStringUntil('\r');
    
  BMPTempC=IBMPTempC.toDouble();
  BMPPresPa=IBMPPres.toDouble();
  DHTTempC = IDHTTempC.toDouble();
  DHTHum = IDHTHum.toDouble();
  RainVal = IRainVal.toDouble();
  //C to F conversions
  TempC=(DHTTempC+BMPTempC)/2;
  TempF=TempC*9/5+32;

  //Pressure conversion
  PresATM = BMPPresPa/101325;


  //Display Command
	if (errorCode == 0) {
	
			Serial.print(F("Text                     "));
			Serial.println(testText());
			delay(500);
		}
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  
  //unsigned long start = micros();
  tft.setBrightness(100);
  tft.setCursor(0, 0);
  if (SleepCheck=="YES"){tft.setTextColor(CYAN);}
  else {tft.setTextColor(WHITE);}
  tft.setTextScale(1.5);
  tft.println("ESP32 Weather Station");

  //Temp color change
  if(TempF>80){tft.setTextColor(RED);}
  else if (TempF>60){tft.setTextColor(GREEN);}
  else {tft.setTextColor(BLUE);}
  tft.setTextScale(2);

  tft.print("Temp: ");
  tft.print(TempF);
  tft.println("*F");
  //tft.print("Temp: ");
  //tft.print(TempC);
  //tft.println("*C");
  tft.setTextColor(BLUE);    
  //tft.setTextScale(1.5);
  tft.print("Hum: ");
  tft.print(DHTHum);
  tft.println("%");
  tft.setTextColor(WHITE);
  //tft.setTextScale(1.5);
  tft.println("Pressure:");
  //tft.setTextScale(1);
  tft.print(BMPPresPa);
  tft.println(" Pa");
  tft.print(PresATM);
  tft.println(" atm");
 // tft.setTextScale(1.5);
  tft.setTextColor(ORANGE);
  tft.print("Rain: ");
  if (RainVal==0){tft.println("YES");}
  else {tft.println("NO");}
 
  
  
 // return micros() - start;
}
