/*
 * ir_rc5.h - decodificador RC5 do receptor IR (modulo de inicio)
 *
 * Parte do firmware do perseguidor RUSH - Dreamcast, baseado no EVA-RT 1.2.2
 * (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0.
 *
 * So contas: recebe o instante (us) e o nivel de cada borda da saida do
 * receptor (KY-022: nivel baixo = portadora presente). Quem le o pino e
 * chama borda() e a interrupcao do hw.cpp. Sem dependencia do Arduino, para
 * ser testado no PC (firmware/testes/teste_ir.cpp).
 *
 * Quadro RC5 (Philips): 14 bits Manchester de 1778 us, o mais significativo
 * primeiro: S1 S2 T A4..A0 C5..C0. "1" = meio bit sem portadora e meio com
 * (na saida do receptor: alto e depois baixo, borda de descida no meio);
 * "0" = o contrario. S1 = 1; S2 = 1 no RC5 e o bit 6 do comando, invertido,
 * no RC5 estendido. T muda a cada aperto do botao do controle.
 */
#pragma once

#include <stdint.h>

struct QuadroRC5 {
  uint8_t endereco;   // 0..31
  uint8_t comando;    // 0..127 (bit 6 = S2 invertido)
  uint8_t toggle;     // 0 ou 1
};

class DecodificadorRC5 {
public:
  void reiniciar();

  // Uma borda da saida do receptor: instante em us (pode dar a volta em
  // 2^32) e nivel DEPOIS da borda (false = baixo = portadora).
  // Devolve true quando um quadro acaba de ser completado (ver quadro()).
  bool borda(uint32_t us, bool nivel);

  const QuadroRC5 &quadro() const { return _q; }

private:
  enum Estado : uint8_t { OCIOSO, RECEBENDO };
  Estado    _estado;
  uint32_t  _tBorda;     // ultima borda (qualquer)
  uint32_t  _tMeio;      // ultima borda de meio de bit
  bool      _nivelMeio;  // nivel depois da ultima borda de meio de bit
  bool      _fronteira;  // ja houve a borda de fronteira desde o ultimo meio
  bool      _semBorda;   // nenhuma borda desde reiniciar()
  uint8_t   _n;          // bits recebidos
  uint16_t  _bits;
  QuadroRC5 _q;
};

// ------------------------------------------------------------------ modulo de inicio
// Significado de um quadro no padrao do startmodule.com (ver config.h).
enum ComandoIR : uint8_t {
  IR_NADA,       // nenhum quadro novo (nao sai de interpretarRC5)
  IR_OUTRO,      // quadro RC5 valido, mas nao e para este robo
  IR_PARTIDA,
  IR_PARADA,
  IR_PROGRAMA    // o juiz mandou gravar um numero de arena novo
};

// arena: numero gravado (par). paradaQualquer: aceita a parada de qualquer arena.
inline ComandoIR interpretarRC5(const QuadroRC5 &q, uint8_t arena, bool paradaQualquer,
                                uint8_t endPrograma, uint8_t endPartida) {
  if (q.endereco == endPrograma) return IR_PROGRAMA;
  if (q.endereco != endPartida) return IR_OUTRO;
  const uint8_t a = (uint8_t)(arena & 0x7E);
  if (q.comando == (uint8_t)(a | 1)) return IR_PARTIDA;
  if (q.comando == a) return IR_PARADA;
  if (paradaQualquer && (q.comando & 1) == 0) return IR_PARADA;
  return IR_OUTRO;
}

// Numero de arena gravado por um quadro de programacao
inline uint8_t arenaDoQuadro(const QuadroRC5 &q) {
  return (uint8_t)(q.comando & 0x7E);
}
