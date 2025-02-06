
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/gpio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define button_A_pin 5
#define button_B_pin 6
#define led_green_pin 11
#define led_blue_pin 12

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

//calback da interrupcao de botoes (falta so a msg no display)
uint32_t last_time_A = 0; // variaveis de tempo para deboucing dos botoes A e B
uint32_t last_time_B = 0;
void gpio_irq_handler(uint gpio, uint32_t events){
  uint32_t current_time = to_us_since_boot(get_absolute_time());// tempo atual em micro segundos
  if(gpio == button_A_pin){
    if(current_time - last_time_A > 200000){ //deboucing de 200 ms
      bool estado = gpio_get(led_green_pin);
      gpio_put(led_green_pin, !estado);
      switch (estado){
        case false: printf("Botao A pressionado! Estado do led verde alterado para desligado (0).\n"); // descricao sobre a operacao no monitor serial
        case true: printf("Botao A pressionado! Estado do led verde alterado para ligado (1).\n");
      }
    }
  } else if(gpio == button_B_pin){
    if(current_time - last_time_B > 200000){ //deboucing de 200 ms
      bool estado2 = gpio_get(led_blue_pin);
      gpio_put(led_blue_pin, !estado2);
      switch (estado2){
        case false: printf("Botao B pressionado! Estado do led azul alterado para desligado (0).\n"); // descricao sobre a operacao no monitor serial
        case true: printf("Botao B pressionado! Estado do led azul alterado para ligado (1).\n");
      }
    }
  }
}

int main()
{
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
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

  bool cor = true;
  while (true)
  {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 10); // Desenha uma string
    ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30); // Desenha uma string
    sdd1360_draw_char_min(&ssd, 'd', 15, 48);   
    sdd1360_draw_char_min(&ssd, 'b', 23, 48);  
    ssd1306_send_data(&ssd); // Atualiza o display

    sleep_ms(1000);
  }
}