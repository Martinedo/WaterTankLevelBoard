#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const int trigPin = 5;    // D1
const int echoPin = 4;    // D2
const int treshold = 100; // cca 1,7 cm // cm = treshold * (SOUND_VELOCITY = 0.034) * 2
const int loopDelay = 3000;

long duration;
long lastDuration;

const char *ssid = "YOUR WIFI SSID NAME";
const char *password = "YOUR WIFI PASSWORD";
String serverName = "https://192.168.88.195:44307/waterlevel";

void setup()
{
  Serial.begin(9600); // Starts the serial communication

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  lastDuration = -1;
}

void loop()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  if (abs(duration - lastDuration) > treshold)
  {
    Serial.print("Duration different from last measuring, lastDuration=" + String(lastDuration) + ", duration=" + String(duration));

    if (WiFi.status() == WL_CONNECTED)
    {
      WiFiClientSecure client;
      HTTPClient http;

      client.setInsecure();

      String serverPath = serverName + "?measuredValue=" + String(duration);
      http.begin(client, serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
  }
  lastDuration = duration;

  delay(loopDelay);
}