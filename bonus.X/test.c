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
#include<util/delay.h>

volatile int execution_time = 0;

ISR(TIMER1_COMPA_vect) {
    execution_time++;
}

#include <avr/pgmspace.h>
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

// Function to print memory usage
void print_memory_usage() {
    
    int pmu=program_memory_usage();
   
    int dmu=data_memory_usage();
    
}

// Function to get program memory usage
int program_memory_usage() {
    extern int __data_start, __data_end, __bss_start, __bss_end;
    return (&__data_end - &__data_start) + (&__bss_end - &__bss_start);
}

// Function to get data memory usage
int data_memory_usage() {
    extern int __heap_start;
    return SP - (uint16_t)&__heap_start;
}

void setup() {
    // Setup timer
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10); // CTC mode, prescaler 1
    OCR1A = F_CPU / 1000 - 1; // Compare value for 1ms
    TIMSK1 = (1 << OCIE1A); // Enable interrupt
    sei(); // Enable global interrupts

    print_memory_usage();
}

void loop() {

    int output[OUTPUT_H][OUTPUT_W];

    custom_convolution(input, kernel, output);

    // print the output tensor

    for (int i = 0; i < OUTPUT_H; i++) {
        for (int j = 0; j < OUTPUT_W; j++) {
            printf("%d ", output[i][j]);
        }
        printf("\n");
    }

    // Add any delay or additional logic as needed
    _delay_ms(1000); // Example: wait for 1 second
}

int main() {
    while(1) {
        loop();
    }
}
