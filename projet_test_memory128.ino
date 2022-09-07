#include <SPI.h>
#include <W25Q128.h>

W25Q128 flash;
word endPage = 0;
word endPageAddress = 0;

void setup() {
  Serial.begin(9600);
  
  //initialize the pcf2127
  flash.init(10); 
  
  //erase the entire chips contents
  //flash.chipErase(); // todo: essayer de lire mémoire sans cette ligne :D
  
  //read the manufacturer ID to make sure communications are OK
  //should output 0xEF
  //byte manID = flash.manufacturerID(); 
  //Serial.print("Manufacturer ID: ");
  //Serial.println(manID,HEX);z
  //put the flash in lowest power state
  flash.powerDown();
}

void loop() {

  flash.releasePowerDown();

  // IMPORTANT : decomment to test with write
  //flash.write(endPage,(byte)endPageAddress,(1+millis())%256);

  consoleOutput(endPage,endPageAddress,flash.read(endPage,(byte)endPageAddress));

  endPageAddress++; 
  endOfPage();

  flash.powerDown();

  delay(1900);
}

// anti over
void endOfPage(){ if( endPageAddress>255 ){ endPage++; endPageAddress= 0; } }

//formats the output for the console
void consoleOutput(unsigned int page, unsigned int address, byte val) {
   Serial.print("Page ");
   Serial.print(page);
   Serial.print(" Address ");
   Serial.print(address);
   Serial.print(": ");
   Serial.println(val);
}
