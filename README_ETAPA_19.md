# Banano VR — Etapa 19

## Apresentacao stereo preparada

A etapa 19 prepara a camada do runtime para trabalhar com as duas views do HMD.

- 2 views stereo: esquerda e direita.
- Quantidade de views centralizada no runtime.
- Parametros recomendados iniciais por olho: 1024x1024.
- A estrutura fica preparada para a futura etapa de apresentacao/renderizacao.

Esta etapa ainda nao entrega frames para um jogo. O OpenXR utiliza `xrLocateViews` para fornecer as poses/projecoes das views e uma camada de composicao para apresentar o resultado. citeturn0search0turn0search2

**Progresso: 19/45.**
