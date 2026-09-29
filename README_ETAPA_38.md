# Banano VR — Etapa 38

## Swapchain OpenXR

A etapa 38 cria a camada inicial de swapchain do Banano VR.

### Implementado

- `XrSwapchain`
- `XrSwapchainCreateInfo`
- `xrCreateSwapchain`
- `xrEnumerateSwapchainImages`
- formato inicial RGBA 8-bit
- largura e altura configuráveis
- três imagens lógicas por swapchain
- validação básica de sessão, dimensões e formato

No OpenXR, swapchains são coleções de imagens fornecidas pelo runtime para que a aplicação renderize e depois envie essas imagens na composição. A especificação também define que a quantidade de imagens é determinada pelo runtime e é consultada com `xrEnumerateSwapchainImages`. citeturn0search0turn0search1turn0search3

Nesta etapa, as imagens ainda são **lógicas**: os recursos reais de GPU/API gráfica ainda não foram conectados. Portanto, isto ainda não significa que o jogo consiga renderizar uma textura real no Banano.

**Progresso: 38/45.**
