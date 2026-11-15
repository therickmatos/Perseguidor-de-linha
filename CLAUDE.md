# CLAUDE.md — contexto para o Claude Code

## Como trabalhar
- Responder em **português**, de forma direta e concisa. O Erick (estudante de eletrônica, IFSP) revisa e corrige o que recebe.
- Antes de afirmar algo sobre a placa, ler `hardware/placa_base/README.md`.
- Unidades em mm. Origem no centro da ventoinha, +y para a frente, o mesmo referencial do seguidor.
- Antes de mudar algo de que o Erick gostou, explicar o que seria alterado. Não decidir pelo Erick.

## O projeto
Perseguidor **RUSH - Dreamcast**. É a placa irmã do seguidor RUSH - SATURN (repositório `therickmatos/Seguidor-de-linha`) e foi encomendada junto com ele na JLC em 15/11/2026.
- O circuito é o do Saturn, sem as abas dos 4 sensores de marca, mais o receptor IR KY-022 (J6) no A5.
- O circuito é baseado no EVA-RT (EXOTIC TEAM MX, AGPL-3.0). Manter o crédito e a licença em qualquer arquivo derivado.
- Fluxo: EasyEDA Pro → Gerber → JLC.

## Arquivos
| Pasta | Conteúdo |
|---|---|
| `hardware/placa_base/` | `.epro` do v4 (encomendado), Gerber final, BOM |
| `ferramentas/bom_epro.py` | BOM a partir do `.epro` |

## Formato .epro (para os scripts)
- É um zip de linhas JSON, com coordenadas em mil (× 0,0254 = mm).
- Serigrafia STRING: camada 3 = cima, 4 = baixo.
