//AQUABOX - CUMBUCO
//Sistema de irrigação automatizado.
//Autor: Eng. José Adriano
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include "LiquidCrystal.h"
#include "RTClib.h"
#include "ModuloRele.hpp"
#include "EventButton.hpp"
#include "CaixaDagua.hpp"

#define SETORMAX 3
#define HORAMAX 23
#define MINUTOSMAX 59
#define DURACAOMAX 59
#define EEPROM_TAMANHO 26
#define EEPROM_INICIO 0
#define OFFSET 4
#define NUMERO_DE_MEMORIAS 6
#define NUMERO_DE_FUNCOES_ATIVAS 4
#define ENDERECO_DO_NUMERO_DE_REGISTROS 0

// --- Mapeamento de Hardware --- 
#define RS 33           //IO33 Pino 8 do Módulo
#define EN 32           //IO32 Pino 7 do Módulo
#define D4 14           //IO14 Pino 12 do Módulo
#define D5 27           //IO27 Pino 11 do Módulo
#define D6 26           //IO26 Pino 10 do Módulo
#define D7 25           //IO25 Pino 9 do Módulo
#define BT_SELECT   17  //Conector select(Key1)/Pino 30 do módulo
#define BT_SALVA   13   //Conector menos(Menos)/Pino 15 do Módulo
#define BT_MAIS    16   //Conector mais(Key2)/Pino 31 do Módulo
#define BT_VOLTA   4    //Conector voltar(Key3)/Pino 31 do Módulo
#define SETOR_1 19      //IO19 Pino 27 do Módulo
#define SETOR_2 18      //IO18 Pino 28 do Módulo
#define SETOR_3 5       //IO5 Pino 29 do Módulo
#define BOMBA 23        //IO23 Pino 21 do Módulo - Antigo 7(SD0)  
#define NIVEL_H 34      //Sensor de nível alto da caixa d'água
#define NIVEL_L 35      //Sensor de nível baixo da caixa d'água
#define UMIDADE 36      //Sensor de umidade

//Variáveis globais
int8_t funcaoAtiva = 0;
uint8_t posicaoRelativaEEPROM = 0;
bool btnMaisFlag = false;   //Flag para botão Mais
bool btnSelectFlag = false; //Flag para botão Menos
bool btnEnterFlag = false;  //Flag para botão Enter
bool btnVoltaFlag = false;  //Flag para botão Volta

//Objetos utilizados
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
RTC_DS3231 rtc;
ModuloRele relays(SETOR_1, SETOR_2, SETOR_3, BOMBA, true);           
EventButton btnMais(BT_MAIS, LOW);
EventButton btnSalva(BT_SALVA, LOW);
EventButton btnEnter(BT_SELECT, LOW);
EventButton btnVoltar(BT_VOLTA, LOW);
CaixaDagua caixa(NIVEL_H, NIVEL_L);

//Protótipo de funções
void lerFuncaoAtiva(void);
void BtnPressionadoMais(void);
void BtnPressionadoMenos(void);
void BtnPressionadoEnter(void);
void BtnPressionadoVoltar(void);
void standyBy(void);
void irrigacao();
void configuraRelogio(void);
void configuraHoraSetorIrriga(void);
void consultaIrriga(void);
void deleteIrriga(void);
int8_t localizarPosicaoLivre(void);
void InitEEPROM(void);

