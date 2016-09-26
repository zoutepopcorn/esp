#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

String alfa = "1234567890qwertyuiopasdfghjkklzxcvbnm QWERTYUIOPASDFGHJKLZXCVBNM_";
byte channel;
int nr = 0;
char *macs[] = { "26 CE 4A EF E1 A0", "4C AC 0A 15 98 10", "DC 71 44 70 23 58", "24 A2 E1 EF 4A CE"}; 

// Beacon Packet buffer
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, 
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                /*22*/  0xc0, 0x6c, 
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 
                /*32*/  0x64, 0x00, 
                /*34*/  0x01, 0x04, 
                /* SSID */
                /*36*/  0x00, 0x06, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72,
                        0x01, 0x08, 0x82, 0x84,
                        0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 
                /*56*/  0x04};                       


void setup() {
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1); 
}


unsigned int hexToDec(String hexString) {
  unsigned int decValue = 0;
  int nextInt;
  for (int i = 0; i < hexString.length(); i++) {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}

void spoofMac(String str) {
    packet[10] = packet[16] = hexToDec(str.substring(0, 2));//random(256);
    packet[11] = packet[17] = hexToDec(str.substring(3, 5));
    packet[12] = packet[18] = hexToDec(str.substring(6, 8));
    packet[13] = packet[19] = hexToDec(str.substring(9, 11));
    packet[14] = packet[20] = hexToDec(str.substring(12, 14));
    packet[15] = packet[21] = hexToDec(str.substring(15, 17));
}


void loop() {
    // Randomize channel //
    if(nr > 3) { nr = 0;  }
    channel = random(1,12); 
    wifi_set_channel(channel);

    spoofMac(macs[nr]);
    // Randomize SSID (Fixed size 6. Lazy right?)
    packet[38] = 'a';
    packet[39] = 'a';
    packet[40] = 'a';
    packet[41] = 'a';
    packet[42] = alfa[random(4)];
    packet[43] = alfa[random(2)];
    
    packet[56] = channel;
    
    wifi_send_pkt_freedom(packet, 57, 0);
    wifi_send_pkt_freedom(packet, 57, 0);
    wifi_send_pkt_freedom(packet, 57, 0);
    delay(1);
    nr++;
}
