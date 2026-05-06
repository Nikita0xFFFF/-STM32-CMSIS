#include "myled.h"

#define TOGGLE_BIT(REG, BIT) ((REG) ^= (1UL << (BIT)))

#ifdef CORE_CM7
#include <stm32h7xx.h>

#define LED_RCC_EN(port)                   \
    do {                                         \
        if (port == GPIOB)                       \
            RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN; \
        if (port == GPIOE)                       \
            RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN; \
    } while (0)
// PB0 и PB14 находятся на порту B, PE1 - на порту E
// Настройка пина в режим OUTPUT
#define LED_SET_OUTPUT(port, moder_mask, moder_out) MODIFY_REG((port)->MODER, (moder_mask), (moder_out))

#define LED_SET_ANALOG(port, moder_mask, moder_an) MODIFY_REG((port)->MODER, (moder_mask), (moder_an))

#define LED_ON(port, pin) SET_BIT((port)->BSRR, (1UL << (pin)))

#define LED_OFF(port, pin) SET_BIT((port)->BSRR, (1UL << ((pin) + 16)))

#define LED_TOGGLE(port, pin) TOGGLE_BIT((port)->ODR, (pin))

#define BUTTON_SET_INPUT(port, moder_mask, moder_in) \
    MODIFY_REG((port)->MODER, (moder_mask), (moder_in))

#define BUTTON_SET_ANALOG(port, moder_mask, moder_an) \
    MODIFY_REG((port)->MODER, (moder_mask), (moder_an))
// ЗЕЛЁНЫЙ LED
#define LED_GREEN_PORT GPIOB
#define LED_GREEN_PIN 0
#define LED_GREEN_MODER_MASK GPIO_MODER_MODE0_Msk
#define LED_GREEN_MODER_OUT GPIO_MODER_MODE0_0
#define LED_GREEN_MODER_AN (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1)

// КРАСНЫЙ LED
#define LED_RED_PORT GPIOB
#define LED_RED_PIN 14
#define LED_RED_MODER_MASK GPIO_MODER_MODE14_Msk
#define LED_RED_MODER_OUT GPIO_MODER_MODE14_0
#define LED_RED_MODER_AN (GPIO_MODER_MODE14_0 | GPIO_MODER_MODE14_1)

// ЖЁЛТЫЙ LED
#define LED_YELLOW_PORT GPIOE
#define LED_YELLOW_PIN 1
#define LED_YELLOW_MODER_MASK GPIO_MODER_MODE1_Msk
#define LED_YELLOW_MODER_OUT GPIO_MODER_MODE1_0
#define LED_YELLOW_MODER_AN (GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1)

#define BUTTON_RCC_EN RCC_AHB4ENR_GPIOCEN
#define BUTTON_PORT        GPIOC
#define BUTTON_PIN         13
#define BUTTON_MODER_MASK  GPIO_MODER_MODE13    
#define BUTTON_PUPDR_MASK  GPIO_PUPDR_PUPD13   
#define BUTTON_PUPDR_UP    GPIO_PUPDR_PUPD13_0  // Подтяжка вверх

#elif defined(CORE_CM4)
#include <stm32f4xx.h>

#define LED_COMMON_PORT GPIOC
#define LED_COMMON_PIN  13

#define LED_RCC_EN(port)    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN

// Макросы для настройки режима
#define LED_SET_OUTPUT(port, moder_mask, moder_out)  MODIFY_REG((port)->MODER, (moder_mask), (moder_out))

#define LED_SET_ANALOG(port, moder_mask, moder_an)  MODIFY_REG((port)->MODER, (moder_mask), (moder_an))

#define LED_ON(port, pin) SET_BIT((port)->BSRR, (1UL << (pin)))

#define LED_OFF(port, pin) SET_BIT((port)->BSRR, (1UL << ((pin) + 16)))

#define LED_TOGGLE(port, pin) TOGGLE_BIT((port)->ODR, (pin))




#define LED_GREEN_PORT  GPIOC  
#define LED_GREEN_PIN   LED_COMMON_PIN 
#define LED_GREEN_MODER_MASK  GPIO_MODER_MODE13_Msk
#define LED_GREEN_MODER_OUT   GPIO_MODER_MODE13_0
#define LED_GREEN_MODER_AN    (GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1)

#define LED_RED_PORT    GPIOC  
#define LED_RED_PIN     LED_COMMON_PIN 
#define LED_RED_MODER_MASK    GPIO_MODER_MODE13_Msk
#define LED_RED_MODER_OUT     GPIO_MODER_MODE13_0
#define LED_RED_MODER_AN      (GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1)

