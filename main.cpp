// ConsoleApplication1.cpp : Defines the entry point for the console application.
//



#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define HAVE_STRUCT_TIMESPEC
#include "pthread.h"

pthread_mutex_t lock;
bool wyswietlajZnalezioneOkna = false;

struct parameters
{
	int * buffer;
	int dlugoscOkna, progDoPrzekroczenia, startIndex, stopIndex, threadId;
};

int random(int min, int max)
{
	int tmp;
	if (max >= min)
		max -= min;
	else
	{
		tmp = min - max;
		min = max;
		max = tmp;
	}
	return max ? (rand() % max + min) : min;
}

int sumaWOknie(int * obecneOkno, int dlugoscOkna) {
	int suma = 0, i;
	for (i = 0; i <= dlugoscOkna; i++) {
		suma += obecneOkno[i];
	}
	return suma;
}

void wypiszOkno(int * obecneOkno, int dlugoscOkna, int threadId) {
	int i;
	

	printf("[%d] : ", threadId);
	for (i = 0; i <= dlugoscOkna; i++) {
		printf("%d ", obecneOkno[i]);
	}
	printf("(%d) \n", sumaWOknie(obecneOkno, dlugoscOkna));
	
}

void zerujOkno(int * obecneOkno, int dlugoscOkna) {
	int i;
	for (i = 0; i < dlugoscOkna; i++) {
		obecneOkno[i] = 0;
	}
}

void * znajdz(void * arg) {
	parameters * param = (parameters*)arg;
	int i;
	int indexObecnegoOkna = 0;
	int * obecneOkno = (int*)(malloc(param->dlugoscOkna * sizeof(int)));
	zerujOkno(obecneOkno, param->dlugoscOkna);

	for (i = param->startIndex; i<param->stopIndex; i++) {
		obecneOkno[indexObecnegoOkna] = param->buffer[i];
		int sumaWObecnymOknie = sumaWOknie(obecneOkno, indexObecnegoOkna);
		if (sumaWObecnymOknie > param->progDoPrzekroczenia) {
			i = i - indexObecnegoOkna;
			if(wyswietlajZnalezioneOkna == true)
			{
				if (param->threadId != -1)
					pthread_mutex_lock(&lock);
				wypiszOkno(obecneOkno, indexObecnegoOkna, param->threadId);
				if (param->threadId != -1)
					pthread_mutex_unlock(&lock);
			}
			indexObecnegoOkna = 0;
			zerujOkno(obecneOkno, param->dlugoscOkna);

		}
		else if (indexObecnegoOkna<param->dlugoscOkna) {
			indexObecnegoOkna++;
		}

		if(indexObecnegoOkna == param->dlugoscOkna) {
			i = i - (indexObecnegoOkna - 1);
			indexObecnegoOkna = 0;
			zerujOkno(obecneOkno, param->dlugoscOkna);
		}
	}
	free(obecneOkno);
	return 0;
}

void wczytajDane(int& i, int& prog, int& okno, int& min, int& max, int& podzial)
{
	printf("Podaj dlugosc tablicy liczb (minimum 5): ");
	scanf("%d", &i);

	if(i < 5)
	{
		printf("Prosze podac dlugosc tablicy > 5");
		exit(1);
	}

	podzial = i / 2;

	printf("Podaj maksymalna dlugosc okna (nie wieksza niz %d): ", podzial);
	scanf("%d", &okno);

	if (okno > podzial)
	{
		printf("Prosze podac dlugosc okna < polowy dlugosci tablicy");
		exit(2);
	}

	printf("Podaj prog, ktory maja przekroczyc liczby w oknie: ");
	scanf("%d", &prog);
	printf("Podaj minimalna wartosc wystepujaca w tablicy: ");
	scanf("%d", &min);

	printf("Podaj maksymalna wartosc wystepujaca w tablicy: ");
	scanf("%d", &max);

	printf("Czy wyswietlac wylosowane liczby i znalezione pary? Wydluza to czas dzialania programu (y/n): \n");
	char dec_c;
	scanf(" %c", &dec_c);
	if(dec_c == 'y')
	{
		wyswietlajZnalezioneOkna = true;
	}

	printf("\n");
}

