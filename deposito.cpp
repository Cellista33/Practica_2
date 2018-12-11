#include <iostream>
#include <ostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <sys/wait.h>


#include "memocomp.hpp"
#include "semaforo.hpp"

using namespace std;
#define TAMAGNO_CAD 20



//-----------Creación de semáforos para sincro con Sensor---------------------//
semaforo s1("sema");
//------------Declaración memorias compartidas--------------//
memocomp memo_depo("memo_depo", OPEN, RDWR, sizeof(float));
memocomp memo_PID ("memo_PID", OPEN, RDWR, sizeof(float));
memocomp memo_Ninicial ("memo_Ninicial", OPEN, RDWR, sizeof(float));



//-----------Declaración de las funciones que interactuan con la bomba--------//
void on_bomba(int);
void off_bomba(int);
void cierre (int);

//----------Variables globales----------//
//Aunque sea poco seguro denomino estas variables como globales y asigno el
//puntero a la memoria compartida de esta forma para hacerlo más rápido//
float nvl_actual;
float *nvl_now_punt;
float *nvl_now;
int escape=0;
float variacion=0.5;


int main(int argc,char *argv[])
{

  signal(SIGUSR1, on_bomba);
  signal(SIGUSR2, off_bomba);
  signal (SIGINT, cierre);
  float pid, Ninicial;

  if (argc==2)
  {
    char *dato = argv[1];
    Ninicial = atof(dato);
  }

  else
  {
    cout << "Se tomara como valor inicial 50" << endl;
    Ninicial = 50;
  }


  //------------------vinculamos las memorias a un puntero--------------//
  float *PID_punt, *Ninicial_punt;

  nvl_now = (float *) memo_depo.getPointer();
  PID_punt = (float *) memo_PID.getPointer();
  Ninicial_punt = (float *) memo_Ninicial.getPointer();
  //------Mandamos el PID del deposito  por la memoria compartida//
  s1.up();

  pid = getpid();
  *PID_punt = pid;
  cout << "> PID del deposito enviado" << pid << endl;




  *Ninicial_punt = Ninicial;
  cout << "Nivel inicial del deposito enviado" << endl;

  //---------Señales recibidas de control-------------------------//
  while (escape==0)
  {
    cout << *Ninicial_punt << endl;
    *Ninicial_punt+=variacion;
        sleep(1);
  }

  return (0);
}

void on_bomba (int sig)
{
  cout << "Estado de la Bomba: activada" << endl;
  variacion=-3.5;
}

void off_bomba(int sig)
{
  cout << "Estado de la Bomba: desactivada" << endl;
  variacion=0.5;
}

void cierre (int signal)
{
  cout << "Cerrando las memorias compartidas";
  memo_depo.cerrar();
  cout << "-";
  memo_PID.cerrar();
  cout << "-";
  memo_Ninicial.cerrar();
  cout << "-> Memorias compartidas cerradas" << endl;

  cout << "Cerrando los semáforos";
  s1.close();
  cout <<"-> Se han cerrado los semáforos";
  escape =1;
}
