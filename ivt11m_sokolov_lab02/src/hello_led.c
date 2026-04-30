#ifdef CORE_CM7
   #include <stm32h7xx.h> // основной заголовочный файл CMSIS для МК серии H7
#elif defined(CORE_CM4)
    #include <stm32f4xx.h>
#endif
   #include <myled.h>

__attribute__((optimize("-O0"))) //директива отключает оптимизацию кода компилятором для этой функции
static void delay(int ms) {
    volatile int counter = SystemCoreClock / 1000 / 100* ms ;
   while (counter > 0) counter -= 1;
}
 int main() {
    myled_enable(); // инициализация выввода светодиода
    while(1){
    myled_toggle(); // переключение «вкл <-> откл»
    delay(500); // пауза между переключениями
 };
} 