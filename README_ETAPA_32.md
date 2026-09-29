# Banano VR — Etapa 32

## Propriedades do sistema OpenXR

A etapa 32 adiciona a consulta de propriedades do sistema identificado na etapa 31.

### Implementado

- `XrSystemProperties`
- nome do sistema: `Banano VR`
- identificador de sistema Banano VR
- propriedades gráficas iniciais
- suporte declarado a orientação e posição
- limite mínimo de camadas de composição
- `xrGetSystemProperties` exposto pelo `xrGetInstanceProcAddr`

A etapa segue o fluxo definido pelo OpenXR: depois de obter um `XrSystemId`, a aplicação pode consultar as propriedades do sistema, incluindo informações gráficas e de tracking.

Esta etapa ainda não cria uma sessão VR nem apresenta frames para jogos.

**Progresso: 32/45.**
