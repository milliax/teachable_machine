#include <PubSubClient.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>

#include "SoftwareSerial.h"

#define WIFI_AP "home"
#define WIFI_PASSWORD "_LetMeIn"

const int MaxLength = 3300;
const char username[] = "test";
const char password[] = "test";
const IPAddress server(192, 168, 0, 23);
const char clientID[] = "arduino";
bool door;
char sensor;
char stat;

WiFiEspClient espClient;
PubSubClient client(espClient);
SoftwareSerial soft(10, 9);

int status = WL_IDLE_STATUS;
unsigned long next;

void callback();
void reconnect();
void readFromSerial();

void setup() {
    Serial.begin(9600);
    soft.begin(9600);
    WiFi.init(&soft);
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("wifi shield not present");
        while (true)
            ;
    }
    // connect to router
    Serial.println("Connecting...");
    while (status != WL_CONNECTED) {
        Serial.print("SSID: ");
        Serial.println(WIFI_AP);
        status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    }
    Serial.println("Connected");
    // set server configuration
    client.setServer(server, 8883);
    client.setCallback(callback);
    client.setSocketTimeout(20);
    client.setKeepAlive(20);
    Serial.println(WiFi.localIP());
    next = millis() + 100;
    door = false;
    // door is closed by default
}

void wifiPart(){
  // make sure wifi is alway online
    status = WiFi.status();
    if (status != WL_CONNECTED) {
        while (status != WL_CONNECTED) {
            Serial.print("Attmpting to connect to WPA SSID: ");
            Serial.println(WIFI_AP);
            status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
            delay(100);
        }
        Serial.println("Wifi connected");
    }
    // make sure broker is always online
    if (!client.connected()) {
        reconnect();
    }
}

void loop() {
    // do this section in every 5 seconds
    if ((long)(millis() - next) >= 0) {
        wifiPart();
        client.loop();
        next = millis() + 5000;
    }
    
    // fetch sensor signal from Serial port
    readFromSerial();

    if(door){
        // door is opened
        // attempt to close
        if(stat == 'C' && sensor == 'C'){
            door = false;
        }
    }else{
        // door is closed
        // attemp to open
        if(stat == 'O' && sensor == 'O'){
            door = true;
        }
    }

    client.loop();
    // to extend connection and recieve signals
}
// a client loop takes 0.08 seconds
// detect wifi signal takes 0.16 seconds

void readFromSerial(){
    if(Serial.available() > 0){
        sensor = (char)Serial.read();
        //using serial port to simulate the normal sensor in convenience store
    }
}

void reconnect() {
    // if not connected try until it works
    while (!client.connected()) {
        if (client.connect(clientID, username, password)) {
            Serial.println("Connected!");
            client.publish("tm/status", "aonline");
            client.subscribe("tm/status");
        } else {
            Serial.print("failed: ");
            Serial.print(client.state());
            Serial.println("retrying");
            delay(100);
        }
    }
}

bool select = false;

void callback(char* topic, byte* payload, unsigned int leng) {
    switch((char)payload[0]){
      case 'O': // on
        stat = 'O';
        break;
      case 'C': // off
        stat = 'C';
        break;
    }
    Serial.println();
}
