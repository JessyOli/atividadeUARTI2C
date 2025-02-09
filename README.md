Introdução às Interfaces de Comunicação Serial com RP2040
UART, SPI e I2C
Repositório criado a fim de armazenar a tarefa realizada para consolidar a compreensão dos conceitos sobre o uso de interfaces de comunicação serial no RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab.

Objetivos
• Compreender o funcionamento e a aplicação de comunicação serial em microcontroladores; <br>
• Aplicar os conhecimentos adquiridos sobre UART e I2C na prática; <br>
• Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812; <br>
• Fixar o estudo do uso botões de acionamento, interrupções e Debounce; <br>
• Desenvolver um projeto funcional que combine hardware e software.

Descrição do Projeto
Utilização obrigatória

Matriz 5x5 de LEDs (endereçáveis) WS2812, conectada à GPIO 7; <br>
LED RGB, com os pinos conectados às GPIOs (11, 12 e 13); <br>
Botão A conectado à GPIO 5; <br>
Botão B conectado à GPIO 6; <br>
Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).

### Funcionalidades do Projeto

__1. Modificação da Biblioteca font.h__ <br>
• Adicionar caracteres minúsculos à biblioteca font.h. <br>

__2. Entrada de caracteres via PC__ <br>
• Utilize o Serial Monitor do VS Code para digitar os caracteres.<br>
• Cada caractere digitado no Serial Monitor deve ser exibido no display SSD1306 (não é necessário suportar o envio de strings completas).<br>
• Quando um número entre 0 e 9 for digitado, um símbolo correspondente ao número deve ser exibido, também, na matriz 5x5 WS2812.<br>

__3. Interação com o Botão A__ <br>
• Pressionar o botão A deve alternar o estado do LED RGB Verde (ligado/desligado). <br>
• A operação deve ser registrada com uma mensagem informativa sobre o estado do LED exibida no display SSD1306 e um texto descritivo sobre a operação enviado ao Serial Monitor. <br>

__5. Interação com o Botão B__ <br>
• Pressionar o botão B deve alternar o estado do LED RGB Azul (ligado/desligado). <br>
• A operação deve ser registrada com uma mensagem informativa sobre o estado do LED deve ser exibida no display SSD1306 e um texto descritivo sobre a operação enviado ao Serial Monitor. <br>

### Requisitos do Projeto

__1. Uso de Interrupções (IRQ) para Botões__ <br>
O código configura interrupções para os botões `BUTTON_A` e `BUTTON_B`:  

```c
gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_callback);
gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_callback);
```

A função `gpio_irq_callback` é usada para tratar as interrupções e alternar o estado dos LEDs:  

```c
void gpio_irq_callback(uint gpio, uint32_t events) {
  uint32_t current_time = to_ms_since_boot(get_absolute_time());
  if (current_time - last_time > 200) { // Debouncing de 200ms
    last_time = current_time;
    if (gpio == BUTTON_A) {
      led_green_state = !led_green_state;
      gpio_put(GREEN_RGB, led_green_state);
      snprintf(string_a, sizeof(string_a), "LED VERDE %s", led_green_state ? "1" : "0");
      printf("Botão A pressionado mudou o estado do LED verde para %s\n", led_green_state ? "1" : "0");
    } else if (gpio == BUTTON_B) {
      led_blue_state = !led_blue_state;
      gpio_put(BLUE_RGB, led_blue_state);
      snprintf(string_b, sizeof(string_b), "LED AZUL %s", led_blue_state ? "1" : "0");
      printf("Botão B pressionado mudou o estado do LED azul para %s \n", led_blue_state ? "1" : "0");
    }
  }
}
```

Isso garante que os botões são processados de forma assíncrona via interrupções.

__2. Debouncing via Software__ <br>
O código implementa um mecanismo de __debouncing__ verificando se passaram 200ms desde a última interrupção:  

```c
if (current_time - last_time > 200) { // Debouncing de 200ms
```

Isso evita múltiplas detecções devido ao efeito de bouncing dos botões.

__3. Controle de LEDs (Comuns e WS2812)__ <br>  

__LEDs Comuns__
Os LEDs são inicializados e controlados corretamente:  

```c
gpio_init(RED_RGB);
gpio_init(GREEN_RGB);
gpio_init(BLUE_RGB);
gpio_set_dir(RED_RGB, GPIO_OUT);
gpio_set_dir(GREEN_RGB, GPIO_OUT);
gpio_set_dir(BLUE_RGB, GPIO_OUT);
gpio_put(RED_RGB, 0);
gpio_put(GREEN_RGB, 0);
gpio_put(BLUE_RGB, 0);
```

O estado dos LEDs muda com base nas interrupções dos botões.

__LEDs WS2812 (Endereçáveis)__
O código usa funções da biblioteca `ws2812.pio.h` para controlar a matriz de LEDs:  

```c
npInit(LED_PIN);
npClear();
npWrite();
```

__4. Utilização do Display 128x64 via I2C__ <br>  

O código inicializa o barramento I2C e configura o display corretamente:  

```c
i2c_init(I2C_PORT, 400 * 1000);
gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
gpio_pull_up(I2C_SDA);
gpio_pull_up(I2C_SCL);
ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
ssd1306_config(ssd);
ssd1306_send_data(ssd);
ssd1306_fill(ssd, false);
ssd1306_send_data(ssd);
```

O display é atualizado com informações dos LEDs e caracteres recebidos pela UART:  

```c
ssd1306_draw_string(&ssd, string_a, 8, 40);
ssd1306_draw_string(&ssd, string_b, 8, 48);
ssd1306_send_data(&ssd);
```

__5. Envio de Informação pela UART__ <br>  

A UART é inicializada corretamente:  

```c
uart_init(UART_ID, 115200);
gpio_set_function(0, GPIO_FUNC_UART);
gpio_set_function(1, GPIO_FUNC_UART);
```

O código lê caracteres digitados no terminal via `getc(stdin)` e os exibe no display:

```c
if(stdio_usb_connected) {
  c[0] = getc(stdin); // Recebe o caractere digitado
  handle_numbers(c[0]);
  ssd1306_draw_string(&ssd, "Caractere: " , 8, 16);
  ssd1306_draw_string(&ssd, c, 80, 16);
  ssd1306_send_data(&ssd);
}