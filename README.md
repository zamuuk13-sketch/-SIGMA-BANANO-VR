# Banano VR PC

## Etapa 7

Identificacao e estabilizacao inicial dos marcadores detectados.

### Requisitos
- Windows
- MinGW-w64 com `g++` no PATH
- Webcam/camera disponivel no Windows

### Compilar
Execute:

`build.bat`

O executavel sera criado em:

`build/BananoVR.exe`

### Nesta etapa
- Cada deteccao tenta encontrar seu marcador cadastrado pelo ID.
- A cor continua sendo usada como primeira identificacao.
- A posicao X/Y cadastrada ajuda a diferenciar dois marcadores da mesma cor.
- O sistema aplica uma suavizacao simples para reduzir pequenas tremidas entre frames.
- A tela informa quantos marcadores foram encontrados e quantos receberam ID.
- A camera e a calibracao da etapa anterior continuam funcionando.

### Importante
O sistema ainda nao faz rastreamento de mao nem envia controles para jogos. Esta etapa prepara a identificacao estavel dos marcadores para o proximo nivel de tracking.
