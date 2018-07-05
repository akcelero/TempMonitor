#include <Arduino.h>
#include <DallasTemperature.h>
#include <Ethernet.h>
#include "settings.h"

void add(char **cmds, Settings& settings, EthernetClient &client) {
  uint8_t adr[8];
  if(8 != sscanf_P(cmds[1], PSTR("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"), adr, adr + 1, adr + 2, adr + 3, adr + 4, adr + 5, adr + 6, adr + 7)) {
    client.println(F("Problem with parsing address"));
    return;
  }
  char tmp[20];
  settings.findAlias(adr, tmp);
  if (tmp[0] != 0) {
    client.print(F("Alias already exists "));
    client.println(tmp);
    return;
  }
  settings.addAlias(adr, cmds[2]);
  client.println(F("OK"));
}