void setup() 
{
  relays.offAll();

  EEPROM.begin(EEPROM_TAMANHO);

  if (! rtc.begin()) 
    {
        lcd.begin(16, 2);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ERRO: RTC");
        lcd.setCursor(0,1);
        lcd.print("Não Localizado");
        while (1);
    }
//Registra os callbacks das teclas
    btnSalva.setOnPressCallback(&BtnPressionadoMenos);
    //btnMenos.setOnReleaseCallback(&btnSoltoMenos);
    btnMais.setOnPressCallback(&BtnPressionadoMais);
    //btnMais.setOnReleaseCallback(&btnSoltoMais);
    btnEnter.setOnPressCallback(&BtnPressionadoEnter);
    //btnSelect.setOnReleaseCallback(&btnSoltoSelect);
    btnVoltar.setOnPressCallback(&BtnPressionadoVoltar);
    //btnSelect.setOnReleaseCallback(&btnSoltoVoltar);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print ("INVENCTUS");
  lcd.setCursor(0,1); 
  lcd.print("AQUABOX  Cumbuco");
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  lcd.clear();

  InitEEPROM();
}


void loop() 
{

  //TODO fazer a função de monitoramento e encher a caixa d'água.
  
  lerFuncaoAtiva();

  switch (funcaoAtiva)
  {
  case 0:
    //Modo de espera por comandos
    standyBy();
    break;

  case 1:
    //Consulta do horário/setor de inrrigação
    consultaIrriga();
    break;

  case 2:
    //Configuração do relógio
    configuraRelogio();
    break;

  case 3:
    //Configuração dos horários/setor de Irrigação
    configuraHoraSetorIrriga();
    break;

  case 4:
    //Altera do horário/setor de inrrigação
    deleteIrriga();
    break;

  case 5:
     
    break;
  }
  
}

void lerFuncaoAtiva(void)
{
  //Varredura no teclado
  btnEnter.process();

  if(btnEnterFlag)
  {
    funcaoAtiva++;
    btnEnterFlag = false;

    if(funcaoAtiva > NUMERO_DE_FUNCOES_ATIVAS)
    {
      funcaoAtiva = 0;
    }
    lcd.clear();
  }
}

void monitoraCaixa(void)
{
  
}

void standyBy(void)
{
  // Criar um sistema para cancelar a irrigaçao. usar uma flag para isso.
  //Lembrar: a irrigação só é cancelada no minuto seguinte
  uint8_t setor = 0;
  uint8_t hora = 0;
  uint8_t minutos = 0;
  
  DateTime now;

  while(funcaoAtiva == 0)
  {
    lerFuncaoAtiva();

    now = rtc.now();
  
    lcd.setCursor(0,0); 
    lcd.print("Aquabox  Cumbuco");
    lcd.setCursor(0,1); 

    if(now.day() < 10)
    {
      lcd.print('0');
      lcd.print(now.day(), DEC);
    }
    else
    {
      lcd.print(now.day(), DEC);
    }
    
    lcd.print('/');

    if(now.month() < 10)
    {
      lcd.print('0');
      lcd.print(now.month(), DEC);
    }
    else
    {
      lcd.print(now.month(), DEC);
    }

    lcd.print(" - ");

    if(now.hour() < 10)
    {
      lcd.print('0');
      lcd.print(now.hour(), DEC);
    }
    else
    {
      lcd.print(now.hour(), DEC);
    }

    lcd.print(':');

    if(now.minute() < 10)
    {
      lcd.print('0');
      lcd.print(now.minute(), DEC);
    }
    else
    {
      lcd.print(now.minute(), DEC);
    }

    lcd.print(':');

    if(now.second() < 10)
    {
      lcd.print('0');
      lcd.print(now.second(), DEC);
    }
    else
    {
      lcd.print(now.second(), DEC);
    }

    //Efetuar a leitura dos dados das posições de memória e checa se está no horário de irrigação
    //A contagem começa em 0
    for(int8_t i = 0; i < NUMERO_DE_MEMORIAS; i++)
    {
      setor = EEPROM.read((i * OFFSET) + 1);
      hora = EEPROM.read((i * OFFSET) + 2);
      minutos = EEPROM.read((i * OFFSET) + 3);

      if((setor != 0) && (hora == now.hour()) && (minutos == now.minute()))
      {
        posicaoRelativaEEPROM = i;
        irrigacao();
        
      }
    }

  }
}

