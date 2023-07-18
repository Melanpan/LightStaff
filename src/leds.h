void displayPixels() {
     // Check if the 'pixelBuffer' is allocated and contains data
    if (pixelBuffer != nullptr && rows > 0 && cols > 0) {
        logger.log(INFO, "Rows: %d, Cols: %d", rows, cols);
        
        // Since the pixelBuffer contains the data in the format R,G,B,R,G,B,R,G,B...
        // which is linear, we can just read the data from the buffer and write it to the LEDs
        // 144 LEDs per row, 3 bytes per LED * rows

        // Display one row of pixels at a time
        int pixelIndex = 0;
        
        for (int row = 0; row < rows; row++) {
          

          // Read NUM_LEDS * 3 of data from the pixelBuffer and write it to the LEDs
          int ledIndex = 0;
          for (int col = 0; col < cols; col++) {
            // Read the next 3 bytes from the pixelBuffer
            uint8_t r = pixelBuffer[pixelIndex++];
            uint8_t g = pixelBuffer[pixelIndex++];
            uint8_t b = pixelBuffer[pixelIndex++];

            // Write the data to the LEDs
            leds[ledIndex++] = CRGBW(r, g, b, 0);
          }
 
          logger.log(DEBUG, "Pixelindex: %d", pixelIndex);
          // Update the LEDs
          FastLED.show();

          // Wait a bit before displaying the next row
          delay(10);
        }

    }
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
    FastLED.show();
  }
}