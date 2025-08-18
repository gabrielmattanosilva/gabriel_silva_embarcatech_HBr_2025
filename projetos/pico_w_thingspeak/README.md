# Criar Conta e Configurar ThingSpeak

https://thingspeak.mathworks.com/

# Compilação do Projeto no Pico SDK

Para compilar o projeto utilizando o Pico SDK, é necessário criar um arquivo `credentials.h` no diretório "inc". Esse arquivo deve conter o número de telefone do estinatário das mensagens, a chave da API CallMeBot e as credenciais da sua rede Wi-Fi.

## Passos para Configuração

1. No diretório "inc" do projeto, crie um arquivo chamado `credentials.h`.
2. Dentro desse arquivo, substitua `nome_da_rede` e `senha` pelas informações da sua rede Wi-Fi e `ABCDEFG123456789` pela chave da API de escrita do ThingSpeak:

   ```bash
   #ifndef CREDENTIALS_H
   #define CREDENTIALS_H

   #define SSID "nome_da_rede"
   #define PASSWORD "senha"
   #define API_KEY "ABCDEFG123456789"

   #endif // CREDENTIALS_H

3. Certifique-se de que o arquivo `credentials.h` está listado no seu .gitignore para que suas credenciais não sejam enviadas para o repositório Git.

Após criar o arquivo com as credenciais, você pode compilar o projeto normalmente utilizando o Pico SDK.