void irrigacao()
{
  uint8_t setor = 0;
  uint8_t minutos = 0;
  uint8_t duracao = 0;
  uint8_t instanteTempo = 0;
  bool reles = false;
  bool irrigandoJardim = true;

  DateTime now;
  now = rtc.now();
  instanteTempo = now.minute();

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Aquabox  Cumbuco");
  lcd.setCursor(0,1); 
  lcd.print("Irrigando: 00:00");

  reles = true;

  setor = EEPROM.read((posicaoRelativaEEPROM * OFFSET) + 1);
  //minutos = EEPROM.read((posicaoRelativaEEPROM * OFFSET) + 3);
  duracao = EEPROM.read((posicaoRelativaEEPROM * OFFSET) + 4);

  while(irrigandoJardim)
  {
    //Varredura no teclado
    btnVoltar.process();

    if(btnVoltaFlag)
    {
      funcaoAtiva = 0;
  
      btnVoltaFlag = false;
      relays.offAll();
      lcd.clear();
    }

    //Ligar os relés de acordo com os dados coletados
    if(reles == true)
    {
      //Liga relés
      relays.on(setor - 1);                   //Liga a válvula do setor requerido
      vTaskDelay(5000 / portTICK_PERIOD_MS);  //Aguarda um tempo para afetivação da válvula
      relays.on(3);                           //Liga a bomba de água
      reles = false;                          //Evita de ficar chamando as funções dos relés.
    }

    now = rtc.now();

    minutos = now.minute();

    if(instanteTempo != minutos)
    {
      instanteTempo = minutos;

      duracao--;
    }

    lcd.setCursor(14,1); 
    lcd.print(duracao/10);
    lcd.print(duracao%10);

    if(duracao == 0)
    {
      funcaoAtiva = 0;
      irrigandoJardim = false;
      relays.off(3);                          //Desliga a bomba
      vTaskDelay(5000 / portTICK_PERIOD_MS);  //Tempo de acomodação da água
      relays.off(setor - 1);
      lcd.clear();
    }
  }
}

