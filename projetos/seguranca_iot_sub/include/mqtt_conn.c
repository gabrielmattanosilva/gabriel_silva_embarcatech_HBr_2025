#include "lwip/apps/mqtt.h"    // Biblioteca MQTT do lwIP
#include "include/mqtt_conn.h" // Header file com as declarações locais
// Base: https://github.com/BitDogLab/BitDogLab-C/blob/main/wifi_button_and_led/lwipopts.h
#include "lwipopts.h" // Configurações customizadas do lwIP

/* Variável global estática para armazenar a instância do cliente MQTT
 * 'static' limita o escopo deste arquivo */
static mqtt_client_t *client;

/* Variável global para armazenar o último timestamp recebido */
static uint32_t ultima_timestamp_recebida = 0;

/* Callback de conexão MQTT - chamado quando o status da conexão muda
 * Parâmetros:
 *   - client: instância do cliente MQTT
 *   - arg: argumento opcional (não usado aqui)
 *   - status: resultado da tentativa de conexão */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if (status == MQTT_CONNECT_ACCEPTED)
    {
        printf("Conectado ao broker MQTT com sucesso!\n");
    }
    else
    {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

/* Função para configurar e iniciar a conexão MQTT
 * Parâmetros:
 *   - client_id: identificador único para este cliente
 *   - broker_ip: endereço IP do broker como string (ex: "192.168.1.1")
 *   - user: nome de usuário para autenticação (pode ser NULL)
 *   - pass: senha para autenticação (pode ser NULL) */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass)
{
    ip_addr_t broker_addr; // Estrutura para armazenar o IP do broker

    // Converte o IP de string para formato numérico
    if (!ip4addr_aton(broker_ip, &broker_addr))
    {
        printf("Erro no IP\n");
        return;
    }

    // Cria uma nova instância do cliente MQTT
    client = mqtt_client_new();
    if (client == NULL)
    {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }

    // Configura as informações de conexão do cliente
    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id, // ID do cliente
        .client_user = user,    // Usuário (opcional)
        .client_pass = pass     // Senha (opcional)
    };

    // Inicia a conexão com o broker
    // Parâmetros:
    //   - client: instância do cliente
    //   - &broker_addr: endereço do broker
    //   - 1883: porta padrão MQTT
    //   - mqtt_connection_cb: callback de status
    //   - NULL: argumento opcional para o callback
    //   - &ci: informações de conexão
    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

/* Callback de confirmação de publicação
 * Chamado quando o broker confirma recebimento da mensagem (para QoS > 0)
 * Parâmetros:
 *   - arg: argumento opcional
 *   - result: código de resultado da operação */
static void mqtt_pub_request_cb(void *arg, err_t result)
{
    if (result == ERR_OK)
    {
        printf("Publicação MQTT enviada com sucesso!\n");
    }
    else
    {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

/* Função para publicar dados em um tópico MQTT
 * Parâmetros:
 *   - topic: nome do tópico (ex: "sensor/temperatura")
 *   - data: payload da mensagem (bytes)
 *   - len: tamanho do payload */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len)
{
    // Envia a mensagem MQTT
    err_t status = mqtt_publish(
        client,              // Instância do cliente
        topic,               // Tópico de publicação
        data,                // Dados a serem enviados
        len,                 // Tamanho dos dados
        0,                   // QoS 0 (nenhuma confirmação)
        0,                   // Não reter mensagem
        mqtt_pub_request_cb, // Callback de confirmação
        NULL                 // Argumento para o callback
    );

    if (status != ERR_OK)
    {
        printf("mqtt_publish falhou ao ser enviada: %d\n", status);
    }
}

/**
 * Função para aplicar cifra XOR (criptografia/decifração)
 *
 * @param input  Ponteiro para os dados de entrada (texto claro ou cifrado)
 * @param output Ponteiro para armazenar o resultado (deve ter tamanho >= len)
 * @param len    Tamanho dos dados em bytes
 * @param key    Chave de 1 byte (0-255) para operação XOR
 *
 * Funcionamento:
 * - Aplica operação XOR bit-a-bit entre cada byte do input e a chave
 * - XOR é reversível: mesma função para cifrar e decifrar
 * - Criptografia fraca (apenas para fins didáticos ou ofuscação básica)
 */
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key)
{
    // Loop por todos os bytes dos dados de entrada
    for (size_t i = 0; i < len; ++i)
    {
        // Operação XOR entre o byte atual e a chave
        // Armazena resultado no buffer de saída
        output[i] = input[i] ^ key;
    }
}

/* Callback para requisição de subscrição */
static void mqtt_sub_request_cb(void *arg, err_t result)
{
    if (result == ERR_OK)
    {
        printf("Subscrição realizada com sucesso!\n");
    }
    else
    {
        printf("Erro na subscrição: %d\n", result);
    }
}

/* Callback para mensagens recebidas */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("Mensagem recebida no tópico: %s\n", topic);
}

/* Callback para dados recebidos */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    // Buffer para armazenar os dados decodificados
    uint8_t decodificado[256];
    
    // Decodifica os dados usando XOR com a chaveb42
    xor_encrypt(data, decodificado, len, 42);
    
    printf("Dados recebidos (criptografados): %.*s\n", len, (const char *)data);
    printf("Dados decodificados: %.*s\n", len, (const char *)decodificado);
    
    // 1. Parse da mensagem JSON
    uint32_t nova_timestamp;
    float valor;
    if (sscanf((const char *)decodificado, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_timestamp) != 2)
    {
        printf("Erro no parse da mensagem!\n");
        return;
    }
    
    // 2. Verificação de replay
    if (nova_timestamp > ultima_timestamp_recebida)
    {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Nova leitura válida: %.2f (ts: %lu)\n", valor, nova_timestamp);
    }
    else
    {
        printf("Replay detectado (ts: %lu <= %lu)\n", nova_timestamp, ultima_timestamp_recebida);
    }
}

/* Função para subscrever a um tópico */
void mqtt_comm_subscribe(const char *topic)
{
    // Configura os callbacks de publicação e dados
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);
    
    // Subscrição com o callback de requisição
    err_t err = mqtt_subscribe(client, topic, 0, mqtt_sub_request_cb, NULL);
    
    if (err != ERR_OK)
    {
        printf("Erro ao subscrever no tópico %s: %d\n", topic, err);
    }
}