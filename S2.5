#include <iostream>
#include <ostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <chrono>
#include <random>
#include <sys/wait.h>


#include "memocomp.hpp"
#include "colamsg.hpp"
#include "semaforo.hpp"

using namespace std;


#define TAMAGNO_CAD 20

const int refresco = 1000000;
const int espera = 1000000;


//------------------Abrimos cola de mensajes para escribir----------------//
colamsg colaNvl("colaNivel", 0, RDWR);
//------------------Creación memoria compartida----------------------------//
memocomp memo_depo ("memo_depo", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_PID ("memo_PID", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_Ninicial ("memo_Ninicial", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));

//-------------------Funciones del sensor-------------------------------------//
double lecturaAleatoria(const double valorSenial, double sigma); //Se encargar de generar de forma aleatoria el ruido
void trat_sig(int);// se encarga de aplicar el ruido a la señal
void cierre (int);// se encarga de cerrar los diferentes procesos que implican la ejecución

//------------Creamos el semáforo para sincronizar con deposito---------------//
semaforo s1("sincro",20);


int main()
{
  int sema = s1.get();
  int i=0;
  signal(SIGINT, cierre);// Si pulsamos ^C nos aseguramos de cerrar todas las
                          //memorias compartidas

//-----------------Vinculamos las memorias compartidas a un puntero-----------//
  static char *nvl_now, *pid, *Ninicial;
  nvl_now = (char *) memo_depo.getPointer();
  pid = (char *) memo_PID.getPointer();
  Ninicial = (char *) memo_Ninicial.getPointer();

// Lo que recibo primero del depósito(que son el valor del PID y después el
// nivel inicial del depósito lo mando por la cadena de caracteres hacia control
  do
  {
    if (i==0)
     {
       cout << "Esperando conexion con deposito";
       if (i>0)
       {
         cout << "-";
       }
     }
       usleep(100000);
  }
  while(sema==20);
  i = 0;
  cout << "> PID del deposito recibida" << endl;
  char splbrPID;
  splbrPID = *pid;
  char *sPuntero;
  sPuntero = &splbrPID;
  colaNvl.send( sPuntero, TAMAGNO_CAD*sizeof(char));//Primero mando el PID
  cout <<"Se acaba de mandar el PID (" << pid << ") al programa de control";
  cout << endl;


  do
  {
    if (i==0)
     {
       cout << "Esperando conexion con deposito";
       if (i>0)
       {
         cout << "-";
       }
     }
     usleep(100000);
  }
  while(sema==21);
  cout << "> Valor inicial del deposito recibida" << endl;

  char plbr_Ninicial;
  plbr_Ninicial = *Ninicial;
  char *plbr_Ptr;
  plbr_Ptr = &plbr_Ninicial;
  colaNvl.send(plbr_Ptr, TAMAGNO_CAD*sizeof(char)); //Mando el valor inicial del depósito
  cout <<"Se acaba de mandar el dato inicial(" << Ninicial << ") al programa de control" << endl;

  int deposito = 10;
  do {

    char plbr_deposito;
    plbr_deposito = *nvl_now;
    char *plbr_de_ptr;
    plbr_de_ptr = &plbr_deposito;
    colaNvl.send(plbr_de_ptr, TAMAGNO_CAD*sizeof(char));
    cout << "Se acaba de mandar el valor del deposito" << endl;




  } while(deposito!=0);

cout << fixed << setprecision(5);


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
  cout << "-> Semáforos cerrados" << endl;

  cout << "Cerrando las colas de mensajes";
  colaNvl.close();
  cout << "-> Colas de mensajes cerradas" << endl;


  cout << "Destrucción de las memorias compartidas";
  memo_depo.unlink();
  cout << "-";
  memo_PID.unlink();
  cout << "-";
  memo_Ninicial.unlink();
  cout << "-> Se han destruido las memorias compartidas" << endl;

  cout << "Destruccion de los semoforos";
  s1.unlink();
  cout << "-> Se ha destruido el semaforo" << endl;

}



double lecturaAleatoria(const double valorSenial, double sigma)
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::normal_distribution<double> N(0.0, sigma);//media(0.0) y desv de la distrib Normal
    double ruido = N(rng);

    return (valorSenial+ruido);
}