//Função para Configurar o relógio
void configuraRelogio(void)
{
  char ajusteRelogio = -1;
  char ajusteDia = 0;
  char ajusteMes = 0;
  char ajusteAno = 20;
  char ajusteHora = -1;
  char ajusteMinutos = -1;

  //unsigned short confRelogio[10];

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print(" Config Relogio ");
  lcd.setCursor(0,1); 
  lcd.print("00/00/00 - 00:00");
        
  while(funcaoAtiva == 2)
  {
    //Varredura no teclado
    btnMais.process();
    btnSalva.process();
    btnEnter.process();
    btnVoltar.process();

    if(btnEnterFlag)
    {
      funcaoAtiva++;
      btnEnterFlag = false;
      btnMaisFlag = false;
      btnSelectFlag = false;
      btnVoltaFlag = false;

      if(funcaoAtiva > NUMERO_DE_FUNCOES_ATIVAS)
      {
        funcaoAtiva = 0;
      }
      lcd.clear();
      btnEnterFlag = false;
    }

    if(btnSelectFlag)
    {
      ajusteRelogio++;

      if(ajusteRelogio > 4)
      {
        ajusteRelogio = 0;
      }

      switch (ajusteRelogio)
      {
      case 0:
        lcd.setCursor(0,0); 
        lcd.print("   Config Dia   ");
        break;

      case 1:
        lcd.setCursor(0,0); 
        lcd.print("   Config Mes   ");
        break;

      case 2:
        lcd.setCursor(0,0); 
        lcd.print("   Config Ano   ");
        break;

      case 3:
        lcd.setCursor(0,0); 
        lcd.print("   Config Hora  ");
        break;

      case 4:
        lcd.setCursor(0,0); 
        lcd.print(" Config Minutos ");
        break;

      case 5:
        lcd.setCursor(0,0);
        lcd.print(" Salvar Config?  ");
        break;

      default:
        break;
      }
      btnSelectFlag = false;
    }

    if(btnMaisFlag)
    {
      unsigned short confRelogio[10];

      switch (ajusteRelogio)
      {
      case 0:
        ajusteDia++;

        if(ajusteDia > 31)
        {
          ajusteDia = 1;
        }
        confRelogio[0] = ajusteDia/10;
        confRelogio[1] = ajusteDia%10;
        lcd.setCursor(0,1);
        lcd.print(confRelogio[0]);
        lcd.print(confRelogio[1]);
        break;

      case 1:
        ajusteMes++;

        if(ajusteMes > 12)
        {
          ajusteMes = 1;
        }
        confRelogio[2] = ajusteMes/10;
        confRelogio[3] = ajusteMes%10;
        lcd.setCursor(3,1);
        lcd.print(confRelogio[2]);
        lcd.print(confRelogio[3]);
        break;

      case 2:
        ajusteAno++;

        if(ajusteAno > 99)
        {
          ajusteAno = 21;
        }
        confRelogio[4] = ajusteAno/10;
        confRelogio[5] = ajusteAno%10;
        lcd.setCursor(6,1);
        lcd.print(confRelogio[4]);
        lcd.print(confRelogio[5]);
        break;

      case 3:
        ajusteHora++;

        if(ajusteHora > 23)
        {
          ajusteHora = 0;
        }

        confRelogio[6] = ajusteHora/10;
        confRelogio[7] = ajusteHora%10;
        lcd.setCursor(11,1);
        lcd.print(confRelogio[6]);
        lcd.print(confRelogio[7]);
        break;

      case 4:
        ajusteMinutos++;

        if(ajusteMinutos > 59)
        {
          ajusteMinutos = 0;
        }
        confRelogio[8] = ajusteMinutos/10;
        confRelogio[9] = ajusteMinutos%10;
        lcd.setCursor(14,1);
        lcd.print(confRelogio[8]);
        lcd.print(confRelogio[9]);
        break;

      case 5:
        //Para gravar a configuração no RTC
        // Esta linha define o RTC com uma data e hora explícitas, por exemplo, para definir
        // 21 de janeiro de 2014 às 3h você executa:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

        uint16_t ano = 2000 + ajusteAno;

        rtc.adjust(DateTime(ano, ajusteMes, ajusteDia, ajusteHora, ajusteMinutos, 0));
        funcaoAtiva = 0;
        lcd.clear();
        break;
      }
      btnMaisFlag = false;
    }

    if(btnVoltaFlag)
    {
      funcaoAtiva = 0;
  
      btnVoltaFlag = false;
      lcd.clear();
    }
  }
}

