#include "stm32f301x8.h"

void init_clock(void){
    RCC->CR |= RCC_CR_HSION; // HSI internal clocks turned on
    while(!(RCC->CR & RCC_CR_HSIRDY)); // Wait for HSI osc. to be ready

    RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN); // port A and port B clock enable

    RCC->CR |= (RCC_CR_HSEBYP | RCC_CR_CSSON); // Onemogoci varnostne mehanizme in omogoci obhod hitre zunanje ure
    RCC->CR |= RCC_CR_HSEON; // Omogoci hitro zunanjo uro
    while(!(RCC->CR & RCC_CR_HSERDY)); // Pocakaj, da je hitra zunanja ura pripravljena
    RCC->CFGR |= RCC_CFGR_SW_HSE; // Nastavi hitro zunanjo uro za uro sistema
}

void init_gpio(void){
    GPIOA->MODER |= (GPIO_MODER_MODER5_0); // Nastavi nozico PA4 na vecnamenski nacin
    GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER0_1); // Nastavi nozico PA1 na vecnamenski nacin
    GPIOA->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER4_1);  // Nastavi nozico PA4 na analogen nacin
    GPIOA->MODER |= GPIO_MODER_MODER2_1;  // Nastavi nozico PA4 na alternativni nacin
    GPIOA->AFR[0] |= 0x700; // Nastavi nozico PA4 na alternativni nacin 7
    GPIOA->OSPEEDR = 0xFFFFFFFF; // Nastavi hitrost vseh nozic na najhitrejso
}

void init_dac(void){
    RCC->APB1ENR |= RCC_APB1ENR_DAC1EN; // Omogoci uro za D-A pretvornik
    DAC->CR |= DAC_CR_EN1; // Prizgi D-A pretvornika
}

void init_adc(void){
    RCC->AHBENR |= RCC_AHBENR_ADC1EN; // Omogoci uro za A-D pretvornik
    ADC1->CR &= ~ADC_CR_ADVREGEN_1; // Spremeni bite A-D pretvornika napetostnega regulatorja iz ugasnjenega (10) v vmesno stanje (00)
    ADC1->CR |= ADC_CR_ADVREGEN_0; // Spremeni bite A-D pretvornika napetostnega regulatorja v prizgano stanje (01)
     for(int k = 0; k<100000; k++){ // Pocakaj, da je napetostni regulator pripravljen za delovanje
            __NOP();
        }
    ADC1_COMMON->CCR |= ADC_CCR_CKMODE_0; // Nastavi vrednost delilnika uporabljene ure

    ADC1->CR |= ADC_CR_ADCAL; // Zacni kalibracijo A-D pretvornika
    while(ADC1->CR & ADC_CR_ADCAL); // Pocakaj, da se kalibracija konca

    for(int k = 0; k<100; k++){ // Pocakaj nekaj ciklov ure (napaka opisana v erati)
            __NOP();
        }

    ADC1->CFGR |= ADC_CFGR_CONT; // Nastavi nacin zajemanja na konstantno zajemanje
    ADC1->SQR1 |= ADC_SQR1_SQ1_0; // Nastavi kanal zajemanja na 1. kanal, ki predstavlja nozico PA0
    ADC1->SMPR1 = ADC_SMPR1_SMP0_1; // Nastavi cas zajemanja na 2,5 cikla ure AD-pretvornika

    ADC1->CR |= ADC_CR_ADEN; // Prizgi A-D pretvornik
    while(!(ADC1->ISR & ADC_ISR_ADRD)); // Pocakaj, da je A-D pretvornik pripravljen 

    ADC1->CR |= ADC_CR_ADSTART; // Zacni z zajemanjem
}
