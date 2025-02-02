/*  
 * Projeto: Controle de LEDs e Botões com Interrupções (Tarefa U4C4O12T)  
 * Autor: Kaian de Souza Gonçalves  
 * Data: 02/02/2024  
 *  
 * Funcionalidades:  
 * - LED vermelho pisca a 5 Hz  
 * - Botões A/B alteram números (0-9) em matriz WS2812 via interrupções  
 * - Debouncing por software (200ms)  
 * - Controle de LEDs comuns e endereçáveis  
 *  
 * Componentes:  
 * - Matriz 5x5 WS2812 (GP7), LED RGB (GP13,11,12), Botões A (GP5) / B (GP6)  
 */  


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "pico/bootrom.h"
#include "ws2818b.pio.h"

// macros
#define LED_COUNT 25
#define LED_PIN 7
#define DEBOUNCE_DELAY_MS 200

#define LED_R 13
#define LED_G 11
#define LED_B 12

// Ajuste o valor (0 a 255) conforme desejar para a intensidade
#define LED_BRIGHTNESS 50  // DEFINE A INTENSIDADE DO LED

// prototipos
void init_hardware(void);
void init_leds(void);
void set_led(int index, uint8_t r, uint8_t g, uint8_t b);
void clear_leds(void);
void write_leds(void);
void exibirNumero(int countBotao);
static void gpio_irq_handler(uint gpio, uint32_t events);

// variaveis globais
int countBotao = 0;
static volatile uint32_t last_time = 0; // Armazena o tempo do ultimo evento em microssegundos

// Matriz com os números (0 a 9) para a matriz de LEDs WS2812
int numerosMatriz[10][25] = {
    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0}, // 0

    {0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0}, // 1

    {0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0}, // 2

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0}, // 3

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0}, // 4

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0}, // 5

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0}, // 6

    {0, 0, 0, 1, 0,
     0, 0, 1, 0, 0,
     0, 1, 0, 0, 0,
     0, 0, 0, 0, 1,
     1, 1, 1, 0, 0}, // 7

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0}, // 8

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0}  // 9
};

PIO np_pio;
uint sm;

struct pixel_t {
  uint8_t G, R, B;
};
typedef struct pixel_t npLED_t;
npLED_t leds[LED_COUNT];

void init_hardware(void)
{
  // Configura botao A na GPIO 5 com pull-up e interrupção
  gpio_init(5);
  gpio_set_dir(5, GPIO_IN);
  gpio_pull_up(5);
  gpio_set_irq_enabled_with_callback(5, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);

  // Configura botão B na GPIO 6 com pull-up e interrupção 
  gpio_init(6);
  gpio_set_dir(6, GPIO_IN);
  gpio_pull_up(6);
  gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_FALL, true);

  // Configura os pinos do LED RGB
  gpio_init(LED_R);
  gpio_set_dir(LED_R, GPIO_OUT);

  gpio_init(LED_G);
  gpio_set_dir(LED_G, GPIO_OUT);

  gpio_init(LED_B);
  gpio_set_dir(LED_B, GPIO_OUT);

  // Inicializa os LEDs endereçáveis WS2812
  init_leds();
  clear_leds();
  write_leds();

  // Inicializa o tempo do ultimo botao pressionado
  last_time = to_us_since_boot(get_absolute_time());
}

void init_leds(void)
{
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;
  sm = pio_claim_unused_sm(np_pio, true);
  ws2818b_program_init(np_pio, sm, offset, LED_PIN, 800000.f);
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i].R = leds[i].G = leds[i].B = 0;
  }
}

void set_led(int index, uint8_t r, uint8_t g, uint8_t b)
{
  if (index < LED_COUNT) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
  }
}

void clear_leds(void)
{
  for (int i = 0; i < LED_COUNT; i++) {
    set_led(i, 0, 0, 0);
  }
}

void write_leds(void)
{
  for (int i = 0; i < LED_COUNT; i++) {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
}

void exibirNumero(int countBotao)
{
  clear_leds();
  for (int incremento = 0; incremento < LED_COUNT; incremento++) {
    if (numerosMatriz[countBotao][incremento] == 1) {
      set_led(incremento,
              (LED_BRIGHTNESS * 1) / 1,           
              (LED_BRIGHTNESS * 1) / 2,          
              (LED_BRIGHTNESS * 1) / 3);          
    }
  }
  write_leds();
}

/** 
 * Função de interrupção para tratar os botões A (GPIO 5) e B (GPIO 6)
 * Implementa debouncing software usando a constante DEBOUNCE_DELAY_MS.
 */
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_time < (DEBOUNCE_DELAY_MS * 1000))
        return;
    
    last_time = current_time;
    
    if (gpio == 5) {  // Botão A: incremento
        countBotao++;
        if (countBotao > 9)
            countBotao = 0;
        exibirNumero(countBotao);
    }
    
    if (gpio == 6) {  // Botão B: decremento
        countBotao--;
        if (countBotao < 0)
            countBotao = 0;
        exibirNumero(countBotao);
    }
}

/**
 * Callback do timer que alterna o estado do LED vermelho (LED_R)
 * para gerar o efeito de piscar 5 vezes por segundo.
 */
bool repeating_timer_callback(struct repeating_timer *t) {
    gpio_put(LED_R, !gpio_get(LED_R));
    return true;
}

int main()
{
  stdio_init_all();
  init_hardware();
  exibirNumero(countBotao);

  // Timer que alterna o LED vermelho a cada 100 ms (5 Hz)
  struct repeating_timer timer;
  add_repeating_timer_ms(-100, repeating_timer_callback, NULL, &timer);

  // Loop principal (as acoes dos botoes são tratadas via IRQ)
  while (true) {
    sleep_ms(1000);
  }
}
