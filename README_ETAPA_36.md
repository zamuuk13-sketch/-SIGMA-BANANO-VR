# Banano VR — Etapa 36

## Primeiro frame loop OpenXR

A etapa 36 adiciona as primeiras funções do ciclo de frames do runtime.

### Implementado

- `XrFrameWaitInfo`
- `XrFrameState`
- `XrFrameBeginInfo`
- `xrWaitFrame`
- `xrBeginFrame`
- tempo previsto de apresentação
- período de frame inicial de aproximadamente 60 Hz
- indicação de `shouldRender`
- validação de sessão em execução

O OpenXR utiliza `xrWaitFrame`, `xrBeginFrame` e `xrEndFrame` para sincronizar o ciclo de renderização com o runtime. `xrWaitFrame` fornece o tempo previsto de apresentação e `xrBeginFrame` marca o início do trabalho de renderização. citeturn0search0turn0search1

Nesta etapa, `xrEndFrame` e a composição de imagens ainda não estão implementados. Portanto, o runtime ainda não apresenta um frame visual real ao jogo.

**Progresso: 36/45.**
