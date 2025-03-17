# ESP32-AquaNet

## Visão Geral

O **ESP32-AquaNet** é o componente de hardware do ecossistema do AQUA Project, desenvolvido para monitorar e coletar dados em tempo real de aquários. Este repositório contém o firmware e as especificações de hardware para o sistema de monitoramento baseado em ESP32, integrado ao [Aplicativo Web AQUA](https://github.com/gabethealigator/aqua-project).

## Objetivo

Este projeto foi desenvolvido como parte de um **TCC** (Trabalho de Conclusão de Curso) com o intuito de criar uma solução completa para entusiastas e profissionais de aquários. O ESP32-AquaNet atua como o módulo de coleta e transmissão de dados, fornecendo informações ao principal aplicativo web do AQUA Project.

## Funcionalidades

- **Monitoramento em Tempo Real**: Coleta dados simultaneamente de múltiplos sensores
- **Conectividade Sem Fio**: Transmite os dados para o aplicativo web via Wi-Fi
- **Baixo Consumo de Energia**: Otimizado para operação prolongada
- **Suporte a Múltiplos Sensores**:
  - Sensor de pH
  - Sensor de temperatura da água
  - Sensor de turbidez
  - Sensor de nível da água
- **Transmissão Automática de Dados**: Envia regularmente os dados para o aplicativo web
- **Operação Autônoma**: Funciona de forma independente com uma configuração mínima

## Requisitos de Hardware

- Placa com microcontrolador ESP32
- Módulo sensor de pH
- Sensor de temperatura DS18B20 à prova d'água
- Sensor de turbidez
- Sensor de nível da água
- Protoboard ou PCB para montagem
- Fios de conexão (jumper wires)
- Fonte de alimentação (USB ou bateria)
- Caixa à prova d'água (recomendado)

## Requisitos de Software

- Arduino IDE ou PlatformIO
- Suporte para placas ESP32 na IDE escolhida
- Bibliotecas necessárias:
  - WiFi.h
  - HTTPClient.h
  - ArduinoJson.h
  - OneWire.h
  - DallasTemperature.h
  - (Outras bibliotecas específicas para cada sensor)

## Instalação e Configuração

### Configuração do Ambiente de Desenvolvimento

1. Instale o Arduino IDE ou PlatformIO.
2. Adicione o suporte para placas ESP32 na sua IDE.
3. Instale as bibliotecas necessárias através do gerenciador de bibliotecas.

### Montagem do Hardware

1. Conecte os sensores ao ESP32 conforme a configuração de pinos definida no código.
2. Garanta uma fonte de alimentação adequada para todos os componentes.
3. Posicione os sensores em locais apropriados dentro do aquário.

### Instalação do Firmware

1. Clone este repositório:
   ```
   git clone https://github.com/yourusername/esp32-aquanet.git
   cd esp32-aquanet
   ```

2. Abra o projeto na sua IDE.

3. Configure as credenciais de Wi-Fi e as informações do servidor no arquivo `config.h`:
   ```cpp
   // Substitua pelas credenciais da sua rede Wi-Fi
   #define WIFI_SSID "YourWiFiSSID"
   #define WIFI_PASSWORD "YourWiFiPassword"

   // Substitua pela URL do seu servidor do AQUA Project
   #define SERVER_URL "http://your-aqua-server.com/api/readings"
   ```

4. Faça o upload do firmware para a placa ESP32.

## Integração com o Aplicativo Web AQUA

Este componente de hardware foi projetado para funcionar de forma integrada com o [Aplicativo Web AQUA](https://github.com/gabethealigator/aqua-project). O ESP32-AquaNet coleta os dados dos sensores e os transmite para o aplicativo web, onde podem ser visualizados, analisados e utilizados para o envio de notificações.

Para configurar o sistema completo:

1. Configure o Aplicativo Web AQUA conforme as instruções do seu repositório.
2. Ajuste o ESP32-AquaNet com a URL correta do servidor.
3. Registre o dispositivo no aplicativo web.
4. Comece a monitorar o seu aquário em tempo real.

## Diagrama de Circuito

Um diagrama detalhado de circuito está disponível no diretório `docs/`, demonstrando como conectar todos os sensores ao microcontrolador ESP32.

## Estrutura do Código

- `esp32-aquanet.ino`: Arquivo principal do sketch Arduino.
- `config.h`: Arquivo com as configurações de Wi-Fi e do servidor.
- `sensors.h`: Funções de inicialização e leitura dos sensores.
- `network.h`: Funções de conexão de rede e transmissão de dados.

## Solução de Problemas

- **Problemas na Leitura dos Sensores**: Certifique-se de calibrar corretamente os sensores antes da implantação.
- **Problemas de Conexão**: Verifique as credenciais do Wi-Fi e a URL do servidor.
- **Problemas de Alimentação**: Cheque a tensão e a capacidade da fonte de alimentação.
- **Erros na Transmissão de Dados**: Garanta que o formato dos dados enviados seja compatível com o que o servidor espera.

## Melhorias Futuras

- Adicionar suporte para tipos adicionais de sensores.
- Implementar armazenamento local para operação offline.
- Desenvolver recursos de gerenciamento de energia para operação com bateria.
- Criar um aplicativo móvel para conexão direta com o dispositivo.
- Adicionar recursos de calibração automática dos sensores.

## Contribuindo

Contribuições para aprimorar o ESP32-AquaNet são muito bem-vindas! Sinta-se à vontade para enviar um Pull Request.

## Agradecimentos

- Agradecimento especial a todos os colaboradores e orientadores que apoiaram o desenvolvimento deste projeto de TCC.
- Gratidão às comunidades ESP32 e Arduino, pelas excelentes documentações e suporte.

## Projetos Relacionados

- [Aplicativo Web AQUA](https://github.com/gabethealigator/aqua-project) - Componente web deste ecossistema
