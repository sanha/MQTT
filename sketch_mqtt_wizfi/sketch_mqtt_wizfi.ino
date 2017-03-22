/*
   Copyright (c) 2008-2015 Nicholas O'Leary

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
  MQTT client connecting with Wiz-Fi client.

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function.
*/

#include <SPI.h>
#include <WizFi250.h>
#include <PubSubClient.h>

// setting static variables
char ssid[] = "CMS_2.4GHz";
char pass[] = "cms302420@";
char topic[] = "arduinoTest";
char clientId[] = "arduinoClient";
int status = WL_IDLE_STATUS;
IPAddress server(192, 168, 1, 45);

WiFiClient wifiClient;
PubSubClient client(wifiClient);
int count;

void callback(char* topic, byte* payload, unsigned int length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
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
  count = 0;
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network ");
  Serial.println(ssid);

  client.setServer(server, 1883);
  client.setCallback(callback);

  reconnect();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }

  String cntStr = String(count++);
  char charBuf[cntStr.length() + 1];
  cntStr.toCharArray(charBuf, cntStr.length() + 1);
  client.publish(topic, charBuf);

  long prevMillis = millis();
  Serial.print("MQTT client connection status: ");
  Serial.println(client.loop());
  long currMillis = millis();
  Serial.print("Looping period: ");
  Serial.println(currMillis - prevMillis);
  
  /*
  long prevMillis = millis();
  Serial.print("wifi client is available?: ");
  Serial.println(wifiClient.available());
  long currMillis = millis();
  Serial.print("Looping period: ");
  Serial.println(currMillis - prevMillis);
  */
}
