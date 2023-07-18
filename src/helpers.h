void configureWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSID_PWD);
  logger.log(INFO, "Connecting to WiFi..");
  debugLed[0] = CRGB::Red;
  FastLED.show();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  debugLed[0] = CRGB::Black;
  FastLED.show();

  logger.log(INFO, "Connected to the WiFi network");
  logger.log(INFO, "IP address: %s", WiFi.localIP().toString().c_str());
  
}

void AllLedsOn() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::BlueViolet;
  }
  FastLED.show();
}

void AllLedsOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}
