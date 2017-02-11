// Yuxin Pan

#include <avr/io.h>
#include "util/delay.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint8_t tot_overflow,pressed,P1=0;
volatile int releaseTime1;


void timer2_delay(){
    
    
    PORTB |= (1<<PB5);   // turn on LED
    
    
    OCR1A=TCNT1+23333; // for indication purpose on LED
    
    TIMSK1 |= 0x02; // enable output compare A interrupt

}


void timer1_init() {
    
    TCCR1B |= (1 << CS12); // set up timer with prescaler = 256
    TCNT1 = 0;
    
    TIMSK1 |= (1 << TOIE1); // enable overflow interrupt
    TIMSK1 |= (1 << ICIE1); // enable Input Capture Interrupt
        
    TCCR1A = 0x40; // toggle on an output compare
    
}


// 200ms=12500
// pressed: 0:not pressed  1:pressed
// code: 2:dot  3:dash  4:space

int main(void) {
    
    int i,sum;
    uint8_t received=0,num=0;
    
    uart_init();    // for serial output
    
    DDRB|=(1<<PB5); // set LED as output, PB5 just means bit number 5 (LED internally connects to PB5)
    
    // 7 segment LEDs
    
    DDRB|=(1<<PB4); // set 4 (pin12) as output
    DDRB|=(1<<PB3); // set 3 (pin11) as output
    DDRB|=(1<<PB2); // set 3 (pin11) as output
    PORTB |= (1<<PB2); // this is voltage high input for 7 segment LEDs
    PORTB |= (1<<PB3); // turn off dash on 7 segment
    PORTB |= (1<<PB4); // turn off dot on 7 segment
    
    // end 7 segment LEDs
    
    timer1_init();
    //timer2_init();
    
    sei(); // enable global interrupts
    
    releaseTime1=0;
    tot_overflow = 0;
    pressed=0;
    sum=0;
    
    while(1) {
        
        received=0;
        
        // dot
        if ((releaseTime1>1875)&(releaseTime1<12500)&(!(tot_overflow))){
            
            PORTB &= ~(1 << PB4);
            num++;
            sum+=7*num;
            P1=0;
            printf(". " );
            releaseTime1=0;
            timer2_delay();            
            
        }
        
        // dash
        if ((TCNT1>=12500)&(pressed)&(!(P1))){
            
            PORTB &= ~(1 << PB3);
            num++;
            sum+=10*num;
            printf("- " );
            P1=1;
            timer2_delay();
            
        }
        
        // space
        if ((TCNT1>=25000)&(!(pressed))){
            
            received=1;
            P1=0;            
            
        }
        
        
        if ((received)&(sum!=0)){
            
            printf("\n" );
            //printf("\nSUM = %d\n",sum );
            
            if (num>5) printf("Invalid Code\n" );
            
            else
                
                switch(sum) {
                    case 27 :
                        printf("A\n" );
                        break;
                        
                    case 73 :
                        printf("B\n" );
                        break;
                        
                    case 82 :
                        printf("C\n" );
                        break;
                        
                    case 45 :
                        printf("D\n" );
                        break;
                        
                    case 42 :
                        printf("S\n" );
                        break;
                        
                    case 7 :
                        printf("E\n" );
                        break;
                        
                    case 24 :
                        printf("N\n" );
                        break;
                        
                    case 10 :
                        printf("T\n" );
                        break;
                        
                    case 147 :
                        printf("1\n" );
                        break;
                        
                    case 141 :
                        printf("2\n" );
                        break;
                        
                    case 132 :
                        printf("3\n" );
                        break;
                        
                    case 120 :
                        printf("4\n" );
                        break;
                        
                    case 105 :
                        printf("5\n" );
                        break;
                        
                    default :
                        printf("Invalid Code\n" );
                }
            
            num=0;
            sum=0;
            
        }
    }
}


// TIMER0 overflow interrupt service routine, called whenever TCNT0 overflows
ISR(TIMER1_OVF_vect) {
    
    tot_overflow=1;
    
}


ISR (TIMER1_COMPA_vect){
    
    TIMSK1 &= ~(1 << OCIE1A);
    PORTB |= (1<<PB3); // turn off dash on 7 segment
    PORTB |= (1<<PB4); // turn off dot on 7 segment
    PORTB &= ~(1 << PB5); // turn off LED    
    
}


ISR(TIMER1_CAPT_vect) {
    
    if (pressed) {
        pressed=0;
        releaseTime1=TCNT1;
    }
    else {
        pressed=1;
        releaseTime1=0;
    }
    TCCR1B ^= (1 << ICES1); // toggle ICES1 to capture next event either rise or fall)
    TCNT1=0;
    tot_overflow=0;
    P1=0;
    
}

