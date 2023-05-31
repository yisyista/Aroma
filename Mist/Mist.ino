/*Paragon Hackathon Competition 2022 
 *Hari dan Tanggal  : 21 Agustus 2022
 *Nama Anggota 1      : Kayyisa Zahratulfirdaus
 *Nama Anggota 2      : Eraraya Morenzo Muten
 *Nama Anggota 3      : Alhan Izzaturohman
 *Nama File         : 
 *Deskripsi         : Program untuk menyemprotkan isi parfum dari device dengan menggunakan mist module dan MQTT dash
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
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

int payload_int = 0;
int dur1 = 0;
int dur2 = 0;
int dur3 = 0;

const int relay = 26;

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
  payload_int = 0;
  for (int i = 0; i < length; i++) {
    payload_int += ((((int)(char)payload[length-1-i]) - 48)*pow(10,i));
  }
  Serial.println(payload_int);

  if (!strcmp(topic, "aroma/cart1")) {
    Serial.println('t');
    
    dur1 = payload_int*1000;
    Serial.println(dur1);
  } else {
    Serial.println('f');
    // nyala mist dur1 milidetik
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    delay(1000); 
    digitalWrite(relay, HIGH);
    Serial.println("Current not Flowing");
    delay(dur1);
    // mist mode 2
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    delay(1000); 
    digitalWrite(relay, HIGH);
    Serial.println("Current not Flowing");
    delay(1000);
    // mist mati
    digitalWrite(relay, LOW);
    Serial.println("Current Flowing");
    delay(1000); 
    digitalWrite(relay, HIGH);
    Serial.println("Current not Flowing");
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
      //client.publish("outTopic", "hello world");
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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
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
