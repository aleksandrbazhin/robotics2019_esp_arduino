#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "WiFi1"
#define STAPSK  "05010501"
#endif

#define LED_PIN 2
#define IR_SEND_PIN 4

IRsend irsend(IR_SEND_PIN );

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_PIN;

bool data_received = false;
uint32_t first_batch = 0, second_batch = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello Robotics!");
  digitalWrite(led, 0);
}

void handleSet() {
  digitalWrite(led, 1);
  String args_message = server.arg(0) + " ";
  String one_number;
  int j = 0;
  uint32_t send_buffer[8];
  for(int i = 0; i < args_message.length(); i++){
      if(args_message[i] != ' '){
        one_number += args_message[i];
      }
      else{
        send_buffer[j] = (uint32_t)one_number.toInt();
        one_number = "";
        j += 1;
      }
  }
  first_batch = (send_buffer[0] << 24) | (send_buffer[1] << 16) | (send_buffer[2] << 8) | send_buffer[3];
  second_batch = (send_buffer[4] << 24) | (send_buffer[5] << 16) | (send_buffer[6] << 8) | send_buffer[7];
  data_received = true;
  server.send(200, "text/plain", "Data received, transmission started");
  int time_offset = server.arg(1).toInt();
  delay(time_offset);
  digitalWrite(led, 0);
}

void setup(void) {
 
  pinMode(led, OUTPUT);

  irsend.begin();
  digitalWrite(led, 0);

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  if (data_received) {
    irsend.sendNEC(0xffffffff, 32);
    irsend.sendNEC(first_batch, 32);
    irsend.sendNEC(second_batch, 32);
    delay(1000);
  }
}
