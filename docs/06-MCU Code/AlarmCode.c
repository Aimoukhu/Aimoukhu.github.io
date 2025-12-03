#include "mcc_generated_files/system/system.h"
#include <xc.h>


// SIMPLE SIREN USING DAC1

int8_t direction;
uint8_t dacValue=0;
int8_t stepSize=5 ;

void Siren_Update(void)
{
    // direction = 1 → HIGH PITCH
    // direction = 0 → LOW PITCH

    if (direction == 1)
    {
        dacValue = 100    ;    
        direction = 0;      
    }
    else
    {
        dacValue = 10;       
        direction = 1;      
    }

    DAC1_SetOutput(dacValue);
}


int main(void)
{
    SYSTEM_Initialize();

    DOOR_SetDigitalInput();     DOOR_SetDigitalMode();
    SWITCH_SetDigitalInput();   SWITCH_SetDigitalMode();
    LED_SetDigitalOutput();     LED_SetDigitalMode();
    MOTOR_SetDigitalOutput();   MOTOR_SetDigitalMode();
    SPEAKER_SetDigitalOutput(); 
    SPEAKER_SetDigitalMode();

    uint8_t override = 0;
    uint8_t lastSwitch = SWITCH_GetValue();

    uint8_t lastDoor = DOOR_GetValue();   // track door open or close

    while(1)
    {
        // SWITCH TOGGLE DETECTION

        uint8_t now = SWITCH_GetValue();

        if (now != lastSwitch)
        {
            __delay_ms(20); // debounce

            if (now == SWITCH_GetValue())
            {
                if (now == 1)
                {
                    override ^= 1;   // toggle state

                    LED_SetLow();
                    DAC1_SetOutput(0);
                }
            }
        }

        lastSwitch = now;

        // MOTOR OUTPUT BASED ON SWITCH STATE
    
        if (override == 1)
            MOTOR_SetHigh();     // override ON = MOTOR SIGNAL SENDS HIGH
        else
            MOTOR_SetLow();      // override OFF = MOTOR SIGNAL SENDS LOW


        // OVERRIDE ENABLED = EVERYTHING OFF
        if (override == 1)
        {
            LED_SetLow();
            DAC1_SetOutput(0);
            continue;
        }


        // DOOR CLOSED = EVERYTHING OFF

        uint8_t doorNow = DOOR_GetValue();

        if (doorNow == 0)
        {
            LED_SetLow();
            DAC1_SetOutput(0);
            lastDoor = 0;
            continue;
        }


        // DOOR JUST OPENED = SIREN START

        if (lastDoor == 0 && doorNow == 1)
        {
            DAC1_SetOutput(0); 
        }

        lastDoor = 1;


        // DOOR OPEN = LED ON + UPDATE DAC SIREN

        LED_SetHigh();

        // Update DAC waveform every loop
        Siren_Update();

        __delay_us(10  );   
    }
}
