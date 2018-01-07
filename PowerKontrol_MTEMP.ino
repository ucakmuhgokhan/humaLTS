/***************************************************************************************************************
 *  Guc Kontrol-> Prototype
 *  
 *  static const uint8_t D0   = 16;
static const uint8_t D1   = 5; //i2c scl
static const uint8_t D2   = 4; //i2c sda
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;//sck
static const uint8_t D6   = 12;//miso
static const uint8_t D7   = 13;//mosi
static const uint8_t D8   = 15;//ss
static const uint8_t D9   = 3;//rx0
static const uint8_t D10  = 1;//tx0

GPIO Pin  I/O Index Number
GPIO0  3
GPIO1  10
GPIO2  4
GPIO3  9
GPIO4  2
GPIO5  1
GPIO6  N/A
GPIO7  N/A
GPIO8  N/A
GPIO9  11
GPIO10   12
GPIO11   N/A
GPIO12   6
GPIO13   7
GPIO14   5
GPIO15   8
GPIO16   0
 ********************************************************************************************************************************/

/*NodeMCU*/
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*const char* ssid = "Kablonet Netmaster-A7A7-G";
const char* pass = "Golmuhendis2009.";*/

char ssid[] = "iPhone";
char pass[] = "golmuhendis";

#define relay_1 D6 // Hearth bit of node mcu  V4 makine 2
#define relay_2 D7 // Hearth bit of node mcu  V5 makine 1

#define analogVal A0

#define multiplexer1       D0
#define multiplexer2       D1
#define multiplexer3       D2

#define portOpen  HIGH
#define portClose LOW

#define ONE_WIRE_BUS D3
#define TEMPERATURE_PRECISION 9

#define stateLed D4  //j1 deki led bağlantı var ise yansın

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;
float tempC;
float temp1, temp2;

int relay_1Pos;
int relay_2Pos;

WidgetLED led1(V6);  // v5 makine 1 i temsileder 
WidgetLED led2(V7);  // v4 makine 2 yi temsil eder

char auth [] = "44262f60b4154361954094c23336219d"; // Blynk AUTH
BlynkTimer timer;

void changeMux(int c, int b, int a) {
  digitalWrite(multiplexer1, a);
  digitalWrite(multiplexer2, b);
  digitalWrite(multiplexer3, c);
}

BLYNK_WRITE(V4) //makine 2
{
  relay_1Pos = param.asInt();

  if(relay_1Pos)
  {
    digitalWrite(relay_1, LOW);
   // Blynk.notify("Röle 2 Acıldı");
    Serial.println("V4 acildi");
    led2.setValue(255);
  }
  else
  {
    digitalWrite(relay_1, HIGH);
   // Blynk.notify("Röle 2 Kapandı");
    Serial.println("V4 kapandi");
    led2.setValue(0);
  }
}

BLYNK_WRITE(V5) //makine 1
{
  relay_2Pos = param.asInt();
  
  Serial.println(relay_2Pos);
  if(relay_2Pos)
  {
    digitalWrite(relay_2, LOW); 
    Serial.println("V5 acildi");   
    led1.setValue(255);
  }
  else
  {
    digitalWrite(relay_2, HIGH);
    Serial.println("V5 kapandi");
    led1.setValue(0);
  }
  
}

BLYNK_READ(V1)  //metan
{
  changeMux(LOW, LOW, LOW);  
  int gg=analogRead(analogVal);
  Serial.print("metan :  ");
  Serial.println(gg);
  Blynk.virtualWrite(V1,analogRead(analogVal) ); 
}

BLYNK_READ(V10)  //co
{
  changeMux(LOW, LOW, HIGH);
  int gg=analogRead(analogVal);
  Serial.print("co :  ");
  Serial.println(gg);
  Blynk.virtualWrite(V10,analogRead(analogVal) ); 
}

BLYNK_READ(V11)  //co2
{
  changeMux(LOW, HIGH, LOW);
  int gg=analogRead(analogVal);
  Serial.print("co2 :  ");
  Serial.println(gg);
  Blynk.virtualWrite(V11,analogRead(analogVal) );  
}

BLYNK_READ(V13) //s2o
{
  changeMux(LOW, HIGH, HIGH);
  int gg=analogRead(analogVal);
  Serial.print("s2o :  ");
  Serial.println(gg);
  Blynk.virtualWrite(V13,analogRead(analogVal) );  
}


void notifyUptime()
{
 // long uptime = millis() / 60000L;

  // Actually send the message.
  // Note:
  //   We allow 1 notification per 15 seconds for now.
//  Blynk.notify(uptime + " dakidır çalışıyor.");
  Blynk.virtualWrite(V0, temp1);  // ortam sıcaklığı
  Blynk.virtualWrite(V12, temp2); //sicaklik 1
  Blynk.virtualWrite(V14, millis()/1000); //sicaklik 2
  Blynk.virtualWrite(V15, millis()/500); //sicaklik 3
  Blynk.virtualWrite(V2, millis()/800); //titreşim

  Serial.println("datalar");
}

void setup () 
{
  pinMode(stateLed OUTPUT);
  digitalWrite(stateLed, HIGH);
  delay(500);
  digitalWrite(stateLed, LOW);
  delay(500);
  digitalWrite(stateLed, HIGH);
  delay(500);
  digitalWrite(stateLed, LOW);
  
  Serial.begin(115200);   
  timer.setInterval(60000L, notifyUptime);  //15 sn
  //timer.setInterval(10000, notifyUptime);
  Serial.println("Baglaniyor...");
  digitalWrite(stateLed, LOW);
  Blynk.begin(auth, ssid, pass);
  Serial.println("Baglandi");
  digitalWrite(stateLed, HIGH);

  pinMode(relay_1, OUTPUT);
  digitalWrite(relay_1, HIGH);
  pinMode(relay_2, OUTPUT);
  digitalWrite(relay_2, HIGH);
  delay(1000);
  
  pinMode(analogVal, INPUT); 
  
  pinMode(multiplexer1, OUTPUT);
  pinMode(multiplexer2, OUTPUT);
  pinMode(multiplexer3, OUTPUT);

  sensors.begin();
  Serial.print("Bulunan sensorler...");
  Serial.print("Bulunan sensor sayisi ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" cihaz.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Sensor 0 adresi bulunamadi"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Sensor 1 adresi bulunamadi");

  // show the addresses we found on the bus
  Serial.print("Sensor 0 adres: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Sensor 1 adres: ");
  printAddress(outsideThermometer);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  Serial.print("Sensor 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();

  Serial.print("Sensor 1 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC); 
  Serial.println();

  led1.setValue(0);
  led2.setValue(0);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}


void loop() 
{
  Blynk.run();
  timer.run();
  //  Blynk.notify("fff");

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  printData(insideThermometer);
  temp1 = tempC;
  printData(outsideThermometer);
  temp2 = tempC;
  delay (100);
}

