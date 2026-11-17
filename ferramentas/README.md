# Ferramentas

## bom_epro.py — BOM a partir do `.epro`
```bash
python3 ferramentas/bom_epro.py "hardware/placa_base/ProPrj_RUSH - Dreamcast - v4_2026-11-15.epro" hardware/placa_base/BOM_v4.csv
```

## pecas_3d/ — peças impressas
Adaptado de `ferramentas/pecas_3d/` do repositório do seguidor. Monta `mecânica/stl/`, separado por filamento em `mecânica/stl/PETG/` e `mecânica/stl/PLA/`:
- o patim da barra V1 (igual ao do seguidor);
- a presilha da vareta para a base de 1,6 mm, com assento;
- a placa de sucção com rebaixo para as cabeças dos pés;
- as cópias dos STL do EVA-RT usados como estão e a caixa de redução espelhada.

As peças paramétricas são derivadas das peças do EVA-RT (© EXOTIC TEAM MX, AGPL-3.0). Guia de impressão e montagem em `mecânica/README.md`.

Dependências: `pip install -r ferramentas/pecas_3d/requisitos.txt`.

- `gerar_pecas.py` — peças paramétricas (espessura da placa, distância vareta–parafuso, folgas, assento). Antes de gravar, reconstrói o `sensor_sledge` e o `barilla_clamp` originais com os mesmos parâmetros e compara com os STL do EVA-RT: tem de dar "reconstrucao confere". A pasta de cada STL vem do conjunto `PLA` (o resto vai para `PETG/`).
- `conferir_montagem.py` — monta em 3D a base (lida do `.epro` do Dreamcast), a V1 (lida do Gerber em `hardware/barra_v1/`), varetas, presilhas, bloco, carcaça, patim, parafusos, placa de sucção, o KY-022 e os componentes da face de baixo. Lista interferências, folgas, vias sob as cabeças de parafuso e a altura até a pista, e desenha as figuras de `mecânica/conferencia/`. A única interferência aceita é a da parede da presilha na borda da base (0,05 mm, como no EVA-RT).

Rodar na raiz do repositório:
```bash
python3 ferramentas/pecas_3d/gerar_pecas.py
```
```bash
python3 ferramentas/pecas_3d/conferir_montagem.py --furos 63.14 --esp-barra 1.6
```

A saída dos dois fica guardada em `mecânica/conferencia/relatorio.txt`. Mudou a distância entre furos ou a espessura da barra: rodar de novo o `conferir_montagem.py`. Nova versão da placa: trocar o nome do `.epro` em `conferir_montagem.py` (`EPRO`) e a espessura em `gerar_pecas.py` (`ESP_BASE`).
