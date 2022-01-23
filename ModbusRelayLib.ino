#define UInt16 uint16_t

// UART2 on GPIO 15 and 16

const int REPONSE_DELAY_MS = 5;
const int RESPONSE_TIMEOUT_MS = 500;
const int BUFF_SIZE = 32;
const int COMMAND_RETRIES = 5;

byte _boardAddress = 3;
byte _inputStates = 0;
byte _payload[BUFF_SIZE];
byte _response[BUFF_SIZE];

//=========================================================================================
// Use the broadcast address 0 to get the board address. Return the board address
// or return 0 on error
//=========================================================================================
uint8_t GetBoardAddress()
{
  byte command[] = { 0x00, 0x03, 0x00, 0x00, 0x00, 0x01 };
  int response = sendCommand(command, sizeof(command), 7);
  if (response != 0)
  {
    _boardAddress = _response[4];
    return _boardAddress;
  }
  return 0;
}

//=========================================================================================
// Use the broadcast address 0 to set the board address. Return 1 on success, 0 on failure
//=========================================================================================
byte SetBoardAddress(byte newAddress)
{
  byte command[] = { 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, newAddress };
  int response = sendCommand(command, sizeof(command), 8);
  if (response != 0)
  {
    _boardAddress = newAddress;
    return 1;
  }
  return 0;
}


//=========================================================================================
// Switch the specified relay. Return 0 on failure, 1 on success
//=========================================================================================
byte SetRelayState(byte relay, int state)
{
  byte stateVal;
  if (state == 0)
  {
    stateVal = 0x00;
  }
  else
  {
    stateVal = 0xFF;
  }
  byte command[] {_boardAddress, 0x05, 0x00, relay, stateVal, 0x00};
  int response = 0;
  for (int retry = 0; retry < COMMAND_RETRIES; retry++)
  {
    response = sendCommand(command, sizeof(command), 8);
    if (response != 0)
    {
      return 1;
    }
  }
  return 0;
}


//=========================================================================================
// Switch ALL relays. Return 0 on failure, 1 on success
//=========================================================================================
byte SetAllRelayState(byte state)
{
  byte stateVal;
  if (state == 0)
  {
    stateVal = 0x00;
  }
  else
  {
    stateVal = 0xFF;
  }
  byte command[] {_boardAddress, 0x0F, 0x00, 0x00, 0x00, 0x08, 0x01, stateVal};
  int response = 0;
  for (int retry = 0; retry < COMMAND_RETRIES; retry++)
  {
    response = sendCommand(command, sizeof(command), 8);
    if (response != 0)
    {
      return -1;
    }
  }
  return 0;
}

//=========================================================================================
// Get the input states as a bitmapped byte. Return -1 on error
//=========================================================================================
int GetInputStates()
{
  byte command[] = { _boardAddress, 0x02, 0x00, 0x00, 0x00, 0x08 };
  int response = sendCommand(command, sizeof(command), 6);
  if (response != 0)
  {
    _inputStates = _response[3];
    return _inputStates;
  }
  return -1;
}

int sendCommand(byte* command, int len, int responseLen)
{
  // Calculate checksum of data
  UInt16 cs = ModRTU_CRC(command, len);

  // Load command into payload
  memcpy(_payload, command, len);
  // Append checksum
  memcpy(&_payload[len], &cs, 2);

  // Write to serial port
  flushInputBuffer();
  //  for (int i = 0; i < len + 2; i++)
  //  {
  //    byte b = _payload[i];
  //    Serial.write(b);
  //  }
  MODBUS.write(_payload, len + 2); // 2 more bytes to include cs
  DBG("SENT:");
  debugArraySerial1(_payload, len + 2);

  // Wait for a bit
  delay(REPONSE_DELAY_MS);

  // Read Serial
  long endTime = millis() + RESPONSE_TIMEOUT_MS;
  int pnt = 0;
  while (true)
  {
    while (MODBUS.available() > 0)
    {
      _response[pnt] = MODBUS.read();
      //Serial1.print(_response[pnt],HEX);
      pnt++;
      if (pnt > BUFF_SIZE) {
        // Stop buffer overflow
        DBG("OVERFLOW");
        debugArraySerial1(_response, pnt);
        return BUFF_SIZE;
      }
      else if (responseLen > 0 && pnt == responseLen)
      {
        // We have enough bytes
        DBG("RECV:");
        debugArraySerial1(_response, pnt);
        return responseLen;
      }
    }
    // Check for timeout
    if (millis() > endTime)
    {
      DBG("TIMEOUT:");
      debugArraySerial1(_response, pnt);
      return pnt;
    }
  }
  return pnt;
}

void flushInputBuffer()
{
  char c;
  // Flush the input buffer
  while (MODBUS.available() > 0) {
    c = MODBUS.read();
  }
}

/*
  From: http://www.ccontrolsys.com/w/How_to_Compute_the_Modbus_RTU_Message_CRC

  Using the 8 character ASCII input DEADBEEF (upper case)
  the checksum is 0xDD18
  The code below agrees with the online calculator here:
  http://www.lammertbies.nl/comm/info/crc-calculation.html
*/

// Compute the MODBUS RTU CRC
// Usage e.g.
//char *t = (char *)"DEADBEEF";
//Serial.println(ModRTU_CRC(t,strlen(t)),HEX);
//CRC should be DD18

UInt16 ModRTU_CRC(byte *buf, int len)
{
  UInt16 crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (UInt16)buf[pos];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

void debugArraySerial1(byte* command, int len)
{
  return;
  for (int i = 0; i < len; i++)
  {
    DBG(command[i], HEX);
    DBG(" ");
  }
  DBG("\r\n");
}
