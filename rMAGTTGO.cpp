//Conor Phan
// Red Id: 823760776
// CS 596
// Final Project
// Remind Me Again...
// This is the code that was used that created the demo as seen in class
// All the ESP32TTGO is doing is getting the sensor data and reporting it to the Raspberry Pi
// After reporting, it gets the instance from the Raspberry Pi and flashes and buzzes based off instance and the silence button installed on TTGO breadboard
// Also has a button to send a message to Raspberry Pi to delete all rows in google sheets
// All the messaging and APIs are done by the Raspberry Pi

// Libraries that must be included to use wifi and sensor
#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include <HttpClient.h>

// Lib_Deps : adafruit/Adafruit AHTX0@^2.0.3; amcewen/HttpClient@^2.2.0

// Defining pins for lights, buttons, and buzzers
#define RED_PIN 13
#define YELLOW_PIN 17
#define GREEN_PIN 2
#define BUTTON 12
#define SIL_BUTTON 36
#define BUZZER 15

// Spped of sound, cm to inch conversion, pins for ultrasonic sensor
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
#define TrigPin 33
#define EchoPin 32

// SSid and Pass to get onto wifi
char ssid[] = "Click Here for WiFi";    // your network SSID (name) 
char pass[] = "FamilyTrustRespect123"; // your network password (use for WPA, or use as key for WEP)

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

// Amount of miliseconds in an hour
int milToHour = 3600000;
// The future time that will allow buzzer to work again
int futTime = 0; 
//Character path as done in lab 2
char tpath[15];
char hpath[15];
char newPath[32];

// Name of the server we want to connect to given by AWS
const char kHostname[] = "192.168.0.66";
// Port for web address
const int kPort = 5000;

//Creating variables for ultrasonic sensor to see distance
long duration;
float distanceCm;
float distanceInch;

// Declare the humidity and temperature sensor
Adafruit_AHTX0 aht;

// Setup function
void setup() {
  Serial.begin(9600);

  // Sets up all the pinmodes for the defined pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLDOWN);
  pinMode(SIL_BUTTON, INPUT_PULLDOWN);

  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  //Beings wifi all used in lab 3
  WiFi.begin((const char*)ssid,(const char*) pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  // Begins humidity and temperature sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

}

// This functions sends to Rasberry Pi the approute for button, which deletes all reminders
void delRem(){
    // Sets up the http.get
  WiFiClient c;
  HttpClient http(c);
  int err = 0;
  // Uses the get function to send path to the correct hostname and port
  err = http.get(kHostname, kPort, "/button?");
  // The rest of this code was from the example given to us in the lab 3
  // If there was no error then it starts request
  if (err == 0)
  {
    Serial.println("Starting Request...");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
}

// Temp count used to see which instance is being used
int tempCount = 0;
char temp;
// Sends a message to Raspberry Pi with Temp and Humi and gets instance back
int sendMessage(){
  sensors_event_t humidity, temp;
  // This fills in the data
  // This code was from the AHT example given by Adafruit AHTX0 example
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  // Declares a new integer checking for error with requests
  int err =0;
  // Sets the headers from temperature and humidity because they will be consistent every loop
  char tempPath[] = "/humTemp?temp=";
  char humiPath[] = "&humi=";
  //Changes the temperature and humidity variable which are float into char arrays
  // With corresponding variables
  dtostrf(temp.temperature,5,2,tpath);
  dtostrf(humidity.relative_humidity,5,2,hpath);
  // Copies the temperature header into newPath
  strcpy(newPath, tempPath);
  // Adds the temperature from sensor to newPath
  strcat(newPath, tpath);
  // Adds humidity header to newPath
  strcat(newPath,humiPath);
  // Adds humidity to newPath
  strcat(newPath, hpath);
  
  // Sets up the http.get
  WiFiClient c;
  HttpClient http(c);
  
  // Uses the get function to send path to the correct hostname and port
  err = http.get(kHostname, kPort, (const char*)newPath);
  // The rest of this code was from the example given to us in the lab 3
  // If there was no error then it starts request
  if (err == 0)
  {
    Serial.println("Starting Request...");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character

                Serial.print(c);
                if(isdigit(c)){
                  tempCount = c - 48;
                }
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  Serial.println();
  Serial.print("Instance is: ");
  Serial.println(tempCount);
  return tempCount;
}

// buzzOnOff sees if buzzer should be on or off
int buzzOnOff = 0;
// Used for the instances being used
int count = 1;
int instance;
// Instances: 1 is Nothing, 2 is Daily, 3 is Reminder, 4 for daily and reminder
void loop() {
  // If the silence button is pressed sets a future time when buzzer will go off again
  if(digitalRead(SIL_BUTTON) == HIGH){
    futTime = millis() + milToHour;
  }
  // Checks if buzzer should be silenced or not based off when silence button was pressed
  if(futTime > millis()){
    buzzOnOff = 1;
  }
  else{
    buzzOnOff = 0; 
  }
  // Sets up ultrasonic sensor
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(EchoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  //Checks to see if ultrasonic sensor distance is less than 15 inches, which then starts the code for sending data to rasberry pi
  if(distanceInch < 15){
    // From lab 3
    WiFiClient c;
    HttpClient http(c);
    // Gets instance by calling function, int is returned
    instance = sendMessage();
    // If instance is 1, then green light goes on for 5 seconds
    if(instance == 1){
      digitalWrite(GREEN_PIN, HIGH);
      sleep(5);
      digitalWrite(GREEN_PIN, LOW);
    }
    // If instance is 2, then yellow light goes on and off for 10 seconds
    else if(instance == 2){
      int i = 0;
      while (i<5){
      digitalWrite(YELLOW_PIN, HIGH);
      sleep(1);
      digitalWrite(YELLOW_PIN, LOW);
      sleep(1);
      i++;
      }
    }
    // If instance is 3, then red light goes on and off for 10 seconds
    else if(instance == 3){
      int i = 0;
      while (i<5){
        // If the delete button is pressed it calls the delete function
        if(digitalRead(BUTTON) == HIGH){
        // Calls function to delete reminders in google sheets
        delRem();
        }
        // Red light on
        digitalWrite(RED_PIN, HIGH);
        // If buzzer is on then it will make a sound, otherwise it will be silent
        if(buzzOnOff == 0){
          tone(BUZZER,261);
        }
        sleep(1);
        // If the delete button is pressed it calls the delete function
        if(digitalRead(BUTTON) == HIGH){
        // Calls function to delete reminders in google sheets
        delRem();
        }
        // Red light off
        digitalWrite(RED_PIN, LOW);
        noTone(BUZZER);
        sleep(1);
        i++;
      }

    }
    // If instance is 4, both red and yellow lights flash
    else if(instance == 4){
      int i = 0;
      while (i<5){
        // If the delete button is pressed it calls the delete function
        if(digitalRead(BUTTON) == HIGH){
        // Calls function to delete reminders in google sheets
        delRem();
        }digitalWrite(YELLOW_PIN, HIGH);
        if(buzzOnOff == 0){
          tone(BUZZER,261);
        }
        digitalWrite(RED_PIN, HIGH);
        sleep(1);
        // If the delete button is pressed it calls the delete function
        if(digitalRead(BUTTON) == HIGH){
        // Calls function to delete reminders in google sheets
        delRem();
        }
        digitalWrite(RED_PIN, LOW);
        noTone(BUZZER);
        digitalWrite(YELLOW_PIN, LOW);
        i++;
        sleep(1);
      }
    }
  }
  // Wait half a second before checking again
  delay(500);
}