#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#include "header.h"


int main(){
	pid_t pid_ex,pid;
	int st,i;
	int shmid,semid;

	key_t key_shm,key_sem;

	key_shm = IPC_PRIVATE;
	key_sem = IPC_PRIVATE;

	shmid = shmget(key_shm, sizeof(BufferMutuaEx), IPC_CREAT | 0664);
	if(shmid < 0){
		perror("errore shmget");
		exit(1);
	}

	BufferMutuaEx * m = (BufferMutuaEx *) shmat(shmid, NULL, 0);
	if(m == (void *)-1){
		perror("errore shmat");
		exit(1);
	}

	for(int i=0; i<DIM; i++) {
        m->stato[i] = LIBERO;
    }

	semid = semget(key_sem, 5, IPC_CREAT | 0664); 
	if(semid < 0){
		perror("errore semget");
		exit(1);
	}
	
	printf("[MASTER] - Shmid : %d, Semid: %d\n",shmid,semid);

	// TODO: inizializzare semafori

	int err;

	err = semctl(semid, SPAZIO_DISPONIBILE, SETVAL, DIM);
    if (err < 0) { perror("Init SPAZIO"); exit(1); }

    err = semctl(semid, MSG_DISP_1, SETVAL, 0); 
    if (err < 0) { perror("Init MSG_1"); exit(1); }

    err = semctl(semid, MSG_DISP_2, SETVAL, 0); 
    if (err < 0) { perror("Init MSG_2"); exit(1); }

    err = semctl(semid, MUTEX_P, SETVAL, 1);
    if (err < 0) { perror("Init MUTEX_P"); exit(1); }

    err = semctl(semid, MUTEX_C, SETVAL, 1);
    if (err < 0) { perror("Init MUTEX_C"); exit(1); }

	srand(time(NULL));

	for(i = 0; i < NPROD; i++){
		pid = fork();
		
		// TODO: ogni figlio produttore (1 e 2) fa PRODS produzioni

		if(pid == 0){
            // Siamo nel figlio
            srand(getpid()); // Per avere valori veramente casuali
            
            // I figli con indice pari producono tipo 1, i dispari tipo 2
            if (i % 2 == 0) {
                // Produttore tipo 1
                for(int j = 0; j < PRODS; j++) {
                    produci_tipo_1(m, semid, rand() % 100);
                }
            } else {
                // Produttore tipo 2
                for(int j = 0; j < PRODS; j++) {
                    produci_tipo_2(m, semid, rand() % 100);
                }
            }
            exit(0);
        } else if(pid < 0){
            perror("errore fork produzione");
            exit(1);
        }

	}

	for(i = 0; i < NCONS; i++){
		pid = fork();

		// TODO: ogni figlio consumatore (1 e 2) fa CONS consumi

		if(pid == 0){
            // Siamo nel figlio
            // Il primo consuma tipo 1, il secondo consuma tipo 2
            if (i == 0) {
                // Consumatore tipo 1
                for(int j = 0; j < CONS; j++) {
                    consuma_tipo_1(m, semid);
                }
            } else {
                // Consumatore tipo 2
                for(int j = 0; j < CONS; j++) {
                    consuma_tipo_2(m, semid);
                }
            }
            exit(0);
        } else if(pid < 0){
            perror("errore fork consumazione");
            exit(1);
        }
		
	}
	
	// TODO: attesa completamento figli

	int status;

	for(int i=0; i<6; i++){
		wait(&status);
	}

	// TODO: rimozione strutture 

	shmdt(m);
	shmctl(shmid, IPC_RMID, NULL);

	semctl(semid, 0, IPC_RMID);

	return 0;
}

