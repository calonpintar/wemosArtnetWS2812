/*
  2 universes & one wire
*/

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// ARTNET CODES
#define ARTNET_DATA 0x50
#define ARTNET_POLL 0x20
#define ARTNET_POLL_REPLY 0x21
#define ARTNET_PORT 6454
#define ARTNET_HEADER 17

WiFiUDP udp;

uint8_t uniData[514];
uint8_t allUnidata[1028];
uint8_t WSpin = 5;
uint16_t uniSize, uniSize1, uniSize2, uniAllsize;
uint8_t hData[ARTNET_HEADER + 1];
uint8_t net = 0;
uint8_t universe;
uint8_t subnet = 0;

uint8_t  bitMask, waktu;

const char* ssid     = "MODULIGHT24";
const char* password = "cobainaja";

IPAddress local_ip(192, 168, 1, 102);
IPAddress gateway_ip(192, 168, 1, 1);
IPAddress subnet_ip(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  delay(10);
  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);
  WiFi.config(local_ip, gateway_ip, subnet_ip);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    
  }

  udp.begin(ARTNET_PORT); // Open ArtNet port

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());

  //pinMode(16, OUTPUT);
  //pinMode(5, OUTPUT);
  //pinMode(4, OUTPUT);
  //pinMode(0, OUTPUT);
  pinMode(WSpin, OUTPUT);
}

void ICACHE_FLASH_ATTR readUDP() {
    if (udp.parsePacket() && udp.destinationIP() == local_ip) {
        udp.read(hData, 18);
     if ( hData[0] == 'A' && hData[4] == 'N') {
         uniSize = (hData[16] << 8) + (hData[17]);
         udp.read(uniData, uniSize);
         
         if (hData[14] == 0) {
          uniSize1 = uniSize;
          for (int ctr = 0; ctr < uniSize1; ctr++) {
            allUnidata[ctr] = uniData[ctr];
          }
          Serial.print("unisize: ");Serial.println(uniSize);
          }
           
         if (hData[14] == 1) {
          uniSize2 = uniSize;
          uniAllsize = uniSize1 + uniSize2;
          for (int ctr = uniSize1; ctr < uniAllsize; ctr++) {
            allUnidata[ctr] = uniData[ctr-uniSize1];
          }
            Serial.print("uniSize1: ");Serial.println(uniSize1);
            //WSpin = 5;
          } 
         //universe = hData[14];
         //if ( hData[14] == (subnet << 4) + 0)  { WSpin = 5;} else WSpin = 16;
   /*      
    switch (universe) {
      case 0:
        WSpin = 5;
       break;
      case 1:
        WSpin = 5;
        break;
      case 2:
        WSpin = 0;
        break;
      case 3:
        WSpin = 2;
        break;
      default:
        WSpin = 16;
        break;
    }      
    */
        //Serial.print(allUnidata[513]);Serial.print(allUnidata[1]);Serial.print(allUnidata[512]);
        //Serial.println();
     }
    }
}

void ICACHE_FLASH_ATTR sendWS() {
    for (uint16_t t = 0; t < uniAllsize; t++) {
        bitMask = 0x80;
        while (bitMask) {
    noInterrupts();
             if ( allUnidata[t] & bitMask ) {
                waktu = 9;
                while (waktu--)  {
                  GPOS = (1 << WSpin);
                  }
                waktu = 3;
                while (waktu--)  {
                  GPOC = (1 << WSpin);
                  }           
                }
             else
            {
            waktu = 3;
                while (waktu--)  {
                  GPOS = (1 << WSpin);
                  }
                waktu = 9;
                while (waktu--)  {
                  GPOC = (1 << WSpin);
                }
            }
       bitMask >>= 1;
    interrupts();
    }
  }
  delay(1);
}

void loop() {  

    readUDP();
    sendWS();

//    Serial.println(udp.destinationIP());
//   Serial.println(local_ip);

}
