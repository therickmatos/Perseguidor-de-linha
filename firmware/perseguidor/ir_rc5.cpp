/*
 * ir_rc5.cpp - decodificador RC5 do receptor IR (modulo de inicio)
 *
 * Parte do firmware do perseguidor RUSH - Dreamcast, baseado no EVA-RT 1.2.2
 * (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0.
 *
 * Decodifica pelas bordas: todo bit tem uma borda no meio; entre dois bits
 * iguais ha mais uma, na fronteira. Entao, a partir da borda do meio de um
 * bit, a proxima borda vem ~889 us depois (fronteira) ou ~1778 us depois
 * (meio do bit seguinte). O valor do bit e o sentido da borda do meio.
 * Qualquer borda fora dessas janelas, ou incoerente com o nivel, descarta
 * o quadro: o RC5 nao tem soma de verificacao, a conferencia e a temporizacao.
 */
#include "ir_rc5.h"

// Janelas em us. O receptor alarga os pulsos com portadora e encurta os sem
// portadora em ate ~200 us; as janelas cobrem isso com folga.
#define FRONTEIRA_MIN   500     // nominal 889
#define FRONTEIRA_MAX  1250
#define MEIO_MIN       1300     // nominal 1778
#define MEIO_MAX       2300
#define SILENCIO_MIN   2600     // linha parada antes do primeiro bit (entre quadros: ~89 ms)
#define N_BITS           14

void DecodificadorRC5::reiniciar() {
  _estado = OCIOSO;
  _tBorda = 0;
  _tMeio = 0;
  _nivelMeio = true;
  _fronteira = false;
  _n = 0;
  _bits = 0;
  _semBorda = true;
  _q.endereco = 0;
  _q.comando = 0;
  _q.toggle = 0;
}

bool DecodificadorRC5::borda(uint32_t us, bool nivel) {
  const bool silencio = _semBorda || (uint32_t)(us - _tBorda) >= SILENCIO_MIN;
  _tBorda = us;
  _semBorda = false;

  if (_estado == RECEBENDO) {
    const uint32_t dm = us - _tMeio;
    if (dm >= MEIO_MIN && dm <= MEIO_MAX) {
      // Meio do bit seguinte. Bits iguais: a linha voltou na fronteira e
      // agora fica no mesmo nivel do meio anterior; bits diferentes: nao
      // houve fronteira e o nivel e o oposto.
      if ((nivel == _nivelMeio) == _fronteira) {
        _bits = (uint16_t)((_bits << 1) | (nivel ? 0 : 1));   // descida = 1
        _tMeio = us;
        _nivelMeio = nivel;
        _fronteira = false;
        if (++_n < N_BITS) return false;
        // S1 (bit 13) e sempre 1 e entrou como o primeiro bit
        _q.endereco = (uint8_t)((_bits >> 6) & 0x1F);
        _q.comando = (uint8_t)((_bits & 0x3F) | ((_bits & 0x1000) ? 0 : 0x40));
        _q.toggle = (uint8_t)((_bits >> 11) & 1);
        _estado = OCIOSO;
        return true;
      }
    } else if (dm >= FRONTEIRA_MIN && dm <= FRONTEIRA_MAX && !_fronteira && nivel != _nivelMeio) {
      _fronteira = true;
      return false;
    }
    _estado = OCIOSO;   // quadro estragado; a borda ainda pode ser o inicio de outro
  }

  // Ocioso: a primeira descida depois de a linha ficar parada e o meio do S1
  if (!nivel && silencio) {
    _estado = RECEBENDO;
    _tMeio = us;
    _nivelMeio = false;
    _fronteira = false;
    _n = 1;
    _bits = 1;
  }
  return false;
}
