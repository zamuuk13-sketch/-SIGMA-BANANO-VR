# Etapa 13 — Primeira DLL real do Banano VR Runtime

O Banano VR agora possui uma DLL separada para o runtime OpenXR:

`build/BananoVRRuntime.dll`

## O que esta etapa implementa

- DLL compartilhada compilada pelo `build.bat`;
- exportação de `xrNegotiateLoaderRuntimeInterface`;
- validação básica dos dados enviados pelo OpenXR Loader;
- negociação da interface de runtime versão 1;
- função inicial `getInstanceProcAddr`.

A documentação oficial do Khronos define que o runtime deve exportar diretamente `xrNegotiateLoaderRuntimeInterface` e que o loader usa essa função para negociar a interface runtime. citeturn0search0

## Limitação intencional

Esta ainda é uma **DLL de bootstrap**. O `getInstanceProcAddr` não expõe as funções OpenXR de uma sessão real e retorna `XR_ERROR_FUNCTION_UNSUPPORTED`.

Portanto:

**Etapa 13:** DLL + negociação inicial ✅  
**HMD virtual funcional:** ainda não  
**Jogos VR detectando HMD:** ainda não

Isso evita registrar uma DLL que finja suportar uma sessão VR quando ela ainda não implementa os comandos necessários.

## Próxima etapa

**Etapa 14:** criar a primeira implementação funcional de `xrGetInstanceProcAddr` e preparar a criação de uma instância OpenXR mínima.
