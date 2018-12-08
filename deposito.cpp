#include <iostream>
#include <ostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/wait.h>


#include "memocomp.hpp"
#include "semaforo.hpp"

using namespace std;
#define TAMAGNO_CAD 20



//-----------Creación de semáforos para sincro con Sensor---------------------//
semaforo s1("sincro");
//------------Declaración memorias compartidas--------------//
memocomp memo_depo("memo_depo", OPEN, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_PID ("memo_PID", OPEN, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_Ninicial ("memo_Ninicial", OPEN, RDWR, TAMAGNO_CAD*sizeof(char));



//-----------Declaración de las funciones que interactuan con la bomba--------//
void on_bomba(int);
void off_bomba(int);
void cierre (int)

//----------Variables globales----------//
//Aunque sea poco seguro denomino estas variables como globales y asigno el
//puntero a la memoria compartida de esta forma para hacerlo más rápido//
double nvl_actual;
static char *nvl_now, nvl_now_paso;



int main()
{

  int sema = s1.get();
  nvl_now = (char *) memo_depo.getPointer();
  signal(SIGUSR1, on_bomba);
  signal(SIGUSR2, off_bomba);
  signal (SIGINT, cierre);


  //------------------vinculamos las memorias a un puntero--------------//
  static char  *pid, *Ninicial, pid_paso, Ninicial_paso;

  pid = (char *) memo_PID.getPointer();
  Ninicial = (char *) memo_Ninicial.getPointer();
  //------Mandamos el PID del deposito  como un char por la memoria compartida//
  int pidI = getpid();
  sprintf( *pid, "%d", pidI); //convertimos el nombre del PID en un char para
  //*pid = pid_paso;                          // Para mandarlo por la memoria compartida
  s1.up(); //Subimos el semáforo para que sensor sepa que ha recibido el pid

  double NinicialI = 50;
  sprintf (Ninicial, "%lf", NinicialI);
  s1.up(); //Subimos el semáforo para que sensor sepa que ha recibido el Ninicial

  *Ninicial = Ninicial_paso;
  nvl_actual = NinicialI;
  //---------Señales recibidas de control-------------------------//
  while (1)
  {
        pause();
  }

  return (0);
}

void on_bomba (int sig)
{
  nvl_actual = nvl_actual - 3.5;
  sprintf(nvl_now_paso, "%lf", nvl_actual);
  *nvl_now = nvl_now_paso
}

void off_bomba(int sig)
{
  nvl_actual = nvl_actual + 0.5;
  sprintf(nvl_now_paso, "%lf", nvl_actual);
  *nvl_now = nvl_now_paso;
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
}
