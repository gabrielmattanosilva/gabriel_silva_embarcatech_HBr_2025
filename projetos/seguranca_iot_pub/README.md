# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: **Gabriel Mattano da Silva**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, 19 de Maio de 2025

---

Roteiro Prático – Segurança em IoT com BitDogLab (C/C++)

Código para a BitDogLab publisher.

## Passos para Configuração

1. No diretório "include" do projeto, crie um arquivo chamado `credentials.h`.
2. Dentro desse arquivo, substitua `nome_wifi` e `senha_wifi` pelas informações da sua rede Wi-Fi, `usuario_broker` e `senha_broker` pelas informações do seu broker MQTT:

    ```bash
    #ifndef CREDENTIALS_H
    #define CREDENTIALS_H

    #define WIFI_SSID       "nome_wifi"
    #define WIFI_PASS       "senha_wifi"
    #define BROKER_USER     "usuario_broker"
    #define BROKER_PASS     "senha_broker"

    #endif // CREDENTIALS_H

3. Certifique-se de que o arquivo `credentials.h` está listado no seu .gitignore para que suas credenciais não sejam enviadas para o repositório Git.

Após criar o arquivo com as credenciais, você pode compilar o projeto normalmente.

---

## 📜 Licença
MIT License - MIT GPL-3.0.