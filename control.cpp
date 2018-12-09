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


#include "colamsg.hpp"
#include "semaforo.hpp"
using namespace std;


//-----------Definimos las constantes---------------//
#define TAMAGNO_CAD 20
const int wait4 = 2000000; // en MICROSEGUNDOS
const int refresco = 1000000; // cada esgundo habra un dato nuevo, por lo tanto este es el tiempo de refreco
const int TIMEOUT = 10; // tiempo de espera de la alarma en segundos
double PID = 0;



//----------Creación de las colas de mensajes-----//
colamsg colaNvl("ColaNivel", CREAT, RDWR, TAMAGNO_CAD*sizeof(char));

//---------Funciones de control------------//

void cierre(int);
void bomba(double, double); // Manda la señal de activar o desactivar la bomba


double Nmax, Nmin;




int main ()
{
    signal(SIGINT, cierre);
    double nivel;
    double Ninicial;
    string nivel_sr;
    char cadena_nivel[TAMAGNO_CAD];
    //signal(SIGALRM, Ctrl_c);  // WatchDog
    Nmin = 35;
    Nmax = 65;
    Ninicial = 50;

    cout << fixed << setprecision(5);


    colaNvl.receive(cadena_nivel, TAMAGNO_CAD*sizeof(char));//Recibe el pid de la bomba
    PID = atoi(cadena_nivel);
    colaNvl.receive(cadena_nivel, TAMAGNO_CAD*sizeof(char));//Recibe el nivel inicial del DEPOSITO
    Ninicial = atoi(cadena_nivel); // Transformamos el
    cout << "\t SISTEMA DE CONTROL DEL DEPOSITO"<< endl;
    cout << "Se ha conectado con el sensor (PID " << PID << " )." << endl;
    usleep(wait4);
    nivel = Ninicial; // Hago que el primer valor de nivel sea el qeu asignamos en el depósito

    do
    {


        colaNvl.receive(cadena_nivel, TAMAGNO_CAD*sizeof(char));
        cout << "Recibida cadena del sensor:  " << cadena_nivel << endl;

        //Transformamos los valores del string a double
        nivel = atof(cadena_nivel);
        system("/usr/bin/clear"); //liumpiamos la pantalla
        cout << "\t SISTEMA DE CONTROL DEL DEPOSITO"<< endl;
        cout << "Nivel inicial:" << Ninicial << "%" << endl;
        cout << "Volumen de seguridad (" << Nmin <<"%," << Nmax << "%)" << endl;
        cout << "Volumen actual: " << nivel <<"%"<< endl;
        bomba(nivel, PID);



        usleep(refresco);


     }
     while (cadena_nivel[0]!='a');

     cierre;
     return(0);
}


void bomba (double vlr_nivel, double PID)
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

void cierre (int signal)
{

    system("/usr/bin/clear"); //limpia la pantalla del terminal
    cout << "SISTEMA CONTROL" << endl;
    cout << "Cerrando colas ";
    colaNvl.close();
    cout << "-";
    colaNvl.unlink();
    cout << "-> Colas cerradas y destruidas";



    cout << "Programa finalizado." << endl << endl;
}
