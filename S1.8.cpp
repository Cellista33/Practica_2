#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


#include "memocomp.hpp"
#include "colamsg.hpp"

using namespace std;


#define TAMAGNO_CAD 20

const int refresco = 1000000;
//------------------Abrimos cola de mensajes para escribir----------------//
colamsg colaNvl("colaNivel", OPEN, WRONLY);
//------------------Creación memoria compartida----------------------------//
memocomp memo_depo ("memoria_deposito", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_PID ("PID_depo", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));
memocomp memo_ini ("nvl_inicial", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));

//-------------------Funciones del sensor-------------------------------------//
double lecturaAleatoria(const double valorSenial, double sigma); //Se encargar de generar de forma aleatoria el ruido
void trat_sig(int);// se encarga de aplicar el ruido a la señal
void cierre (int);// se encarga de cerrar los diferentes procesos que implican la ejecución




int main()
{
  //signal(SIGINT, SIG_IGN) // ignoramos ctrl+c porque se cierra todo desde el programa control
//-----------------Vinculamos las memorias compartidas a un puntero-----------//
  char *memoria, *pid, *Ninicial;
  memoria = (char *) memo_depo.getPointer();
  pid = (char *) memo_PID.getPointer();
  Ninicial = (char *) memo_Ninicial.getPointer();



  char splbrPID[TAMAGNO_CAD];
  int pid = getpid();
  sprintf(splbrPID, "%d", pid);
  char *sPuntero;
  sPuntero = &splbrPID[0];
  colaNvl.send( sPuntero, TAMAGNO_CAD*sizeof(char));//Primero mando el PID
  cout <<"Se acaba de mandar el PID (" << pid << ") al programa de control";
  cout << endl;



  char plbr_Ninicial[TAMAGNO_CAD];
  int Ninicial = 50; //Modificar para mandar por memoria compartida dando el valor deposito
  sprintf(plbr_Ninicial, "%d", Ninicial);
  char *plbr_Ptr;
  plbr_Ptr = &plbr_Ninicial[0];
  colaNvl.send(plbr_Ptr, TAMAGNO_CAD*sizeof(char)); //Mando el valor inicial del depósito
  cout <<"Se acaba de mandar el dato inicial(" << Ninicial << ") al programa de control" << endl;

  int deposito=10;
  do {
    deposito=deposito+1;

    char plbr_deposito[TAMAGNO_CAD];
    sprintf (plbr_deposito, "%d", deposito);
    char *plbr_de_ptr;
    plbr_de_ptr = &plbr_deposito[0];
    colaNvl.send(plbr_de_ptr, TAMAGNO_CAD*sizeof(char));
    cout << "Se acaba de mandar el valor del deposito" << endl;




  } while(deposito!=0);

cout << fixed << setprecision(5);


}



double lecturaAleatoria(const double valorSenial, double sigma)
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::normal_distribution<double> N(0.0, sigma);//media(0.0) y desv de la distrib Normal
    double ruido = N(rng);

    return (valorSenial+ruido);
}
