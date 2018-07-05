#include <SPI.h>
#include <Ethernet.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include  <FlexiTimer2.h>
#include "show.h"
#include "add.h"
#include "remove.h"
#include "settings.h"
#include "set.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(23);
EthernetClient client = NULL;
OneWire oneWire(2);
DallasTemperature sensors(&oneWire);
Settings settings;
volatile int sec = 0;
volatile long long minute = 0;

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

ISR(TIMER1_COMPA_vect) {
  sec += 4 * 10;
  if (sec >= 60) {
    sec = 0;
    minute++;
  }
}

void setupInt() {
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
  TIMSK1 = (1 << OCIE1A);
  OCR1A = 0xf425 - 1;
}

void setup() {
  Serial.begin(9600);
  DDRB |= (1 << 1);
  setupInt();
  sei();
  settings.begin();
  if (settings.getStaticIpFlag()) {
    uint8_t ip[4], dns[4], gateway[4], subnet[4];
    settings.getIp(ip);
    settings.getGateway(gateway);
    settings.getSubnet(subnet);
    Ethernet.begin(mac, ip, dns, gateway, subnet);
  } else {
    Ethernet.begin(mac);
  }
  sensors.begin();
  server.begin();
  Serial.println(Ethernet.localIP());
}


EthernetClient receiverServer;
void sendData() {
  if (receiverServer.connect(settings.getURL(), 80) == 1) {
    sensors.begin();
    int counter = 61; // strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<response>\n</response>")
    uint8_t adr[8];
    char alias[30];
    float temp;
    
    char tmp_sensor[150];
    sensors.requestTemperatures();
    for (int i = 0; i < sensors.getDeviceCount(); i++) {
      sensors.getAddress(adr, i);
      settings.findAlias(adr, alias);
      temp = sensors.getTempCByIndex(i);
      sprintf(tmp_sensor, "<sensor>\n<name>%s</name>\n<id>%02X%02X%02X%02X%02X%02X%02X%02X</id>\n<temp>%02d.%02d</temp>\n</sensor>\n",
          alias, adr[0], adr[1], adr[2], adr[3], adr[4], adr[5], adr[6], adr[7], (int)temp, abs((int)(temp * 100) % 100));
      counter += strlen(tmp_sensor);
    }

    // ----------------------------------------------
    
    receiverServer.println(F("POST /data HTTP/1.0"));
    receiverServer.println(F("Content-type: application/xml"));
    receiverServer.print(F("Content-Length: "));
    receiverServer.println(counter);
    receiverServer.println();

    receiverServer.print(F("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<response>\n"));
    for (int i = 0; i < sensors.getDeviceCount(); i++) {
      sensors.getAddress(adr, i);
      settings.findAlias(adr, alias);
      temp = sensors.getTempCByIndex(i);
      sprintf(tmp_sensor, "<sensor>\n<name>%s</name>\n<id>%02X%02X%02X%02X%02X%02X%02X%02X</id>\n<temp>%02d.%02d</temp>\n</sensor>\n",
          alias, adr[0], adr[1], adr[2], adr[3], adr[4], adr[5], adr[6], adr[7], (int)temp, abs((int)(temp * 100) % 100));
      receiverServer.print(tmp_sensor);
    }
    receiverServer.print(F("</response>\n\n"));
    receiverServer.stop();
    return;
  }
}

char input[110];
uint8_t ptr_input;
uint8_t ptr_cmds;
char *cmds[5];
void evalCmd() {
  ptr_input = 0;
  ptr_cmds = 0;
  do {
    cmds[ptr_cmds++] = input + ptr_input;
    while (input[ptr_input] != ' ' && input[ptr_input] && ptr_input < 100) ptr_input++;
    while (input[ptr_input] == ' ' && ptr_input < 100) {
      input[ptr_input++] = 0;
    }
  } while (ptr_cmds < 5);
  
  if (strcmp_P(cmds[0], PSTR("show")) == 0) {
    show(cmds, sensors, settings, client);

  } else if (strcmp_P(cmds[0], PSTR("set")) == 0) {
    set(cmds, settings, client);

  } else if (strcmp_P(cmds[0], PSTR("remove")) == 0) {
    remove(cmds, settings, client);

  } else if (strcmp_P(cmds[0], PSTR("add")) == 0) {
    add(cmds, settings, client);

  } else if (strcmp_P(cmds[0], PSTR("help")) == 0) {
    client.println(F("show [connected|aliases|period|url|token|ethconfig]\nset token {16 alfnum characters}\nset period {number of minuts between sending data}\nset [url|ip|dns|gateway|subnet] {0-255.0-255.0-255.0-255}\nset staticIP {on|off}\nremove {number of alias to remove}\nadd {8 numbers 0-FF splitted by ':' as an address} {alias up to 19 characters}\nhelp\nexit"));

  } else if (strcmp_P(cmds[0], PSTR("exit")) == 0) {
    client.stop();
    
  } else {
    client.print(F("Bad command "));
    client.println(cmds[0]);
  }
}

void loop() {
  if (!client || !client.connected()) {
    client = server.available();
    if (client && client.connected())
      while (client.available()) client.read();
    client.println(F("Hello!"));
    client.print(F("Free memory[B]: "));
    client.println(freeMemory());
    client.println(F("type \"help\" for help"));
    client.print(F("-> s"));
  }
  if (client && client.available()) {
    if (client.available() <= 100) {
      ptr_input = 0;
      while (client.available() > 1) {
        char c = client.read();
        if (!isPrintable(c) || c == '\n') {
          break;
        }
        input[ptr_input++] = c;
        input[ptr_input] = 0;
      }
      while (client.available())client.read();
      evalCmd();
      client.print(F("-> "));
    } else {
      while (client.available())client.read();
      client.println(F("Commend too long"));
    }
  }
  if (Serial.available()) {
    String serial_cmd = Serial.readStringUntil('\n');
    if (serial_cmd == F("turnOnDHCP")) {
      settings.setStaticIpFlag(0);
      Serial.println(F("static IP off"));
    } else if (serial_cmd == F("resetMem")) {
      for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
      }
    }
  }

  if (!client.connected() && minute >= settings.getPeriod()) {
    sendData();
    minute = 0;
  }
}
