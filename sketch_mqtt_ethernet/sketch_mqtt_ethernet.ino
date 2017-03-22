/*
   Copyright (C) 2017 Seoul National University

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
  MQTT client connecting with Wi-Fi client.

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// setting static variables
char topicToPub[] = "MISTExampleSub";
char topicToSub[] = "MISTExamplePub";
char clientId[] = "arduinoClient";
byte mac[]    = {  0x98, 0x83, 0x89, 0x21, 0x69, 0x6E };
IPAddress ip(192, 168, 1, 204);
IPAddress server(192, 168, 1, 45);
int sensorPin = A0;
int ledPin = 7;

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // print the message
  /*Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");*/
  String strPayload = String((char *) payload);
  //Serial.println(strPayload);

  // handle the led output
  if (strPayload.startsWith("ON")) {
    digitalWrite(ledPin, HIGH);
  } else if (strPayload.startsWith("OFF")) {
    digitalWrite(ledPin, LOW);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      if (client.subscribe(topicToSub, 0)) {
        Serial.println("Subscription success.");
      } else {
        Serial.println("Subsription is failed.");
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // initialize output pin
  pinMode(ledPin, OUTPUT);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
  
  reconnect();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  delay(50);
  int sensorValue = analogRead(sensorPin);
  String cntStr = String(sensorValue);
  char charBuf[cntStr.length() + 1];
  cntStr.toCharArray(charBuf, cntStr.length() + 1);
  client.publish(topicToPub, charBuf);

  // Serial.print("MQTT client connection status: ");
  // Serial.print(client.loop());
  client.loop();
  
  /*Serial.print(", value is ");
  Serial.println(sensorValue);*/
  
  /*
  long prevMillis = millis();
  Serial.print("wifi client is available?: ");
  Serial.println(wifiClient.available());
  long currMillis = millis();
  Serial.print("Looping period: ");
  Serial.println(currMillis - prevMillis);
  */
}