//Configuração de setor e horário para inrrigar
void configuraHoraSetorIrriga(void)
{
  unsigned short numRegistros = 0;
  char ajusteItem = -1;
  char setor = 0;
  char hora = -1;
  char minutos = -1;
  char duracao = -1;

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print(" Config Inrriga ");
  lcd.setCursor(0,1); 
  lcd.print("S:0 H:00:00 D:00");

  while(funcaoAtiva == 3)
  {
      //Varredura no teclado
      btnMais.process();
      btnSalva.process();
      btnEnter.process();
      btnVoltar.process();

    if(btnEnterFlag)
    {
      funcaoAtiva++;
      
      if(funcaoAtiva > NUMERO_DE_FUNCOES_ATIVAS)
      {
        funcaoAtiva = 0;
      }
      lcd.clear();
      btnEnterFlag = false;
      btnMaisFlag = false;
      btnSelectFlag = false;
      btnVoltaFlag = false;
    }

    if(btnSelectFlag)
    {
      ajusteItem++;

      if(ajusteItem > 4)
      {
        ajusteItem = 0;
      }

      switch (ajusteItem)
      {
      case 0:
        lcd.setCursor(0,0); 
        lcd.print("  Config Setor  ");
        break;

      case 1:
        lcd.setCursor(0,0); 
        lcd.print("  Config Hora   ");
        break;

      case 2:
        lcd.setCursor(0,0); 
        lcd.print(" Config Minutos ");
        break;

      case 3:
        lcd.setCursor(0,0); 
        lcd.print(" Config Duracao ");
        break;

      case 4:
        lcd.setCursor(0,0);
        lcd.print("NReg:  Salvar   ");
        break;
      }

      btnSelectFlag = false;
    }
   
    if(btnMaisFlag)
    {
      unsigned short confIrriga[7];
      int8_t posicaoLivre = 0;

      switch (ajusteItem)
      {
      case 0:
        setor++;

        if(setor > 3)
        {
          setor = 0;
        }
        confIrriga[0] = setor%10;
        lcd.setCursor(2,1);
        lcd.print(confIrriga[0]);
        break;

      case 1:
        hora++;

        if(hora > HORAMAX)
        {
          hora = 0;
        }
        confIrriga[1] = hora/10;
        confIrriga[2] = hora%10;
        lcd.setCursor(6,1);
        lcd.print(confIrriga[1]);
        lcd.print(confIrriga[2]);
        break;

      case 2:
        minutos++;

        if(minutos > MINUTOSMAX)
        {
          minutos = 0;
        }
        confIrriga[3] = minutos/10;
        confIrriga[4] = minutos%10;
        lcd.setCursor(9,1);
        lcd.print(confIrriga[3]);
        lcd.print(confIrriga[4]);
        break;

      case 3:
        duracao++;

        if(duracao > MINUTOSMAX)
        {
          duracao = 0;
        }

        confIrriga[5] = duracao/10;
        confIrriga[6] = duracao%10;
        lcd.setCursor(14,1);
        lcd.print(confIrriga[5]);
        lcd.print(confIrriga[6]);
        break;
      
      case 4:
        
        posicaoLivre = localizarPosicaoLivre();             //Se livre retorna a posição, se não retorna 0

        if((posicaoLivre != 0) && (setor != 0))            //Setor e posição livre válidos grava dados na posição, se não mostra ERRO
        {
          //Caso exista posição livre e setor for válido 

          numRegistros = EEPROM.read(ENDERECO_DO_NUMERO_DE_REGISTROS);

          EEPROM.write(ENDERECO_DO_NUMERO_DE_REGISTROS, numRegistros + 1);
          EEPROM.write(posicaoLivre, setor);
          EEPROM.write(posicaoLivre + 1, hora);
          EEPROM.write(posicaoLivre + 2, minutos);
          EEPROM.write(posicaoLivre + 3, duracao);
          //EEPROM.commit();  //Não esquecer de ativar para produção

          lcd.setCursor(14,0);
          lcd.print("OK");
          
        }
        else
        {
          while(btnVoltaFlag == false)
          {
            //Caso não exista posição livre
            btnVoltar.process();

            lcd.setCursor(0,0);
            lcd.print(" ERRO: SETOR ou ");
            lcd.setCursor(0,1);
            lcd.print("POSICOES  LIVRES");
          }
          btnVoltaFlag = false;
        }

        numRegistros = EEPROM.read(ENDERECO_DO_NUMERO_DE_REGISTROS);
        lcd.setCursor(5,0);
        lcd.print(numRegistros);
        break;
      }
      btnMaisFlag = false;
    }

    if(btnVoltaFlag)
    {
      funcaoAtiva = 0;
  
      btnVoltaFlag = false;
      lcd.clear();
    }

  }
}

