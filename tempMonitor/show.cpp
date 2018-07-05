#include <Arduino.h>
#include <DallasTemperature.h>
#include <Ethernet.h>
#include "settings.h"

void show(char **cmds, DallasTemperature &sensors, Settings &settings, EthernetClient &client) {
  
  if (strcmp_P(cmds[1], PSTR("connected")) == 0) {
    sensors.begin();
    client.print(F("Devices ("));
    client.print(sensors.getDeviceCount());
    client.println(F("):"));
    uint8_t adr[8];
    char out_str[30];
    for (int i = 0; i < sensors.getDeviceCount(); i++) {
      sensors.getAddress(adr, i);
      sprintf_P(out_str, PSTR("%d) %hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"),
            i + 1, adr[0], adr[1], adr[2], adr[0], adr[3], adr[4], adr[5], adr[6], adr[7]);
      client.println(out_str);
    }
    
  } else if (strcmp_P(cmds[1], PSTR("aliases")) == 0) {
    client.print(F("Aliases "));
    client.println(settings.getCurrentDevNumber());
    byte adr[8];
    char alias[20];
    char out_str[50];
    for (int i = 0; i < settings.getCurrentDevNumber(); i++) {
      settings.getAlias(i, alias);
      settings.getAddress(i, adr);
      sprintf_P(out_str, PSTR("%d) %s %hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"),
            i + 1, alias, adr[0], adr[1], adr[2], adr[0], adr[3], adr[4], adr[5], adr[6], adr[7]);
      client.println(out_str);
    }
    
  } else if (strcmp_P(cmds[1], PSTR("period")) == 0) {
    client.print(F("Period: "));
    client.println(settings.getPeriod());
    
  } else if (strcmp_P(cmds[1], PSTR("url")) == 0) {
    uint8_t *adr = settings.getURL();
    char out_str[25];
    sprintf_P(out_str, PSTR("URL: %d.%d.%d.%d"), adr[0], adr[1], adr[2], adr[3]);
    client.println(out_str);
    
  } else if (strcmp_P(cmds[1], PSTR("token")) == 0) {
    client.print(F("token: "));
    client.println(settings.getToken());
    
  } else if (strcmp_P(cmds[1], PSTR("ethconfig")) == 0) {
    uint8_t ip[4], dns[4], gateway[4], subnet[4];
    settings.getIp(ip);
    settings.getGateway(gateway);
    settings.getDNS(dns);
    settings.getSubnet(subnet);
    client.print(F("static IP "));
    client.println((settings.getStaticIpFlag() ? F("ON") : F("OFF")));
    char tmp[20];
    sprintf_P(tmp, PSTR("static IP: %d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
    client.println(tmp);
    sprintf_P(tmp, PSTR("DNS: %d.%d.%d.%d"), dns[0], dns[1], dns[2], dns[3]);
    client.println(tmp);
    sprintf_P(tmp, PSTR("Gateway: %d.%d.%d.%d"), gateway[0], gateway[1], gateway[2], gateway[3]);
    client.println(tmp);
    sprintf_P(tmp, PSTR("Subnet: %d.%d.%d.%d"), subnet[0], subnet[1], subnet[2], subnet[3]);
    client.println(tmp);
  } else {
    client.print(F("Bad command '"));
    client.println(cmds[1]);
    client.println("'");
  }
  client.println(F("----"));
}
