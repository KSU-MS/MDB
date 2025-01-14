#pragma once

#include <mcp_can.h>

#include "CAN_Defs.hpp"
#include "Pin_Defs.hpp"

// Start the MCP with CS pin with whatever is defined in MDB-V3.hpp
MCP_CAN CAN(CAN_CS);

void init_CAN() {
  pinMode(CAN_CS, OUTPUT);
  digitalWrite(CAN_CS, HIGH);

  Serial.print("Init MCP2515 on pin ");
  Serial.println(CAN_CS);
  CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
  // Change to normal mode to allow messages to be transmitted
  CAN.setMode(MCP_NORMAL);
}

// For some fucking reason if I try to use size of in this function it will
// return a straight up fucking false number and I have no fucking clue why
void send_CAN(uint16_t id, int len, uint8_t buf[]) {
  CAN.sendMsgBuf(id, len, buf);
}
