#include "stm32f301x8.h"

void init_usart2(void){
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Prizgi uro APB1
    USART2->BRR = 0xEA6; // Nastavi pravilno hitrost posiljanja
    USART2->CR1 |= USART_CR1_TE; // Omogoci oddajnik USART
    USART2->CR1 |= USART_CR1_UE; // Omogoci komunikacijo USART
}

void usart_send_byte(unsigned char byte){
    USART2->TDR = byte; // Zapisi bajt podatkov v register USART2_TDR
    while(!(USART2->ISR & USART_ISR_TC)); // Pocakaj dokler ni posiljanje koncano
}
