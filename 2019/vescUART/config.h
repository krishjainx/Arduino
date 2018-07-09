#ifndef CONFIG_H
#define CONFIG_H

//Determine board to use
#ifdef __AVR_ATmega2560__
#define SERIALIO Serial1
#define DEBUGSERIAL Serial
#endif

#ifdef ARDUINO_AVR_NANO
#define SERIALIO Serial
#define DEBUGSERIAL Serial
#endif

#ifdef ARDUINO_AVR_MICRO
#define SERIALIO Serial1
#define DEBUGSERIAL Serial
#endif

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "datatypes.h"
#include "local_datatypes.h"

#endif
