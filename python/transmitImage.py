import websocket
import time
import cv2
import numpy
import tqdm
from PIL import Image

bytes_per_pixel = 3 # RGB

def ImageToBytes(image):
   # loop over the image
    bytes = []
    for row in range(0, image.size[1]):
        # initialize the byte array for the line

        for col in range(0, image.size[0]):
            # get the RGB pixel at the given coordinate and
            # convert it to a byte
            r, g, b = image.getpixel((col, row))
            bytes.append(r)
            bytes.append(g)
            bytes.append(b)

        # yield the line
    # print first 144 bytes
    print(bytes[0:144])

    return bytes

ws = websocket.WebSocket()
ws.connect("ws://192.168.1.80/ws")

image = Image.open("test003.png").convert("RGB")
# rotate image 90 degrees
#image = image.rotate(90, expand=True)

# Limit image width to 144 pixels, but keep the aspect ratio (height can remain the same)
image.thumbnail((144, 144), Image.LANCZOS)

image.save("output.jpg")

# Convert image to bytes
image_bytes = ImageToBytes(image)

# assert if it fits in psram of 8MB
assert len(image_bytes) < 8 * 1024 * 1024

print(image.size)
# First send command 0x00 to indicate that we are sending an image, followed by the image size and bytes per pixel (rgb/rgbw)
data = [
    b'\x00',                            # A single byte (byte 0)
    int(image.size[1]).to_bytes(2, byteorder='little'),   # Two bytes for rows
    int(image.size[0]).to_bytes(2, byteorder='little'),   # Two bytes for cols
    int(bytes_per_pixel).to_bytes(2, byteorder='little')  # Two bytes for bytesPerPixel
]
# print data as hex string (b"".join(data))
ws.send_binary(b"".join(data))

time.sleep(0.500) # wait for esp32 to be ready to receive image

# send image bytes in 1024 byte chunks, starting with the first byte being 0x01
for i in tqdm.tqdm(range(0, len(image_bytes), 1024)):
    data = bytearray([0x01] + image_bytes[i:i+1024])

    ws.send_binary(bytes(data))
    time.sleep(0.005)


# send command 0x02 to indicate that the image is complete
ws.send_binary(b'\x02')

