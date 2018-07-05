#include <Arduino.h>
#include <Ethernet.h>
#include "settings.h"


void set(char **cmds, Settings& settings, EthernetClient &client) {
  if (strcmp_P(cmds[1], PSTR("url")) == 0) {
    settings.setURL(cmds[2]);
    client.println(F("OK"));
    
  } else if (strcmp_P(cmds[1], PSTR("token")) == 0) {
    settings.setToken(cmds[2]);
    client.println(F("OK"));
    
  } else if (strcmp_P(cmds[1], PSTR("period")) == 0) {
    settings.setPeriod(atoi(cmds[2]));
    client.println(F("OK"));
    
  } else if (strcmp_P(cmds[1], PSTR("ip")) == 0) {
    settings.setIp(cmds[2]);
    client.println(F("OK"));
    
  } else if (strcmp_P(cmds[1], PSTR("dns")) == 0) {
    settings.setDNS(cmds[2]);
    client.println(F("OK"));
    
  } else if (strcmp_P(cmds[1], PSTR("gateway")) == 0) {
    settings.setGateway(cmds[2]);
    client.println(F("OK"));
    
  } else if (strcmp_P(cmds[1], PSTR("subnet")) == 0) {
    settings.setSubnet(cmds[2]);
    client.println(F("OK"));

  } else if (strcmp_P(cmds[1], PSTR("staticIP")) == 0) {
    settings.setStaticIpFlag(strcmp(cmds[2], "on") == 0);
    client.println(F("OK"));
    
  } else {
    client.print(F("Bad command "));
    client.println(cmds[1]);
  }
}

