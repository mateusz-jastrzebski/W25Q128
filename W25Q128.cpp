#include "W25Q128.h"

// Helper to read a single byte over SPI (full-duplex) sending a dummy byte
static inline uint8_t spi_xfer(spi_inst_t *spi, uint8_t v) {
    uint8_t rx;
    spi_write_read_blocking(spi, &v, &rx, 1);
    return rx;
}

void W25Q128::init(spi_inst_t *spi, uint cs_gpio) {
    m_spi = spi;
    m_cs = cs_gpio;
    gpio_init(m_cs);
    gpio_set_dir(m_cs, GPIO_OUT);
    gpio_put(m_cs, 1);
    // (Assumes SPI already configured globally with correct mode 0)
    releasePowerDown();
    writeDisable();
}

uint8_t W25Q128::read(uint16_t page, uint8_t address) {
    uint32_t addr = ((uint32_t)page << 8) | address; // 24-bit address
    cs_low();
    spi_xfer(m_spi, W25Q_READ_DATA);
    uint8_t a2 = (addr >> 16) & 0xFF;
    uint8_t a1 = (addr >> 8) & 0xFF;
    uint8_t a0 = (addr >> 0) & 0xFF;
    spi_xfer(m_spi, a2);
    spi_xfer(m_spi, a1);
    spi_xfer(m_spi, a0);
    uint8_t val = spi_xfer(m_spi, 0x00);
    cs_high();
    waitWhileBusy();
    return val;
}

void W25Q128::write(uint16_t page, uint8_t address, uint8_t val) {
    uint32_t addr = ((uint32_t)page << 8) | address;
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_PAGE_PROGRAM);
    spi_xfer(m_spi, (addr >> 16) & 0xFF);
    spi_xfer(m_spi, (addr >> 8) & 0xFF);
    spi_xfer(m_spi, (addr >> 0) & 0xFF);
    spi_xfer(m_spi, val);
    cs_high();
    waitWhileBusy();
    writeDisable();
}

void W25Q128::initStreamWrite(uint16_t page, uint8_t address) {
    uint32_t addr = ((uint32_t)page << 8) | address;
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_PAGE_PROGRAM);
    spi_xfer(m_spi, (addr >> 16) & 0xFF);
    spi_xfer(m_spi, (addr >> 8) & 0xFF);
    spi_xfer(m_spi, (addr >> 0) & 0xFF);
}

void W25Q128::streamWrite(uint8_t val) {
    spi_xfer(m_spi, val);
}

void W25Q128::closeStreamWrite() {
    cs_high();
    waitWhileBusy();
    writeDisable();
}

void W25Q128::initStreamRead(uint16_t page, uint8_t address) {
    uint32_t addr = ((uint32_t)page << 8) | address;
    cs_low();
    spi_xfer(m_spi, W25Q_READ_DATA);
    spi_xfer(m_spi, (addr >> 16) & 0xFF);
    spi_xfer(m_spi, (addr >> 8) & 0xFF);
    spi_xfer(m_spi, (addr >> 0) & 0xFF);
}

uint8_t W25Q128::streamRead() {
    return spi_xfer(m_spi, 0x00);
}

void W25Q128::closeStreamRead() {
    cs_high();
    waitWhileBusy();
}

void W25Q128::powerDown() {
    cs_low();
    spi_xfer(m_spi, W25Q_POWER_DOWN);
    cs_high();
    waitWhileBusy();
}

void W25Q128::releasePowerDown() {
    cs_low();
    spi_xfer(m_spi, W25Q_RELEASE_POWER_DOWN);
    cs_high();
    waitWhileBusy();
}

void W25Q128::chipErase() {
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_CHIP_ERASE);
    cs_high();
    waitWhileBusy();
    writeDisable();
}

void W25Q128::beginChipErase() {
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_CHIP_ERASE);
    cs_high();
    // do not wait; caller will poll isBusy()
}

bool W25Q128::isBusy() {
    return (readStatus1() & 0x01) != 0; // WIP bit
}

void W25Q128::sectorErase(uint32_t addr) {
    // addr is absolute flash address (must be 4KB aligned)
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_SECTOR_ERASE);
    spi_xfer(m_spi, (addr >> 16) & 0xFF);
    spi_xfer(m_spi, (addr >> 8) & 0xFF);
    spi_xfer(m_spi, (addr >> 0) & 0xFF);
    cs_high();
    waitWhileBusy();
    writeDisable();
}

