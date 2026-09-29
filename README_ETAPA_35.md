# Banano VR — Etapa 35

## Início da sessão OpenXR

A etapa 35 adiciona o início do ciclo de vida da sessão Banano VR.

### Implementado

- `XrSessionBeginInfo`
- estados básicos da sessão
- `xrBeginSession`
- validação da configuração `PRIMARY_STEREO`
- proteção contra iniciar a mesma sessão duas vezes
- estado interno `running`

Após uma sessão ser iniciada com sucesso, o OpenXR considera a sessão em execução e a aplicação pode prosseguir para o frame loop com `xrWaitFrame`, `xrBeginFrame` e `xrEndFrame`. citeturn0search0turn0search1

O Banano ainda não implementa o frame loop nesta etapa e ainda não apresenta frames ao usuário.

**Progresso: 35/45.**
