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





//------------------Abrimos cola de mensajes para escribir----------------//
colamsg colaNvl("colaNivel", OPEN, WRONLY, TAMAGNO_CAD*sizeof(char));
//------------------Creación memoria compartida----------------------------//
memocomp memo_depo ("memo_depo", CREAT, RDWR, sizeof(int));
memocomp memo_PID ("memo_PID", CREAT, RDWR, sizeof(float));
memocomp memo_Ninicial ("memo_Ninicial", CREAT, RDWR, sizeof(float));

//-------------------Funciones del sensor-------------------------------------//
double lecturaAleatoria(const double valorSenial, double sigma); //Se encargar de generar de forma aleatoria el ruido
void trat_sig(int);// se encarga de aplicar el ruido a la señal
void cierre (int);// se encarga de cerrar los diferentes procesos que implican la ejecución

//------------Creamos el semáforo para sincronizar con deposito---------------//
semaforo s1("sema",0);




int escape = 0;




int main(void)
{


  double segnal_ruido, sigma=0.01;
  system("/usr/bin/clear"); //limpia la pantalla del terminal
  cout << "\t \033[1;4;37;40mPROGRAMA SENSOR\033[0m" << endl;
  char cad_nvl[TAMAGNO_CAD];
  int sema = s1.get();
  signal(SIGINT, cierre);// Si pulsamos ^C nos aseguramos de cerrar todas las
                         //memorias compartidas

//-----------------Vinculamos las memorias compartidas a un puntero-----------//
  float  *pid_punt, *Ninicial_punt;

  int *nvl_now_punt;
  nvl_now_punt = (int *) memo_depo.getPointer();
  *nvl_now_punt = 0;


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
    system("/usr/bin/clear"); //limpia la pantalla del terminal
    cout << "\n \t \033[1;4;37;40mPROGRAMA SENSOR\033[0m" << endl;

    nivel_actual = *Ninicial_punt;

    segnal_ruido = lecturaAleatoria(nivel_actual, sigma);
    sprintf(cad_nvl, "%f", segnal_ruido);
    colaNvl.send(cad_nvl, TAMAGNO_CAD*sizeof(char));
    cout << "\nNivel actual del depósito: " << nivel_actual<< "%" << endl;
    cout << "Nivel actual del depósito con ruido: " << segnal_ruido<< "%" << endl;

    cout.flush();
    sleep(1);


    if (*nvl_now_punt==71959141) {
      std::cout << "Se ha cerrado el programa deposito" << '\n';
      std::cout << "A continuacion se cerraran el resto." << '\n';
      sleep(2);
      int comprobacion = 71959141;
      cierre (comprobacion);
    }





  } while(escape == 0);
}

void cierre (int signal)
{

  int *nvl_now_punt;
  nvl_now_punt = (int *) memo_depo.getPointer();
  char cad_nvl[TAMAGNO_CAD];
  escape = 1;

  if (signal!=71959141)
  {
    *nvl_now_punt = 71959141;
  }

  int nivel_actual = *nvl_now_punt;
  sprintf(cad_nvl, "%d", nivel_actual);
  colaNvl.send(cad_nvl, TAMAGNO_CAD*sizeof(char));
  sleep(2);



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

  cout << "\n\n\tPrograma sensor finalizado.\n" << endl;

}



double lecturaAleatoria(const double valorSenial, double sigma)
{
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::normal_distribution<double> N(0.0, sigma);//media(0.0) y desv de la distrib Normal
    double ruido = N(rng);

    return (valorSenial+ruido);
}
