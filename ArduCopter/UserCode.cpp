#include "Copter.h"

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif


uint8_t check_rc=0;
uint8_t check_rc2=0;
uint16_t usr_rc7={};
uint16_t usr_rc8={};

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
			rc().get_pwm((uint8_t)7,usr_rc7);
			rc().get_pwm((uint8_t)8,usr_rc8);

   if(usr_rc7 < 1500)
      check_rc=0;

    // put your 1Hz code here
   if(usr_rc7 > 1500 and check_rc==0)
   {  
				  gcs().send_text(MAV_SEVERITY_INFO, "Initiated GYRO CAL\n");
				  ins.init_gyro();
				  if (!ins.gyro_calibrated_ok_all()) {
				      gcs().send_text(MAV_SEVERITY_INFO, "FALSE\n");
				  }
				  ahrs.reset_gyro_drift();
				  gcs().send_text(MAV_SEVERITY_INFO, "TRUE\n");

      ins.acal_init();
      ins.start_accel_cal();
      gcs().send_text(MAV_SEVERITY_INFO,"start_accel_Cal\n");
      check_rc=1;  
   }

   if(usr_rc8 < 1500)
      check_rc2=0;

    // put your 1Hz code here
   if(usr_rc8 > 1500 and check_rc2==0)
   {  
      ins.switch_accel_cal();
      //gcs().send_text(MAV_SEVERITY_INFO,"start_accel_Cal\n");
      check_rc2=1; }

}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{

}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(uint8_t ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
										  if (ch_flag == 2)
										  copter.user_compassmot(); 
}

void Copter::userhook_auxSwitch2(uint8_t ch_flag)
{
    // put your aux switch #2 handler here (CHx_OPT = 48)
}

void Copter::userhook_auxSwitch3(uint8_t ch_flag)
{
    // put your aux switch #3 handler here (CHx_OPT = 49)
}
#endif
