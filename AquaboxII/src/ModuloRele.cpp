/*
  /  INVENCTUS - Soluções Tecnológicas
  /  Biblioteca para acionamento de relés
  /  Data: 07/10/2020
  /  Responsável: Eng. José Adriano
  /  Baseado no curso: Arduino Avançado
*/

#include "ModuloRele.hpp"
#include "Arduino.h"

ModuloRele::ModuloRele(const int pin0, const int pin1, const int pin2, const int pin3,const bool activate_state):
    a_state(activate_state),                                //inicializa a classe.
    pins{pin0, pin1, pin2, pin3,},                                //Quando inicializar vetor faça com {}
    state{false, false, false, false}                              //Estas constantes estão no private da classe
{
    for (size_t i = 0; i < N_RELAYS; i++) pinMode(pins[i], OUTPUT);
}

//Set de funções para trabalhar com os relés individualmente
void ModuloRele::set(const size_t index, const bool state) //Seta um relé para um determinado estado
{
    bool s;
    s = state ^ (!a_state);                                 //Utiliza uma XOR para acertar o nivel lógico pelo tipo de placa

    this->state[index] = s;                                 //Atualiza o estado do relé para futuras utilizações
    digitalWrite(pins[index], s);                           //Seta o pino específico com o estado pedido
}

void ModuloRele::on(const size_t index)                    //Liga um determinado relé
{
    state[index] = a_state;
    digitalWrite(pins[index], a_state);
}

void ModuloRele::off(const size_t index)                   //Desliga um determinado relé
{
    state[index] = !a_state;
    digitalWrite(pins[index], !a_state);
}

void ModuloRele::toggle(const size_t index)                //Troca o estado de um determinado relé
{
    state[index] = !state[index];
    digitalWrite(pins[index], state[index]);
}

//Set de funções para trabalhar com todos os relés simultaneamente
void ModuloRele::setAll(const bool state)                  //Seta um determinado estado para todos os relés
{
    for (size_t i = 0; i < N_RELAYS; i++) set(i, state);
}

void ModuloRele::onAll(void)                              //Liga todos os relés
{
    for (size_t i = 0; i < N_RELAYS; i++) on(i);
}

void ModuloRele::offAll(void)                              //Desliga todos os relés
{
    for (size_t i = 0; i < N_RELAYS; i++) off(i);
}

void ModuloRele::toggleAll(void)                           //Traca o estado de todos os relés
{
    for (size_t i = 0; i < N_RELAYS; i++) toggle(i);
}
