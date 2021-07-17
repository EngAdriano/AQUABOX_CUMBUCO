/*
  /  INVENCTUS - Soluções Tecnológicas
  /  Biblioteca para gerenciamento de caixa d'água
  /  Data: 16/07/2021
  /  Responsável: Eng. José Adriano
*/

#include <Arduino.h>
#include "CaixaDagua.hpp"

CaixaDagua::CaixaDagua(const int sensor_H, const int Sensor_L):
  pins{sensor_H, Sensor_L} 
{
  for (size_t i = 0; i < N_PINOS; i++) pinMode(pins[i], INPUT);
  
  statusPinos[SENSOR_L] = lerStatusSensor(SENSOR_L);
  statusPinos[SENSOR_H] = lerStatusSensor(SENSOR_H);
}

bool CaixaDagua::caixaVazia(void)
{
  statusPinos[SENSOR_L] = lerStatusSensor(SENSOR_L);
  
  if(!statusPinos[SENSOR_L])
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool CaixaDagua::caixaCheia(void)
{
  statusPinos[SENSOR_H] = lerStatusSensor(SENSOR_H);
  
  if(!statusPinos[SENSOR_H])
  {
    return false;
  }
  else
  {
    return true;
  }
}

bool CaixaDagua::lerStatusSensor(const size_t index)
{
  return digitalRead(pins[index]);
}
