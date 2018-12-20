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
int PID;


//----------Creación de las colas de mensajes-----//
colamsg colaNvl("colaNivel", CREAT, RDONLY, TAMAGNO_CAD*sizeof(char));


//---------Funciones de control------------//

void cierre(int);
void bomba(float, int); // Manda la señal de activar o desactivar la bomba


float Nmax, Nmin, escape=0;




int main (int argc,char *argv[])
{
    system("/usr/bin/clear"); //limpia la pantalla del terminal
    cout << "\n \t \033[1;4;37;40mPROGRAMA SISTEMA DE CONTROL DEL DEPÓSITO\033[0m";
    cout << endl;

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
    cout << "\nSe tomaran como límites del intervalo de seguridad (";
    cout << Nmin << "%,";
    cout << Nmax << "%)" << endl;
    signal(SIGINT, cierre);
    float nivel, Ninicial;
    int cerrado = 0;


    char cad_nvl[TAMAGNO_CAD];



    cout << "Conectando con el sensor" << endl;


    colaNvl.receive(cad_nvl, TAMAGNO_CAD*sizeof(char));//Recibe el pid de la bomba
    PID = (int)atof(cad_nvl);


    cout << "Se ha conectado con el sensor (PID " << PID << " )." << endl;

    colaNvl.receive(cad_nvl, TAMAGNO_CAD*sizeof(char));//Recibe el nivel inicial del DEPOSITO
    Ninicial = atof(cad_nvl); // Transformamos el
    nivel = Ninicial; // Hago que el primer valor de nivel sea el qeu asignamos en el depósito
    bomba(nivel, PID);


    while (escape==0)
    {


        colaNvl.receive(cad_nvl, TAMAGNO_CAD*sizeof(char));
        //cout << "Recibida cadena del sensor:  " << cad_nvl << endl;


        //Transformamos los valores de la cadena a float
        nivel = atof(cad_nvl);
        if (nivel != 71959141)
        {
          system("/usr/bin/clear"); //limpia la pantalla del terminal
          cout << "\n \t \033[1;4;37;40mPROGRAMA SISTEMA DE CONTROL DEL DEPÓSITO\033[0m";

          cout <<"\nNivel inicial:" << Ninicial << "%" << endl;
          cout << " Volumen de seguridad (" << Nmin <<"%," << Nmax << "%)" << endl;

          if (nivel <= Nmax && nivel >=Nmin)
          {
            cout << " Volumen actual: ";
            printf ("\033[42m %f\033[0m", nivel);
            cout << "%" << endl;
          }
          else if (nivel>=Nmax || nivel <= Nmin)
          {
            cout << " Volumen actual: ";
            printf ("\033[41m %f \033[0m", nivel);
            cout << "%" << endl;
          }
          cout.flush();
          bomba(nivel, PID);
          sleep(1);

        }

        else if (nivel == 71959141 && cerrado == 0){
          cout << "\n Se han cerrado programas necesarios para el correcto funcionamiento" << endl;
          cout << " A continuación se cerraran los programas restantes" << endl;

          cierre (PID);
          cerrado = 1;
        }



     }

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
    escape = 1;


    kill (PID, SIGTERM);
    usleep(1000);
    //system("/usr/bin/clear"); //limpia la pantalla del terminal
    cout << "\n Cerrando colas ";
    colaNvl.close();
    cout << "-";
    colaNvl.unlink();
    cout << "-> Colas cerradas y destruidas" << endl;

    cout << "\n\n\tPrograma control finalizado." << endl << endl;
}
