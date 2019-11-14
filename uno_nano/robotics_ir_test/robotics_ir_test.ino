#include <IRremote.h>

int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
uint8_t rcv_counter = 0;
uint8_t received_data[8];

void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    if (rcv_counter == 0) {
      if (results.value == 0xffffffff) {
        rcv_counter = 1;
      }
    } else {      
      if (rcv_counter == 1) {
        received_data[0] = (uint8_t)(results.value >> 24);
        received_data[1] = (uint8_t)(results.value >> 16);
        received_data[2] = (uint8_t)(results.value >> 8);
        received_data[3] = (uint8_t)(results.value);
      } else if (rcv_counter == 2) {
        received_data[4] = (uint8_t)(results.value >> 24);
        received_data[5] = (uint8_t)(results.value >> 16);
        received_data[6] = (uint8_t)(results.value >> 8);
        received_data[7] = (uint8_t)(results.value);        
      }
      rcv_counter ++;
    }
    if(rcv_counter > 2) {
      rcv_counter = 0;
      Serial.print("Received data:");
      for (int i = 0; i < 8; i++) {
        Serial.print(" ");
        Serial.print(received_data[i]);
      }
      Serial.println();
    }
    irrecv.resume(); // Receive the next value
  }
}