#define LED_YELLOW_PORT GPIOC  
#define LED_YELLOW_PIN  LED_COMMON_PIN 
#define LED_YELLOW_MODER_MASK GPIO_MODER_MODE13_Msk
#define LED_YELLOW_MODER_OUT  GPIO_MODER_MODE13_0
#define LED_YELLOW_MODER_AN   (GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1)


// #define LED_GPIO_PORT GPIOC
// #define LED_GPIO_PIN 13

// #define LED_MODER_ANALOG (GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1)
#define BUTTON_RCC_EN RCC_AHB1ENR_GPIOAEN
#define BUTTON_PORT GPIOA
#define BUTTON_PIN 0
#define BUTTON_MODER_MASK GPIO_MODER_MODE0_Msk
#define BUTTON_PUPDR_MASK GPIO_PUPDR_PUPD0_Msk
#define BUTTON_PUPDR_UP GPIO_PUPDR_PUPD0_0

#else
#error "Define CORE_CM4 or CORE_CM7 in build_flags"
#endif

void led_enable(led_t led) {
    if (led & led_green) {
        LED_RCC_EN(LED_GREEN_PORT);
        LED_SET_OUTPUT(LED_GREEN_PORT, LED_GREEN_MODER_MASK, LED_GREEN_MODER_OUT);
    }
    if (led & led_red) {
        LED_RCC_EN(LED_RED_PORT);
        LED_SET_OUTPUT(LED_RED_PORT, LED_RED_MODER_MASK, LED_RED_MODER_OUT);
    }
    if (led & led_yellow) {
        LED_RCC_EN(LED_YELLOW_PORT);
        LED_SET_OUTPUT(LED_YELLOW_PORT, LED_YELLOW_MODER_MASK, LED_YELLOW_MODER_OUT);
    }
}

void led_on(led_t led) {
    if (led & led_green) {
        LED_ON(LED_GREEN_PORT, LED_GREEN_PIN);
    }
    if (led & led_red) {
        LED_ON(LED_RED_PORT, LED_RED_PIN);
    }
    if (led & led_yellow) {
        LED_ON(LED_YELLOW_PORT, LED_YELLOW_PIN);
    }
}

void led_off(led_t led) {
    if (led & led_green) {
        LED_OFF(LED_GREEN_PORT, LED_GREEN_PIN);
    }
    if (led & led_red) {
        LED_OFF(LED_RED_PORT, LED_RED_PIN);
    }
    if (led & led_yellow) {
        LED_OFF(LED_YELLOW_PORT, LED_YELLOW_PIN);
    }
}

void led_toggle(led_t led) {
    if (led & led_green) {
        LED_TOGGLE(LED_GREEN_PORT, LED_GREEN_PIN);
    }
    if (led & led_red) {
        LED_TOGGLE(LED_RED_PORT, LED_RED_PIN);
    }
    if (led & led_yellow) {
        LED_TOGGLE(LED_YELLOW_PORT, LED_YELLOW_PIN);
    }
}

void led_disable(led_t led) {
    if (led & led_green) {
        LED_SET_ANALOG(LED_GREEN_PORT, LED_GREEN_MODER_MASK, LED_GREEN_MODER_AN);
    }
    if (led & led_red) {
        LED_SET_ANALOG(LED_RED_PORT, LED_RED_MODER_MASK, LED_RED_MODER_AN);
    }
    if (led & led_yellow) {
        LED_SET_ANALOG(LED_YELLOW_PORT, LED_YELLOW_MODER_MASK, LED_YELLOW_MODER_AN);
    }
}

void button_enable() {
    RCC->AHB1ENR |=  BUTTON_RCC_EN;    // тактирование GPIOA
    BUTTON_PORT->MODER &= ~BUTTON_MODER_MASK;  // режим input для PA0
    BUTTON_PORT->PUPDR &= ~BUTTON_PUPDR_MASK;  //  // Очистка подтяжки
    BUTTON_PORT->PUPDR |= BUTTON_PUPDR_UP;     //
}

uint8_t button_is_pressed(void) {
    return (~(BUTTON_PORT->IDR) & GPIO_IDR_ID0);  //  (Нажата(0)) и 1 = 0
}
//========= led =========
// void myled_enable() {
//     RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;       // тактирование GPIOX
//     LED_GPIO_PORT->MODER &= ~LED_MODER_MASK;   // очистка битов для PX
//     LED_GPIO_PORT->MODER |= LED_MODER_OUTPUT;  // режим output для PX
// }

// void myled_toggle() {
//     TOGGLE_BIT(LED_GPIO_PORT->ODR, LED_GPIO_PIN);  // переключение PX
// }

// void myled_disable() {
//     LED_GPIO_PORT->MODER &= ~LED_MODER_MASK;   // очистка битов для PX
//     LED_GPIO_PORT->MODER |= LED_MODER_ANALOG;  // analog mode
// }
