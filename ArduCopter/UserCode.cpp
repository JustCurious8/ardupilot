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

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
  rc().get_pwm((uint8_t)6,usr_rc6);
  rc().get_pwm((uint8_t)7,usr_rc7);
  rc().get_pwm((uint8_t)8,usr_rc8);
  rc().get_pwm((uint8_t)9,usr_rc9);

 
  if(usr_rc7 >= 1850)
  {    
        gcs().send_text(MAV_SEVERITY_INFO, "CUSTOM KILLSWITCH Geofence Breached");
        // stop motors
        copter.motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::SHUT_DOWN);
        copter.motors->output();

        // disarm as well
        copter.arming.disarm(AP_Arming::Method::TERMINATION);
  }


  if(usr_rc8 >= 1850 and src1_compass==0)
  {    
    AP_Param::set_by_name("EK3_SRC1_YAW", (int8_t)1); 
    gcs().send_text(MAV_SEVERITY_INFO, "EK3_SRC1_YAW set to 1");
    src1_compass=1;
  }
  else if(usr_rc8 <= 1200 and src1_compass==1)
    src1_compass=0;

  if(usr_rc6 >= 1900 and src1_gsf==0)
  {
    AP_Param::set_by_name("EK3_SRC1_YAW", (int8_t)8); 
    gcs().send_text(MAV_SEVERITY_INFO, "EK3_SRC1_YAW set to 8");
    src1_gsf=1;
  }
  else if(usr_rc6 <= 1200 and src1_gsf==1)
    src1_gsf=0;

  if(usr_rc9 >= 1900 and src1_disable==0)
  {
    AP_Param::set_by_name("EK3_SRC1_YAW", (int8_t)0); 
    gcs().send_text(MAV_SEVERITY_INFO, "EK3_SRC1_YAW set to 0");
    src1_disable=1;
  }
  else if(usr_rc9 <= 1200 and src1_disable==1)
    src1_disable=0;

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
    // put your 1Hz code here
}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(uint8_t ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
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
