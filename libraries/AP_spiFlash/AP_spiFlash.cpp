#include <AP_HAL/AP_HAL.h>
#include "AP_spiFlash.h"


#define JEDEC_WRITE_ENABLE           0x06
#define JEDEC_WRITE_DISABLE          0x04
#define JEDEC_READ_STATUS            0x05
#define JEDEC_WRITE_STATUS           0x01
#define JEDEC_READ_DATA              0x03
#define JEDEC_FAST_READ              0x0b
#define JEDEC_DEVICE_ID              0x9F
#define JEDEC_PAGE_WRITE             0x02

#define JEDEC_BULK_ERASE             0xC7
#define JEDEC_SECTOR4_ERASE          0x20 // 4k erase
#define JEDEC_BLOCK32_ERASE          0x52 // 32K erase
#define JEDEC_BLOCK64_ERASE          0xD8 // 64K erase

#define JEDEC_STATUS_BUSY            0x01
#define JEDEC_STATUS_WRITEPROTECT    0x02
#define JEDEC_STATUS_BP0             0x04
#define JEDEC_STATUS_BP1             0x08
#define JEDEC_STATUS_BP2             0x10
#define JEDEC_STATUS_TP              0x20
#define JEDEC_STATUS_SEC             0x40
#define JEDEC_STATUS_SRP0            0x80

/*
  flash device IDs taken from betaflight flash_m25p16.c

  Format is manufacturer, memory type, then capacity
*/
#define JEDEC_ID_MACRONIX_MX25L3206E   0xC22016
#define JEDEC_ID_MACRONIX_MX25L6406E   0xC22017
#define JEDEC_ID_MACRONIX_MX25L25635E  0xC22019
#define JEDEC_ID_MICRON_M25P16         0x202015
#define JEDEC_ID_MICRON_N25Q064        0x20BA17
#define JEDEC_ID_MICRON_N25Q128        0x20ba18
#define JEDEC_ID_WINBOND_W25Q16        0xEF4015
#define JEDEC_ID_WINBOND_W25Q32        0xEF4016
#define JEDEC_ID_WINBOND_W25Q64        0xEF4017
#define JEDEC_ID_WINBOND_W25Q128       0xEF4018
#define JEDEC_ID_WINBOND_W25Q256       0xEF4019
#define JEDEC_ID_CYPRESS_S25FL128L     0x016018
#define reset1 0x66
#define reset2 0x99


#define block0_addr 0x000000
#define block1_addr 0x020000
#define block2_addr 0x030000
#define block3_addr 0x040000
#define block4_addr 0x050000
#define block5_addr 0x060000
#define block6_addr 0x070000
#define block7_addr 0x080000
#define block8_addr 0x090000
#define block9_addr 0x0A0000
#define block10_addr 0x0B0000

// to get platform specific HAL to perform peripheral specific operations
extern const AP_HAL::HAL &hal;

// Setting Write Enable Latch (WEL) bit in the Status Register
void AP_spiFlash::WriteEnable_flash(void)
{
  WaitReady();
  uint8_t b = JEDEC_WRITE_ENABLE;
  WITH_SEMAPHORE(dev->get_semaphore());
  dev->transfer(&b, 1, nullptr, 0);

}


uint8_t* AP_spiFlash::W25_Read_Data(uint32_t pageNum)
{
  memset(buffer, 0, 256);

  
  if(pageNum == 0 || pageNum > df_NumPages+1) {
        hal.console->printf("Invalid page read %lu\n", pageNum);
        return buffer;
  }

  uint32_t PageAdr = (pageNum-1) * df_PageSize;

  WaitReady();

  WITH_SEMAPHORE(dev->get_semaphore());
  dev->set_chip_select(true);
  send_command_addr(JEDEC_READ_DATA, PageAdr);
  dev->transfer(nullptr, 0, buffer, 256);
  dev->set_chip_select(false);

  if(buffer[0]==0xFF)
     memset(buffer, 0, 256);
     
  return buffer;

}


void AP_spiFlash::W25_Write_Data(uint32_t pageNum, uint8_t* data, uint32_t sz)
{

    if (pageNum == 0 || pageNum > df_NumPages+1) {
        hal.console->printf("Invalid page write %lu\n", pageNum);
        return;
    }

  if(data==NULL)
    return;

  uint32_t PageAdr = (pageNum-1) * df_PageSize;

  memset(buffer, 0, sz);
  memcpy(buffer,data,sz);

  WriteEnable_flash();

  WITH_SEMAPHORE(dev->get_semaphore());
  dev->set_chip_select(true);
  send_command_addr(JEDEC_PAGE_WRITE, PageAdr);
  dev->transfer(buffer, sz, nullptr, 0);
  dev->set_chip_select(false);
}


void AP_spiFlash::chip_erase(void)
{
    WriteEnable_flash();
    
    WITH_SEMAPHORE(dev->get_semaphore());
    uint8_t cmd = JEDEC_BULK_ERASE;
    dev->transfer(&cmd, 1, nullptr, 0);
}

