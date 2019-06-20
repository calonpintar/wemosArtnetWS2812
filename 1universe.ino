//1 UNIVERSE to 1 pin


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
uint8_t WSpin = 5;
uint16_t uniSize;
uint8_t hData[ARTNET_HEADER + 1];
uint8_t net = 0;
uint8_t universe;
uint8_t subnet = 0;

uint8_t  bitMask, timeCtr;

const char* ssid     = "MODULIGHT24";
const char* password = "cobainaja";

IPAddress local_ip(192, 168, 1, 106); //your esp8226 supossed IP
IPAddress gateway_ip(192, 168, 1, 1); //your network gateway IP
IPAddress subnet_ip(255, 255, 255, 0); //your network subnet mask

void setup() {
  //Serial.begin(115200);
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

  pinMode(16, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
}

void ICACHE_FLASH_ATTR readUDP() {
    if (udp.parsePacket()) {
        udp.read(hData, 18);
     if ( hData[0] == 'A' && hData[4] == 'N') {
         uniSize = (hData[16] << 8) + (hData[17]);
         udp.read(uniData, uniSize);
         universe = hData[14];
         
    switch (universe) {
      case 0:
        WSpin = 5;
       break;
      case 1:
        WSpin = 4;
        break;
      default:
        WSpin = 16;
        break;
    }      
     }
    }
}

void ICACHE_FLASH_ATTR sendWS() {
    for (uint16_t t = 0; t < uniSize; t++) {
        bitMask = 0x80;
        while (bitMask) {
    noInterrupts();
             if ( uniData[t] & bitMask ) {
                timeCtr = 9;
                while (timeCtr--)  {
                  GPOS = (1 << WSpin);
                  }
                timeCtr = 3;
                while (timeCtr--)  {
                  GPOC = (1 << WSpin);
                  }           
                }
             else
            {
            timeCtr = 3;
                while (timeCtr--)  {
                  GPOS = (1 << WSpin);
                  }
                timeCtr = 9;
                while (timeCtr--)  {
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
}
