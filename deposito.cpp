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
memocomp memo_depo("memo_depo", OPEN, RDWR, sizeof(int));
memocomp memo_PID ("memo_PID", OPEN, RDWR, sizeof(float));
memocomp memo_Ninicial ("memo_Ninicial", OPEN, RDWR, sizeof(float));



//-----------Declaración de las funciones que interactuan con la bomba--------//
void on_bomba(int);
void off_bomba(int);
void cierre (int);

//----------Variables globales----------//
//Aunque sea poco seguro denomino estas variables como globales y asigno el
//puntero a la memoria compartida de esta forma para hacerlo más rápido//

int escape=0, *nvl_now_punt;
float variacion=0.5;





int main(int argc,char *argv[])
{
  system("/usr/bin/clear"); //limpia la pantalla del terminal
  cout << "\n \t \033[1;4;37;40mPROGRAMA DEPÓSITO\033[0m" << endl;

  signal(SIGUSR1, on_bomba);
  signal(SIGUSR2, off_bomba);
  signal (SIGINT, cierre);
  signal (SIGTERM, cierre);
  float pid, Ninicial;

  if (argc==2)
  {
    char *dato = argv[1];
    Ninicial = atof(dato);
  }

  else
  {
    Ninicial = 50;
  }
  cout << "Se tomara como valor inicial: "<< Ninicial << "%"<< endl;


  //------------------vinculamos las memorias a un puntero--------------//
  float *PID_punt, *Ninicial_punt;

  nvl_now_punt = (int *) memo_depo.getPointer();
  PID_punt = (float *) memo_PID.getPointer();
  Ninicial_punt = (float *) memo_Ninicial.getPointer();
  //------Mandamos el PID del deposito  por la memoria compartida//
  s1.up();



  pid = getpid();
  *PID_punt = pid;
  cout << " PID (" << pid << ") del deposito enviado" << endl;




  *Ninicial_punt = Ninicial;
  cout << "Nivel inicial del deposito enviado" << endl;

  //---------Señales recibidas de control-------------------------//
  while (escape==0)
  {

    system("/usr/bin/clear"); //limpia la pantalla del terminal
    cout << "\n \t \033[1;4;37;40mPROGRAMA DEPÓSITO\033[0m" << endl;
    *Ninicial_punt+=variacion;
    cout <<"\nNivel del deposito: " << *Ninicial_punt << "%"<< endl;
    if(variacion == 0.5)
    {
      cout << "Situación actual de la bomba : ";
      cout << "\033[41mDESACTIVADA\033[0m" << endl;
    }
    else if (variacion == -3.5)
    {
      cout << "Situación actual de la bomba : ";
      cout << "\033[42mACTIVADA\033[0m" << endl;
    }
    cout.flush();

    sleep(1);
    if (*nvl_now_punt==71959141) {
      std::cout << "\nSe ha cerrado el programa sensor" << '\n';
      std::cout << "A continuacion se cerraran el resto." << '\n';
      sleep(2);
      cierre (*nvl_now_punt);
    }

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
  escape =1;
  if (signal!=71959141)
  {
    *nvl_now_punt = 71959141;
  }

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

  cout << "\n\n\tPrograma depósito finalizado." << endl << endl;

}
