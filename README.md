# RUSH - Dreamcast: perseguidor

Placa do robô perseguidor, irmã do seguidor [RUSH - SATURN](https://github.com/therickmatos/Seguidor-de-linha), encomendada na JLC junto com ele em 15/11/2026.
As duas placas compartilham o circuito, que é baseado no [EVA-RT](https://github.com/EXOTIC-TEAM-MX/EVA-RT) (© EXOTIC TEAM MX, AGPL-3.0).

- 99,8 × 58,4 mm, sem as abas dos sensores de marca do Saturn
- ATmega328P-AU, 2× DRV8874PWPR (motores coreless 1020, ITRIP ≈ 4,9 A), AO3400A na ventoinha
- Receptor IR KY-022 (J6)
- LiPo 2S (XT30), 78M05

## Estado
- [x] Layout no EasyEDA Pro (v4)
- [x] Gerber conferido e **placa encomendada na JLC (15/11/2026)**
- [x] Firmware (`firmware/perseguidor/`, adaptado do seguidor; compila e passa no simulador, falta testar no robô)
- [x] Peças para imprimir e guia de montagem (`mecânica/`; conferido só em 3D)
- [ ] Montagem
- [ ] Teste do sentido dos motores e da ventoinha
- [ ] Ajuste na pista

## Estrutura
| Pasta | Conteúdo |
|---|---|
| [`hardware/placa_base/`](hardware/placa_base/README.md) | **placa encomendada**: `.epro` do v4, Gerber, BOM |
| [`hardware/barra_v1/`](hardware/barra_v1/README.md) | Gerber da barra de sensores V1 (a mesma do seguidor) |
| [`firmware/`](firmware/README.md) | firmware do robô (largada e parada pelo módulo de início IR), simulador e teste do IR no PC |
| [`mecânica/`](mecânica/README.md) | STL para imprimir (presilha para a base de 1,6 mm, patim da V1, peças do EVA-RT), parafusos, montagem e conferência 3D |
| [`ferramentas/`](ferramentas/README.md) | `bom_epro.py` (BOM a partir do `.epro`), `pecas_3d/` (gera os STL e faz a conferência 3D) |
| `referencias/` | STL e Gerber da barra VISION do EVA-RT usados pelas ferramentas |

## Licença
GNU Affero General Public License v3.0 (`LICENSE`), a mesma do EVA-RT.
