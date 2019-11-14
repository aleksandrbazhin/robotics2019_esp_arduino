#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "WiFi1"
#define STAPSK  "05010501"
#endif

#define LED_PIN 2
#define RECV_PIN 4
IRrecv irrecv(RECV_PIN);
decode_results results;

uint8_t rcv_counter = 0;
uint8_t received_data[8];

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_PIN;

bool data_received = false;
uint32_t first_batch = 0, second_batch = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "Hello, Robotics 2019 tester!");
  digitalWrite(led, 0);
}

void handleTest() {
  digitalWrite(led, 1);
  String data_response = "";
  for (int i = 0; i < 8; i++) {
    data_response += String(received_data[i], DEC) + " ";
  }
  server.send(200, "text/plain", data_response);
  digitalWrite(led, 0);
}

void setup(void) {
 
  pinMode(led, OUTPUT);

  irrecv.enableIRIn();
  digitalWrite(led, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

//  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());

  MDNS.begin("esp8266");
  
  server.on("/", handleRoot);
  server.on("/test", handleTest);

  server.begin();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  if (irrecv.decode(&results)) {
    uint32_t rcv_value = (uint32_t)results.value;
    if (rcv_counter == 0) {
      if (rcv_value == 0xffffffff) {
        rcv_counter = 1;
      }
    } else {      
      if (rcv_counter == 1) {
        received_data[0] = (uint8_t)(rcv_value >> 24);
        received_data[1] = (uint8_t)(rcv_value >> 16);
        received_data[2] = (uint8_t)(rcv_value >> 8);
        received_data[3] = (uint8_t)(rcv_value);
      } else if (rcv_counter == 2) {
        received_data[4] = (uint8_t)(rcv_value >> 24);
        received_data[5] = (uint8_t)(rcv_value >> 16);
        received_data[6] = (uint8_t)(rcv_value >> 8);
        received_data[7] = (uint8_t)(rcv_value);        
      }
      rcv_counter ++;
    }
    if(rcv_counter > 2) {
      rcv_counter = 0;
    }
    irrecv.resume(); // Receive the next value
  }
}
