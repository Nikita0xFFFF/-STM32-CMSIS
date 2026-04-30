#include "myled.h"

#ifdef CORE_CM7
    #include <stm32h7xx.h>
    
    void myled_enable() {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
        GPIOE->MODER &= ~GPIO_MODER_MODE1_Msk;
        GPIOE->MODER |= GPIO_MODER_MODE1_0;
    }
    
    void myled_toggle() {
        GPIOE->ODR ^= (1 << 1);  // PE1
    }
    
    void myled_disable() {
        GPIOE->MODER &= ~GPIO_MODER_MODE1_Msk;
        GPIOE->MODER |= GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1;
    }

#elif defined(CORE_CM4)
    #include <stm32f4xx.h>
    
    void myled_enable() {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;      // тактирование GPIOC
        GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;   // очистка битов для PC13
        GPIOC->MODER |= GPIO_MODER_MODE13_0;      // режим output для PC13
    }
    
    void myled_toggle() {
        GPIOC->ODR ^= (1 << 13);                  // переключение PC13
    }
    
    void myled_disable() {
        GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;   // очистка битов для PC13
        GPIOC->MODER |= GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1;  // analog mode
    }

#else
    #error "Define CORE_CM4 or CORE_CM7 in build_flags"
#endif