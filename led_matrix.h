#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "pico/stdlib.h"

#define MATRIX_SIZE 25  // 5x5 LEDs
#define LED_PIN 7       // Pino de controle dos LEDs

// Pixel no formato GRB
typedef struct {
    uint8_t G, R, B;
} pixel_t;

// Armazena o estado dos LEDs
extern pixel_t leds[MATRIX_SIZE];

// Inicializa a matriz de LEDs no pino especificado
void npInit(uint pin);

// Define a cor de um LED específico na matriz
void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b);

// Desliga os LEDs
void npClear(void);

// Atualiza a matriz física com os valores definidos em `leds`
void npWrite(void);

// Desenha uma imagem na matriz de LEDs usando 3 matrizes de cores (R, G, B)
void npDraw(const uint8_t vetorR[5][5], const uint8_t vetorG[5][5], const uint8_t vetorB[5][5]);

// Converte um número para uma representação adequada na matriz de LEDs
int handle_numbers(char num);

#endif  // LED_MATRIX_H
