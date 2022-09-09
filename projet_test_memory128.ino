#include <SPI.h>
#include <W25Q128.h>

W25Q128 flash;
uint16_t page= 0;
byte adress= 0;

void setup(){
  Serial.begin(9600);
  
  flash.init(10); // initialize the pcf2127
  
  //Serial.print("Manufacturer ID: ");Serial.println(flash.manufacturerID(),HEX);//should output 0xEF
  //put the flash in lowest power state
  flash.powerDown();
}

void loop(){
  flash.releasePowerDown();

  //flash.write(page,adress,(96+millis())%256); // write

  consoleOutput(page,adress,flash.read(page,adress)); // read

  adress++; if( adress==0 ){ page++; } // prepar next
  // -- you can speed :
  // adress+=16;
  // -- or faster like :
  // page+=4096;

  flash.powerDown();

  delay(1000);
}

//formats the output for the console
void consoleOutput(uint16_t page, byte address, byte val){ Serial.print("Page ");Serial.print(page);Serial.print("/65536 Address ");Serial.print(address);Serial.print(": ");Serial.println(val); }
