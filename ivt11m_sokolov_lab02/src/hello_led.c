#ifdef CORE_CM7
#include <stm32h7xx.h>  // основной заголовочный файл CMSIS для МК серии H7
#elif defined(CORE_CM4)
#include <stm32f4xx.h>
#endif
#include <myled.h>
#include <stdio.h>
#include <vterm.h>

__attribute__((optimize("-O0")))  // директива отключает оптимизацию кода компилятором для этой функции
static void delay(int ms) {
    volatile int counter = SystemCoreClock / 1000 / 6 * ms;
    while (counter > 0)
        counter -= 1;
}

uint8_t button_is_pressed(void);

#include <stdio.h>
#include "stm32f4xx.h"


int main(void) {
    uint8_t button_was_pressed = 0;

    // Инициализация
    led_enable(led_all);  // Включить все три LED
    button_enable();      // Инициализировать кнопку (исправлено с button_init)
    myled_enable();
    myled_on();
    led_t current_led = led_green;  // Начинаем с зеленого
    vterm_init(115200);
    static int counter = 0;  // Инкремент после инициализации
    while (1) {
        
        led_off(led_all);
        printf("\r%s %d %s", "Количество переходов", counter++, "раз(а)\n");
        // Переключаемся на следующий светодиод
        switch (current_led) {
        case led_green:
            current_led = led_yellow;
            break;
        case led_yellow:
            current_led = led_red;
            break;
        case led_red:
            current_led = led_green;
            break;
        default:
            current_led = led_all;
            break;
        }

        // Включаем выбранный светодиод

        led_on(current_led);  
       
        delay(1000);
    }
}
