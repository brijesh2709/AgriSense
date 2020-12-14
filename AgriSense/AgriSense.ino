#include "WiFiEsp.h"
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

#define SOIL_MOISTURE_PIN A0
#define LDR_PIN A1
#define ThermistorPin A2
const int buttonPin = 5;    
#define motorPin A5

float avg[3]={0,0,0};

char ssid[] = "..SSID.."; 
char pass[] = "..PASSWORD..";
int status = WL_IDLE_STATUS;
char server[] =  "..SERVER IP..";

char soilMoist[50];
char ldrSense[50];
char tempVal[50];
char get_request[200];
int buttonState = 0;        

WiFiEspClient client;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  WiFi.init(&Serial1);

  pinMode(motorPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  
  Serial.println("You're connected to the network");
  printWifiStatus();
}

void loop() {
  Serial.println();
  buttonState = digitalRead(buttonPin);
  float soilMoisturePer = getMoisturePercentage();
  String soilMoist_str = String(soilMoisturePer);
  int soilMoist_str_len = soilMoist_str.length();
  soilMoist_str.toCharArray(soilMoist, soilMoist_str_len);

  float LDRValPer = getLightPercentage();
  String ldrSense_str = String(LDRValPer);
  int ldrSense_str_len = ldrSense_str.length();
  ldrSense_str.toCharArray(ldrSense, ldrSense_str_len);

  float tempValPer = getTemperature();
  String tempVal_str = String(tempValPer);
  int tempVal_str_len = tempVal_str.length();
  tempVal_str.toCharArray(tempVal, tempVal_str_len);

  if (!client.connected()){
  Serial.println("Starting connection to server...");
  client.connect(server, 5000);
  }
  Serial.println("Connected to server");
  
  sprintf(get_request,"GET /data?soilMoist=%s&ldrSense=%s&tempVal=%s HTTP/1.1\r\nHost: 18.221.147.67\r\nConnection: close\r\n\r\n\n", soilMoist, ldrSense, tempVal);
  client.print(get_request);
  delay(500);

  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  if (buttonState == HIGH) {
      digitalWrite(motorPin, HIGH);
      delay(120000);
  }
  else {
       digitalWrite(motorPin, LOW);
  }

   if(soilMoisturePer > 80.00)
  {
    digitalWrite(motorPin, LOW);
  }
  
  delay(10000); 
}

void printWifiStatus()
{
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    long rssi = WiFi.RSSI();
    Serial.print("Signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

float getMoisturePercentage(void)
{
  float moisture_percentage;
  int sensor_analog;
  sensor_analog = analogRead(SOIL_MOISTURE_PIN);
  moisture_percentage = ( 100 - ( (sensor_analog/4096.00) * 100 ) );
  moisture_percentage = (float)moisture_percentage;
  return moisture_percentage;
}

float getLightPercentage(void)
{
  int ldrRawVal;
  float percentage;
  ldrRawVal = analogRead(LDR_PIN);    
  percentage = (float(ldrRawVal)*100)/4096;
  return percentage;
}

float getTemperature(void)
{
  int R1 = 10000
  float C1 = 1.009249522e-03
  float C2 = 2.378405444e-04
  float C3 = 2.019202697e-07
  static int avgArrayIndex=0;
  int Vo;
  float logR2,R2, T, Tc;
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (4096.0 / float(Vo) - 1.0);
  logR2 = log(R2);
  T = (1.0 / (C1 + C2*logR2 + C3*logR2*logR2*logR2));
  Tc = T - 273.15;
  avg[avgArrayIndex++] = Tc;
  if(avgArrayIndex > 2)
  avgArrayIndex = 0;
  Tc = (avg[0] + avg[1] + avg[2])/3;
  return Tc;
}
