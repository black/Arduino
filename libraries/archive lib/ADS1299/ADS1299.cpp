//
//  ADS1299DIAISY.cpp   ARDUINO LIBRARY FOR COMMUNICATING WITH TWO
//  DAISY-CHAINED ADS1299 BOARDS
//
//  Created by Conor Russomanno, Luke Travis, and Joel Murphy. Summer, 2013
//
//  Extended by Chip Audette through April 2014
//

#include "ADS1299.h"

void ADS1299::initialize(int _DRDY, int _RST, int _CS, int _FREQ, boolean _isDaisy) {
  pinMode(9,OUTPUT);
  pinMode(9,LOW);
  delay(1000);
  pinMode(9,HIGH);
  delay(4000);
  isDaisy = _isDaisy;
  DRDY = _DRDY;
  CS = _CS;
  int FREQ = _FREQ;
  int RST = _RST;

  pinMode(9,OUTPUT);

  delay(50);  // recommended power up sequence requiers Tpor (~32mS)
  pinMode(RST, OUTPUT);
  pinMode(RST, LOW);
  delayMicroseconds(4);  // toggle reset pin
  pinMode(RST, HIGH);
  delayMicroseconds(20);  // recommended to wait 18 Tclk before using device (~8uS);


  // **** ----- SPI Setup ----- **** //

  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  spi_init(spi0,/_FREQ*10^6);
  spi_set_slave(spi0, false);
  spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
  gpio_set_function(4, GPIO_FUNC_SPI);
  gpio_set_function(6, GPIO_FUNC_SPI);
  gpio_set_function(7, GPIO_FUNC_SPI);


  // **** ----- End of SPI Setup ----- **** //

  // initalize the  data ready chip select and reset pins:
  pinMode(DRDY, INPUT);
  pinMode(CS, OUTPUT);

  digitalWrite(CS, HIGH);
  digitalWrite(RST, HIGH);
}

