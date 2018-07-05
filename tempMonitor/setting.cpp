#include <Arduino.h>
#include "settings.h"
#include <EEPROM.h>

uint8_t Settings::getCurrentDevNumber() {return currentDevNum;};
uint8_t Settings::getPeriod() {return period;}
byte *Settings::getURL() {return url;}
char *Settings::getToken() {return token;}
bool Settings::getStaticIpFlag() {return staticIpFlag == 1;};

void Settings::setPeriod(int d) {period = d; save();}
void Settings::setToken(char *tmp_token) {
  memcpy(token, tmp_token, 16);
  token[16] = 0;
  save();
}

void Settings::getAlias(int i, char *tab) {loadArray(tab, 400 + (i * 20), 20);}

void Settings::getAddress(int i, uint8_t* tab) {loadArray(tab, 200 + (i * 8), 8);}

void Settings::removeAlias(int rem) {
  if (rem < 0 || rem >= currentDevNum) {
    return;
  }
  uint8_t tmp[20];
  for (int i = rem; i < currentDevNum - 1; i++) {
    loadArray(tmp, 400 + 20 * (i + 1), 20);
    saveArray(tmp, 400 + 20 * i, 20);
    loadArray(tmp, 200 + 8 * (i + 1), 8);
    saveArray(tmp, 200 + 8 * i, 8);
  }
  currentDevNum--;
  save();
}

void Settings::findAlias(uint8_t *adr, uint8_t *alias) {
  uint8_t adr_tmp[8];
  for (int i = 0; i < currentDevNum; i++) {
    getAddress(i, adr_tmp);
    if (0 == memcmp(adr, adr_tmp, 8)) {
      getAlias(i, alias);
      return;
    }
  }
  alias[0] = 0;
}

void Settings::addAlias(uint8_t *adr, char *alias) {
  char tmp[30];
  sprintf_P(tmp, PSTR("%x:%x:%x:%x:%x:%x:%x:%x"), adr[0], adr[1], adr[2], adr[3], adr[4], adr[5], adr[6], adr[7]);
  Serial.println(tmp);
  if (currentDevNum >= maxDevNum)
    return;
  saveArray(adr, 200 + (8 * currentDevNum), 8);
  saveArray(alias, 400 + (20 * currentDevNum), 20);
  currentDevNum++;
  save();
}

void Settings::saveArray(uint8_t* tab, uint16_t start, uint16_t size) {
  for (int i = 0; i < size; i++)
    EEPROM.write(start + i, tab[i]);
}

void Settings::loadArray(uint8_t* tab, uint16_t start, uint16_t size) {
  for (int i = 0; i < size; i++)
    tab[i] = EEPROM.read(start + i);
}

void Settings::save() {
  saveArray(url, 0, 4);
  saveArray(token, 100, 17);
  saveArray(ip, 116, 4);
  saveArray(dns, 120, 4);
  saveArray(gateway, 124, 4);
  saveArray(subnet, 128, 4);
  EEPROM.write(132, staticIpFlag);
  EEPROM.write(133, period);
  EEPROM.write(190, currentDevNum);
}

void Settings::load() {
  loadArray(url, 0, 4);
  loadArray(token, 100, 17);
  token[16] = 0;
  loadArray(ip, 116, 4);
  loadArray(dns, 120, 4);
  loadArray(gateway, 124, 4);
  loadArray(subnet, 128, 4);
  staticIpFlag = EEPROM.read(132);
  period = EEPROM.read(133);
  currentDevNum = EEPROM.read(190);
}

void Settings::getIp(uint8_t* dest) {memcpy(dest, ip, 4);}
void Settings::getDNS(uint8_t* dest) {memcpy(dest, dns, 4);}
void Settings::getGateway(uint8_t* dest) {memcpy(dest, gateway, 4);}
void Settings::getSubnet(uint8_t* dest) {memcpy(dest, subnet, 4);}

void Settings::setAndSaveEthAdr(char *str, uint8_t *tab) {
  sscanf_P(str, PSTR("%hhu.%hhu.%hhu.%hhu"), tab, tab + 1, tab + 2, tab + 3);
  save();
}

void Settings::setURL(char *str) {setAndSaveEthAdr(str, url);}
void Settings::setIp(char *str) {setAndSaveEthAdr(str, ip);}
void Settings::setDNS(char *str) {setAndSaveEthAdr(str, dns);}
void Settings::setGateway(char *str) {setAndSaveEthAdr(str, gateway);}
void Settings::setSubnet(char *str) {setAndSaveEthAdr(str, subnet);}

void Settings::setStaticIpFlag(uint8_t flag) {
  staticIpFlag = (uint8_t)flag;
  save();
}

void Settings::begin() {
  load();
}

