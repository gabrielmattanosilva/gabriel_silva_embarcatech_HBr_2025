#include "unity.h"                       // Framework para testes unitários
#include "../inc/internal_temperature.h" // Arquivo de cabeçalho com as funções sob teste
#include "pico/stdlib.h"                 // Biblioteca padrão do Raspberry Pi Pico

void setUp(void)
{
    // Configuração inicial antes de cada teste
    stdio_init_all(); // Inicializa comunicação serial USB/UART
    sleep_ms(2000);   // Aguarda 2s para estabilizar conexão serial
    printf("\n=== Starting Temperature Sensor Tests ===\n");
}

void tearDown(void)
{
    // Limpeza após cada teste (opcional)
    // Pode ser usado para resetar hardware entre testes
}

void test_adc_to_celsius(void) {
    // Teste 1: Valor de referência (27°C)
    uint16_t adc_value = 876; // ~0.706V
    float expected_temp = 27.0f;
    float actual_temp = adc_to_celsius(adc_value);
    printf("[TEST 1] ADC=%d -> %.2f°C (Expected: %.2f°C)\n",
           adc_value, actual_temp, expected_temp);
    TEST_ASSERT_FLOAT_WITHIN(0.2f, expected_temp, actual_temp);

    // Teste 2: Verificação com 0.8V
    adc_value = 992; // Valor calculado para 0.8V
    expected_temp = 81.62f; // (0.8V-0.706V)/0.001721 ≈ 54.62 → 27 + 54.62 ≈ 81.62°C
    actual_temp = adc_to_celsius(adc_value);
    printf("[TEST 2] ADC=%d -> %.2f°C (Expected: %.2f°C)\n",
           adc_value, actual_temp, expected_temp);
    TEST_ASSERT_FLOAT_WITHIN(1.0f, expected_temp, actual_temp); // Margem maior
}

int main(void)
{
    // Inicializa o framework de testes
    UNITY_BEGIN();

    // Executa todos os casos de teste
    RUN_TEST(test_adc_to_celsius);

    // Finalização com resumo dos testes
    printf("\n=== Test Results ===\n");
    printf("Total Tests: 2\n");
    printf("Passed: See above\n\n");

    return UNITY_END(); // Retorna o status dos testes (0 = sucesso)
}