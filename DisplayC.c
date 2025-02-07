
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include <ctype.h>  //para isupper e islower
#include "inc/uart.h"
#include "hardware/pio.h"
#include "pio_matrix.pio.h"



#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define button_A_pin 5
#define button_B_pin 6
#define led_green_pin 11
#define led_blue_pin 12
#define NUM_PIXELS 25 //matriz led
#define OUT_PIN 7 // matriz led
ssd1306_t ssd; // Inicializa a estrutura do display ficou aqui para nao dar erro de compilacao

// vetor com a representacao dos numeros 0-9(invertidos na horizontal para melhor visibilidade, algumas correcoes em pixels isolados)
double numeros[10][25] = {
    {0,1,1,1,0, 
     0,1,0,1,0,
     0,1,0,1,0,
     0,1,0,1,0,
     0,1,1,1,0}, // 0 

    {0,0,1,0,0,
     0,0,1,0,0,
     0,0,1,0,0,
     0,1,1,0,0,
     0,0,1,0,0}, // 1

    {0,1,1,1,0,
     0,1,0,0,0, 
     0,1,1,1,0, 
     0,0,0,1,0, 
     0,1,1,1,0}, // 2

    {0,1,1,1,0,
     0,0,0,1,0, 
     0,1,1,1,0, 
     0,0,0,1,0, 
     0,1,1,1,0}, // 3

    {0,1,0,0,0,
     0,0,0,1,0, 
     0,1,1,1,0, 
     0,1,0,1,0, 
     0,1,0,1,0}, // 4

    {0,1,1,1,0,
     0,0,0,1,0, 
     0,1,1,1,0, 
     0,1,0,0,0, 
     0,1,1,1,0}, // 5

    {0,1,1,1,0,
     0,1,0,1,0, 
     0,1,1,1,0, 
     0,1,0,0,0, 
     0,1,1,1,0}, // 6

    {0,0,0,0,1,
     0,1,0,0,0, 
     0,0,1,0,0, 
     0,0,0,1,0, 
     1,1,1,1,1}, // 7

    {0,1,1,1,0,
     0,1,0,1,0, 
     0,1,1,1,0, 
     0,1,0,1,0, 
     0,1,1,1,0}, // 8 

    {0,1,1,1,0,
     0,0,0,1,0, 
     0,1,1,1,0, 
     0,1,0,1,0, 
     0,1,1,1,0}  // 9
};

void inicializador_perifericos(){
  gpio_init(button_A_pin);
  gpio_set_dir(button_A_pin,GPIO_IN);
  gpio_pull_up(button_A_pin);

  gpio_init(button_B_pin);
  gpio_set_dir(button_B_pin,GPIO_IN);
  gpio_pull_up(button_B_pin);

  gpio_init(led_blue_pin);
  gpio_set_dir(led_blue_pin,GPIO_OUT);
  gpio_put(led_blue_pin,false);

  gpio_init(led_green_pin);
  gpio_set_dir(led_green_pin,GPIO_OUT);
  gpio_put(led_green_pin,false);
}

//calback da interrupcao de botoes 
uint32_t last_time_A = 0; // variaveis de tempo para deboucing dos botoes A e B
uint32_t last_time_B = 0;
 bool estado;
 bool estado2;
void gpio_irq_handler(uint gpio, uint32_t events){
  uint32_t current_time = to_us_since_boot(get_absolute_time());// tempo atual em micro segundos
  if(gpio == button_A_pin){
    if(current_time - last_time_A > 200000){ //deboucing de 200 ms
      estado = gpio_get(led_green_pin);
      gpio_put(led_green_pin, !estado);// mmuda o estado do led
      estado = !estado;
            // Atualiza a tela do display
            ssd1306_fill(&ssd, 0); // Limpa a tela
            ssd1306_draw_string(&ssd, "LED VERDE", 6, 10);
            ssd1306_draw_string(&ssd, estado ? "ON" : "OFF", 85, 10); // Escreve "ON" ou "OFF para o estado do led
            ssd1306_send_data(&ssd); // Envia os dados para o display
          
        printf("Botao A pressionado! Estado do led verde = %d.\n", estado);// mensagem via serial
     last_time_A = current_time;
     
    }
  } else if(gpio == button_B_pin){
    if(current_time - last_time_B > 200000){ //deboucing de 200 ms
     estado2 = gpio_get(led_blue_pin);
      gpio_put(led_blue_pin, !estado2);// muda o estado do led
      estado2 = !estado2;
           // Atualiza a tela do display
            ssd1306_fill(&ssd, 0); // Limpa a tela
            ssd1306_draw_string(&ssd, "LED AZUL", 6, 20);
            ssd1306_draw_string(&ssd, estado2 ? "ON" : "OFF", 85, 20); // Escreve "ON" ou "OFF para o estado do led
            ssd1306_send_data(&ssd); // Envia os dados para o display

        printf("Botao B pressionado! Estado do led azul = %d.\n", estado2);// mensagem via serial
      last_time_B = current_time;
    }
  }
}

//para matriz de led 
uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para atualizar a matriz de LEDs
void atualiza_display(int numero, PIO pio, uint sm) {
    uint32_t valor_led;
    for (int i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(numeros[numero][i], 0.0, 0.0); // Assume cor vermelha 
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}



int main()
{
  PIO pio = pio0;
    bool ok;
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &pio_matrix_program);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  //inicializa os perifericos (led e botao)
  inicializador_perifericos();
  
  //interrupcoes para os botoes
  gpio_set_irq_enabled_with_callback(button_A_pin, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(button_B_pin, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  stdio_init_all();

  bool cor = true;
  while (true)
  {
     char serial = getchar(); // Recebe um caractere da comunicação serial

    // Verifica se é uma letra
    if (isalpha(serial)) {
        ssd1306_fill(&ssd, 0); // Limpa a tela antes de desenhar
        
        // Verifica se o caractere é maiúsculo ou minúsculo
        if (isupper(serial)) {
            printf("Digitou: %c.\n", serial);
            ssd1306_draw_char(&ssd, serial, 10, 20);
        
        } else if (islower(serial)) {
            printf("Recebido: %c.\n", serial);
            ssd1306_draw_char_min(&ssd, serial, 10, 20);
        }

        // Atualiza o display
        ssd1306_send_data(&ssd);
    }
    
    switch (serial){
      case '0': atualiza_display(0,pio0,0); ssd1306_fill(&ssd, 0); ssd1306_draw_char(&ssd, serial, 10, 20);ssd1306_send_data(&ssd); break;
      case '1': atualiza_display(1,pio0,0); ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20);ssd1306_send_data(&ssd);break;
      case '2': atualiza_display(2,pio0,0); ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20);ssd1306_send_data(&ssd);break;
      case '3': atualiza_display(3,pio0,0); ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20);ssd1306_send_data(&ssd);break;
      case '4': atualiza_display(4,pio0,0); ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20); ssd1306_send_data(&ssd);break;
      case '5': atualiza_display(5,pio0,0);ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20); ssd1306_send_data(&ssd);break;
      case '6': atualiza_display(6,pio0,0);ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20); ssd1306_send_data(&ssd);break;
      case '7': atualiza_display(7,pio0,0);ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20); ssd1306_send_data(&ssd);break;
      case '8': atualiza_display(8,pio0,0);ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20); ssd1306_send_data(&ssd);break;
      case '9': atualiza_display(9,pio0,0);ssd1306_fill(&ssd, 0);ssd1306_draw_char(&ssd, serial, 10, 20); ssd1306_send_data(&ssd);break;
      
    }
  }
}