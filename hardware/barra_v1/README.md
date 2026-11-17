# Barra de sensores V1

Gerber da barra V1 (12× QRE1113 + 74HC4067), a mesma do seguidor RUSH - SATURN. Cópia de `hardware/barra_v1/` do repositório `therickmatos/Seguidor-de-linha`.

- Furos de fixação Ø2,05 em x = ±44,626 mm; borda lateral em x = ±48,25 mm (lidos do Gerber por `ferramentas/pecas_3d/conferir_montagem.py`).
- Sensores a cada 7,8 mm. No Gerber os sensores estão nos canais 2 a 13 do mux; o teste de bancada usou 4 a 15 (ver `MAPA_MUX` em `firmware/perseguidor/config.h`).
- Espessura: **1,6 mm** (medida pelo Erick; o Gerber não informa). É a que define o patim (`mecânica/stl/PETG/patim_v1_*_barra1.6.stl`).
