#ifdef CORE_CM7
#include <stm32h7xx.h>  // основной заголовочный файл CMSIS для МК серии H7
#elif defined(CORE_CM4)
#include <stm32f4xx.h>
#endif
#include <myled.h>

__attribute__((optimize("-O0")))  // директива отключает оптимизацию кода компилятором для этой функции
static void delay(int ms) {
    volatile int counter = SystemCoreClock / 1000 / 100 * ms;
    while (counter > 0)
        counter -= 1;
}

uint8_t button_is_pressed(void);

int main(void) {
    uint8_t button_was_pressed = 0;

    // Инициализация
    led_enable(led_all);  // Включить все три LED
    button_enable();      // Инициализировать кнопку (исправлено с button_init)

    led_t current_led = led_green;  // Начинаем с зеленого

    while (1) {
        // Проверяем нажатие кнопки
        if (button_is_pressed() && !button_was_pressed) {
            led_off(led_all);
            button_was_pressed = 1;
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
                     current_led = led_green;
                     break;
            }

            // Включаем выбранный светодиод

           
        } else if (!button_is_pressed()) {
            button_was_pressed = 0;
        }
        
        led_toggle(current_led);
        
        
        switch (current_led) {
        case led_green:
            delay(200);   
            break;
        case led_yellow:
            delay(500);   
            break;
        case led_red:
            delay(1000);  
        default:
            delay(500);
            break;
        }
    }
}
