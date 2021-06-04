/*****************************************************************************************
 * INVENCTUS Soluções Tecnológicas (C)2021
 * 
 * Arquivo: eventoBotao.cpp
 * Descrição: arquivo para leitura de uma botao
 * Autor: Eng. José Adriano
 * Data: 18/02/2021
 * 
 * informações: 
 *
 * ***************************************************************************************/

#include <Arduino.h>
#include "EventButton.hpp"

EventButton::EventButton()
{
    
}

EventButton::EventButton(const int pin, const bool press_level):
    pin(pin),
    level(press_level)
{
    pinMode(this->pin, INPUT);
    last_state = digitalRead(this->pin);
}

void EventButton::setOnPressCallback(void (*callback)(void))
{
    onPress = callback;
}

void EventButton::setOnReleaseCallback(void (*callback)(void))
{
    onRelease = callback;
}

void EventButton::process(void)
{
    bool state;

    state = digitalRead(pin);
    vTaskDelay(20 / portTICK_PERIOD_MS);

    if(state != last_state)
    {
        if(state == level)
        {
            if(onPress != NULL) 
            {
                onPress();
            }
        }
        else
        {
            if(onRelease != NULL)
            {
                onRelease();
            }
        }
        
        last_state = state;
    }
}