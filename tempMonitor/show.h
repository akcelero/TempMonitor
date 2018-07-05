#include <Arduino.h>
#include <DallasTemperature.h>
#include <Ethernet.h>
#include "settings.h"

void show(char**, DallasTemperature&, Settings&, EthernetClient&);
