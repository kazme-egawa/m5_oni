#include <Arduino.h>
#include <M5Stack.h>
#include <Ethernet.h>
#include <ArduinoOSC.h>
#include <ESP8266Audio.h>

bool ONI = true;
const int THRESH = 20;

//setup for hosting wifi OSC
const char *ssid = "m5_oni";         // SSID
const char *pass = "egaegapyonpyon"; // password
const int incomingPort = 7070;       // 受信ポート番号
const int outgoingPort = 8080;       // 送信ポート番号

const IPAddress ipServer(192, 168, 1, 1);   // server IPアドレス
const IPAddress ipGateway(192, 168, 1, 1);  // gateway IPアドレス
const IPAddress subnet(255, 255, 255, 0);   // サブネットマスク
const IPAddress ipClient(192, 168, 1, 255); // client IPアドレス

const char *host = "192.168.1.255";

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

void rcv_rssi(const OscMessage &msg)
{
  int rssi = msg.arg<int>(0);

  M5.Lcd.setCursor(0, 195);
  M5.Lcd.printf("rssi: %d", rssi);

  if (rssi < THRESH)
  {
    if (ONI)
    {
      OscWiFi.send(host, outgoingPort, "/oni", 1);
      OscWiFi.post();
      ONI = false;
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
    else
    {
      OscWiFi.send(host, outgoingPort, "/oni", 0);
      OscWiFi.post();
      ONI = true;
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
  }
}

void setup()
{
  M5.begin();
  M5.Power.begin();
  M5.Lcd.clearDisplay();
  M5.Lcd.drawJpgFile(SD, "/oni.JPG");
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextSize(2);

  WiFi.softAP(ssid, pass);
  delay(100);
  WiFi.softAPConfig(ipServer, ipGateway, subnet);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.printf("ssid: %s", ssid);
  M5.Lcd.setCursor(0, 65);
  M5.Lcd.printf("incomingPort: %d", incomingPort);
  M5.Lcd.setCursor(0, 105);
  M5.Lcd.printf("outgoingPort: %d", outgoingPort);
  M5.Lcd.setCursor(0, 155);
  M5.Lcd.printf("Oni deeeeesu!!!");
  delay(1000);
  playMP3("/oni.mp3");

  OscWiFi.subscribe(incomingPort, "/rssi", rcv_rssi);
}

void loop()
{
  OscWiFi.parse();
}
