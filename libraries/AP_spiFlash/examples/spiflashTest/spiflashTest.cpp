#include <AP_HAL/AP_HAL.h>
#include <AP_spiFlash/AP_spiFlash.h>
#include <AP_Common/AP_Common.h>
#include <stdio.h>

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

void setup();
void loop();


char* tran_buff =(char*)"8888/285447700/771890300/01000/000/000/222\n";
AP_spiFlash wq25;
char* rx_buf;
bool counter=0;

void setup()
{
    hal.scheduler->delay(5000);
    hal.console->printf("initialising_flash_device\n");
    wq25.wq25_init();

	hal.scheduler->delay(5000);
	hal.console->printf("erasing_flash\n");
	wq25.chip_erase();
	hal.scheduler->delay(3000);

}


void loop()
{
    if(wq25.Busy())
    {
    	hal.console->printf("flash_busy\n");
    	hal.scheduler->delay(3000);
    }
    
    else
    {
    	if(counter==0)
    	{
    		wq25.W25_Write_Data(1,(uint8_t*)tran_buff,strlen(tran_buff));
    		hal.console->printf("WRITING DATA: %s",tran_buff);
    		hal.scheduler->delay(5000);
    		counter=1;
    	}

		rx_buf=(char*)wq25.W25_Read_Data(1);
		hal.console->printf("READING DATA: %s",rx_buf);
	    hal.scheduler->delay(2000);
	    
    }

}

AP_HAL_MAIN();
