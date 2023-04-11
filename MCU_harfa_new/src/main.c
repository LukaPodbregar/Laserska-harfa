#include "main.h"
#include "mcu_setup.h"
#include "usart_setup.h"

int main(void)
{
    int DAC_OUT[7] = {0, 682, 1364, 2046, 2728, 3410, 4095}; // Izhodi D-A pretvornika definirani kot polje celostevilskih vrednosti
    int i = 0; // Spremenljivka, ki steje ponovitve glavne “while” zanke
    int l = 0; // Spremenljivka, ki steje stevilo vzorcev za kalibracijo
    int delay = 8000; // Spremenljivka, ki doloca dolzino zakasnitve
    float ambient_value_current = 0; // Spremenljivka za shranjevanje trenutne vrednosti na A-D pretvorniku
    float differential = 0; // Spremenljivka za racunanje tekocega povprecja
    float mean = 0; // Spremenljivka za racunanje tekocega povprecja

    init_clock();
    init_gpio();
    init_dac();
    init_adc();
    init_usart2();

    while(l<2500){ // Zajemi 2500 vzorcev
        DAC->DHR12R1 = DAC_OUT[i]; // Nastavi D-A pretvornik na vrednost iz polja DAC_OUT
        for(int k = 0; k<5000; k++){ // Cakalna funkcija
            __NOP();
        }
        GPIOA->BSRR |= GPIO_BSRR_BS_5; // Nastavi bit nozice PA5 na 1, kar prizge laser
        if (ADC1->ISR & ADC_ISR_EOC){ // Preveri, ce je vrednost iz registra A-D pretvornika pripravljena za branje
            l++;
            ambient_value_current = ADC1->DR; // Shrani prebrano vrednost iz registra A-D pretvornika
            differential = (ambient_value_current - mean) / l; // Opravi izračun tekocega povprecja
            mean = mean + differential;
        }
        for(int k = 0; k<delay; k++){
            __NOP();
        }
        GPIOA->BSRR |= GPIO_BSRR_BR_5; // Nastavi bit nozice PA5 na 0, kar ugasne laser 
        i++;
        if (i == 7){ // Ce se je while zanka izvedla sedemkrat, nastavi vrednost stevila, ki steje iteracije na 0 (zacni novo stopnico)
            i = 0;
        }
    }

    for(int k = 0; k<1000000; k++){ // Za konec kalibracije ugasni laser za kratek cas
                    __NOP();
                }

    while(1){
        DAC->DHR12R1 = DAC_OUT[i];
        for(int k = 0; k<5000; k++){
            __NOP();
        }
        GPIOA->BSRR |= GPIO_BSRR_BS_5;
        if (ADC1->ISR & ADC_ISR_EOC){ // Preveri, ce je vrednost iz registra A-D pretvornika pripravljena za branje
            ambient_value_current = ADC1->DR;
            if ((ambient_value_current - mean > 200)){ // Ce je zaznan dotik strune izvedi posiljanje podatkov
                delay = 3300; // Skrajsaj dolzino zakasnitve
                switch(i){
                    case 0: // Ce je prekinjena struna 0, poslji bajt 5 
                        usart_send_byte('5');
                        break;
                    case 1:
                        usart_send_byte('6');
                        break;
                    case 2:
                        usart_send_byte('0');
                        break;
                    case 3:
                        usart_send_byte('1');
                        break;
                    case 4:
                        usart_send_byte('2');
                        break;
                    case 5:
                        usart_send_byte('3');
                        break;
                    case 6:
                        usart_send_byte('4');
                        break;
                    }
                }
            else{ // Ce dotik strune ni zaznan, nastavi zakasnitev na zacetno dolzino
                    delay = 8000;
                }
            }
        for(int k = 0; k<delay; k++){ 
            __NOP();
        }
        GPIOA->BSRR |= GPIO_BSRR_BR_5;
        i++;
        if (i == 7){
            i = 0;
        }
    }
}
