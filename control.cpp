//-------------------------------------------------------------------------//
//		Asignatura INFORMATICA INDUSTRIAL                          //
//		Grado en Ingeniería Electrónica y Automática               //
///		Grupo IEIA31                                                ///
///-------------------------------------------------------------------------///
///			Programa de control sobre el sensor y la bomba      ///
///                                                                         ///
///-------------------------------------------------------------------------///
///				Autor                                       ///
/// 	Álvaro Cantera Álvarez (71959141F)                                  ///
///                                                                         ///
///	Fecha: Viernes 14-12-2018                                           ///
///-------------------------------------------------------------------------///

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


#include "colamsg.hpp"
#include "semaforo.hpp"
using namespace std;


//-----------Definimos las constantes---------------//
#define TAMAGNO_CAD 15
const int TIMEOUT = 10; // tiempo de espera de la alarma en segundos




//----------Creación de las colas de mensajes-----//
colamsg colaNvl("colaNivel", CREAT, RDONLY, TAMAGNO_CAD*sizeof(char));


//---------Funciones de control------------//

void cierre(int);
void bomba(float, int); // Manda la señal de activar o desactivar la bomba


float Nmax, Nmin, escape=0;




int main (int argc,char *argv[])
{
    if (argc==3)
    {
      char *d1 = argv[1];
      Nmin = atof(d1);
      char *d2 = argv[2];
      Nmax = atof(d2);
    }
    else
    {
      Nmin = 35;
      Nmax = 65;


    }
    signal(SIGINT, cierre);
    float nivel;
    float Ninicial;
    int PID;
    char cad_nvl[TAMAGNO_CAD];


    cout << "\t SISTEMA DE CONTROL DEL DEPOSITO"<< endl;
    cout << "Conectando con el sensor" << endl;


    colaNvl.receive(cad_nvl, TAMAGNO_CAD*sizeof(char));//Recibe el pid de la bomba
    PID = (int)atof(cad_nvl);


    cout << "Se ha conectado con el sensor (PID " << PID << " )." << endl;

    colaNvl.receive(cad_nvl, TAMAGNO_CAD*sizeof(char));//Recibe el nivel inicial del DEPOSITO
    Ninicial = atof(cad_nvl); // Transformamos el
    nivel = Ninicial; // Hago que el primer valor de nivel sea el qeu asignamos en el depósito
    bomba(nivel, PID);


    do
    {


        colaNvl.receive(cad_nvl, TAMAGNO_CAD*sizeof(char));
        cout << "Recibida cadena del sensor:  " << cad_nvl << endl;

        //Transformamos los valores de la cadena a float
        nivel = atof(cad_nvl);
        //system("/usr/bin/clear"); //liumpiamos la pantalla
        cout << "\t SISTEMA DE CONTROL DEL DEPOSITO"<< endl;
        cout << "Nivel inicial:" << Ninicial << "%" << endl;
        cout << "Volumen de seguridad (" << Nmin <<"%," << Nmax << "%)" << endl;
        cout << "Volumen actual: " << nivel <<"%"<< endl;
        bomba(nivel, PID);



        sleep(1);


     }
     while (escape==0);
}


void bomba (float vlr_nivel, int PID)
{
    if (vlr_nivel > Nmax)
    {
      kill(PID, SIGUSR1);
    }
    else if (vlr_nivel < Nmin)
    {
      kill(PID, SIGUSR2);
    }
}

void cierre (int sign)
{

  cout << "SISTEMA CONTROL" << endl;
    escape = 1;
    //system("/usr/bin/clear"); //limpia la pantalla del terminal
    cout << "Cerrando colas ";
    colaNvl.close();
    cout << "-";
    colaNvl.unlink();
    cout << "-> Colas cerradas y destruidas";


    cout << "->Semaforo cerrado y destruido" << endl;



    cout << "Programa finalizado." << endl << endl;
}
