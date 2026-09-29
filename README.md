# Banano VR PC

## Etapa 5

Sistema inicial de calibracao para os marcadores.

### Requisitos
- Windows
- MinGW-w64 com `g++` no PATH

### Compilar
Execute:

`build.bat`

O executavel sera criado em:

`build/BananoVR.exe`

### Nesta etapa
- Tolerancia de cor configuravel de 0 a 100.
- Tolerancia de posicao configuravel de 0 a 100.
- Botao para aplicar a calibracao.
- A tolerancia de cor sera usada posteriormente para reconhecer cores aproximadas, sem exigir RGB exato.
- A tolerancia de posicao sera usada posteriormente para ajudar a diferenciar marcadores da mesma cor pela posicao/layout.
- O cadastro e o mapeamento dos marcadores da etapa anterior continuam funcionando.

A camera ainda nao e processada nesta etapa. O reconhecimento real dos marcadores sera adicionado na etapa de tracking.
