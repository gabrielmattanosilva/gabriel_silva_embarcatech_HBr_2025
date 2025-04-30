# Unidade 1 - Tarefa: Monitor da temperatura interna da MCU

## Enunciado:
Faça um programa em C para ler a temperatura interna do RP2040. Converta a leitura do ADC em um valor em °C.

Baseado no datashhet do RP2040 (página 563 - 4.9.5.Temperature Sensor)
https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf

---

# Unidade 1 - Tarefa: Prática - Teste Unitário

## Enunciado:
O ADC interno da Raspberry Pi Pico W pode ser usado para medir a temperatura através de um canal especial (sensor embutido). O valor lido do ADC (12 bits) deve ser convertido para graus Celsius usando a fórmula da documentação oficial:

T = 27 - (ADC_voltage - 0.706)/0.001721

Essa função converte a tensão lida do sensor de temperatura para Celsius, assumindo Vref = 3.3V e resolução de 12 bits.

Tarefa:
1. Implemente a função float adc_to_celsius(uint16_t adc_val);
2. Escreva uma função de teste unitário que verifica se a função retorna o valor correto (com margem de erro) para uma leitura simulada de ADC. Sugere-se o uso da biblioteca Unity para o teste unitário.
3. Use um teste com valor de ADC conhecido (ex.: para 0.706 V, a temperatura deve ser 27 °C).

## Como fazer a build dos Unit Tests:
```bash
cd C:\Users\gabri\OneDrive\Documentos\EmbarcaTech\gabriel_silva_embarcatech_HBr_2025\projetos\temp_interna_RP2040\src
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake -G "Ninja" ..
ninja
