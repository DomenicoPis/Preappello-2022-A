			/*----IMPLEMENTAZIONE DELLE PROCEDURE----*/
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "semafori.h"
#include <math.h>

void produci_tipo_1(BufferMutuaEx * p, int semid, int value) {

	// TODO: completare produzione 1

	    Wait_Sem(semid, SPAZIO_DISPONIBILE);
        Wait_Sem(semid, MUTEX_P);
        
        int index = 0;
        while(index < DIM && p->stato[index] != LIBERO) {
            index++;
        }
       
        p->stato[index] = IN_USO;
        
        Signal_Sem(semid, MUTEX_P);
        
        sleep(1); 
        p->buffer[index] = value;
        
        p->stato[index] = OCC_TIPO1;
        Signal_Sem(semid, MSG_DISP_1);

	printf("[Produttore %d] ho prodotto %d di tipo 1, in posizione %d\n", getpid(), value, index);
	

}

void produci_tipo_2(BufferMutuaEx * p, int semid, int value) {

	// TODO: completare produzione 2 

	    Wait_Sem(semid, SPAZIO_DISPONIBILE);
        Wait_Sem(semid, MUTEX_P);
        

        int index = 0;
        while(index < DIM && p->stato[index] != LIBERO) {
            index++;
        }
       
        p->stato[index] = IN_USO;
        
        Signal_Sem(semid, MUTEX_P);
        
        sleep(1); 
        p->buffer[index] = value;
        
        p->stato[index] = OCC_TIPO2;
        Signal_Sem(semid, MSG_DISP_2);

	printf("[Produttore %d] ho prodotto %d di tipo 2, in posizione %d\n", getpid(), value, index);

}

void consuma_tipo_1(BufferMutuaEx * p, int semid) {

	// TODO: completare consumo 1

	Wait_Sem(semid, MSG_DISP_1);
    Wait_Sem(semid, MUTEX_C);
        
    int index = 0;
    while(index < DIM && p->stato[index] != OCC_TIPO1) {
		index++;
    }
        
    p->stato[index] = IN_USO;
        
    Signal_Sem(semid, MUTEX_C);
        
    sleep(1);
    int value = p->buffer[index];
        
    p->stato[index] = LIBERO;
    Signal_Sem(semid, SPAZIO_DISPONIBILE);

	printf("[Consumatore %d] consumo elemento %d di tipo 1 in posizione %d\n",getpid(), value, index);	

}

void consuma_tipo_2(BufferMutuaEx * p, int semid) {
	
	// TODO: completare consumo 2

	Wait_Sem(semid, MSG_DISP_2);
    Wait_Sem(semid, MUTEX_C);
        
    int index = 0;
    while(index < DIM && p->stato[index] != OCC_TIPO2) {
        index++;
    }
        
    p->stato[index] = IN_USO;
        
	Signal_Sem(semid, MUTEX_C);
        
    sleep(1);
    int value = p->buffer[index];
        
    p->stato[index] = LIBERO;
    Signal_Sem(semid, SPAZIO_DISPONIBILE);

	printf("[Consumatore %d] consumo elemento %d di tipo 2 in posizione %d\n",getpid(), value, index);	

}
