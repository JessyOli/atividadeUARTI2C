#include "led_matrix.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include <stdio.h>

// Declaração do buffer de pixels
pixel_t leds[MATRIX_SIZE];
PIO np_pio;
uint sm;

// Carrega programa PIO
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &tarefa_UART_SPI_I2C_program);
    np_pio = pio0;

    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }

    tarefa_UART_SPI_I2C_program_init(np_pio, sm, offset, pin, 800000.f);

    // Limpa buffer de LEDs
    npClear();
}

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear() {
    for (uint i = 0; i < MATRIX_SIZE; ++i) {
        npSetLED(i, 0, 0, 0);
    }
}

void npWrite() {
    for (uint i = 0; i < MATRIX_SIZE; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

void npDraw(const uint8_t (*vetorR)[5], const uint8_t (*vetorG)[5], const uint8_t (*vetorB)[5]) {
    int i, j, idx, col;
    for (i = 0; i < 5; i++) {
        idx = (4 - i) * 5;
        for (j = 0; j < 5; j++) {
            col = (i % 2 == 0) ? (4 - j) : j;
            npSetLED(idx + col, vetorR[i][j], vetorG[i][j], vetorB[i][j]);
        }
    }
    npWrite();
}

uint8_t vetorG[5][5] = {0};

uint8_t vetorR[5][5] = {
    {160, 160, 160, 160, 160},
    {160, 160, 160, 160, 160},
    {160, 160, 160, 160, 160},
    {160, 160, 160, 160, 160},
    {160, 160, 160, 160, 160}
};

uint8_t vetorB[5][5] = {0};

// Função para exibir um número na matriz
void draw_number(const uint8_t vetorR[5][5], const uint8_t vetorG[5][5], const uint8_t vetorB[5][5]) {
    npDraw(vetorR, vetorG, vetorB);
}

// Definição dos números
const uint8_t numbers[10][5][5] = {
    { {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1} }, // 0
    { {0,0,1,0,0}, {0,1,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1} }, // 1
    { {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1} }, // 2
    { {1,1,1,1,1}, {0,0,0,0,1}, {0,1,1,1,0}, {0,0,0,0,1}, {1,1,1,1,1} }, // 3
    { {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,0,1} }, // 4
    { {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {0,0,0,0,1}, {1,1,1,1,1} }, // 5
    { {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1} }, // 6
    { {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0} }, // 7
    { {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1} }, // 8
    { {1,1,1,1,1}, {1,0,0,0,1}, {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,0,1} }  // 9
};

// Manipula a exibição de números na matriz
int handle_numbers(char num) {
    if (num >= '0' && num <= '9') {
        // Passa os vetores corretos do número para a função de desenho
        draw_number(numbers[num - '0'], vetorG, vetorB);
        printf("%c!\n", num);
        return 0;
    } else {
        npClear();
        return 1;
    }
}
