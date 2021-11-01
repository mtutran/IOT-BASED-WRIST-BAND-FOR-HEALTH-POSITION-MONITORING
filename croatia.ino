#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <DFRobot_MAX30102.h>
#include <ESP8266WiFi.h>

DFRobot_MAX30102 particleSensor;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32 
#define OLED_RESET    -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char* ssid = "K1"; 
const char* password = "12345678";

WiFiClient client;

// ThingSpeak Settings
const int channelID = YOUR-CHANNEL-ID; 
String writeAPIKey = "Q0MMQ6M6OSJHRQRU"; 
const char* server = "api.thingspeak.com";
const int postingInterval = 2 * 1000; 

void setup() {  
//  Serial.begin(9600);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  display.display();

  mlx.begin(); 

  while (!particleSensor.begin()) {
//    Serial.println("MAX30102 was not found");
  delay(1000);
  }
  particleSensor.sensorConfiguration(50, SAMPLEAVG_4, MODE_MULTILED, SAMPLERATE_100,PULSEWIDTH_411, ADCRANGE_16384);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
    delay(100);
    }
//  Serial.println("\r\nWiFi connected");
}

int32_t SPO2; 
int8_t SPO2Valid; 
int32_t heartRate;
int8_t heartRateValid;
float temp;

void loop() {
  
  particleSensor.heartrateAndOxygenSaturation(&SPO2, &SPO2Valid, &heartRate, &heartRateValid);
  temp = mlx.readObjectTempF();
  
  display.clearDisplay();
  
  display.setTextSize(1);   
                   
  display.setTextColor(WHITE);             
  display.setCursor(0,4);                
  display.println("Temp"); 
  display.setCursor(45,4);
  display.println(temp, 1);
  display.setCursor(100,4);
  display.println("F");
                             
  display.setCursor(0,18);                
  display.println("Heart"); 
  display.setCursor(50,18);
  display.println(heartRate,DEC);
  display.setCursor(100,18);
  display.println("bps");

  display.setCursor(0,32);                
  display.println("spO2"); 
  display.setCursor(50,32);
  display.println(SPO2,DEC);
  display.setCursor(110,32);

  display.display();
  
  delay(1000);


  if (client.connect(server, 80)) {
    String body = "&field1=" + String(temp, 1) + "&field2=" + String(heartRate, 1)+ "&field3=" + String(SPO2, 1);
  
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(body.length());
    client.print("\n\n");
    client.print(body);
    client.print("\n\n");
  }
  client.stop();
}
