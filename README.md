# RUSH - Dreamcast: perseguidor

Placa do robô perseguidor, irmã do seguidor [RUSH - SATURN](https://github.com/therickmatos/Seguidor-de-linha), encomendada na JLC junto com ele em 15/11/2026.
As duas placas compartilham o circuito, que é baseado no [EVA-RT](https://github.com/EXOTIC-TEAM-MX/EVA-RT) (© EXOTIC TEAM MX, AGPL-3.0).

- 99,8 × 58,4 mm, sem as abas dos sensores de marca do Saturn
- ATmega328P-AU, 2× DRV8874PWPR (motores coreless 1020, ITRIP ≈ 4,9 A), AO3400A na ventoinha
- Receptor IR KY-022 (J6), para o módulo de início
- LiPo 2S (XT30), 78M05

## Onde encontrar
| Para… | Abra |
|---|---|
| ver a placa encomendada (Gerber, `.epro`, o que foi pedido à JLC) | [`hardware/placa_base/`](hardware/placa_base/README.md) |
| comprar e soldar os componentes | [`hardware/placa_base/BOM_v4.csv`](hardware/placa_base/BOM_v4.csv) |
| imprimir as peças (STL e material de cada uma: PLA ou PETG) | [`mecânica/stl/`](mecânica/stl/) e [`mecânica/README.md`](mecânica/README.md#o-que-imprimir) |
| comprar parafusos, varetas e rolamentos | [`mecânica/README.md`, "Ferragens"](mecânica/README.md#ferragens) |
| montar o robô | [`mecânica/README.md`, "Ordem de montagem"](mecânica/README.md#ordem-de-montagem) |
| gravar e ajustar o firmware | [`firmware/README.md`](firmware/README.md#gravar) |
| configurar o módulo de início IR (controle NEC ou RC5, KY-022 na J6) | [`firmware/README.md`, "Módulo de início (IR)"](firmware/README.md#módulo-de-início-ir) |
| ler o regulamento | [`referencias/TRIF 2026 - Perseguidor de Linha.pdf`](referencias/TRIF%202026%20-%20Perseguidor%20de%20Linha.pdf) |
| consultar o projeto original | [`referencias/eva-rt/`](referencias/README.md) |

## Estado
- [x] Layout no EasyEDA Pro (v4)
- [x] Gerber conferido e **placa encomendada na JLC (15/11/2026)**
- [x] Firmware (`firmware/perseguidor/`, adaptado do seguidor; compila e passa no simulador, falta testar no robô)
- [x] Peças para imprimir em PLA e PETG e guia de montagem (`mecânica/`, para base e barra de 1,6 mm; conferido só em 3D)
- [ ] Montagem
- [ ] Teste do sentido dos motores e da ventoinha
- [ ] Ajuste na pista

## Estrutura
Em uso na montagem e na pista:

| Pasta | Conteúdo |
|---|---|
| [`hardware/`](hardware/placa_base/README.md) | `placa_base/`: **placa encomendada** (`.epro` do v4, Gerber, BOM). `barra_v1/`: Gerber da barra de sensores (12 QRE1113, 1,6 mm), a mesma do seguidor |
| [`mecânica/`](mecânica/README.md) | STL para imprimir (`mecânica/stl/PETG/` e `mecânica/stl/PLA/`), material, parafusos, ordem de montagem e conferência 3D (base e barra de 1,6 mm) |
| [`firmware/`](firmware/README.md) | `perseguidor/` (firmware do robô, com largada e parada pelo módulo de início IR), `testes/` (simulador do controle e teste do IR no PC) |

Para gerar arquivos e para consulta:

| Pasta | Conteúdo |
|---|---|
| [`ferramentas/`](ferramentas/README.md) | `pecas_3d/` (gera os STL de `mecânica/` e faz a conferência 3D) e `bom_epro.py` (BOM a partir do `.epro`) |
| [`referencias/`](referencias/README.md) | regulamento do TRIF 2026 e a parte do EVA-RT que as ferramentas leem (STL e Gerber da barra VISION) |
| [`historico/`](historico/README.md) | branches antigos no GitHub e o que cada um tem |

O seguidor (placa "RUSH - SATURN") está em [therickmatos/Seguidor-de-linha](https://github.com/therickmatos/Seguidor-de-linha).

## Licença
GNU Affero General Public License v3.0 (`LICENSE`), a mesma do EVA-RT.
