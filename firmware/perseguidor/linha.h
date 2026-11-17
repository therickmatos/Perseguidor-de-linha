/*
 * linha.h - calibracao e posicao da linha
 *
 * Baseado no EVA-RT 1.2.2 (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0, e no
 * firmware do seguidor RUSH - SATURN (mesma escala de posicao e erro). O
 * perseguidor nao tem sensores de marca: so a barra.
 *
 * So contas: quem le o ADC e o hw.cpp. Sem dependencia do Arduino, para
 * compilar tambem no PC (firmware/testes).
 */
#pragma once

#include <stdint.h>
#include "config.h"

#define N_CANAIS       (N_SENSORES)            // so a barra

#define POS_MAX        ((N_SENSORES - 1) * 1000)   // 11000
#define ERRO_MAX       (POS_MAX / 2)               // 5500

// Minimo e maximo brutos (8 bits) de cada canal
struct Calibracao {
  uint8_t mn[N_CANAIS];
  uint8_t mx[N_CANAIS];
};

// Resultado de Calibrador::finalizar(): um bit por sensor sem contraste
#define CAL_FALHA_BARRA    ((uint16_t)((1u << N_SENSORES) - 1))

class Calibrador {
public:
  void iniciar();
  void amostrar(const uint8_t v[N_CANAIS]);   // passe a media de algumas leituras
  // Copia o resultado e devolve um bit por canal sem contraste (0 = tudo ok)
  uint16_t finalizar(Calibracao &c) const;

private:
  uint8_t _mn[N_CANAIS];
  uint8_t _mx[N_CANAIS];
};

// 0..255 a partir do bruto; 255 = linha se 'inverter' corrigir a polaridade
uint16_t ganhoDoCanal(uint8_t mn, uint8_t mx);
uint8_t  normalizar(uint8_t bruto, uint8_t mn, uint16_t ganho, bool inverter);

class Linha {
public:
  void configurar(const Calibracao &c);
  void reiniciar();                              // posicao volta para o centro

  // Uma vez por ciclo. podeCruzar = false quando o robo esta em curva: ai
  // uma faixa larga embaixo da barra e a propria linha, nao a linha de partida.
  void atualizar(const uint8_t bruto[N_SENSORES], bool podeCruzar = true);

  int16_t erro() const         { return _erro; }     // -5500..+5500, + = linha a direita
  bool    vista() const        { return _vista; }
  bool    cruzamento() const   { return _cruz; }     // faixa atravessada (linha de partida) embaixo da barra
  bool    medida() const       { return _medida; }   // o erro e uma medida deste ciclo
  uint8_t norm(uint8_t i) const { return _norm[i]; } // 0..255, 255 = linha
  uint8_t largura() const      { return _largura; }  // sensores vizinhos ativos (o maior grupo)

private:
  uint8_t  _mn[N_SENSORES];
  uint16_t _ganho[N_SENSORES];
  uint8_t  _norm[N_SENSORES];
  int16_t  _erro;
  int16_t  _hist[4];     // erro dos ultimos ciclos (para antes de a faixa entrar)
  uint8_t  _histPos;
  uint8_t  _faixa;       // ciclos seguidos com o erro mantido por faixa larga
  uint8_t  _salto;       // ciclos seguidos recusando um salto
  uint8_t  _largura;
  bool     _vista;
  bool     _cruz;
  bool     _medida;
};
