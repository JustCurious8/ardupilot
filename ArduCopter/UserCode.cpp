#include "Copter.h"

uint8_t check_rc=0;
uint8_t check_rc2=0;
uint16_t usr_roll={};
uint16_t usr_pitch={};


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
    if (hal.util->get_soft_armed()) {
        return;
    }
    
    static uint32_t accel_switch_gesture_begin = 0;
    static bool accel_switch_check = false;
    uint32_t tnow = millis();
    
    if(usr_roll > 1850 and usr_pitch > 1850 and !motors->armed())
    {
        if (tnow-accel_switch_gesture_begin > 1000*2 and !accel_switch_check)
        {   
            AP::ins().switch_accel_cal();
            accel_switch_check = true;                                                        
        }
    }
    
    else 
    {  accel_switch_gesture_begin = tnow;
       accel_switch_check = false;
    }
    
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
    rc().get_pwm((uint8_t)1,usr_roll);
    rc().get_pwm((uint8_t)2,usr_pitch);

    //gcs().send_text(MAV_SEVERITY_INFO, "Roll:%u,Pitch:%u\n",usr_roll,usr_pitch);

    if (hal.util->get_soft_armed()) {
        return;
    }
    
    static uint32_t inflight_compass_cal_begin = 0;
    static bool inflight_compass_cal_check = false;

    bool compass_gesture_detected = inflight_compass_cal_begin != 0 && !motors->armed() && usr_roll > 1850 && usr_pitch < 1150;
    uint32_t tnow = millis();

    if (!compass_gesture_detected)
    {   inflight_compass_cal_begin = tnow;
        //inflight_compass_cal_check = false;        
                                                   }
    else if (tnow-inflight_compass_cal_begin > 1000*4 and !inflight_compass_cal_check)
    {   gcs().send_text(MAV_SEVERITY_INFO, "Initiated inflight cal\n");
        compass.set_learn_type(Compass::LEARN_INFLIGHT, false);   
        inflight_compass_cal_check = true; 
        notify.set_led_override((float)1);
        AP_Notify::handle_rgb((uint8_t)0, (uint8_t)0, (uint8_t)255);
                                                   }
   
    if(inflight_compass_cal_check and !compass_gesture_detected)
    {   if(notify.get_led_override())
           notify.set_led_override((float)0);
           
        inflight_compass_cal_check = false;
    }

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
