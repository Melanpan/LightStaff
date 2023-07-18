import websocket
import _thread
import time
import rel
import random



def on_message(ws, message):
    print(message)

def on_error(ws, error):
    print(error)

def on_close(ws, close_status_code, close_msg):
    print("### closed ###")

def on_open(ws):
    currentIndex = 0
    while True:
        if currentIndex == 0:
            rgb = [255, 0, 0] * 144
            currentIndex = 1
        elif currentIndex == 1:
            rgb = [0, 0, 255] * 144
            currentIndex = 2
        elif currentIndex == 2:
            rgb = [0, 255, 0] * 144
            currentIndex = 0

        transmit = bytearray(rgb)
        ws.sock.send_binary(transmit)
        time.sleep(0.250)


if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("ws://192.168.1.80/ws",
                              on_open=on_open,
                              on_message=on_message,
                              on_error=on_error,
                              on_close=on_close)

    ws.run_forever(dispatcher=rel, reconnect=5)  # Set dispatcher to automatic reconnection, 5 second reconnect delay if connection closed unexpectedly
    rel.signal(2, rel.abort)  # Keyboard Interrupt
    rel.dispatch()