void W25Q128::beginSectorErase(uint32_t addr) {
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_SECTOR_ERASE);
    spi_xfer(m_spi, (addr >> 16) & 0xFF);
    spi_xfer(m_spi, (addr >> 8) & 0xFF);
    spi_xfer(m_spi, (addr >> 0) & 0xFF);
    cs_high();
    // caller polls isBusy()
}

void W25Q128::beginPageProgram(uint32_t addr, const uint8_t *data, uint32_t len) {
    if (len == 0) return;
    if (len > PAGE_SIZE) len = PAGE_SIZE; // clamp
    // Ensure it does not cross page boundary
    uint32_t page_off = addr & (PAGE_SIZE - 1);
    if (page_off + len > PAGE_SIZE) {
        len = PAGE_SIZE - page_off;
    }
    writeEnable();
    cs_low();
    spi_xfer(m_spi, W25Q_PAGE_PROGRAM);
    spi_xfer(m_spi, (addr >> 16) & 0xFF);
    spi_xfer(m_spi, (addr >> 8) & 0xFF);
    spi_xfer(m_spi, (addr >> 0) & 0xFF);
    spi_write_blocking(m_spi, data, len);
    cs_high();
    // No wait here; caller must poll isBusy() then optionally writeDisable() if desired (not strictly required after each op)
}

uint8_t W25Q128::manufacturerID() {
    cs_low();
    spi_xfer(m_spi, W25Q_MANUFACTURER_ID);
    spi_xfer(m_spi, 0x00); // dummy 3 bytes address per JEDEC ID CMD 0x90
    spi_xfer(m_spi, 0x00);
    spi_xfer(m_spi, 0x00);
    uint8_t mid = spi_xfer(m_spi, 0x00); // manufacturer ID
    cs_high();
    waitWhileBusy();
    return mid;
}

void W25Q128::writeBuffer(uint32_t addr, const uint8_t *data, uint32_t len) {
    while (len) {
        uint32_t page_off = addr & (PAGE_SIZE - 1);
        uint32_t space = PAGE_SIZE - page_off;
        uint32_t chunk = (len < space) ? len : space;
        writeEnable();
        cs_low();
        spi_xfer(m_spi, W25Q_PAGE_PROGRAM);
        spi_xfer(m_spi, (addr >> 16) & 0xFF);
        spi_xfer(m_spi, (addr >> 8) & 0xFF);
        spi_xfer(m_spi, (addr >> 0) & 0xFF);
        spi_write_blocking(m_spi, data, chunk);
        cs_high();
        waitWhileBusy();
        writeDisable();
        addr += chunk;
        data += chunk;
        len -= chunk;
    }
}

void W25Q128::readBuffer(uint32_t addr, uint8_t *data, uint32_t len) {
    while (len) {
        uint32_t chunk = len; // can read across boundary, but limit to, say, 1024
        if (chunk > 1024) chunk = 1024;
        cs_low();
        spi_xfer(m_spi, W25Q_READ_DATA);
        spi_xfer(m_spi, (addr >> 16) & 0xFF);
        spi_xfer(m_spi, (addr >> 8) & 0xFF);
        spi_xfer(m_spi, (addr >> 0) & 0xFF);
        spi_read_blocking(m_spi, 0xFF, data, chunk);
        cs_high();
        addr += chunk;
        data += chunk;
        len -= chunk;
    }
}

uint8_t W25Q128::readStatus1() {
    cs_low();
    spi_xfer(m_spi, W25Q_READ_STATUS_REGISTER_1);
    uint8_t status = spi_xfer(m_spi, 0x00);
    cs_high();
    return status;
}

void W25Q128::writeEnable() {
    cs_low();
    spi_xfer(m_spi, W25Q_WRITE_ENABLE);
    cs_high();
}

void W25Q128::writeDisable() {
    cs_low();
    spi_xfer(m_spi, W25Q_WRITE_DISABLE);
    cs_high();
}

void W25Q128::waitWhileBusy() {
    // Poll WIP bit (bit0) of status register 1
    while (true) {
        uint8_t s = readStatus1();
        if ((s & 0x01) == 0) break;
        tight_loop_contents();
    }
}
