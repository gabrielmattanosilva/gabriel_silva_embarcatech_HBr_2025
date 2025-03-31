# Obter Chave da API CallMeBot

Siga o tutorial: https://www.callmebot.com/blog/free-api-whatsapp-messages/

# Compilação do Projeto no Pico SDK

Para compilar o projeto utilizando o Pico SDK, é necessário criar um arquivo `credentials.h` no diretório "inc". Esse arquivo deve conter o número de telefone do estinatário das mensagens, a chave da API CallMeBot e as credenciais da sua rede Wi-Fi.

## Passos para Configuração

1. No diretório "inc" do projeto, crie um arquivo chamado `credentials.h`.
2. Dentro desse arquivo, substitua `nome_da_rede` e `senha` pelas informações da sua rede Wi-Fi, `+5500000000000` pelo número de telefone do destinatário e `0000000` pela chave da API CallMeBot:

   ```bash
   #ifndef CREDENTIALS_H
   #define CREDENTIALS_H

   #define SSID "nome_da_rede"
   #define PASSWORD "senha" 
   #define PHONE_NUMBER "+5500000000000"
   #define API_KEY "0000000"

   #endif // CREDENTIALS_H

3. Certifique-se de que o arquivo `credentials.h` está listado no seu .gitignore para que suas credenciais não sejam enviadas para o repositório Git.

Após criar o arquivo com as credenciais, você pode compilar o projeto normalmente utilizando o Pico SDK.