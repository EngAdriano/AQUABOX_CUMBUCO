/*
  /  INVENCTUS - Soluções Tecnológicas
  /  Biblioteca para acionamento de relés
  /  Data: 07/10/2020
  /  Responsável: Eng. José Adriano
  /  Baseado no curso: Arduino Avançado
*/

#ifndef MODULORELE_HPP
#define MODULORELE_HPP

#include "Arduino.h"

const size_t N_RELAYS = 4;      //Número de relés que serão controlados

class ModuloRele
{

    public:
        ModuloRele(const int pin0, const int pin1, const int pin2, const int pin3,const bool activate_state);

        //Set de funções para trabalhar com os relés individualmente
        void set(const size_t index, const bool state); //Seta um relé para um determinado estado
        void on(const size_t index);                    //Liga um determinado relé
        void off(const size_t index);                   //Desliga um determinado relé
        void toggle(const size_t index);                //Troca o estado de um determinado relé

        //Set de funções para trabalhar com todos os relés simultaneamente
        void setAll(const bool state);                  //Seta um determinado estado para todos os relés
        void onAll(void);                               //Liga todos os relés
        void offAll(void);                              //Desliga todos os relés
        void toggleAll(void);                           //Traca o estado de todos os relés

    private:
    
        bool a_state;

        int pins[N_RELAYS];
        bool state[N_RELAYS];
};

#endif // MODULORELE_HPP