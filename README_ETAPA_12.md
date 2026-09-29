# Etapa 12 — Identificacao do Banano VR como runtime OpenXR

O Banano VR agora possui um manifesto OpenXR com o nome **Banano VR Runtime** e um script de registro para aparecer na lista de runtimes disponíveis do Windows.

## Arquivos

- `openxr/banano_vr_runtime.json`
- `openxr/register_banano_runtime.bat`

## Segurança da etapa

O script usa a chave de usuário:

`HKCU\\Software\\Khronos\\OpenXR\\1\\AvailableRuntimes`

Ele **não altera o ActiveRuntime** do Windows. Portanto, instalar/testar esta etapa não troca o runtime OpenXR que já está configurado no PC.

## Importante

O manifesto aponta para a futura DLL `BananoVRRuntime.dll`. Essa DLL ainda será construída nas próximas etapas. Portanto, o Banano ainda não pode executar sessões OpenXR nem ser usado por um jogo VR como HMD nesta etapa.

A documentação do Khronos define que o Windows usa o Registro para localizar o runtime ativo e que runtimes disponíveis podem ser enumerados pela chave `AvailableRuntimes`. O manifesto descreve a biblioteca que o loader deverá carregar. 

## Próxima etapa

**Etapa 13:** criar o primeiro componente real do runtime/HMD, começando pela DLL e pela negociação com o OpenXR Loader.
