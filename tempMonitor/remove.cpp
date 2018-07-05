#include <Arduino.h>
#include <Ethernet.h>
#include "settings.h"

void remove(char **cmds, Settings &settings, EthernetClient &client) {
  settings.removeAlias(atoi(cmds[1]) - 1);
  client.println(F("OK"));
}

