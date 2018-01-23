/*
 * Author: Texas Instruments
 * Submitted by: Abhishek Pal, 173074015, EE Department, IIT Bombay

 * Description: This code is for Lab 3: Key Debounce with Timer & Interrupt
 * Filename: lab-3.c
 * Functions: setup(), config(), detectSw1Press(), main()
 * Global Variables: sw2count
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

#define IDLE    0x00
#define PRESS   0x01
#define RELEASE 0x02

unsigned char sw1_state, sw2_state;
uint8_t ui8LED = 2;
uint8_t sw2count = 0;

/*
 * Function Name: setup()
 * Input: none
 * Output: none
 * Description: Set crystal frequency,enable GPIO Peripherals and unlock Port F pin 0 (PF0)
 * Example Call: setup();
 */
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //unlock PF0 based on requirement

}

/*

 * Function Name: led_pin_config() & sw_pin_config()
 * Input: none
 * Output: none
 * Description: Set Port F Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.
			   Set Port F Pin 0 and 4 as input, enable pull up on both these pins.

 * Example Call: pin_config();
 */

void led_pin_config(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

}

/*
void sw_pin_config(void)
{
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01 ;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0 ;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
 */

unsigned char detectSw1Press()
{
    uint8_t sw1read  = 1;
    uint8_t gpiodata = 0;
    unsigned char flag = 0x00;

    gpiodata = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4); // |GPIO_PIN_0);
    sw1read = (gpiodata & 0x10) >> 4;

    switch(sw1_state)
    {
    case IDLE:
        if(sw1read == 0) //check if pin is low, meaning switch is on
        {
            sw1_state = PRESS;
        }
        else
        {
            sw1_state = IDLE;
        }
        break;

    case PRESS:
        if(sw1read == 0) //check if pin is low, meaning switch is on
        {
            sw1_state = RELEASE;
            flag = 0x01;
        }
        else
        {
            sw1_state = IDLE;
            flag = 0;
        }
        break;

    case RELEASE:
        if(sw1read == 0) //check if pin is low, meaning switch is on
        {
            sw1_state = RELEASE;
        }
        else
        {
            sw1_state = IDLE;
        }
        break;

    default:
        sw1_state = IDLE;
        break;
    }

    return flag;
}

///////////////////////////////////////////////

unsigned char detectSw2Press()
{
    uint8_t sw2read  = 1;
    uint8_t gpiodata = 0;
    unsigned char flag = 0x00;

    gpiodata = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0); // |GPIO_PIN_0);
    sw2read = (gpiodata & 0x01);

    switch(sw2_state)
    {
    case IDLE:
        if(sw2read == 0) //check if pin is low, meaning switch is on
        {
            sw2_state = PRESS;
        }
        else
        {
            sw2_state = IDLE;
        }
        break;

    case PRESS:
        if(sw2read == 0) //check if pin is low, meaning switch is on
        {
            sw2_state = RELEASE;
            flag = 0x01;
        }
        else
        {
            sw2_state = IDLE;
            flag = 0;
        }
        break;

    case RELEASE:
        if(sw2read == 0) //check if pin is low, meaning switch is on
        {
            sw2_state = RELEASE;
        }
        else
        {
            sw2_state = IDLE;
        }
        break;

    default:
        sw2_state = IDLE;
        break;
    }

    return flag;
}

///////////////////////////////////////////////
// Timer Interrupt Handler
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if(detectSw2Press())
    {
        sw2count++;
    }

    if(detectSw1Press())
    {
        if (ui8LED == 8)
        {
            ui8LED = 2;
        }
        else
        {
            ui8LED = ui8LED*2;
        }
        //        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
    }

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

}


/******** Main *******/

int main(void)
{
    uint32_t ui32Period;
    //    sw1_state = IDLE;
    //    sw2_state = IDLE;

    //Clock Setup
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //GPIO Config
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    //GPIO SW Inputs: unlock PIN_0
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01 ;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0 ;
    //GPIO SW Inputs:
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //Timer Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    //Calculate delay
    ui32Period = (SysCtlClockGet() / 100) / 2; // 50Hz for 20ms
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

    //Interrupt Enable
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    //Timer Enable
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Normal Operation
    while(1)
    {
    }
}
