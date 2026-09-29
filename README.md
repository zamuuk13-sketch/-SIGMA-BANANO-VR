# Banano VR PC

## Etapa 9

Primeira camada de hand tracking baseada na imagem da webcam.

### Nesta etapa
- Detecta uma regiao aproximada da mao pela imagem da camera.
- Calcula o centro aproximado da mao.
- Estima uma ponta de dedo pela extremidade mais distante do centro.
- Mostra uma bolinha branca na ponta estimada.
- Mostra um contorno azul discreto da regiao detectada.
- Mantem os marcadores e o tracking da etapa 8.
- Nao mostra landmarks internos.

### Importante
Esta e uma primeira camada de hand tracking, ainda nao um rastreador profissional de dedos. A arquitetura foi mantida leve para permitir refinamento nas proximas etapas.