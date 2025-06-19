#ifndef SCANNER_H
#define SCANNER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"
#include <RF24.h>

void initScanner();
void runScanner(U8G2& u8g2, bool& shouldExit);

#endif