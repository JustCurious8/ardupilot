#pragma once

#include <AP_HAL/AP_HAL.h>
#include <AP_Common/AP_Common.h>

// class to handle different operations with SPI flash device
class AP_spiFlash {
public:
	// Empty Constructor
    AP_spiFlash() { }

	// Needs to be call only one time to setup the spi device and get the corresponding SPI bus to use SPI hal APIs
	void wq25_init();

	// Used for device identification and accordingly setting total number of pages, blocks and sectors
	bool wq25_getSectorCount();
	
	// Setting Write Enable Latch (WEL) bit in the Status Register prior to every Page Program/Write and Chip Erase
	void WriteEnable_flash();
	
	// Method to read data on each page
	uint8_t* W25_Read_Data(uint32_t addr);
	
	// Method to write data on each page
	void W25_Write_Data(uint32_t addr, uint8_t* data, uint32_t sz);
	
	// Method to erase all the contents/blocks (all the bits at every address set to 1)
	void chip_erase();
	
	// Method to check for if SPI flash device is in idle state or busy state
    bool Busy();

protected:

    // number of bytes in a page
	uint32_t df_PageSize;    
    // number of pages in a (generally 64k) block
	uint16_t df_PagePerBlock;
	// number of pages in a (generally 4k) sector
	uint16_t df_PagePerSector;
	// number of pages on the chip
	uint32_t df_NumPages;

    static const uint16_t page_size_max = 256;
	uint8_t *buffer;


private:

	
	AP_HAL::OwnPtr<AP_HAL::SPIDevice> dev;

	// send a command with an address
    void send_command_addr(uint8_t cmd, uint32_t address);

	bool flash_died;
	
	// wait for busy flag to be cleared
    void              WaitReady();
    
    //Read the status register
    uint8_t           ReadStatusReg();
    void              Enter4ByteAddressMode(void);

	bool use_32bit_address;

};