void consultaIrriga(void)
{
  int8_t numRegistros = 0;
  char setor = 0;
  char hora = 0;
  char minutos = 0;
  char duracao = 0;
  //char posicaoDaMemoria = 0;

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Cons: Re:0 Mem:0");
  lcd.setCursor(0,1); 
  lcd.print("S:0 H:00:00 D:00");

  while(funcaoAtiva == 1)
  {
    
    //Varredura no teclado
      btnMais.process();
      btnSalva.process();
      btnEnter.process();
      btnVoltar.process();

      numRegistros = EEPROM.read(ENDERECO_DO_NUMERO_DE_REGISTROS);
      lcd.setCursor(9,0);
      lcd.print(numRegistros);

    if(btnEnterFlag)  //Menu principal
    {
      funcaoAtiva++;
      
      if(funcaoAtiva > NUMERO_DE_FUNCOES_ATIVAS)
      {
        funcaoAtiva = 0;
      }
      lcd.clear();
      btnEnterFlag = false;
      btnMaisFlag = false;
      btnSelectFlag = false;
      btnVoltaFlag = false;
    }

    if(btnVoltaFlag)    //Volta a tela principal
    {
      funcaoAtiva = 0;
  
      btnVoltaFlag = false;
      lcd.clear();
    }

    if(btnSelectFlag)   //Seleciona o submenu
    {
      unsigned short confIrriga[7];
      static int8_t posicaoMemoria = 0;
      int8_t posicaoReal = 0;
        
      posicaoReal = (posicaoMemoria * OFFSET) + 1;

      setor = EEPROM.read(posicaoReal);
      hora = EEPROM.read(posicaoReal + 1);
      minutos = EEPROM.read(posicaoReal + 2);
      duracao = EEPROM.read(posicaoReal + 3);

      lcd.setCursor(15,0);
      lcd.print(posicaoMemoria+1);

      confIrriga[0] = setor%10;
      lcd.setCursor(2,1);
      lcd.print(confIrriga[0]);
      confIrriga[1] = hora/10;
      confIrriga[2] = hora%10;
      lcd.setCursor(6,1);
      lcd.print(confIrriga[1]);
      lcd.print(confIrriga[2]);
      confIrriga[3] = minutos/10;
      confIrriga[4] = minutos%10;
      lcd.setCursor(9,1);
      lcd.print(confIrriga[3]);
      lcd.print(confIrriga[4]);
      confIrriga[5] = duracao/10;
      confIrriga[6] = duracao%10;
      lcd.setCursor(14,1);
      lcd.print(confIrriga[5]);
      lcd.print(confIrriga[6]);

      posicaoMemoria++;

      if(posicaoMemoria > 5)
      {
        posicaoMemoria = 0;
      }

      btnSelectFlag = false;
    }
  }
}
   
