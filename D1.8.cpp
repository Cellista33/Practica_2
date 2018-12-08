#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "memocomp.hpp"

using namespace std;
#define TAMAGNO_CAD 20


//------------Declaración memorias compartidas--------------//
memocomp memo_depo("memo_depo", OPEN, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_PID ("memo_PID", OPEN, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_Ninicial ("memo_Ninicial", OPEN, RDWR, TAMAGNO_CAD*sizeof(char));

//-----------Declaración de las funciones que interactuan con la bomba--------//
void on_bomba();
void off_bomba();

//----------Variables globales----------//
//Aunque sea poco seguro denomino estas variables como globales y asigno el
//puntero a la memoria compartida de esta forma para hacerlo más rápido//
double nvl_actual;
char *nvl_now, nvl_now_paso;



int main()
{
  nvl_now = (char *) memo_depo.getPointer();
  signal(SIGUSR1, on_bomba);
  signal(SIGUSR2, off_bomba);

  //------------------vinculamos las memorias a un puntero--------------//
  char  *pid, *Ninicial, pid_paso, Ninicial_paso;

  pid = (char *) memo_PID.getPointer();
  Ninicial = (char *) memo_Ninicial.getPointer();
  //------Mandamos el PID del deposito  como un char por la memoria compartida//
  int pidI = getPid();
  sprintf( pid_paso, "%d", pidI); //convertimos el nombre del PID en un char para
  *pid = pid_paso;                            // Para mandarlo por la memoria compartida
  double NinicialI = 50;
  sprintf ( Ninicial_paso, "%lf", NinicialI);//convertimos el nivel Ninicial a un char
                                        //Para mandarlo por la memoria compartida
  *Ninicial = Ninicial_paso;
  nvl_actual = NinicialI;
  //---------Señales recibidas de control-------------------------//
  while (1)
  {
        pause();
  }

  return (0);
}

void on_bomba ()
{
  nvl_actual = nvl_actual - 3.5;
  sprintf(nvl_now_paso, "%lf", nvl_actual);
  *nvl_now = nvl_now_paso
}

void off_bomba()
{
  nvl_actual = nvl_actual + 0.5;
  sprintf(nvl_now_paso, "%lf", nvl_actual);
  *nvl_now = nvl_now_paso;
}
