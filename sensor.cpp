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


#define TAMAGNO_CAD 15

int escape = 0;


//------------------Abrimos cola de mensajes para escribir----------------//
colamsg colaNvl("colaNivel", OPEN, WRONLY, TAMAGNO_CAD*sizeof(char));
//------------------Creación memoria compartida----------------------------//
memocomp memo_depo ("memo_depo", CREAT, RDWR, sizeof(float));
memocomp memo_PID ("memo_PID", CREAT, RDWR, sizeof(float));
memocomp memo_Ninicial ("memo_Ninicial", CREAT, RDWR, sizeof(float));

//-------------------Funciones del sensor-------------------------------------//
double lecturaAleatoria(const double valorSenial, double sigma); //Se encargar de generar de forma aleatoria el ruido
void trat_sig(int);// se encarga de aplicar el ruido a la señal
void cierre (int);// se encarga de cerrar los diferentes procesos que implican la ejecución

//------------Creamos el semáforo para sincronizar con deposito---------------//
semaforo s1("sema",0);




int main(void)
{
  int sema = s1.get();
  char cad_nvl[TAMAGNO_CAD];
  signal(SIGINT, cierre);// Si pulsamos ^C nos aseguramos de cerrar todas las
                          //memorias compartidas

//-----------------Vinculamos las memorias compartidas a un puntero-----------//
  float *nvl_now_punt, *pid_punt, *Ninicial_punt;
  nvl_now_punt = (float *) memo_depo.getPointer();
  pid_punt = (float *) memo_PID.getPointer();
  Ninicial_punt = (float *) memo_Ninicial.getPointer();

// Lo que recibo primero del depósito(que son el valor del PID y después el
// nivel inicial del depósito lo mando por la cadena de caracteres hacia control
  cout << "Esperando conexion con deposito--->";
  s1.down();

  cout << "Conexion con el deposito establecida" << endl;


  float pid;
  pid = *pid_punt;
  sprintf(cad_nvl, "%f", pid);
  cout << "PID del deposito recibido" << endl;

  colaNvl.send(cad_nvl, TAMAGNO_CAD*sizeof(char));
  cout << "Se acaba de mandar el PID(" << pid <<") al programa de control" << endl;



  float Ninicial;
  Ninicial = *Ninicial_punt;

  sprintf(cad_nvl, "%f", Ninicial);
  cout << "Nivel inicial del deposito recibido" << endl;

  colaNvl.send(cad_nvl, TAMAGNO_CAD*sizeof(char)); //Mando el valor inicial del depósito

  cout <<"Se acaba de mandar el nivel inicial(" << Ninicial << ") al programa de control" << endl;


  float nivel_actual;
  do {

    nivel_actual = *Ninicial_punt;
    sprintf(cad_nvl, "%f", nivel_actual);
    colaNvl.send(cad_nvl, TAMAGNO_CAD*sizeof(char));
    cout << "Se acaba de mandar el valor del deposito: " << nivel_actual << endl;
    sleep(1);






  } while(escape == 0);


return (0);
}

void cierre (int signal)
{
  escape = 1;
  cout << "\n Cerrando las memorias compartidas";
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
  cout << "-> Se han destruido los semaforos" << endl;

}



double lecturaAleatoria(const double valorSenial, double sigma)
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::normal_distribution<double> N(0.0, sigma);//media(0.0) y desv de la distrib Normal
    double ruido = N(rng);

    return (valorSenial+ruido);
}
