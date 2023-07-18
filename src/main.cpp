#define FASTLED_ALLOW_INTERRUPTS 0
#include <defines.h>
#include <Arduino.h>
#include <ELog.h>
Elog logger; // https://registry.platformio.org/libraries/x385832/Elog

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <fastled.h>
#include <spi.h>
#include <oled.h>
#include "FastLED_RGBW.h" // RGBW hack
#include <variables.h>
#include <webserver.h>
#include <leds.h>
#include <helpers.h>

//TODO: https://github.com/tzapu/WiFiManager/tree/master
//https://github.com/me-no-dev/ESPAsyncWebServer/tree/master#async-websocket-event

// https://www.partsnotincluded.com/fastled-rgbw-neopixels-sk6812/

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];
CRGB debugLed[1];

// #define LCD_SDA 21
// #define LCD_SCL 22
// OLED Display=OLED(LCD_SDA, LCD_SCL,16);

///@brief This task is responsible for updating the LEDs
void FastLEDshowTask(void *pvParameters) {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block until we need to update the LEDs
    displayPixels();
}

///@brief Interrupt handler for the trigger button
void triggerButtonPressed() {
  // debounce 
  if (millis() - lastTriggerButtonPress < 250) return;
  manualTrigger = true;
  lastTriggerButtonPress = millis();
}

/// @brief  Manually trigger the LED update
void manualTriggered() {
   if (pixelBuffer != nullptr && rows > 0 && cols > 0) {
    if (bufferReady) {
      logger.log(INFO, "Manual trigger!");
      xTaskNotifyGive(FastLEDshowTaskHandle);
    }
   } else {
    logger.log(ERROR, "Manual trigger but no pixel buffer is available.");
   }

}

void setup() {
  Serial.begin(115200);
  Serial.println("OK");
  logger.addSerialLogging(Serial, "Main", DEBUG);

  //FastLED.addLeds<SK6812, GPIO_NUM_47, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
  FastLED.addLeds<WS2812B, GPIO_NUM_47, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.addLeds<WS2812, GPIO_NUM_48, RGB>(debugLed, 1);  // GRB ordering is typical

  // check for PSRAM
  if (psramFound()) {
    logger.log(INFO, "PSRAM found");
  } else {
    logger.log(ERROR, "PSRAM not found");
    while (1) {};
  }
  //"Total PSRAM: %d", ESP.getPsramSize()
  //log_d("Free PSRAM: %d", ESP.getFreePsram()
  logger.log(INFO, "Total PSRAM: %d", ESP.getPsramSize());
  logger.log(INFO,  "Free PSRAM: %d", ESP.getFreePsram());

  // Configure trigger button on pin 41
  pinMode(4, INPUT_PULLUP);
  // set as input and attach interrupt

  attachInterrupt(4, triggerButtonPressed, FALLING);

  AllLedsOff();
  configureWifi();

  server.on("/index", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.htm");
  });
  
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 10000, NULL,2, &FastLEDshowTaskHandle, FASTLED_SHOW_CORE);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.onNotFound(notFound);
  server.begin();

  // Disable watchdog while debugging
  disableCore0WDT();
  disableLoopWDT();


}

void loop() {
  // if lastDataPacket is more than 5 seconds ago, turn off LEDs
  
  // EVERY_N_MILLISECONDS(1000) {
  //   Serial.println(digitalRead(4));
  // }

  if (manualTrigger) {
    manualTriggered();
    manualTrigger = false;
  }

  EVERY_N_MILLISECONDS(1000) {
    ws.cleanupClients();
  }

}
