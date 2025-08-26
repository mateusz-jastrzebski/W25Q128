#ifndef W25Q128_h
#define W25Q128_h

#include <cstdint>
#include "hardware/spi.h"
#include "pico/stdlib.h"

// Command opcodes
#define W25Q_WRITE_ENABLE            0x06
#define W25Q_WRITE_DISABLE           0x04
#define W25Q_PAGE_PROGRAM            0x02
#define W25Q_READ_STATUS_REGISTER_1  0x05
#define W25Q_READ_DATA               0x03
#define W25Q_CHIP_ERASE              0xC7
#define W25Q_SECTOR_ERASE            0x20
#define W25Q_POWER_DOWN              0xB9
#define W25Q_RELEASE_POWER_DOWN      0xAB
#define W25Q_MANUFACTURER_ID         0x90

class W25Q128 {
public:
    // Initialize with SPI instance and chip-select GPIO
    void init(spi_inst_t *spi, uint cs_gpio);

    uint8_t read(uint16_t page, uint8_t address);
    void    write(uint16_t page, uint8_t address, uint8_t val);

    // Paged stream write/read helpers
    void initStreamWrite(uint16_t page, uint8_t address);
    void streamWrite(uint8_t val);
    void closeStreamWrite();

    void initStreamRead(uint16_t page, uint8_t address);
    uint8_t streamRead();
    void closeStreamRead();

    void powerDown();
    void releasePowerDown();
    void chipErase(); // blocking (legacy)
    void beginChipErase(); // non-blocking start
    bool isBusy();         // poll WIP bit
    uint8_t manufacturerID();

    // Bulk operations (handle page boundaries internally)
    void writeBuffer(uint32_t addr, const uint8_t *data, uint32_t len);
    void readBuffer(uint32_t addr, uint8_t *data, uint32_t len);
    void sectorErase(uint32_t addr); // 4KB sector erase (blocking)
    void beginSectorErase(uint32_t addr); // non-blocking sector erase start
    void beginPageProgram(uint32_t addr, const uint8_t *data, uint32_t len); // non-blocking page (<=256, single page)
    static constexpr uint32_t TOTAL_SIZE_BYTES = 16u * 1024u * 1024u; // 128Mbit
    static constexpr uint32_t PAGE_SIZE = 256; // bytes

private:
    spi_inst_t *m_spi = nullptr;
    uint m_cs = 0;

    void cs_low()  { gpio_put(m_cs, 0); }
    void cs_high() { gpio_put(m_cs, 1); }

    void writeEnable();
    void writeDisable();
    void waitWhileBusy();
    uint8_t readStatus1();
};

#endif