void AP_spiFlash::wq25_init(void)
{
	    dev = hal.spi->get_device("spiflash");
	    if (!dev) {
	        AP_HAL::panic("PANIC: AP_spiflash SPIDeviceDriver not found");
	        return;
	    }
        
        buffer = (uint8_t *)hal.util->malloc_type(page_size_max, AP_HAL::Util::MEM_DMA_SAFE);
        if (buffer == nullptr) {
            AP_HAL::panic("Out of DMA memory for logging");
        }

	    if (!wq25_getSectorCount()) {
	        flash_died = true;
	        return;
	    }

            if (use_32bit_address) {
                Enter4ByteAddressMode();
            }

	    flash_died = false; 
	    
}

bool AP_spiFlash::wq25_getSectorCount(void)
{
        WaitReady();
	    WITH_SEMAPHORE(dev->get_semaphore());

        uint8_t cmd = JEDEC_DEVICE_ID;
	    dev->transfer(&cmd, 1, buffer, 4);

	    uint32_t id = buffer[0] << 16 | buffer[1] << 8 | buffer[2];

	    uint32_t blocks = 0;

	    switch (id) {
	    case JEDEC_ID_WINBOND_W25Q16:
	    case JEDEC_ID_MICRON_M25P16:
	        blocks = 32;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        use_32bit_address = false;
	        break;
	    case JEDEC_ID_WINBOND_W25Q32:
	    case JEDEC_ID_MACRONIX_MX25L3206E:
	        blocks = 64;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        use_32bit_address = false;
	        break;
	    case JEDEC_ID_MICRON_N25Q064:
	    case JEDEC_ID_WINBOND_W25Q64:
	    case JEDEC_ID_MACRONIX_MX25L6406E:
	        blocks = 128;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        use_32bit_address = false;
	        break;
	    case JEDEC_ID_MICRON_N25Q128:
	    case JEDEC_ID_WINBOND_W25Q128:
	    case JEDEC_ID_CYPRESS_S25FL128L:
	        blocks = 256;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        use_32bit_address = false;
	        break;
	    case JEDEC_ID_WINBOND_W25Q256:
	    case JEDEC_ID_MACRONIX_MX25L25635E:
	        blocks = 512;
	        df_PagePerBlock = 256;
	        df_PagePerSector = 16;
	        use_32bit_address = true;
	        break;
	    default:
	        hal.scheduler->delay(2000);
	        hal.console->printf("Unknown SPI Flash 0x%08lx\n", id);
	        return false;
	    }

	    df_PageSize = 256;
	    df_NumPages = blocks * df_PagePerBlock;

	    hal.scheduler->delay(2000);
	    hal.console->printf("SPI Flash 0x%08lx found pages=%lu erase=%luk\n",id, df_NumPages, (df_PagePerBlock * (uint32_t)df_PageSize)/1024);
	    return true;
}


/*
  wait for busy flag to be cleared
 */
void AP_spiFlash::WaitReady()
{
    if (flash_died) {
        return;
    }

    uint32_t t = AP_HAL::millis();
    while (Busy()) {
        hal.scheduler->delay_microseconds(100); 
        if (AP_HAL::millis() - t > 5000) {
            hal.console->printf("SPIFlash: flash_died\n");
            flash_died = true;
            break;
        }
    }
}

bool AP_spiFlash::Busy()
{
    return (ReadStatusReg() & (JEDEC_STATUS_BUSY | JEDEC_STATUS_SRP0)) != 0;
}


// Read the status register
uint8_t AP_spiFlash::ReadStatusReg()
{
    WITH_SEMAPHORE(dev->get_semaphore());
    uint8_t cmd = JEDEC_READ_STATUS;
    uint8_t status;
    dev->transfer(&cmd, 1, &status, 1);
    return status;
}

void AP_spiFlash::Enter4ByteAddressMode(void) 
{
    WITH_SEMAPHORE(dev->get_semaphore());

    const uint8_t cmd = 0xB7;
    dev->transfer(&cmd, 1, nullptr, 0);
}

/*
  send a command with an address
*/
void AP_spiFlash::send_command_addr(uint8_t command, uint32_t PageAdr)
{
    uint8_t cmd[5];
    cmd[0] = command;
    if (use_32bit_address) {
        cmd[1] = (PageAdr >> 24) & 0xff;
        cmd[2] = (PageAdr >> 16) & 0xff;
        cmd[3] = (PageAdr >>  8) & 0xff;
        cmd[4] = (PageAdr >>  0) & 0xff;
    } else {
        cmd[1] = (PageAdr >> 16) & 0xff;
        cmd[2] = (PageAdr >>  8) & 0xff;
        cmd[3] = (PageAdr >>  0) & 0xff;
    }

    dev->transfer(cmd, use_32bit_address?5:4, nullptr, 0);
}


