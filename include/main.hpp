#pragma once

#include <Arduino.h>
#include <Chrono.h>
#include <SPI.h>

#include "CAN_Defs.hpp"
#include "CAN_MCP2515.hpp"
#include "MAX2253X.hpp"
#include "Pin_Defs.hpp"
#include "git.hpp"

// This just sets whether or not the positive end of the board is facing the
// front of the car or the rear so that the temp data is relative to the car
#define FORWARD
#define DEBUG

// Set target module
uint16_t id_a = MODULE_1_A;
uint16_t id_b = MODULE_1_B;
