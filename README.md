# Banano VR PC

## Etapa 6

Primeiro visualizador e rastreador de camera do Banano VR.

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
- Visualizador simples da camera no aplicativo.
- Botao para iniciar/parar a camera.
- Deteccao inicial por cor usando a tolerancia configurada.
- O programa procura Azul, Vermelho, Amarelo, Verde, Roxo, Laranja e Branco.
- A tela mostra quantos marcadores coloridos foram encontrados.
- O processamento usa amostragem leve para manter a etapa simples e sem bibliotecas externas de visao computacional.
- A calibracao da etapa 5 continua controlando a tolerancia de cor.

### Importante
Esta ainda e uma primeira camada de deteccao. Ela nao faz tracking de mao, nao envia dados para o celular e ainda nao transforma a deteccao em controles VR reais. A proxima evolucao pode adicionar a visualizacao dos pontos/bolas detectados e a identificacao por ID/posicao.