void ustawWatki(int i, int prog, int okno, int podzial, int* buffer, parameters& thread1Params, parameters& thread2Params, parameters& joinParams)
{
	thread1Params.dlugoscOkna = thread2Params.dlugoscOkna = joinParams.dlugoscOkna = okno;
	thread1Params.buffer = thread2Params.buffer = joinParams.buffer = buffer;
	thread1Params.progDoPrzekroczenia = thread2Params.progDoPrzekroczenia = joinParams.progDoPrzekroczenia = prog;

	joinParams.threadId = 0;
	thread1Params.threadId = 1;
	thread2Params.threadId = 2;

	thread1Params.startIndex = 0;
	thread1Params.stopIndex = podzial;
	thread2Params.startIndex = i % 2 == 0 ? podzial : podzial+1;
	thread2Params.stopIndex = i;

	joinParams.startIndex = thread1Params.stopIndex - (okno/2);
	joinParams.stopIndex = thread2Params.startIndex + (okno/2);
}

int main(int argc, char *argv[]) {
	int i, n, prog, okno, min, max, podzial;
	wczytajDane(i, prog, okno, min, max, podzial);
	
	int * buffer = (int*)malloc(i * sizeof(int));
	if (buffer == NULL)
		exit(1);

	srand(time(0));
	for (n = 0; n<i; n++)
		buffer[n] = random(min, max);

	if(wyswietlajZnalezioneOkna)
	{
		printf("Wylosowane liczby:\n");
		for (n = 0; n < i; n++) {
			if (n == podzial)
			{
				printf("|| ");
			}
			printf("%d ", buffer[n]);
		}
	}
	printf("\n");

	

	struct parameters thread1Params, thread2Params, joinParams;
	ustawWatki(i, prog, okno, podzial, buffer, thread1Params, thread2Params, joinParams);

	float startTimeForThreads = (float)clock() / CLOCKS_PER_SEC;
	pthread_t thread1, thread2;
	pthread_mutex_init(&lock, NULL);
	int result1 = pthread_create(&thread1, NULL, znajdz, (void *)&thread1Params);
	int result2 = pthread_create(&thread2, NULL, znajdz, (void *)&thread2Params);

	if (result1 != 0 || result2 != 0) {
		perror("Could not create thread.");
	}

	znajdz((void*)&joinParams);
	
	int * thread1Result, * thread2Result;
	pthread_join(thread1, (void **)&thread1Result);
	pthread_join(thread2, (void **)&thread2Result);
	pthread_mutex_destroy(&lock);
	float endTimeForThreads = (float)clock() / CLOCKS_PER_SEC;
	float timeElapsedForThreads = endTimeForThreads - startTimeForThreads;
	printf("\n\n Czas dla rozwiazania wielowatkowego: %.3f \n", timeElapsedForThreads);




	printf("Czy przeprowadzic test rozwiazania jednowatkowego? (y/n): \n");
	char dec_c;
	scanf(" %c", &dec_c);
	
	if(dec_c == 'y')
	{
		thread1Params.threadId = thread2Params.threadId = joinParams.threadId = -1;
		float startTimeForSingle = (float)clock() / CLOCKS_PER_SEC;
		znajdz((void*)&thread1Params);
		znajdz((void*)&joinParams);
		znajdz((void*)&thread2Params);
		float endTimeForSingle = (float)clock() / CLOCKS_PER_SEC;
		float timeElapsedForSingle = endTimeForSingle - startTimeForSingle;
		printf("\n\n Czas dla rozwiazania jednowatkowego: %.3f \n", timeElapsedForSingle);
		printf("\n\n Czas dla rozwiazania wielowatkowego: %.3f \n", timeElapsedForThreads);
	}




	free(buffer);
	system("pause");

	return 0;
}


