# Import the PyDuino library
from pyduino import *
import cv2
import numpy as np

def display_temperature(text):
  """
  Displays a given temperature reading in a large font on a white window.

  Args:
      text: The temperature string to display (e.g., "20.5°C").
  """

  # Define font size and color
  font_scale = 2.0
  font_color = (0, 0, 255)  # Blue color

  # Create a white image
  img = np.zeros((400, 600, 3), np.uint8)
  img.fill(255)  # Fill with white color (BGR format)

  # Get text size
  (text_width, text_height), _ = cv2.getTextSize(text, cv2.FONT_HERSHEY_SIMPLEX, font_scale, 1)

  # Calculate text position for centering
  text_x = int((img.shape[1] - text_width) / 2)
  text_y = int((img.shape[0] + text_height) / 2)

  # Put the text on the image
  cv2.putText(img, text, (text_x, text_y), cv2.FONT_HERSHEY_SIMPLEX, font_scale, font_color, 2, cv2.LINE_AA)

  # Display the image
  cv2.imshow("Temperature", img)
  cv2.waitKey(1)  # Wait for a key press to close the window


# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

esp = devices['ESP32_1']

esp.digitalWrite(32, 1)
esp.digitalWrite(25, 0)

while True:
    reading = esp.analogRead(33)

    temp = map_range(496,
                     -20,
                     1575,
                     20,
                     reading)

    temp = int(temp*10) / 10

    display_temperature(f"Temp: {temp} C")

    time.sleep(0.5)



