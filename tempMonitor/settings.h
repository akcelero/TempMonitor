#ifndef SETTINGS
#define SETTINGS
#include <Arduino.h>

class Settings {
  static const uint32_t maxDevNum = 24;
  uint8_t currentDevNum;
  uint8_t period = 5;
  char token[17];
  byte url[4];

  uint8_t ip[4];
  uint8_t dns[4];
  uint8_t gateway[4];
  uint8_t subnet[4];
  uint8_t staticIpFlag;

  void save();
  void load();
  void saveArray(uint8_t*, uint16_t, uint16_t);
  void loadArray(uint8_t*, uint16_t, uint16_t);
  void setAndSaveEthAdr(char*, uint8_t*);

public:

  uint8_t getCurrentDevNumber();

  void findAlias(uint8_t*, uint8_t*);
  void getAlias(int, char*);
  void getAddress(int, uint8_t*);
  void addAlias(uint8_t*, char*);
  void removeAlias(int);

  uint8_t getPeriod();
  void setPeriod(int);

  byte *getURL();
  void setURL(char*);

  char *getToken();
  void setToken(char*);

  bool getStaticIpFlag();
  void getIp(uint8_t*);
  void getDNS(uint8_t*);
  void getGateway(uint8_t*);
  void getSubnet(uint8_t*);

  void setIp(char*);
  void setDNS(char*);
  void setGateway(char*);
  void setSubnet(char*);
  void setStaticIpFlag(uint8_t);

  void begin();
};
#endif