void deleteIrriga(void)
{
  int8_t numRegistros = 0;
  char setor = 0;
  char hora = 0;
  char minutos = 0;
  char duracao = 0;
  int8_t posicaoRealDeleta = 0; //Posição para deletar

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Del: Re:0  Mem:0");
  lcd.setCursor(0,1); 
  lcd.print("S:0 H:00:00 D:00");

  while(funcaoAtiva == 4)
  {
    //Varredura no teclado
      btnMais.process();
      btnSalva.process();
      btnEnter.process();
      btnVoltar.process();

      numRegistros = EEPROM.read(ENDERECO_DO_NUMERO_DE_REGISTROS);
      lcd.setCursor(8,0);
      lcd.print(numRegistros);

    if(btnEnterFlag)
    {
      funcaoAtiva++;
      
      if(funcaoAtiva > NUMERO_DE_FUNCOES_ATIVAS)
      {
        funcaoAtiva = 0;
      }
      lcd.clear();
      btnEnterFlag = false;
      btnMaisFlag = false;
      btnSelectFlag = false;
      btnVoltaFlag = false;
    }

    if(btnVoltaFlag)
    {
      funcaoAtiva = 0;
  
      btnVoltaFlag = false;
      lcd.clear();
    }

    if(btnSelectFlag)   //Seleciona o submenu
    {
      unsigned short confIrriga[7];
      static int8_t posicaoDeleta = 0;
      
      posicaoRealDeleta = (posicaoDeleta * OFFSET) + 1;

      setor = EEPROM.read(posicaoRealDeleta);
      hora = EEPROM.read(posicaoRealDeleta + 1);
      minutos = EEPROM.read(posicaoRealDeleta + 2);
      duracao = EEPROM.read(posicaoRealDeleta + 3);

      lcd.setCursor(15,0);
      lcd.print(posicaoDeleta+1);

      confIrriga[0] = setor%10;
      lcd.setCursor(2,1);
      lcd.print(confIrriga[0]);
      confIrriga[1] = hora/10;
      confIrriga[2] = hora%10;
      lcd.setCursor(6,1);
      lcd.print(confIrriga[1]);
      lcd.print(confIrriga[2]);
      confIrriga[3] = minutos/10;
      confIrriga[4] = minutos%10;
      lcd.setCursor(9,1);
      lcd.print(confIrriga[3]);
      lcd.print(confIrriga[4]);
      confIrriga[5] = duracao/10;
      confIrriga[6] = duracao%10;
      lcd.setCursor(14,1);
      lcd.print(confIrriga[5]);
      lcd.print(confIrriga[6]);

      posicaoDeleta++;

      if(posicaoDeleta > 5)
      {
        posicaoDeleta = 0;
      }

      btnSelectFlag = false;
    }

    if(btnMaisFlag)
    {
      if(posicaoRealDeleta != 25)
      {
        EEPROM.write(ENDERECO_DO_NUMERO_DE_REGISTROS, numRegistros - 1);
        EEPROM.write(posicaoRealDeleta, 0);
        EEPROM.write(posicaoRealDeleta + 1, 0);
        EEPROM.write(posicaoRealDeleta + 2, 0);
        EEPROM.write(posicaoRealDeleta + 3, 0);

        //EEPROM.commit();  //Reativar esta função quando o programa entrar em produção

        lcd.setCursor(15,0);
        lcd.print("OK");
      }
      
      posicaoRealDeleta = EEPROM_TAMANHO - 1;
      btnMaisFlag = false;
    }
    /*
    numRegistros = EEPROM.read(ENDERECO_DO_NUMERO_DE_REGISTROS);
    //numRegistros = (numRegistros + 1)%10;
    lcd.setCursor(6,0);
    lcd.print(numRegistros);
    */
  }
}

int8_t localizarPosicaoLivre(void)    //Localiza posição livre para armazenar horário/setor de irrigação
{
  int8_t posicao = 0;
  bool memoriaLivre = true;

  for(int8_t i = EEPROM_INICIO; i < NUMERO_DE_MEMORIAS; i++)
  {
    memoriaLivre = EEPROM.read((i * OFFSET) + 1);

    if(memoriaLivre == false )
    {
      posicao = ((i * OFFSET) + 1);
      return posicao;                 //Se localizar uma posiçãoo livre, retorna com sua posicao
    }
  }
  return 0;                           //Se não localizar posição livre, retorna 0
}

void InitEEPROM(void)               //Roda apenas uma vez, quando módulo for resetado de fábrica/Módulo novo
{
  char posicaoInicial = 0;

  posicaoInicial = EEPROM.read(EEPROM_INICIO);

  if(posicaoInicial == 255)
  {
    for(int i = EEPROM_INICIO; i < EEPROM_TAMANHO; i++)
    {
      EEPROM.write(i,0);
    }
    EEPROM.commit();
  }
  lcd.clear();
}

void BtnPressionadoMais(void)
{
  btnMaisFlag = true;             //Incrementa/Salva ajustes
}

void BtnPressionadoMenos(void)
{
  btnSelectFlag = true;           //Seleção em sub menus
}

void BtnPressionadoEnter(void)
{
  btnEnterFlag = true;            //Seleciona o menu principal
}

void BtnPressionadoVoltar(void)
{
  btnVoltaFlag = true;            //Retorna a tela principal
}