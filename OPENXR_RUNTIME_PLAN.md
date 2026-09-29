# Etapa 11 — Ponte OpenXR

A Etapa 11 prepara o ponto de integracao entre o runtime Banano VR e o ecossistema OpenXR.

O OpenXR Loader no Windows encontra o runtime ativo por meio do Registro e carrega o manifesto JSON correspondente. O manifesto aponta para a biblioteca do runtime. A biblioteca precisa negociar a interface com o loader por `xrNegotiateLoaderRuntimeInterface`.

No Banano VR, esta etapa cria somente a interface interna e o ponto de entrada para essa integracao. Ainda nao registramos um runtime no Windows nem afirmamos que jogos ja detectam um HMD.

A proxima etapa pode implementar o primeiro componente real da ponte, mantendo a arquitetura separada do tracking da camera.

Fonte: Khronos OpenXR Loader specification — https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/main/specification/loader/runtime.adoc
