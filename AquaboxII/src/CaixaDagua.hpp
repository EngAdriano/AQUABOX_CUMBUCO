/*
  /  INVENCTUS - Soluções Tecnológicas
  /  Biblioteca para gerenciamento de caixa d'água
  /  Data: 16/07/2021
  /  Responsável: Eng. José Adriano
*/

#ifndef CAIXADAGUA_HPP
#define CAIXADAGUA_HPP

#include "Arduino.h"

#define SENSOR_H 0
#define SENSOR_L 1
#define N_PINOS 2

class CaixaDagua
{

public:
    CaixaDagua(const int sensor_H, const int Sensor_L);

    bool caixaVazia(void);
    bool caixaCheia(void);
    bool lerStatusSensor(const size_t index);

    
private:
   int pins[N_PINOS];           //Configuração dos pinos de Sensor_H e Sensor_L
   bool statusPinos[N_PINOS];   //Monitora o status de cada pino relacionado a caixa d'água
};

#endif // CAIXADAGUA_HPP