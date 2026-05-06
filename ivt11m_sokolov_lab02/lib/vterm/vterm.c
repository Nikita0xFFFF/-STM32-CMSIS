#include "vterm.h"
#include "stm32f4xx.h"
#include <stdio.h>

#ifndef HSI_VALUE
#define HSI_VALUE 16000000U
#endif

// ЯВНО УКАЗЫВАЕМ ПРАВИЛЬНУЮ ЧАСТОТУ ДЛЯ STM32F401
// USART1 тактируется от APB2, который обычно 84 MHz

// Используйте SystemCoreClock
extern uint32_t SystemCoreClock;

#define BLOCK_ON(COND) do{}while(COND)

static int lib_init_flag = 0;

static void uart_init(int baudrate) {
    // Включаем тактирование
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    // Настройка PA9 (TX) как Alternate Function
    GPIOA->MODER &= ~GPIO_MODER_MODER9;
    GPIOA->MODER |= GPIO_MODER_MODER9_1;
    
    // Настройка PA10 (RX) как Alternate Function
    GPIOA->MODER &= ~GPIO_MODER_MODER10;
    GPIOA->MODER |= GPIO_MODER_MODER10_1;
    
    // Выбор AF7 для USART1 на PA9
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;
    GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL9_Pos);
    
    // Выбор AF7 для USART1 на PA10
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL10;
    GPIOA->AFR[1] |= (7 << GPIO_AFRH_AFSEL10_Pos);
    
    // Настройка скорости Medium для PA9
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0;
    
    // Настройка скорости Medium для PA10
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR10;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_0;
    
    // Pull-up на RX
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
    
    // Сброс USART1
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    
    // 8 бит данных, 1 стоп-бит, без четности
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR2 &= ~USART_CR2_STOP;
    
    // ПРАВИЛЬНЫЙ РАСЧЁТ BRR для 84 MHz
    // BRR = clock / baudrate
      // Используем реальную частоту
    uint32_t clock = SystemCoreClock;
    
    // Если SystemCoreClock не установлена, используем HSI
    if (clock == 0) clock = 16000000U;
    
    uint32_t usartdiv = clock / baudrate;
    USART1->BRR = ((usartdiv % 16) << 0) | ((usartdiv / 16) << 4);
    
    // Включаем USART
    USART1->CR1 |= USART_CR1_UE;
    
    // Небольшая задержка для стабилизации
    for(volatile int i = 0; i < 10000; i++);
    
    // ОТЛАДОЧНЫЙ ВЫВОД - должен работать!
    const char msg[] = "UART OK\r\n";
    for(int i = 0; i < sizeof(msg)-1; i++) {
        while(!(USART1->SR & USART_SR_TXE));
        USART1->DR = msg[i];
    }
}

static inline void uart_putch(char ch) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = ch;
}

static inline char uart_getch(void) {
    while(!(USART1->SR & USART_SR_RXNE));
    return USART1->DR & 0xFF;
}

void vterm_init(int baudrate) {
    if (lib_init_flag == 0){
        uart_init(baudrate);
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
        lib_init_flag = 1;
    }
}

unsigned char vterm_keypressed(void){
    if ((USART1->SR & USART_SR_RXNE) == 0)
        return 0;
    return USART1->DR & 0xFF;
}

void vterm_write(char *ptr, int len) {
    if (!lib_init_flag)
        return;
    for (int i = 0; i < len; i++)
        uart_putch(ptr[i]);
    while(!(USART1->SR & USART_SR_TC));
}

int vterm_gets(char *buf, int size, int echo) {
    if (!lib_init_flag || !buf || size == 0)
        return -1;
    
    int i = 0;
    while (i < size - 1) {
        char ch = uart_getch();
        if (echo)
            uart_putch(ch);
        
        if (ch == '\n' || ch == '\r') {
            if (echo && ch == '\r')
                uart_putch('\n');
            break;
        }
        buf[i++] = ch;
    }
    buf[i] = '\0';
    return i;
}

int _write(int file, char *ptr, int len) {
    if ((file == 1 || file == 2) && lib_init_flag) {
        vterm_write(ptr, len);
        return len;
    }
    return -1;
}

int _read(int file, char *ptr, int len) {
    if (lib_init_flag && len > 0 && file == 0) {
        int i = 0;
        while (i < len) {
            if (USART1->SR & USART_SR_RXNE) {
                ptr[i++] = USART1->DR & 0xFF;
            }
        }
        return i;
    }
    return -1;
}