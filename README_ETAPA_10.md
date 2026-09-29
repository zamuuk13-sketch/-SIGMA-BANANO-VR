# Etapa 10 — Arquitetura do VR Runtime

Nesta etapa o Banano VR PC ganha a primeira camada separada do futuro runtime de realidade virtual.

## Objetivo

Preparar uma arquitetura em que:

**Jogo VR → OpenXR → runtime/ponte Banano VR → HMD + controles virtuais**

A escolha inicial de backend fica preparada para **SteamVR / OpenXR**, mantendo a camada Banano separada da captura da câmera e do futuro transporte com o celular.

## O que foi criado

- `src/vr_runtime.h`
  - estados do runtime;
  - identificação do backend;
  - informações básicas do runtime.
- `src/vr_runtime.cpp`
  - inicialização da camada;
  - estado `Base pronta`;
  - backend inicial `SteamVR / OpenXR`.

## Importante

A Etapa 10 **não finge que um HMD virtual já existe**. Ela cria a fundação do runtime.

OpenXR trabalha com um loader que encontra um runtime ativo; um runtime OpenXR é o componente que controla o sistema VR completo.

Por isso, as próximas etapas vão implementar a ponte/dispositivo de forma progressiva, em vez de tentar resolver HMD, poses, controladores e vídeo em uma única etapa.

## Próxima etapa

**Etapa 11:** começar a camada real de integração com OpenXR/SteamVR.
