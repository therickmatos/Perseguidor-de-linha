/*
 * hw.h - acesso ao hardware (ATmega328P, placa RUSH - Dreamcast v4)
 *
 * Baseado no EVA-RT 1.2.2 (c) 2025 EXOTIC TEAM MX, licenca AGPL-3.0
 * Adaptado do firmware do seguidor RUSH - SATURN.
 */
#pragma once

#include <Arduino.h>
#include "config.h"
#include "linha.h"
#include "params.h"
#include "ir_rc5.h"

void hwIniciar();

// ---- sensores (8 bits, referencia AVCC)
// Barra na ordem do robo (0 = esquerda)
void    hwLerSensores(uint8_t bruto[N_CANAIS]);
void    hwLerMedia(uint8_t bruto[N_CANAIS], uint8_t leituras);   // calibracao
uint8_t hwLerCanalMux(uint8_t canal);    // um canal do 74HC4067 (diagnostico)
uint8_t hwLerAdc(uint8_t adc);           // uma entrada do ATmega (diagnostico)

// ---- receptor IR (A5, por interrupcao de mudanca de nivel)
// true = chegou um quadro RC5 desde a ultima chamada (fica so o mais novo)
bool hwIrQuadro(QuadroRC5 &q);
bool hwIrNivel();                        // nivel atual do pino (true = sem portadora)

// ---- bateria (A7, divisor 20k/10k)
void     hwBateriaAtualizar();                    // uma leitura para o filtro
uint16_t hwBateriaMv(const Parametros &p);
// Fator dos PWM, x256: vref / tensao atual. 256 se a compensacao estiver
// desligada ou a leitura nao fizer sentido para uma 2S.
uint16_t hwEscalaBateria(const Parametros &p);

// ---- saidas
void hwMotores(int16_t esq, int16_t dir);   // -255..255, + = para a frente
void hwVentoinha(uint8_t pwm);
void hwLedA(bool ligado);
void hwLedB(bool ligado);
void hwLedC(bool ligado);
void hwLeds(bool ligado);                   // os tres

// ---- botoes (true = apertado)
bool hwBotao(uint8_t n);                    // 1 ou 2
