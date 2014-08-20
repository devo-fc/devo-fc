/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <AP_Notify.h>

extern const AP_HAL::HAL& hal;
extern uint8_t num_sat; 
extern uint8_t init_on;
void AP_BoardLED::init(void)
{
    // setup the main LEDs as outputs
    hal.gpio->pinMode(HAL_GPIO_A_LED_PIN, GPIO_OUTPUT);
    hal.gpio->pinMode(HAL_GPIO_B_LED_PIN, GPIO_OUTPUT);
    hal.gpio->pinMode(HAL_GPIO_C_LED_PIN, GPIO_OUTPUT);

    // turn all lights off
    hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_OFF);
    hal.gpio->write(HAL_GPIO_B_LED_PIN, HAL_GPIO_LED_OFF);
    hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);
}

/*
  main update function called at 50Hz
 */
 uint8_t init_cnt,init_turn,blcnt,Gps_cnt;
extern uint8_t batt_low,arm_open;
void AP_BoardLED::update(void)
{
    _counter++;
	
    // we never want to update LEDs at a higher than 16Hz rate
    if (_counter % 3 != 0) {
        return;
    }

    // counter2 used to drop frequency down to 16hz
    uint8_t counter2 = _counter / 3;

    // initialising
    if (AP_Notify::flags.initialising) {
        // blink LEDs A and C at 8Hz (full cycle) during initialisation
        if (init_cnt++>1) {
			init_cnt=0;
			if(init_turn)
            hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_ON);
			else 
            hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_OFF);
			init_turn=!init_turn;
        //    hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);
        } 
        return;
	}

    // save trim and ESC calibrations
    if (AP_Notify::flags.save_trim || AP_Notify::flags.esc_calibration) {
        static uint8_t save_trim_counter = 0;
        if ((counter2 & 0x2) == 0) {
            save_trim_counter++;
        }
        switch(save_trim_counter) {
            case 0:
                hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);
                hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_ON);
                break;

            case 1:
                hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_OFF);
                hal.gpio->write(HAL_GPIO_B_LED_PIN, HAL_GPIO_LED_ON);
                break;

            case 2:
                hal.gpio->write(HAL_GPIO_B_LED_PIN, HAL_GPIO_LED_OFF);
                hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_ON);
                break;

            default:
                save_trim_counter = -1;
        }
        return;
    }

    // arming light
    static uint8_t arm_counter = 0;

	if(!init_on)
		{
		
			if(init_turn)
            hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_ON);
			else 
            hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_OFF);
			init_turn=!init_turn;

		}
	else 

	if(batt_low)
		{
		if (init_cnt++>10) {
			init_cnt=0;
			if(init_turn)
            hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_ON);
			else 
            hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_OFF);
			init_turn=!init_turn;
        	}
		}
	else
	if (AP_Notify::flags.armed) {
        // red led solid
        hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_ON);
    }else
        {
    if (arm_open)
    	{PORTA^=0x20;}
	else 
        hal.gpio->write(HAL_GPIO_A_LED_PIN, HAL_GPIO_LED_OFF);
    	}

    // gps light
    switch (AP_Notify::flags.gps_status) {
   
  //  switch (3) {
        case 0:
            // no GPS attached
            hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);
            break;

        case 1:
            // GPS attached but no lock, blink at 4Hz
            hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);

            break;

        case 2:
            hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);

            break;

        case 3:
            // solid blue on gps lock
            if(Gps_cnt++>1)
            	{Gps_cnt=0;
            	if(num_sat>=6)
            		{
					if(++blcnt > 2*num_sat) blcnt = 0;
        			if(blcnt >= 12 && ((blcnt%2) == 0)) 		
						{hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_ON);} 
					else {hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);}
            		}
				else hal.gpio->write(HAL_GPIO_C_LED_PIN, HAL_GPIO_LED_OFF);
            	}
            break;        
    }
}

