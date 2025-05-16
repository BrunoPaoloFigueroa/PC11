#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
char modo=0;
char canal=0;
float V_A0,V_A1; //a0 temp, a1 pot
void gate(char gate){//cambio de puerto A0 a A1

  ADMUX&=0xF0;
  ADMUX|=gate;

}
void display(float valor){

  int val = (int)(valor * 10); // Convierte, ej: 0.73 -> 73
  if (val > 999) val = 999;

  int c = val / 100;        // Centenas
  int d = (val / 10) % 10;  // Decenas
  int u = val % 10;         // Unidades

  // Mostrar centenas
  PORTB |= 0x07;           // Apaga todos los displays
  PORTD = (PORTD & 0x0F) | (c << 4); // Envia BCD al 74LS48
  PORTB &= ~(1 << 2);        // Enciende display 1
  _delay_ms(1);

  // Mostrar decenas
  PORTB |= 0x07;
  PORTD = (PORTD & 0x0F) | (d << 4);
  PORTB &= ~(1 << 1);        // Enciende display 2
  _delay_ms(1);

  // Mostrar unidades
  PORTB |= 0x07;
  PORTD = (PORTD & 0x0F) | (u << 4);
  PORTB &= ~(1 << 0);        // Enciende display 3
  _delay_ms(1);
}



void adc_config(){

    ADMUX|=(1<<REFS0);
    ADCSRA|=(1<<ADEN)|(1<<ADIE)|(1<<ADPS2);
    gate(0);
    ADCSRA|=(1<<ADSC);
}

ISR(ADC_vect){
    switch(canal){
      case 0:
      
      V_A0=(ADC*500.0/1023);
      canal=1;
     
      
      break;

      case 1:
      V_A1=(ADC*99.9/1023);
      canal=0;
      
      
      break;

    }
    gate(canal);
    ADCSRA|=(1<<ADSC);
}
void config_ext(){

      EIMSK|=0X03;
      EICRA|=0x0A;
      DDRD&=~(0x0C);

}
ISR(INT1_vect){

  if (modo == 0) {
    modo = 1;
} else {
    modo= 0;
}

}

int main(void){

    DDRD|=0xF0;
    DDRB|=0x0F;
    PORTB&=~(0x08);
    adc_config();
    config_ext();
    sei();

    while(1){


      if (modo== 0) {
        display(V_A0);
    } else {
        display(V_A1);
    }
        
      if(V_A1>V_A0){
      PORTB|=0x08;

      }
      else if(V_A1<=V_A0){
        PORTB&=~(0x08);

      }
    }


  return 0;
}




























