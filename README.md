# Banano VR PC

## Etapa 8

Overlay visual simples para representar o tracking dos marcadores.

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
- Cada deteccao continua sendo identificada pelo sistema de ID da etapa 7.
- Um pequeno circulo branco aparece sobre cada marcador detectado.
- O circulo acompanha a posicao suavizada do marcador.
- Quando o ID e reconhecido, o numero do marcador aparece dentro do circulo.
- Quando o ID nao e reconhecido, aparece `?`.
- O overlay fica sobre a imagem da camera sem desenhar esqueleto, pontos ou linhas de mao.
- A calibracao, a deteccao por cor e o mapeamento de controles continuam funcionando.

### Importante
Esta etapa ainda nao implementa rastreamento de mao. O circulo branco representa somente a posicao atual do marcador detectado. O proximo nivel pode usar essa base visual para integrar a mao/finger tracking sem poluir a tela com landmarks.
