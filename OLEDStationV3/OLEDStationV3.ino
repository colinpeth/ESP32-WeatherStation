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
  float batteryLevel;
  float voltage;
  float batteryPercent;
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
	  Serial.println(F("OLEDBaseStation: V3"));
	  tft.setTextColor(YELLOW);
	  tft.setTextScale(2);
  }
  tft.clearScreen();

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
  if (!client) {Serial.println("Not Connected"); return;}// Return cuts the function (loop) if client class is not connected
  
  digitalWrite(EspLed, LOW); 
 
    
  IBMPTempC = client.readStringUntil('\r');
  IBMPPres = client.readStringUntil('\r');
  IDHTTempC = client.readStringUntil('\r');
  IDHTHum = client.readStringUntil('\r');
  IRainVal = client.readStringUntil('\r');
  batteryLevel= (client.readStringUntil('\r')).toDouble();
  voltage= (client.readStringUntil('\r')).toDouble();
  batteryPercent=(client.readStringUntil('\r')).toDouble();
    
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

  //Prevents False Readings
  if(TempF>120){return;}

  //Display Command
	if (errorCode == 0) {
	
			Serial.print(F("Text                     "));
			Serial.println(testText());
      delay(1000);

      Serial.println(batteryLevel);
      Serial.println(voltage);
      Serial.println(batteryPercent);
		}
}

unsigned long testText() {
  
  unsigned long start = micros();
  
  tft.setBrightness(100);
  tft.setCursor(0, 0);
  if (SleepCheck=="YES"){tft.setTextColor(CYAN);}
  else {tft.setTextColor(WHITE);}
  tft.setTextScale(1.5);
  tft.println("ESP32 Weather Station");

  //Temp color Change and Temp Print
  if(TempF>80){tft.setTextColor(RED);}
  else if (TempF>60){tft.setTextColor(GREEN);}
  else {tft.setTextColor(BLUE);}
  tft.setTextScale(2);
  tft.setCursor(0,8);
  tft.println("Temp: ");
  tft.setCursor(50,10);
  tft.fillRect(50, 8, 202, 17, BLACK); //Draws box around numbers and sets to black before print
  tft.print(TempF);
  tft.println("*F");

  //Humidity print
  tft.setCursor(0,25);
  tft.setTextColor(PURPLE);
  tft.print("Hum: ");
  tft.fillRect(40,25,210,15,BLACK);
  tft.print(DHTHum);
  tft.println("%");

 
  //Pressure Print
  tft.setCursor(0,40);
  tft.setTextColor(WHITE);
  tft.setTextScale(2);
  tft.print("Pres:");
  tft.fillRect(40,40,210,15,BLACK);
  tft.print(PresATM);
  tft.println(" atm");

  //Rain Print
  tft.setCursor(0,55);
  tft.setTextColor(ORANGE);
  tft.print("Rain: ");
  tft.fillRect(40,55,210,15,BLACK);
  if (RainVal==0){tft.println("YES");}
  else {tft.println("NO");}

  tft.setCursor(0,70);
  tft.setTextColor(WHITE);
  tft.print("Batt: ");
  tft.fillRect(40,70,210,15,BLACK);
  tft.print(batteryPercent);
  tft.println("%");
 
}
