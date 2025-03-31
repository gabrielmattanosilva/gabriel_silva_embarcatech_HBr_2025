#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

/**
 * @file display_oled.h
 * @brief Biblioteca para controle do display OLED SSD1306 via I2C.
 *
 * Esta biblioteca fornece funções para inicializar, limpar e exibir
 * texto no display OLED SSD1306 utilizando comunicação I2C.
 *
 * @note Esta biblioteca foi baseada no projeto disponível em:
 *       https://github.com/BitDogLab/BitDogLab-C/blob/main/display_oled
 * 
 * @author Gabriel Mattano da Silva
 * @date 2025
 */

// Definição dos pinos I2C utilizados para comunicação com o display OLED
#define I2C_SDA 14  ///< Pino GPIO para SDA (dados do I2C)
#define I2C_SCL 15  ///< Pino GPIO para SCL (clock do I2C)

/**
 * @brief Inicializa o display OLED SSD1306.
 *
 * Configura a interface I2C e prepara o display para uso.
 * Deve ser chamada antes de qualquer outra função desta biblioteca.
 */
void display_init(void);

/**
 * @brief Limpa completamente o conteúdo do display OLED.
 */
void display_clear(void);

/**
 * @brief Exibe um conjunto de strings no display OLED.
 *
 * @param text Array de strings a serem exibidas (terminado por NULL).
 * @param y Posição vertical inicial em linhas de 8 pixels (0 para o topo).
 */
void display_text(const char *text[], int y);

#endif // DISPLAY_OLED_H
