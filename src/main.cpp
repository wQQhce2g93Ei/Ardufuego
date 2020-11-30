#include <Arduino.h>
#include <string>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FireplaceRF.h>

// Set wifi credentials
const char* ssid = "my_ssid";
const char* password = "my_ssid_password";

// Set MQTT info. username/password to NULL if no auth necessary
const char* mqtt_server = "192.168.0.XXX";
const char* mqtt_username = "username";
const char* mqtt_password = "password";

const char* power_stat_topic = "fireplace/power/stat";
const char* power_cmnd_topic = "fireplace/power/cmnd";
const char* flame_stat_topic = "fireplace/flame/stat";
const char* flame_cmnd_topic = "fireplace/flame/cmnd";
const char* light_stat_topic = "fireplace/light/stat";
const char* light_cmnd_topic = "fireplace/light/cmnd";
const char* fan_stat_topic   = "fireplace/fan/stat";
const char* fan_cmnd_topic   = "fireplace/fan/cmnd";

// Set PIN to use for 433mhz tx module
const int rf_tx_pin = D0;
FireplaceRF fireplace = FireplaceRF(rf_tx_pin);

WiFiClient espClient;
PubSubClient client = PubSubClient(espClient);

void callback(char* topic, byte* payload, unsigned int length)
{
  std::string topicString = std::string(topic);
  std::string payloadString = std::string((char*)payload, length);
  Serial.println("MQTT message received [" + String(topic) + "] -- " + String(payloadString.c_str()));

  if (topicString.compare(power_cmnd_topic) == 0)
  {
    if (payloadString.compare("ON") == 0 || payloadString.compare("on") == 0)
    {
      Serial.println("Fireplace Power Command - ON");
      fireplace.start();
    }
    else if (payloadString.compare("OFF") == 0 || payloadString.compare("off") == 0)
    {
      Serial.println("Fireplace Power Command - OFF");
      fireplace.stop();
    }
  }
  else if (topicString.compare(flame_cmnd_topic) == 0)
  {
    int level = atoi(payloadString.c_str());
    if (level >= 1 && level <= 5)
    {
      Serial.println("Fireplace Flame Command - " + String(payloadString.c_str()));
      fireplace.setFlame(level);
    }
    else
    {
      Serial.println("Fireplace Flame Command FAILED - " + String(payloadString.c_str()));
    }
  }
  else if (topicString.compare(light_cmnd_topic) == 0)
  {
    int level = atoi(payloadString.c_str());
    if (level >= 0 && level <= 3)
    {
      Serial.println("Fireplace Light Command - " + String(payloadString.c_str()));
      fireplace.setAux1(level);
    }
    else
    {
      Serial.println("Fireplace Light Command FAILED - " + String(payloadString.c_str()));
    }
  }
  else if (topicString.compare(fan_cmnd_topic) == 0)
  {
    int speed = atoi(payloadString.c_str());
    if (speed >= 0 && speed <= 3)
    {
      Serial.println("Fireplace Fan Command - " + String(payloadString.c_str()));
      fireplace.setFan(speed);
    }
    else
    {
      Serial.println("Fireplace Fan Command FAILED - " + String(payloadString.c_str()));
    }
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT server at " + String(mqtt_server));
    if (client.connect(mqtt_server, mqtt_username, mqtt_password))
    {
      Serial.println("Connected to MQTT server at " + String(mqtt_server));

      client.subscribe(power_cmnd_topic);
      Serial.println("Subscribed to MQTT topic " + String(power_cmnd_topic));

      client.subscribe(flame_cmnd_topic);
      Serial.println("Subscribed to MQTT topic " + String(flame_cmnd_topic));

      client.subscribe(light_cmnd_topic);
      Serial.println("Subscribed to MQTT topic " + String(light_cmnd_topic));

      client.subscribe(fan_cmnd_topic);
      Serial.println("Subscribed to MQTT topic " + String(fan_cmnd_topic));
    }
    else
    {
      Serial.println("Connection to MQTT server at " + String(mqtt_server) + " failed, rc=" + String(client.state()) + " try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting Ardufuego");

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}
