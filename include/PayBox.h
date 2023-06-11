#ifndef __PAY_BOX_H__
#define __PAY_BOX_H__

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <TFT_eSPI.h>


// TFT输出
#define PAY_TFT_BAK GPIO_NUM_32

#define TZ 8            // 中国时区为8
#define DST_MN 0        // 默认为0
#define TZ_MN ((TZ)*60) //时间换算
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)

extern TFT_eSPI tft;
extern char buf[];

void inline payTFTInit(uint32_t brightness) {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.setCursor(0, 0);
  ledcSetup(0, 2000, 10);
  ledcAttachPin(PAY_TFT_BAK, 0);
  ledcWrite(0, brightness);
}

void inline setupOTAConfig() {
  ArduinoOTA.onStart([] {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OTA Update", 160, 60, 4);
    tft.drawRoundRect(60, 160, 200, 20, 6, TFT_ORANGE);
  });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    sprintf(buf, "%d / %d", pro, total);
    tft.drawCentreString(buf, 160, 120, 2);
    int pros = pro * 200 / total;
    tft.fillRoundRect(60, 160, pros, 20, 6, TFT_ORANGE);
  });
  ArduinoOTA.onEnd([] {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Update Succeed!!", 160, 60, 4);
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    tft.drawCentreString("Restarting...", 160, 140, 2);
  });
  ArduinoOTA.begin();
}

// 通过AutoConfig配置Wifi热点
void inline autoConfigWifi() {
  tft.println("Start Wifi Connect!");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_MODE_APSTA);
    tft.println("Use EspTouch App to Connect!");
    WiFi.beginSmartConfig();
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    WiFi.stopSmartConfig();
    WiFi.mode(WIFI_MODE_STA);
  }
  tft.println("Wifi Connected!");
  sprintf(buf, "IP: %s", WiFi.localIP().toString().c_str());
  tft.println(buf);
}

void inline bindKeyEvent(int pin, void (*intRoutine)()) {
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), intRoutine, FALLING);
}

#endif
