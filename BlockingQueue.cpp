#include "BlockingQueue.h"


template<class T>
BlockingQueue<T>::BlockingQueue(int dimensione)
{
	size = dimensione;
	queueState = APERTA;
	counter = 0;
};

template<class T>
BlockingQueue<T>::~BlockingQueue()
{
};

template<class T>
bool BlockingQueue<T>::preleva(T& res) {
	//innanzittutto prendo lock che gestisce l'accesso alla strutttura dati della coda
	std::unique_lock lockQueue(mutexQueue);

	if (queueState == CHIUSA)
		//caso in cui la coda e' stata chiusa per cui non e' possibile prelevare
		return false;

	//ho messo il while anziche' un if solamente per coprire anche il caso delle notifiche spurie
	while (counter == 0) {
		/**
		** caso in cui la coda e' vuota ma ancora aperta per cui devo rilasciare il lock sulla coda e mettere questo thread in pausa 
		****																															**/																									
		//rilascio il lock sulla coda
		lockQueue.unlock();
		//metto il thread in attesa tramite una condition variable
		std::unique_lock<std::mutex> ul(threadMutexEmpty);
		cvSynchronizerEmpty.wait(ul);


		/***
		** a seguito di una notify il thread si e' risvegliato ma a questo punto devo ricontrollare se la coda e' aperta o chiusa
		****																													**/
		//pertanto riacquisisco il lock
		lockQueue.lock();
		if (queueState == CHIUSA)
			//caso in cui la coda e' stata chiusa per cui non e' possibile prelevare
			return false;
	}

	//se sono qui significa che la coda non e' vuota ed e' aperta
	//per cui estraggo il primo elemento disponibile e ritorno true
	res = coda.pop_front();
	counter--;
	if (counter == size - 1)
		//se il conteggio era pari a size significa che probabilmente c'erano dei produttori in coda per cui adesso ne libero uno
		cvSynchronizerFull.notify_one();

	return true;
};

class myexception : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Impossibile inserire un elemento dato che la coda e' chiusa\n";
	}
} myex;

template<class T>
void BlockingQueue<T>::inserisci(T val) {
	//funzionamento analogo a preleva
	std::unique_lock lockQueue(mutexQueue);

	if (stato == CHIUSA)
		throw myex;

	while (counter == size) {
		lockQueue.unlock();
		std::unique_lock<std::mutex> ul(threadMutexFull);
		cvSynchronizerFull.wait(ul);
		if (stato == CHIUSA)
			throw myex;
	}

	coda.push_back(val);
	counter++;
	if(counter == 1)
		cvSynchronizerEmpty.notify_one();
	return;
};


template<class T>
int BlockingQueue<T>::conteggio() {
	std::lock_guard lockQueue(mutexQueue);
	return counter;
};

template<class T>
void BlockingQueue<T>::chiudi() {
	std::lock_guard lockQueue(mutexQueue);
	queueState = CHIUSA;
	cvSynchronizerEmpty.notify_all();
	cvSynchronizerFull.notify_all();
};

//main di prova
#include <iostream>
int main() {
	std::cout << "Hello world!\n";
}