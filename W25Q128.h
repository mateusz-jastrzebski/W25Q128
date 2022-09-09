#ifndef W25Q128_h
#define W25Q128_h

#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0x04
#define PAGE_PROGRAM 0x02
#define READ_STATUS_REGISTER_1 0x05
#define READ_DATA 0x03
#define CHIP_ERASE 0xC7
#define POWER_DOWN 0xB9
#define RELEASE_POWER_DOWN 0xAB
#define MANUFACTURER_ID 0x90

#include "Arduino.h"
#include "SPI.h"

class W25Q128{
public:
  void init(int8_t FLASH_SS);
  void write(uint16_t page, byte adress, byte val);
  byte read(uint16_t page, byte adress);
  void initStreamWrite(uint16_t page, byte adress);
  void streamWrite(byte val);
  void closeStreamWrite();
  void initStreamRead(uint16_t page, byte adress);
  byte streamRead();
  void closeStreamRead();
  void powerDown();
  void releasePowerDown();
  void chipErase();
  byte manufacturerID();
private:
  int _FLASH_SS;
  void writeEnable();
  void writeDisable();
  void notBusy();
};

#endif
