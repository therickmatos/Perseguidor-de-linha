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
- [ ] Montagem
- [ ] Teste do sentido dos motores e da ventoinha
- [ ] Firmware

## Estrutura
| Pasta | Conteúdo |
|---|---|
| [`hardware/placa_base/`](hardware/placa_base/README.md) | **placa encomendada**: `.epro` do v4, Gerber, BOM |
| `ferramentas/` | `bom_epro.py` (BOM a partir do `.epro`) |

## Licença
GNU Affero General Public License v3.0 (`LICENSE`), a mesma do EVA-RT.
