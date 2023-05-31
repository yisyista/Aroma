/*Paragon Hackathon Competition 2022 
 *Hari dan Tanggal  : 21 Agustus 2022
 *Nama Anggota 1      : Kayyisa Zahratulfirdaus
 *Nama Anggota 2      : Eraraya Morenzo Muten
 *Nama Anggota 3      : Alhan Izzaturohman
 *Nama File         : Pump.ino
 *Deskripsi         : Program untuk mencampur parfum dari tiga buah cartridge melalui MQTT Dash
 *                    
 *                    
 */

#include <WiFi.h>
#include <PubSubClient.h>

// Setting WiFi

const char* ssid = "Nama Wifi";
const char* password = "Password Wifi";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

int payload_int = 0;  // for converting payload from byte to int
const int pump1 = 15; // pump1 relay pin
const int pump2 = 2; // pump2 relay pin
const int pump3 = 4; // pump3 relay pin
int dur1 = 0;         // pump1 activation duration
int dur2 = 0;         // pump2 activation duration
int dur3 = 0;         // pump3 activation duration


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // converting payload from byte to int
  payload_int = 0; // restart sum from zero
  for (int i = 0; i < length; i++) {
    // add from last digit
    // byte 48 = int 1
    payload_int += ((((int)(char)payload[length-1-i]) - 48)*pow(10,i));
  }
  Serial.println(payload_int);

  if (!strcmp(topic, "aroma/cart1")) {
    // pump1 activation duration = payload from cart1 topic
    dur1 = payload_int*1000; // in ms
    Serial.println(dur1);

  } else if (!strcmp(topic, "aroma/cart2")) {
    // pump2 activation duration = payload from cart2 topic
    dur2 = payload_int*1000; // in ms
    Serial.println(dur2);

  } else if (!strcmp(topic, "aroma/cart3")) {
    // pump3 activation duration = payload from cart3 topic
    dur3 = payload_int*1000; // in ms
    Serial.println(dur3);

  } else if (!strcmp(topic, "aroma/mix"))  {
    // pump activation order
    digitalWrite(pump1, LOW); // pump1 on
    delay(dur1); 
    digitalWrite(pump1, HIGH); // pump1 off
    delay(1000);

    digitalWrite(pump2, LOW); // pump2 on
    delay(dur2); 
    digitalWrite(pump2, HIGH); // pump2 off
    delay(1000);

    digitalWrite(pump3, LOW); // pump3 on
    delay(dur3); 
    digitalWrite(pump3, HIGH); // pump3 off
    delay(1000);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("aroma/cart1");
      client.subscribe("aroma/cart2");
      client.subscribe("aroma/cart3");
      client.subscribe("aroma/mix");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);
  pinMode(pump3, OUTPUT);
  digitalWrite(pump1, HIGH); // off
  digitalWrite(pump2, HIGH); // off
  digitalWrite(pump3, HIGH); // off
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
