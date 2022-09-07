#include <M5Core2.h>
#include <WiFi.h>

/*----------------------------------------------------------
    MH-Z19 CO2 sensor for M5core2 and send ambient
  ----------------------------------------------------------*/
#include "Ambient.h"
#include <MHZ19_uart.h>

WiFiClient client;
Ambient ambient;

unsigned int channelId = ; // AmbientのチャネルID
const char* writeKey = ""; // ライトキー

const int rx_pin = 32; //Serial rx pin no
const int tx_pin = 33; //Serial tx pin no

unsigned long cnt = 0; 

MHZ19_uart mhz19;

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  setup
  ----------------------------------------------------------*/
void setup()
{
  WiFi.begin("", "");
  Serial.begin(115200);

  M5.begin(true, true, true, false);  //grove端子がcore2だとI2C用のため
  M5.Lcd.setBrightness(10);
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10,  110);
  M5.Lcd.println("Hello. This is CO2 sensor");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.println("WIFI is connected!");
  M5.Lcd.print("WIFI is connected!");
  ambient.begin(channelId, writeKey, &client);
  delay(500);
  Serial.println("MH-Z19 is warming up now.");

  mhz19.begin(rx_pin, tx_pin);
  mhz19.setAutoCalibration(false);

  delay(10 * 1000); 

  M5.Lcd.clear(WHITE);
}

/*----------------------------------------------------------
    MH-Z19 CO2 sensor loop
  ----------------------------------------------------------*/
void loop()
{
  int co2ppm = mhz19.getCO2PPM();
  int temp = mhz19.getTemperature();

  unsigned long program_start = millis();

  if(program_start - cnt > 30 *1000)
  {
  cnt = program_start ;  
  
  ambient.set(1, String(co2ppm).c_str());
  ambient.set(2, String(temp).c_str());
  ambient.send();
  
  Serial.println("send Ambient!");
  }
  else
  {
  Serial.print("co2: ");
  Serial.println(co2ppm);
  Serial.print("temp: ");
  Serial.println(temp);

  M5.Lcd.setCursor(20, 20);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(5);
  M5.Lcd.print("CO2");

  M5.Lcd.setCursor(48 , 60);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(5);
  M5.Lcd.print(String(co2ppm) + " ppm");

  M5.Lcd.setCursor(20 , 140);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(5);
  M5.Lcd.print("temp");

  M5.Lcd.setCursor(48, 180);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(5);
  M5.Lcd.print(String(temp) + " C");
  
  delay(5000);

  M5.Lcd.fillScreen((co2ppm < 1000 ? WHITE : (co2ppm >= 1500 ? RED : YELLOW)));
  }
}
