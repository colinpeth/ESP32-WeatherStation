#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_BMP085.h> 
Adafruit_BMP085 bmp;
#include <dht.h>
#define dataPin 15 // Defines pin number to which the sensor is connected
dht DHT; // Creats a DHT object
int rainPin = 2;
int thresholdValueOfRain = 1400;
char ssid[] = "WeatherStation";           // SSID of your AP
char pass[] = "ilikeweather";         // password of your AP
IPAddress server(192,168,4,15);     // IP address of the AP
WiFiClient client;
void setup() 
{
  Serial.begin(115200);
  if (!bmp.begin()) 
  {
  Serial.println("BMP180 sensor not found");
  while (1) {}
  }
   WiFi.mode(WIFI_STA);              // Wifi as client mode
  WiFi.begin(ssid, pass);           // Credentials to connect to the Access Point
  
  Serial.println("Connection to the AP");    
  while (WiFi.status() != WL_CONNECTED) {       
    Serial.print(".");
    delay(500);
  
  }
  Serial.println("Connected");
  Serial.print("LocalIP:");          
  Serial.println(WiFi.localIP());               
  Serial.println("MAC:" + WiFi.macAddress());
  Serial.print("Gateway:"); 
  Serial.println(WiFi.gatewayIP());
  Serial.print("AP MAC:"); 
  Serial.println(WiFi.BSSIDstr());

}
 
void loop() {
    
  

    //DHT11
    int readData = DHT.read22(dataPin); // Reads the data from the sensor
    float DHTtempC = DHT.temperature; // Gets the values of the temperature
    float DHThum = DHT.humidity-20; // Gets the values of the humidity

    //BMP180 Temp and Pressure
    double BMPtempC=bmp.readTemperature();//Gets Temp from BMP
    double BMPPres=bmp.readPressure(); //Gets Altitude from BMP
    
    //Temp Average and F conversion
    double TempC=(DHTtempC+BMPtempC)/2;
    double TempF=TempC*9/5+32;

    //Rain Sensor
    int RainVal = digitalRead(rainPin);
    
    
    Serial.print("Temperature BMP = ");
    Serial.println(BMPtempC);
    Serial.print("Temperature DHT = ");
    Serial.println(DHTtempC);
    
    Serial.print("Average of Sensors Temperature = ");
    Serial.print(TempC);
    Serial.print(" *C");
    Serial.println("Temperature = ");
    Serial.print(TempF);
    Serial.println(" *F");

    Serial.print("Humidity = ");
    Serial.print(DHThum);
    Serial.println("%");
    
    Serial.print("Pressure = ");
    Serial.print(BMPPres);
    Serial.println(" Pa");

    Serial.print("Rain Val: ");
    Serial.println(RainVal);


 

     if (!client.connect(server, 80)) {          // Connect to the server through the port 80
          Serial.println("connection failed");  // If server not connected run connection again
          return;                                 // return terminates the function (loop)
      }
  
    client.print(BMPtempC);
    client.print('\r');
    client.print(BMPPres);
    client.print('\r');
    client.print(DHTtempC);
    client.print('\r');
    client.print(DHThum);
    client.print('\r');
    client.print(RainVal);
    client.print('\r');
    
    
  String answer = client.readStringUntil('\r');   // Receive value from the server until \r
  Serial.println(answer);
 // client.flush();
  client.stop();
    
}
