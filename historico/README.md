# Histórico

O perseguidor não tem material antigo guardado aqui: a placa encomendada (v4) está em `hardware/placa_base/`, e a v3 ficou no estudo de caso do repositório do seguidor. Esta pasta lista só os branches.

## Branches antigos no GitHub (17/11/2026)
O branch principal é o `main`. Até 17/11 ele estava parado em 15/11 (`7c6843f`, placa encomendada), e o trabalho seguia em `claude/eager-fermi-x961wa`. Em 17/11 o `main` avançou, só por fast-forward, até incluir os dois branches abaixo.

Apagar um branch no GitHub tem volta: a página "Branches" mostra "Restore" logo depois.

Já estão inteiros no `main`, então dá para apagar sem perder nada:
- `claude/eager-fermi-x961wa`: firmware (`firmware/perseguidor/`, simulador e teste do IR), primeira versão de `mecânica/` e das ferramentas 3D, barra V1 de 1,6 mm, regulamento do TRIF 2026 e controle NEC (17/11).
- `claude/elegant-carson-hgloag`: mecânica para imprimir só em PLA e PETG (presilha com assento, placa de sucção com rebaixo, `mecânica/stl/PETG/` e `mecânica/stl/PLA/`) e esta reorganização (17/11).

Não há branch com algo que falte no `main`.

No repositório do seguidor há mais dois branches com versões antigas do perseguidor (`ccr-0f7dfdf7-6w0a8n`, de 11/11, e `claude/bold-carson-hbu2ra`, de 15/11). Os dois foram superados por este repositório e estão listados no [`historico/README.md` do seguidor](https://github.com/therickmatos/Seguidor-de-linha/blob/main/historico/README.md).
