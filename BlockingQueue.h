#pragma once
#ifndef BlockingQueue_H_
#define BlockingQueue_H_

#include <mutex>
#include <list>
#include <exception>

typedef enum{APERTA, CHIUSA} stato;

template <class T>
class BlockingQueue
{
private:
	std::mutex mutexQueue;
	std::mutex threadMutexEmpty;
	std::condition_variable cvSynchronizerEmpty;
	std::mutex threadMutexFull;
	std::condition_variable cvSynchronizerFull;
	std::list<T> coda;
	stato queueState;
	int size;
	int counter;
public:
	BlockingQueue(int size);
	~BlockingQueue();
	bool preleva(T& res);
	void inserisci(T val);
	int conteggio();
	void chiudi();
};


#endif // !BlockingQueue_H_



//DA CHIEDERE: DOVE DICHIARARE I MUTEX E SE NE SONO VERAMENTE NECESSARI 2