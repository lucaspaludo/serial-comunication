#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 1000000 // frequencia do microcontrolador
#include <util/delay.h>
#define USART_BAUDRATE 1200 // taxa de transmissão desejada
#define STOPBITS 2 // número de bits de parada 1 ou 2
#define BAUD_PRESCALE ((F_CPU/ (USART_BAUDRATE * (long)16))-1) //calcula o valor do prescaler da usart

int NUM[7] = {63,6,91,79,102,109,125}; //criação do vetor com números que serão mostrados no display
int conversor;

ISR(ADC_vect)
{
	conversor = ADCW;
	ADCSRA|=0b01000000; // inicia nova conversão
}

void usart_send()
{
	while((UCSRA&0b00100000)==0)
	{}
	UDR = conversor/4;
}

void usart_init() // inicia a comunicação serial
{
	UCSRB |= (1 << RXEN) | (1 << TXEN); // Liga a transmissão e a recepção
	
	#if STOPBITS == 2
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1) | (1 << USBS);
	// Usa 8-bit com 2 stop bits
	
	#else
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // // Usa 8-bit com 1 stop bits
	
	#endif
	UBRRL = BAUD_PRESCALE; // Parte baixa da taxa de transmissão
	UBRRH = (BAUD_PRESCALE >> 8); // Parte alta da taxa de transmissão
	UCSRB |= (1 << RXCIE); // Habilita a interrupção de transmissão
}

ISR(USART_RXC_vect) // interrupção de recepção
{
	unsigned char recebido;
	recebido = UDR;

	if(recebido>=0 && recebido<25){ //tensão entre 0 e 0,5 -> número 0 no display
		PORTB = NUM[0];
	}
	
	if(recebido>=25 && recebido<77){ //tensão entre 0,5 e 1,5 -> número 1 no display
		PORTB = NUM[1];
	}
	
	if(recebido>=77 && recebido<128){ //tensão entre 1,5 e 2,5 -> número 2 no display
		PORTB = NUM[2];
	}
	
	if(recebido>=128 && recebido<179){ //tensão entre 2,5 e 3,5 -> número 3 no display
		PORTB = NUM[3];
	}
	
	if(recebido>=179 && recebido<230){ //tensão entre 3,5 e 4,5 -> número 4 no display
		PORTB = NUM[4];
	}
	
	if(recebido>=230 && recebido<=256){ //tensão entre 4,5 e 5 -> número 5 no display
		PORTB = NUM[5];
	}
	
}

int main(void)
{
	DDRB=0b11111111;
	usart_init();
	sei();
	
	/*--------------CONVERSOR A/D-------------*/
	//CONFIGURAÇÃO DOS REGISTRADORES DO CONVERSOR A/D
	ADMUX = 0b01000000;			  //define entrada ADC0 do conversor A/D com referência no pino AVCC
	ADCSRA = 0b11001111;		  //define prescaler de 128, habilita a interrupção pelo conversor A/D, inicia a conversão e habilita o conversor A/D
	/*----------------------------------------*/

	while(1)
	{
		usart_send();
		_delay_ms(100);
	}
}
