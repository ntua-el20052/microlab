#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "input.h"
#include "kernel.h"
#define F_CPU 16000000UL
#define PADDING 2
#define STRIDE 1
#define MAX_LINE_LEN 100
#define OUTPUT_C 1
#define OUTPUT_H (I_H - K_H + 2*PADDING)/STRIDE + 1
#define OUTPUT_W (I_W - K_W + 2*PADDING)/STRIDE + 1
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

int execution_time = 0;

ISR(TIMER1_COMPA_vect) {
	execution_time++;
}


void padded_set(int input[I_C][I_H][I_W], int output[I_C][I_H+2*PADDING][I_W+2*PADDING]){
	
	int output_height = I_H+2*PADDING;
	int output_width = I_W+2*PADDING;
	int input_channels = I_C;

	int array[I_C][I_H+2*PADDING][I_W+2*PADDING];

	for (int i = 0; i < output_height; i++) {
		for (int j = 0; j < output_width; j++) {
			for (int c = 0; c < input_channels; c++) {
				if(i<PADDING || j<PADDING || i>=output_height-PADDING || j>=output_width-PADDING){
					output[c][i][j] = 0;
				}
				else{
					output[c][i][j] = input[c][i-PADDING][j-PADDING];
				}
			}
		}
	}
}

void custom_convolution(int input[I_C][I_H][I_W],int kernel[K_C][K_H][K_W],int output[OUTPUT_H][OUTPUT_W]) {
	// write a convolution function that takes in an input tensor and a kernel tensor

	int k=0,l=0;

	assert(STRIDE > 0);
	assert(PADDING >= 0);

	// apply padding to the input table if necessary without torch library

	int temp[I_C][I_H+2*PADDING][I_W+2*PADDING];
	if(PADDING>0){
		padded_set(input,temp);
	}

	//set the output tensor to zero

	for(int i=0;i<OUTPUT_H;i++){
		for(int j=0;j<OUTPUT_W;j++){
			output[i][j] = 0;
		}
	}

	// loop over the output pixels
	for (int i = 0; i < OUTPUT_H; i++) {
		for (int j = 0; j < OUTPUT_W; j++) {
			// loop over the input channels
			for (int c = 0; c < I_C; c++) {
				//loop over the kernel pixels
				for (int m = 0; m < K_H; m++) {
					for (int n = 0; n < K_W; n++) {
						k = i*STRIDE + m;
						l = j*STRIDE + n;
						if(PADDING>0){
							output[i][j] += temp[c][k][l] * kernel[c][m][n];
						}
						else{
							output[i][j] += input[c][k][l] * kernel[c][m][n];
						}
					}
				}
			}

		}
	}
}


void setup() {
	// Setup timer
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS10); // CTC mode, prescaler 1
	OCR1A = F_CPU/1000 - 1; // Compare value for 1ms
	TIMSK1 = (1 << OCIE1A); // Enable interrupt
	sei(); // Enable global interrupts

}

void usart_init(unsigned int ubrr){
	UCSR0A=0;
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
	UBRR0H=(unsigned char)(ubrr>>8);
	UBRR0L=(unsigned char)ubrr;
	UCSR0C=(3 << UCSZ00);
	return;
}

void usart_transmit(uint8_t data){
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0=data;
}


void usart_transmit_string(const char *a){
	for (int i=0; i< strlen(a); i++){
		usart_transmit(a[i]);
	}
}


int main() {
	usart_init(103);
	char timer[50];
	//,out[100];
	setup();
	while(1) {
		
		int output[OUTPUT_H][OUTPUT_W];
		custom_convolution(input, kernel, output);
		 
		_delay_ms(35);
	    sprintf(timer,"%d",execution_time);
		usart_transmit_string(timer);
		usart_transmit_string("\n");
		//execution_time=0;
		
		 
	}
	

	return 0;
	
	
}

