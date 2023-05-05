# RemindMeAgain
### Final Project for CS 596

# ReadMe for TTGO side code:

This code is used to control the ESP32TTGO board and is part of the "Remind Me Again" final project for CS 596. The purpose of this code is to gather sensor data from the board and report it to the Raspberry Pi. It then receives an instance from the Raspberry Pi and activates the buzzer and flash LED based on that instance and the silence button installed on the TTGO breadboard. It also has a button to send a message to the Raspberry Pi to delete all rows in Google Sheets. All messaging and APIs are done by the Raspberry Pi.

### Requirements
The following libraries are required for this code to function:

- Adafruit_AHTX0
- WiFi
- HttpClient
- Arduino

The following hardware is needed for this code to function:

- ESP32TTGO board
- Ultrasonic sensor
- LEDs
- Buttons
- Buzzer

The specific pins for the LEDs, buttons, and buzzer are defined in the code. The ultrasonic sensor uses pins TrigPin and EchoPin.

### How to Use
To use this code, simply upload it to the ESP32TTGO board. The code will automatically gather sensor data and report it to the Raspberry Pi. To delete all reminders, press the button on the board.


# Raspberry Pi Side of Code
This code is designed to run on a Raspberry Pi. It uses Flask, Google Sheets API, News API, and Open Weather API to send information to your phone via IFTTT webhook. This code is run on the Raspberry Pi and is designed to run automatically at a certain time, triggered by a CRON job.

### Installation
First, you will need to install Python 3 on your Raspberry Pi. Once you have done that, you can install the required Python libraries using pip. Open a terminal window on your Raspberry Pi and type:

### Configuration
To use this code, you will need to configure the IFTTT webhook and the Google Sheets API.
As well as get new API keys for OpenWeather and NewsAPI

### IFTTT Webhook
To use the IFTTT webhook, you will need to create a new applet in IFTTT. The webhook URL should be https://maker.ifttt.com/trigger/{event}/with/key/{key}, where {event} is the name of the event you want to trigger and {key} is your unique IFTTT key. Replace {event} and {key} in the code with your own values.

### Google Sheets API
To use the Google Sheets API, you will need to create a new Google API project and enable the Google Sheets API. You will also need to create a service account and download the JSON key file. Rename the key file to creds.json and place it in the same directory as the code.

### Open Weather API
To use the Open Weather API, you will need to create an account and get an API key from their website. Replace api_key in the code with your own key.

### News API
To use the News API, you will need to create an account and get an API key from their website. Replace api_key in the code with your own key.

### Usage
In Rasberry PI:
  export FLASK_APP=flaskServerCS596
  python3 -m flask run --host=0.0.0.0


Contact Information
This code was written by Conor Phan and 
Email: conorphan1@gmail.com.
If you have any questions or concerns about this code, please contact Conor Phan.

Powerpoint Demo: https://docs.google.com/presentation/d/1awXMM0R2P9MCi0kYBfQlhiYfmwHvr1Eu2TiK8V-bEno/edit#slide=id.p

Youtube Demo: https://youtu.be/CLyErHZBiUc

## Use of Device:
![Use of IOT](https://user-images.githubusercontent.com/81449923/236358791-ec2af00b-8d2f-47a8-a857-468c41db02a3.PNG)

## Device Itself:
![TTGO and Raspberry](https://user-images.githubusercontent.com/81449923/236358821-1d7de8ad-7fa0-4521-a222-2f87e4e25852.JPG)