//System Commands
void ADS1299::WAKEUP() {
  digitalWrite(CS, LOW);
  transfer(_WAKEUP);
  digitalWrite(CS, HIGH);
  delayMicroseconds(3);  //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}

void ADS1299::STANDBY() {  // only allowed to send WAKEUP after sending STANDBY
  digitalWrite(CS, LOW);
  transfer(_STANDBY);
  digitalWrite(CS, HIGH);
}

void ADS1299::RESET() {  // reset all the registers to default settings
  digitalWrite(CS, LOW);
  transfer(_RESET);
  delayMicroseconds(12);  //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  digitalWrite(CS, HIGH);
}

void ADS1299::START() {  //start data conversion
  digitalWrite(CS, LOW);
  transfer(_START);
  digitalWrite(CS, HIGH);
}

void ADS1299::STOP() {  //stop data conversion
  digitalWrite(CS, LOW);
  transfer(_STOP);
  digitalWrite(CS, HIGH);
}

void ADS1299::RDATAC() {
  digitalWrite(CS, LOW);
  transfer(_RDATAC);
  digitalWrite(CS, HIGH);
  delayMicroseconds(3);
}
void ADS1299::SDATAC() {
  digitalWrite(CS, LOW);
  transfer(_SDATAC);
  digitalWrite(CS, HIGH);
  delayMicroseconds(3);  //must wait 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}


// Register Read/Write Commands
byte ADS1299::getDeviceID() {  // simple hello world com check
  byte data = RREG(0x00);
  if (verbose) {  // verbose otuput
    Serial.print(F("Device ID "));
    printHex(data);
  }
  return data;
}

byte ADS1299::RREG(byte _address) {    //  reads ONE register at _address
  byte opcode1 = _address + 0x20;      //  RREG expects 001rrrrr where rrrrr = _address
  digitalWrite(CS, LOW);               //  open SPI
  transfer(opcode1);                   //  opcode1
  transfer(0x00);                      //  opcode2
  regData[_address] = transfer(0x00);  //  update mirror location with returned byte
  digitalWrite(CS, HIGH);              //  close SPI
  if (verbose) {                       //  verbose output
    printRegisterName(_address);
    printHex(_address);
    Serial.print(", ");
    printHex(regData[_address]);
    Serial.print(", ");
    for (byte j = 0; j < 8; j++) {
      Serial.print(bitRead(regData[_address], 7 - j));
      if (j != 7) Serial.print(", ");
    }

    Serial.println();
  }
  return regData[_address];  // return requested register value
}

// Read more than one register starting at _address
void ADS1299::RREGS(byte _address, byte _numRegistersMinusOne) {
  byte opcode1 = _address + 0x20;   //  RREG expects 001rrrrr where rrrrr = _address
  digitalWrite(CS, LOW);            //  open SPI
  transfer(opcode1);                //  opcode1
  transfer(_numRegistersMinusOne);  //  opcode2
  for (int i = 0; i <= _numRegistersMinusOne; i++) {
    regData[_address + i] = transfer(0x00);  //  add register byte to mirror array
  }
  digitalWrite(CS, HIGH);  //  close SPI
  if (verbose) {           //  verbose output
    for (int i = 0; i <= _numRegistersMinusOne; i++) {
      printRegisterName(_address + i);
      printHex(_address + i);
      Serial.print(", ");
      printHex(regData[_address + i]);
      Serial.print(", ");
      for (int j = 0; j < 8; j++) {
        Serial.print(bitRead(regData[_address + i], 7 - j));
        if (j != 7) Serial.print(", ");
      }
      Serial.println();
    }
  }
}

void ADS1299::WREG(byte _address, byte _value) {  //  Write ONE register at _address
  byte opcode1 = _address + 0x40;                 //  WREG expects 010rrrrr where rrrrr = _address
  digitalWrite(CS, LOW);                          //  open SPI
  transfer(opcode1);                              //  Send WREG command & address
  transfer(0x00);                                 //  Send number of registers to read -1
  transfer(_value);                               //  Write the value to the register
  digitalWrite(CS, HIGH);                         //  close SPI
  regData[_address] = _value;                     //  update the mirror array
  if (verbose) {                                  //  verbose output
    Serial.print(F("Register "));
    printHex(_address);
    Serial.println(F(" modified."));
  }
}

void ADS1299::WREGS(byte _address, byte _numRegistersMinusOne) {
  byte opcode1 = _address + 0x40;   //  WREG expects 010rrrrr where rrrrr = _address
  digitalWrite(CS, LOW);            //  open SPI
  transfer(opcode1);                //  Send WREG command & address
  transfer(_numRegistersMinusOne);  //  Send number of registers to read -1
  for (int i = _address; i <= (_address + _numRegistersMinusOne); i++) {
    transfer(regData[i]);  //  Write to the registers
  }
  digitalWrite(CS, HIGH);  //  close SPI
  if (verbose) {
    Serial.print(F("Registers "));
    printHex(_address);
    Serial.print(F(" to "));
    printHex(_address + _numRegistersMinusOne);
    Serial.println(F(" modified"));
  }
}


void ADS1299::updateChannelData() {
  byte inByte;
  int nchan = 8;          //assume 8 channel.  If needed, it automatically changes to 16 automatically in a later block.
  digitalWrite(CS, LOW);  //  open SPI

  // READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
  for (int i = 0; i < 3; i++) {  //  read 3 byte status register from ADS 1 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
    inByte = transfer(0x00);
    stat_1 = (stat_1 << 8) | inByte;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {  //  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
      inByte = transfer(0x00);
      channelData[i] = (channelData[i] << 8) | inByte;
    }
  }

  if (isDaisy) {
    nchan = 16;
    // READ CHANNEL DATA FROM SECOND ADS IN DAISY LINE
    for (int i = 0; i < 3; i++) {  //  read 3 byte status register from ADS 2 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
      inByte = transfer(0x00);
      stat_2 = (stat_1 << 8) | inByte;
    }

    for (int i = 8; i < 16; i++) {
      for (int j = 0; j < 3; j++) {  //  read 24 bits of channel data from 2nd ADS in 8 3 byte chunks
        inByte = transfer(0x00);
        channelData[i] = (channelData[i] << 8) | inByte;
      }
    }
  }

  digitalWrite(CS, HIGH);  //  close SPI

  //reformat the numbers
  for (int i = 0; i < nchan; i++) {  // convert 3 byte 2's compliment to 4 byte 2's compliment
    if (bitRead(channelData[i], 23) == 1) {
      channelData[i] |= 0xFF000000;
    } else {
      channelData[i] &= 0x00FFFFFF;
    }
  }
}


//read data
void ADS1299::RDATA() {  //  use in Stop Read Continuous mode when DRDY goes low
  byte inByte;
  stat_1 = 0;  //  clear the status registers
  stat_2 = 0;
  int nchan = 8;          //assume 8 channel.  If needed, it automatically changes to 16 automatically in a later block.
  digitalWrite(CS, LOW);  //  open SPI
  transfer(_RDATA);

  // READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
  for (int i = 0; i < 3; i++) {  //  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
    inByte = transfer(0x00);
    stat_1 = (stat_1 << 8) | inByte;
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {  //  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
      inByte = transfer(0x00);
      channelData[i] = (channelData[i] << 8) | inByte;
    }
  }

  if (isDaisy) {
    nchan = 16;

    // READ CHANNEL DATA FROM SECOND ADS IN DAISY LINE
    for (int i = 0; i < 3; i++) {  //  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
      inByte = transfer(0x00);
      stat_2 = (stat_1 << 8) | inByte;
    }

    for (int i = 8; i < 16; i++) {
      for (int j = 0; j < 3; j++) {  //  read 24 bits of channel data from 2nd ADS in 8 3 byte chunks
        inByte = transfer(0x00);
        channelData[i] = (channelData[i] << 8) | inByte;
      }
    }
  }

  for (int i = 0; i < nchan; i++) {  // convert 3 byte 2's compliment to 4 byte 2's compliment
    if (bitRead(channelData[i], 23) == 1) {
      channelData[i] |= 0xFF000000;
    } else {
      channelData[i] &= 0x00FFFFFF;
    }
  }
}



// String-Byte converters for RREG and WREG
void ADS1299::printRegisterName(byte _address) {
  if (_address == ID) {
    Serial.print(F("ID, "));  //the "F" macro loads the string directly from Flash memory, thereby saving RAM
  } else if (_address == CONFIG1) {
    Serial.print(F("CONFIG1, "));
  } else if (_address == CONFIG2) {
    Serial.print(F("CONFIG2, "));
  } else if (_address == CONFIG3) {
    Serial.print(F("CONFIG3, "));
  } else if (_address == LOFF) {
    Serial.print(F("LOFF, "));
  } else if (_address == CH1SET) {
    Serial.print(F("CH1SET, "));
  } else if (_address == CH2SET) {
    Serial.print(F("CH2SET, "));
  } else if (_address == CH3SET) {
    Serial.print(F("CH3SET, "));
  } else if (_address == CH4SET) {
    Serial.print(F("CH4SET, "));
  } else if (_address == CH5SET) {
    Serial.print(F("CH5SET, "));
  } else if (_address == CH6SET) {
    Serial.print(F("CH6SET, "));
  } else if (_address == CH7SET) {
    Serial.print(F("CH7SET, "));
  } else if (_address == CH8SET) {
    Serial.print(F("CH8SET, "));
  } else if (_address == BIAS_SENSP) {
    Serial.print(F("BIAS_SENSP, "));
  } else if (_address == BIAS_SENSN) {
    Serial.print(F("BIAS_SENSN, "));
  } else if (_address == LOFF_SENSP) {
    Serial.print(F("LOFF_SENSP, "));
  } else if (_address == LOFF_SENSN) {
    Serial.print(F("LOFF_SENSN, "));
  } else if (_address == LOFF_FLIP) {
    Serial.print(F("LOFF_FLIP, "));
  } else if (_address == LOFF_STATP) {
    Serial.print(F("LOFF_STATP, "));
  } else if (_address == LOFF_STATN) {
    Serial.print(F("LOFF_STATN, "));
  } else if (_address == GPIO) {
    Serial.print(F("GPIO, "));
  } else if (_address == MISC1) {
    Serial.print(F("MISC1, "));
  } else if (_address == MISC2) {
    Serial.print(F("MISC2, "));
  } else if (_address == CONFIG4) {
    Serial.print(F("CONFIG4, "));
  }
}

//SPI communication methods
byte ADS1299::transfer(byte data) {
  __disable_irq();
  spi_write_blocking(spi0, &data, 1);
  while (spi_is_busy(spi0))
    ;
  uint8_t received_data;
  spi_read_blocking(spi0, 0, &received_data, 1);
  __enable_irq();
  return received_data;
}

// Used for printing HEX in verbose feedback mode
void ADS1299::printHex(byte _data) {
  Serial.print("0x");
  if (_data < 0x10) Serial.print("0");
  Serial.print(_data, HEX);
}

//-------------------------------------------------------------------//
//-------------------------------------------------------------------//
//-------------------------------------------------------------------//