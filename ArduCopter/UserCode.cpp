#include "Copter.h"

Location target2(285449232,771886204,0,Location::AltFrame::ABOVE_HOME);

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
    // put your 10Hz code here
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

   int32_t bearing=0;
   bearing=gps.location().get_bearing_to(target2);
   //gcs().send_text(MAV_SEVERITY_INFO, "BEARING:%d, yaw:%f, DIFF:%d\n",bearing,degrees(ahrs.get_yaw()),out);
  
   int a=(int)bearing*0.01;
   int b=(int)degrees(ahrs.get_yaw()) - 45;
   int cal=0;
   cal=(abs(a-b)%360);
   int out=0;
   out=cal > 180 ? 360 - cal : cal;

   //gcs().send_text(MAV_SEVERITY_INFO, "DIFF:%d\n",out);
   //gcs().send_text(MAV_SEVERITY_INFO, "BEARING:%d, yaw:%f, DIFF:%d\n",bearing,degrees(ahrs.get_yaw()),out);
   //gcs().send_text(MAV_SEVERITY_INFO, "BEARING:%d, yaw:%d, DIFF:%d\n",a,b,out);

   if(out>30 and copter.control_mode == Mode::Number::GUIDED and stage==2)
   {
      flightmode->auto_yaw.set_fixed_yaw((bearing*0.01f) + 45,0.0f,0,0);
      gcs().send_text(MAV_SEVERITY_INFO, "BEARING:%d, yaw:%d, DIFF:%d\n",a,b,out);
   }

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
