//*****************************************************************************
// Universidad del Valle de Guatemala
// Simulación de Circuitos y Fabricación de PCBs
// Sofía Salguero - 19236 - Ing. Tecnología de Audio
//*****************************************************************************

//*****************************************************************************
//Librerias
//*****************************************************************************
#include <Arduino.h>     //libreria arduino
#include <TimerOne.h>    //libreria para Timer
#include "Display7Seg.h" //libreria para display 7 segmentos

//*****************************************************************************
//Definicion etiquetas
//*****************************************************************************
#define BUZZER 3 // Buzzer
#define BUTTON 2 //Pushbutton
#define POT A0   // Potenciometro

//Leds
#define Led1 5
#define Led2 6
#define Led3 9
#define Led4 10

//Display 7 segmentos
#define a 19
#define b 18
#define c 17
#define d 16
#define e 15
#define f 13
#define g 12
#define dP 11

#define display1 7
#define display2 8
#define display3 4

//*****************************************************************************
//Varibles globales
//*****************************************************************************
int bpm = 40;   // BPM
int COMPAS = 0; // Compases: 0/4, 1/4, 2/4, 3/4, 4/4
int ms_ON;      //Buzzer on
int ms_OFF;     //Buzzer off

//Dividir BPM para display
int centena = 0;
int decena = 0;
int unidad = 0;

int contadorTimer = 0; //contador para display

int contador = 0; //contador para leds

int buzzState = LOW;              //Estado del Buzzer
unsigned long previousMillis = 0; //Milis previos
//*****************************************************************************
//Prototipos de funcion
//*****************************************************************************
void compas(void);
void ISR_ON(void);
void BPM(void);
void display7Seg(int contadorTimer);
void Leds(void);

//*****************************************************************************
//Configuracion
//*****************************************************************************
void setup()
{
  Serial.begin(9600);

  //Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, buzzState);

  //Leds
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);

  //Timer para displays
  Timer1.initialize(7000); //Se actualizan cada 7 milisegundos
  Timer1.attachInterrupt(ISR_ON);

  //Interrupcion para boton
  attachInterrupt(digitalPinToInterrupt(BUTTON), compas, RISING);

  //Displays
  configurarDisplay(a, b, c, d, e, f, g, dP);
  pinMode(display1, OUTPUT);
  pinMode(display2, OUTPUT);
  pinMode(display3, OUTPUT);

  digitalWrite(display1, LOW);
  digitalWrite(display2, LOW);
  digitalWrite(display3, LOW);

  desplegar7Seg(0);
}

//*****************************************************************************
//Loop principal
//*****************************************************************************
void loop()
{
  BPM();                      //Tomar valor BPM
  display7Seg(contadorTimer); //Mostrar BPM en los displays
  Leds();                     //Encender Leds

  //Encender y apagar Buzzer usando milis
  unsigned long currentMillis = millis();

  if ((buzzState == HIGH) && (currentMillis - previousMillis >= ms_ON))
  {
    buzzState = LOW;                 // Apagarlo
    previousMillis = currentMillis;  // Recordar tiempo de acción
    digitalWrite(BUZZER, buzzState); // Update estado de buzzer
  }
  else if ((buzzState == LOW) && (currentMillis - previousMillis >= ms_OFF))
  {
    buzzState = HIGH;                 // Encenderlo
    previousMillis = currentMillis;  // Recordar tiempo de acción
    digitalWrite(BUZZER, buzzState); // Update estado de buzzer

    contador++; //Indicar en qué tiempo se encuentra el compás

    if (contador > 4) //No puede sobrepasar 4 tiempos
    {
      contador = 1;
    }
  }
}

//*****************************************************************************
// Función para interrupción de contadorTimer para Displays
//*****************************************************************************
void ISR_ON(void)
{
  contadorTimer++; //aumenta el contador de timer

  if (contadorTimer > 2) //si es mayor a 2 regresa a cero
  {
    contadorTimer = 0;
  }
}

//*****************************************************************************
// Aumentar Compás
//*****************************************************************************
void compas(void)
{
  COMPAS++;

  if (COMPAS > 4)
  {
    COMPAS = 1;
  }
}
//*****************************************************************************
// Función para configurar BPM con potenciómetro
//*****************************************************************************
void BPM(void)
{
  bpm = map(analogRead(POT), 0, 1023, 40, 240);
  // lectura de 10 bits
  // 40 = BPM minimo
  // 240 = MPM máximo

  ms_ON = 6000 / bpm;   // Conversion a ms encendido
  ms_OFF = 54000 / bpm; //conversion a ms apagado

  //Dividir valor para displays
  int temp = bpm;
  centena = temp / 100.0;
  temp = temp - centena * 100.0;
  decena = temp / 10.0;
  temp = temp - decena * 10.0;
  unidad = temp;
}

//*****************************************************************************
// Función leds
//*****************************************************************************

void Leds(void)
{
  switch (contador)
  {
  case 0:
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    digitalWrite(Led4, LOW);

    break;

  case 1:
    digitalWrite(Led1, HIGH);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    digitalWrite(Led4, LOW);

    break;

  case 2:
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, HIGH);
    digitalWrite(Led3, LOW);
    digitalWrite(Led4, LOW);

    break;

  case 3:
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, HIGH);
    digitalWrite(Led4, LOW);

    break;

  case 4:
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    digitalWrite(Led4, HIGH);

    break;

  default:
    noTone(BUZZER);
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    digitalWrite(Led4, LOW);
    break;
  }
}

//*****************************************************************************
//Función para encender displays
//*****************************************************************************
void display7Seg(int contadorTimer)
{
  switch (contadorTimer)
  {
  case 0:
    //desplegar decenas
    digitalWrite(display1, HIGH);
    digitalWrite(display2, LOW);
    digitalWrite(display3, LOW);
    desplegarPunto(0);
    desplegar7Seg(centena);
    break;

  case 1:
    //Desplegar unidades
    digitalWrite(display1, LOW);
    digitalWrite(display2, HIGH);
    digitalWrite(display3, LOW);
    desplegarPunto(1);
    desplegar7Seg(decena);
    break;

  case 2:
    //Desplegar decimal
    digitalWrite(display1, LOW);
    digitalWrite(display2, LOW);
    digitalWrite(display3, HIGH);
    desplegarPunto(0);
    desplegar7Seg(unidad);
    break;

  default:
    break;
  }
}