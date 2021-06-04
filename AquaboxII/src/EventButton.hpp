/*****************************************************************************************
 * INVENCTUS Soluções Tecnológicas (C)2021
 * 
 * Arquivo: eventoBotao.hpp
 * Descrição: arquivo de cabeçalho para leitura de teclas
 * Autor: Eng. José Adriano
 * Data: 16/02/2021
 * 
 * informações: 
 *
 * ***************************************************************************************/

#ifndef EVENTBUTTON__HPP
#define EVENTBUTTON__HPP

#include <Arduino.h>

typedef void(*event)(void); //Estou criando um tipo para melhorar meu entendimento do código

class EventButton
{
private:
    
    int pin;
    bool level;

    bool last_state;

    //void (*onPress)(void);  //Testa ao pressionar
    //void (*onRelease)(void); //Testa ao soltar
    event onPress;
    event onRelease;



public:

    EventButton();
    EventButton(const int pin, const bool press_level);

    //void setOnPressCallback(void (*callback)(void));
    //void setOnReleaseCallback(void (*callback)(void)); ou

    void setOnPressCallback(event callback);
    void setOnReleaseCallback(event callback);

    void process(void);
    
};

#endif