static TaskHandle_t FastLEDshowTaskHandle = 0;

bool ledMode = false; // false = RGB, true = RGBW
bool updateLeds = false; // true if we need to update the LEDs
bool bufferReady = false; // true if we have received the buffer
bool manualTrigger = false;

int totalBytes = 0;
int currentByteIndex = 0;
int lastTriggerButtonPress = 0;

uint8_t *pixelBuffer;
uint8_t rows = 0;
uint8_t cols = 0;
uint8_t bytesPerPixel = 3;