# Projeto: Controle de Display e Matriz de LEDs com Raspberry Pi Pico

## LINK DO VIDEO: https://drive.google.com/file/d/1x_BXIf7581uKFOMA5eVatQO7PwyA-5uO

OBS: Nao foi mostrado no video mas assim como pede a atv. no monitor serial mostra as chegadas dos caracteres tambem.
## Descrição do Projeto

Este projeto criado por Dorivaldo Jesus da Silva Jr, que implementa um sistema de controle de um display OLED SSD1306 e uma matriz de LEDs utilizando a Raspberry Pi Pico. O sistema permite a interação através de dois botões físicos, LEDs indicadores e entrada de dados via comunicação serial.

O projeto foi desenvolvido utilizando a linguagem C e o SDK do Raspberry Pi Pico, utilizando os periféricos de hardware GPIO, I2C e PIO.

## Funcionalidades

- **Controle de Display OLED SSD1306**: Exibição de caracteres recebidos via comunicação serial e status dos LEDs.
- **Controle da Matriz de LEDs**: Exibição de números de 0 a 9 na matriz, acionados por entrada serial.
- **Controle de LEDs indicadores**: Dois LEDs (azul e verde) podem ser ligados e desligados pelos botões A e B.
- **Interrupção por botões**: O estado dos LEDs é alterado ao pressionar os botões, refletindo no display OLED.
- **Comunicação Serial**: Captura caracteres digitados no terminal serial e os exibe no display.

## Componentes Utilizados

- **Raspberry Pi Pico**
- **Display OLED SSD1306 (comunicação I2C)**
- **Matriz de LEDs 5x5**
- **Dois botões físicos**
- **Dois LEDs indicadores (verde e azul)**

## Mapeamento de Pinos

| Componente        | Pino Raspberry Pi Pico |
|------------------|----------------------|
| SDA (I2C)       | GPIO 14 |
| SCL (I2C)       | GPIO 15 |
| Botão A        | GPIO 5  |
| Botão B        | GPIO 6  |
| LED Verde       | GPIO 11 |
| LED Azul        | GPIO 12 |
| Matriz de LEDs  | GPIO 7  |

## Estrutura do Código

O código é dividido em várias seções:

- **Inicialização de Periféricos**: Configura GPIOs para botões e LEDs.
- **Interrupções dos botões**: Callback que altera o estado dos LEDs e exibe no display.
- **Controle da Matriz de LEDs**: Função que recebe números e os exibe na matriz de LEDs.
- **Loop Principal**: Aguarda entrada serial para exibir caracteres no display e alterar o conteúdo da matriz de LEDs.

## Como Usar

1. **Conecte a Raspberry Pi Pico** à fonte de alimentação e ao computador via USB.
2. **Compile e carregue o código** na Pico utilizando o SDK do Raspberry Pi.
3. **Interaja com o sistema**:
   - Pressione os botões físicos para ligar/desligar os LEDs.
   - Digite caracteres na comunicação serial para exibi-los no display.
   - Digite números de 0 a 9 para exibi-los na matriz de LEDs.

## Licença

Este projeto é de código aberto e pode ser utilizado livremente para fins educacionais e de pesquisa.

