#include <ESPAsyncWebServer.h>
#include <ELog.h>

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void websocketHandleBinaryLeds(uint8_t *data, size_t len, AsyncWebSocketClient * client) {
  
  // First byte indicates the command
  // 0x00 - Prepare buffer
  // 0x01 - Receive data
  // 0x02 - Ready buffer
  // 0x03 - Display buffer

  if (data[0] == 0x00) { // prepare buffer
    // Byte 1-2 indicates the number of rows
    // Byte 3-4 indicates the number of columns
    // Byte 5-6 indicates the number of bytes per pixel
    logger.log(DEBUG, "Got %d bytes of data", sizeof(data));
    // print data (as hex) to serial for debugging
    for (int i = 0; i < len; i++) {
      logger.log(DEBUG, "0x%02X ", data[i]);
    }

    // reset the buffer
    if (pixelBuffer != NULL) {
      logger.log(INFO, "Freeing buffer");
      free(pixelBuffer);
    }

    rows = ((int)data[1] | ((int)data[2] << 8));;
    cols = ((int)data[3] | ((int)data[4] << 8));;
    bytesPerPixel = ((int)data[5] | ((int)data[6] << 8));
    totalBytes = rows * int(cols * bytesPerPixel);
    
    logger.log(INFO, "Total rows: %d", rows);
    logger.log(INFO, "Total cols: %d", cols);
    logger.log(INFO, "Bytes per pixel: %d", bytesPerPixel);
    logger.log(INFO, "Total bytes needed: %d", totalBytes);

    pixelBuffer = (uint8_t *)ps_malloc(totalBytes);

    logger.log(INFO, "Buffer address: %d", pixelBuffer);
    logger.log(INFO, "PSRAM free: %d", ESP.getFreePsram());
    logger.log(INFO, "PSRAM used: %d", ESP.getPsramSize() - ESP.getFreePsram());
    currentByteIndex = 0;

  } else if (data[0] == 0x01) {
    // copy data to pixelBuffer
    memcpy(pixelBuffer + currentByteIndex, data + 1, len - 1); 
    currentByteIndex += len - 1;
  
  } else if (data[0] == 0x02) {
    bufferReady = true;
    
  } else if (data[0] == 0x03) {
    if (bufferReady) xTaskNotifyGive(FastLEDshowTaskHandle);
  } else {
    logger.log(ERROR, "Invalid command received: 0x%02X", data[0]);
  }

}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
   if(type == WS_EVT_CONNECT){ // client has connected
    logger.log(INFO, "ws[%s][%u] connect", server->url(), client->id());
   } else if (type == WS_EVT_DISCONNECT) { // client has disconnected
    logger.log(INFO, "ws[%s][%u] disconnect", server->url(), client->id());
   } else if (type == WS_EVT_ERROR) {
    logger.log(ERROR, "ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
        //logger.log(DEBUG, "ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
        if (info->opcode == WS_TEXT) {
           // would this work for json?

        } else { // binary LED data
            websocketHandleBinaryLeds(data, len, client);
      }
    }
  }
}
