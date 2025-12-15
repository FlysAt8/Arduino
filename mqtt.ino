// Select your modem:
#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(8, 9);  // RX, TX
#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
#define TINY_GSM_YIELD() { delay(2); }

// Your GPRS credentials, if any
const char apn[]      = "internet.mts.ru";
const char gprsUser[] = "mts";
const char gprsPass[] = "mts";

// MQTT details
const char* broker = "dev.rightech.io";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

#define PUB_DELAY (5 * 1000) /* 5 seconds */

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

#include <iarduino_DHT.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 10
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
iarduino_DHT sensor1(3); 
int aPin=A0;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  SerialMon.println(topic);
  SerialMon.write(payload, len);
  SerialMon.println();
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("mqtt-sasha_fed585-mq2p13");

  if (status == false) {
    SerialMon.println(" fail");
    return false;
  }
  SerialMon.println(" success");

  mqtt.subscribe("base/relay/led1");
  return mqtt.connected();
}


void setup() {

  dht.begin();
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  SerialMon.println("Wait...");

  // Set GSM module baud rate
  SerialAT.begin(9600);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback([] (char* topic, byte * payload, unsigned int len)  {
    SerialMon.write(payload, len);
    SerialMon.println();
  });
}


long last = 0;
void publishTemperature() {
  long now = millis();
  if (now - last > PUB_DELAY) { 
    int Tair = dht.readTemperature();
    int Hair = dht.readHumidity();
    int Hgnd = analogRead(aPin);
    Hgnd = 100-(float(Hgnd)/1023*100);
    mqtt.publish("base/state/Atemperature", String(Tair).c_str());
    mqtt.publish("base/state/Ahumidity", String(Hair).c_str());
    mqtt.publish("base/state/Ghumidity", String(Hgnd).c_str());
    last = now;
  }
}

void loop() {
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network re-connected");
    }

    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected()) {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) {
        SerialMon.println("GPRS reconnected");
      }
    }
  }

  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  mqtt.loop();
  publishTemperature();
}