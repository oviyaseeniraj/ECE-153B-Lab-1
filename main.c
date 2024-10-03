/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 1
 */
#include "stm32l476xx.h"

void Init(){
    // Enable HSI
    RCC->CR |= ((uint32_t)RCC_CR_HSION);
    while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );

    // Select HSI as system clock source
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );

    // Enable GPIO Clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable Clock for GPIO Port A (LED)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // Enable Clock for GPIO Port C (Button)

    // Initialize Green LED
    GPIOA->MODER &= ~(GPIO_MODER_MODE5); // Clear MODER5[1:0] for PA5
    GPIOA->MODER |= (GPIO_MODER_MODE5_0);  // Set PA5 as General purpose output mode
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5);      // Set PA5 to push-pull
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5); // No pull-up, no pull-down

    // Initialize User Button
    GPIOC->MODER &= ~(GPIO_MODER_MODE13); // Clear MODER13[1:0] for PC13 to set as input
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13); // No pull-up, no pull-down for PC13
}

int main(void){
    // Initialization
    Init();

    // Polling to Check for User Button Presses
    uint32_t ledState = 0; // LED state tracking variable
    uint32_t buttonState = 0; // Button state tracking variable

    while(1){
        // Check if button state is stable (simple debouncing)
        if(((GPIOC->IDR & (1UL << 13)) == 0) && (buttonState == 0))
				{
            buttonState = 1; // Update the button state to pressed

            // Toggle LED
            if(ledState == 0)
						{
                GPIOA->BSRR = (1UL << 5); // Turn on the LED, 5 b/c pin 5
                ledState = 1; // Update the LED state
            }
						else
							{
                GPIOA->BSRR = (1UL << (5 + 16)); // Turn off the LED
                ledState = 0; // Update the LED state
            }

            // Simple software debouncing delay
            for(uint32_t i = 0; i < 100000; i++){} // Adjust this value based on your system's clock speed
        }
				else if((GPIOC->IDR & (1UL << 13)) != 0)
				{
            buttonState = 0; // Button released, reset the button state
        }
    }
    return 0;
}
