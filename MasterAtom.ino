#include <WiFi.h>

WiFiServer server(80);                // Set a object server as a WiFiServer class
IPAddress IP(192,168,4,15);           // Select ip and mask
IPAddress mask = (255, 255, 255, 0);

// The devices that want to connect to our Access Point will need to use the next credentials.
char ssid[] = "WeatherStation";           // SSID for the Access Point
char pass[] = "ilikeweather";         // password for the Access Point

byte EspLed = 2;                      // Built in lED on the ESP32

void setup() {

  Serial.begin(115200);

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

void loop() {

  WiFiClient client = server.available(); // Return a client object to the class if there is a client available
  if (!client) {return;}                  // Return cuts the function (loop) if client class is not connected

  digitalWrite(EspLed, LOW);

  String IBMPTempC = client.readStringUntil('\r');
  String IBMPPres = client.readStringUntil('\r');
  String IDHTTempC = client.readStringUntil('\r');
  String IDHTHum = client.readStringUntil('\r');
  String IRainVal = client.readStringUntil('\r');

  double BMPTempC=IBMPTempC.toDouble();
  double BMPPresPa=IBMPPres.toDouble();
  double DHTTempC = IDHTTempC.toDouble();
  double DHTHum = IDHTHum.toDouble();
  double RainVal = IRainVal.toDouble();

  //C to F conversions
  double TempC=(DHTTempC+BMPTempC)/2;
  double TempF=TempC*9/5+32;

  //Pressure conversion
  double PresATM = BMPPresPa/101325;

  //Print
    Serial.print("Temperature BMP = ");
    Serial.println(BMPTempC);
    Serial.print("Temperature DHT = ");
    Serial.println(DHTTempC);

    Serial.print("Average of Sensors Temperature = ");
    Serial.print(TempC);
    Serial.println(" *C");
    Serial.print("Temperature = ");
    Serial.print(TempF);
    Serial.println(" *F");

    Serial.print("Humidity = ");
    Serial.print(DHTHum);
    Serial.println("%");

    Serial.print("Pressure = ");
    Serial.print(BMPPresPa);
    Serial.println(" Pa");

    Serial.print("Pressure = ");
    Serial.print(PresATM);
    Serial.println(" atm");


    //Rain If Else
    Serial.print(RainVal);
    Serial.print("Rain: ");
     if(RainVal < 1600){
    Serial.println(" Yes ");

    }
    else {
    Serial.println(" No ");
     }

    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  digitalWrite(EspLed, HIGH);
}
