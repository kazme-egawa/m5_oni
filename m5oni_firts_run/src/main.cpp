#include <Arduino.h>
#include <M5Stack.h>
#include <ArduinoOSC.h>
#include <ESP8266Audio.h>

// WiFi stuff
const char *ssid = "m5_oni";
const char *pwd = "egaegapyonpyon";
const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

// for ArduinoOSC
const char *host = "192.168.1.255";
const int incomingPort = 8080; // 受信ポート番号
const int outgoingPort = 7070; // 送信ポート番号

#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

void playMP3(const char filename[])
{
  file = new AudioFileSourceSD(filename);
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S(0, 1);
  out->SetOutputModeMono(true);
  out->SetGain(1.0);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  while (mp3->isRunning())
  {
    if (!mp3->loop())
      mp3->stop();
  }
}

void rcv_oni(const OscMessage &msg)
{
  int oni = msg.arg<int>(0);
  if (oni == 1)
  {
    playMP3("/oni.mp3");

    M5.Lcd.drawJpgFile(SD, "/oni.JPG");
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.printf("ssid: %s", ssid);
    M5.Lcd.setCursor(0, 65);
    M5.Lcd.printf("incomingPort: %d", incomingPort);
    M5.Lcd.setCursor(0, 105);
    M5.Lcd.printf("outgoingPort: %d", outgoingPort);
    M5.Lcd.setCursor(0, 155);
    M5.Lcd.printf("Oni deeeeesu!!!");
  }
  else
  {
    playMP3("/run.mp3");

    M5.Lcd.drawJpgFile(SD, "/run.JPG");
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.printf("ssid: %s", ssid);
    M5.Lcd.setCursor(0, 65);
    M5.Lcd.printf("incomingPort: %d", incomingPort);
    M5.Lcd.setCursor(0, 105);
    M5.Lcd.printf("outgoingPort: %d", outgoingPort);
    M5.Lcd.setCursor(0, 155);
    M5.Lcd.printf("run!!!");
  }
  delay(5000);
}

void setup()
{
  M5.begin();
  M5.Power.begin();
  M5.Lcd.clearDisplay();
  M5.Lcd.drawJpgFile(SD, "/run.JPG");
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextSize(2);

#ifdef ESP_PLATFORM
  WiFi.disconnect(true, true);
  delay(1000);
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, pwd);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  M5.Lcd.setCursor(0, 20);
  M5.Lcd.printf("ssid: %s", ssid);
  M5.Lcd.setCursor(0, 65);
  M5.Lcd.printf("incomingPort: %d", incomingPort);
  M5.Lcd.setCursor(0, 105);
  M5.Lcd.printf("outgoingPort: %d", outgoingPort);
  M5.Lcd.setCursor(0, 155);
  M5.Lcd.printf("run!!!");
  delay(1000);
  playMP3("/run.mp3");

  OscWiFi.subscribe(incomingPort, "/oni", rcv_oni);
}

void loop()
{
  int rssi = 0;
  for (int i = 0; i < 100; i++)
  {
    rssi += WiFi.RSSI();
  }
  rssi = rssi / -100;
  M5.Lcd.setCursor(0, 195);
  M5.Lcd.printf("rssi: %d", rssi);
  OscWiFi.send(host, outgoingPort, "/rssi", rssi);
  OscWiFi.post();
  delay(200);
}
