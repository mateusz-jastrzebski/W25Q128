// W25Q128.cpp - Arduino library for communicating with the Winbond W25Q128 Serial Flash.
// v.16bits by Created by Derek Evans, July 17, 2016. // edit to v.128bits by msnbrest 2022-09-06

#include "Arduino.h"
#include "W25Q128.h"

/*
* Initializes the W25Q128 by setting the input slave select pin
* as OUTPUT and writing it HIGH. Also initializes the SPI bus,
* sets the SPI bit order to MSBFIRST and the SPI data mode to 
* SPI_MODE3, ensures the flash is not in low power mode, and
* that flash write is disabled.
*/
void W25Q128::init(int8_t FLASH_SS){
	// Argument The Slave Select or Chip Select pin used by Arduino to select W25Q128.
	pinMode(FLASH_SS, OUTPUT);
	digitalWrite(FLASH_SS, HIGH);
	_FLASH_SS = FLASH_SS;
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE3);
	releasePowerDown();
	writeDisable();
}

/*
* Reads a byte from the flash page and page address. The W25Q128 has
* 65536 pages with 256 bytes in a page. Both page and byte addresses
* start at 0. Page ends at address 65535 and page address ends at 255.
*/
byte W25Q128::read(uint16_t page, byte adress){
	// Arguments page and adress to begin reading
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(READ_DATA);
	SPI.transfer((page >> 8) & 0xFF);
	SPI.transfer((page >> 0) & 0xFF);
	SPI.transfer(adress);
	byte val = SPI.transfer(0);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
	return val;
}

/*
* Writes a byte to the flash page and page address. The W25Q128 has
* 65536 pages with 256 bytes in a page. Both page and byte addresses
* start at 0. Page ends at address 65535 and page address ends at 255.
*/
void W25Q128::write(uint16_t page, byte adress, byte val){
	// Arguments page and adress to begin writing
	writeEnable();
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(PAGE_PROGRAM);
	SPI.transfer((page >> 8) & 0xFF);
	SPI.transfer((page >> 0) & 0xFF);
	SPI.transfer(adress);
	SPI.transfer(val);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
	writeDisable();
}

/*
* Initializes flash for stream write, e.g. write more than one byte
* consecutively. Both page and byte addresses start at 0. Page 
* ends at address 65535 and page address ends at 255.
*/
void W25Q128::initStreamWrite(uint16_t page, byte adress){
	// Arguments page and adress to begin writing
	writeEnable();
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(PAGE_PROGRAM);
	SPI.transfer((page >> 8) & 0xFF);
	SPI.transfer((page >> 0) & 0xFF);
	SPI.transfer(adress);
}

/*
* Writes a byte to the W25Q128. Must be first called after 
* initStreamWrite and then consecutively to write multiple bytes.
*/
void W25Q128::streamWrite(byte val){
	// Argument byte writed to the flash.
	SPI.transfer(val);
}

/*
* Close the stream write. Must be called after the last call to streamWrite.
*/
void W25Q128::closeStreamWrite(){
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
	writeDisable();
}

/*
* Initializes flash for stream read, e.g. read more than one byte
* consecutively. Both page and byte addresses start at 0. Page 
* ends at address 65535 and page address ends at 255.
*/
void W25Q128::initStreamRead(uint16_t page, byte adress){
	// Arguments page and adress to begin reading
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(READ_DATA);
	SPI.transfer((page >> 8) & 0xFF);
	SPI.transfer((page >> 0) & 0xFF);
	SPI.transfer(adress);
}

/*
* Reads a byte from the W25Q128. Must be first called after 
* initStreamRead and then consecutively to read multiple bytes.
*/
byte W25Q128::streamRead(){
	return SPI.transfer(0);
}

/*
* Close the stream read. Must be called after the last call to streamRead.
*/
void W25Q128::closeStreamRead(){
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
}

/*
* Puts the flash in its low power mode.
*/
void W25Q128::powerDown(){
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(POWER_DOWN);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
}

/*
* Releases the flash from its low power mode. Flash cannot be in 
* low power mode to perform read/write operations.
*/
void W25Q128::releasePowerDown(){
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(RELEASE_POWER_DOWN);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
}

/*
* Erases all data from the flash.
*/
void W25Q128::chipErase(){
	writeEnable();
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(CHIP_ERASE);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
	writeDisable();
}

/*
* Reads the manufacturer ID from the W25Q128. Should return 0xEF.
*/
byte W25Q128::manufacturerID(){
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(MANUFACTURER_ID);
	SPI.transfer(0);
	SPI.transfer(0);
	SPI.transfer(0);
	byte val = SPI.transfer(0);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
	return val;
}



// # Private Methods

/*
* Halts operation until the flash is finished with its 
* write/erase operation. Bit 0 of Status Register 1 of the 
* W25Q128 is 1 if the chip is busy with a write/erase operation. 
*/
void W25Q128::notBusy(){
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(READ_STATUS_REGISTER_1);
	while(bitRead(SPI.transfer(0),0) & 1){}
	digitalWrite(_FLASH_SS, HIGH);
}

/*
* Sets Bit 1 of Status Register 1. Bit 1 is the write enable 
* latch bit of the status register. This bit must be set prior 
* to every write/erase operation. 
*/
void W25Q128::writeEnable(){
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(WRITE_ENABLE);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
}

/*
* Clears Bit 1 of Status Register 1. Bit 1 is the write enable 
* latch bit of the status register. Clearing this bit prevents 
* the flash from being written or erased. 
*/
void W25Q128::writeDisable(){
	digitalWrite(_FLASH_SS, LOW);
	SPI.transfer(WRITE_DISABLE);
	digitalWrite(_FLASH_SS, HIGH);
	notBusy();
}
