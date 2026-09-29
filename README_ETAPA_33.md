# Banano VR — Etapa 33

## Configuração de visão OpenXR

A etapa 33 adiciona ao runtime a enumeração da configuração de visão suportada pelo Banano VR.

### Implementado

- `xrEnumerateViewConfigurations`
- `xrGetViewConfigurationProperties`
- `xrEnumerateViewConfigurationViews`
- configuração `XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO`
- 2 views: esquerda e direita
- resolução recomendada e máxima inicial de 1024x1024 por olho
- 1 sample por view

O OpenXR permite que a aplicação enumere as configurações de visão suportadas e depois consulte as características de cada view antes de criar/iniciar a sessão. citeturn0search0

Esta etapa ainda não cria uma sessão, swapchain ou entrega frames para um jogo.

**Progresso: 33/45.**
