#define HAVE_STRUCT_TIMESPEC
#include <iostream>
#include <fstream>
#include <conio.h>
#include <string>
#include <pthread.h>
//#include <time.h>

//#include "stdafx.h"

#define NUM_THREADS 2
int lock;

using namespace std;

struct parameters
{
	int threadId;
	string tekst, tekst_szukany;
};

int counter = 0;

void * szukajWszystkichFraz(void * arg)
{
	parameters * param = (parameters *)arg;
	int tid = param->threadId;

	size_t znalezionaPozycja = param->tekst.find(param->tekst_szukany);
	if (znalezionaPozycja == std::string::npos)
	{
		return 0;
	}

	do
	{
		counter++;
		znalezionaPozycja = param->tekst.find(param->tekst_szukany, znalezionaPozycja + param->tekst_szukany.size());
	} while (znalezionaPozycja != std::string::npos);

	return 0;
}

void ustawWatki(int podzial, string tekst, string szukany, parameters* threadParams)
{
	for (int i = 0; i<NUM_THREADS; i++) {
		threadParams[i].tekst = tekst.substr(i*podzial, podzial);
		threadParams[i].threadId = i;
		threadParams[i].tekst_szukany = szukany;
	}
}

using namespace std;


int main()
{
	int podzial;
	string text = "bla bla bla bla bla bla bla";
	string text_threads[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	int rc;
	int i;

	fstream plik;
	plik.open("napis.txt", ios::in);
	if (plik.good())
	{
		string bufor;

		while (!plik.eof())
		{
			getline(plik, bufor);
			text += bufor;
		}
		plik.close();
	}
	else cout << "Error! Nie udalo otworzyc sie pliku!" << endl;

	podzial = text.length() / NUM_THREADS;

	parameters * params = new parameters[NUM_THREADS];
	


	cout << "szukane slowo: \"bla\" " << endl;


	ustawWatki(podzial, text, "bla", params);

	for (i = 0; i < NUM_THREADS; i++) {
		rc = pthread_create(&threads[i], NULL, szukajWszystkichFraz, (void *)&params[i]);
		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			return -1;
		}
	}

	int * threaResult = new int[NUM_THREADS];
	

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], (void **)&threaResult[i]);
	}


	cout << "Znaleziono wystapien: " << counter;
	system("pause");
	return(0);
}
