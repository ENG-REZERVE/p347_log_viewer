//--------------------------------------------------------------------
#include <sys/time.h>
#include <errno.h>
#include "syncobjs.hpp"
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// TEvent
//--------------------------------------------------------------------
TEvent::TEvent(int EventAttributes, bool ManualReset, 
				bool InitialState, char* Name, bool UseCOMWait)
{
  FManualReset = ManualReset;
  FState = InitialState;
  pthread_mutex_init(&FMutex,NULL);
  pthread_cond_init(&FCond, NULL);
  Fwaiting_count = 0;
  FTerminating = false;

}
//--------------------------------------------------------------------
TEvent::~TEvent() {
  pthread_mutex_lock(&FMutex);
  FTerminating = true;
  pthread_mutex_unlock(&FMutex);

  int waiting_count;

  do {
    SetEvent();
    pthread_mutex_lock(&FMutex);
    waiting_count = Fwaiting_count;
    pthread_mutex_unlock(&FMutex);
  } while (waiting_count != 0);
}
//--------------------------------------------------------------------
void TEvent::ResetEvent() {
  pthread_mutex_lock(&FMutex);
  FState = false;
  pthread_mutex_unlock(&FMutex);
}
//--------------------------------------------------------------------
void TEvent::SetEvent() {
  pthread_mutex_lock(&FMutex);
  FState = true;
  if (FManualReset) 
	pthread_cond_broadcast(&FCond);
  else
    pthread_cond_signal(&FCond);
  pthread_mutex_unlock(&FMutex);
}
//--------------------------------------------------------------------
TWaitResult TEvent::WaitFor(int Timeout) {
  TWaitResult res;

  pthread_mutex_lock(&FMutex);
  if (FTerminating) {
    pthread_mutex_unlock(&FMutex);
	return wrError;
  }

  Fwaiting_count++;

  if (Timeout == INFINITE) {
    while (!FState)
	  pthread_cond_wait(&FCond,&FMutex);
	res = wrSignaled;
  } else {
	struct timespec timeout;
	struct timeval tv;
	int delta_sec = Timeout/1000;
	int delta_nsec = (Timeout - delta_sec*1000)*1000000;
	bool while_flag = true;

//	time(&timeout.tv_sec);
	gettimeofday(&tv,NULL);
//	timeout.tv_sec += delta_sec;
	timeout.tv_sec = tv.tv_sec + delta_sec;
	timeout.tv_nsec = tv.tv_usec*1000+delta_nsec;
	delta_sec = timeout.tv_nsec/1000000000;
	timeout.tv_sec+= delta_sec;
	timeout.tv_nsec-= delta_sec*1000000000;
	
	while (while_flag) {
		switch (pthread_cond_timedwait(&FCond,&FMutex,&timeout)) {
			case 0:
				if (FState) {
					res = wrSignaled;
					while_flag = false;
				}
				break;

			case ETIMEDOUT:
				res = FState ? wrSignaled : wrTimeout;
				while_flag = false;
				break;


			default:
                res = wrError;
				while_flag = false;
		}
	}
	
  }

  if (!FManualReset) 
	FState = false;

  if (FTerminating) res = wrAbandoned;

  Fwaiting_count--;
  pthread_mutex_unlock(&FMutex);
  return res;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// TCriticalSection
//--------------------------------------------------------------------
TCriticalSection::TCriticalSection() 
{
    pthread_mutexattr_init(&FAttr);
    pthread_mutexattr_settype(&FAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&FMutex, &FAttr);
}
//--------------------------------------------------------------------
TCriticalSection::~TCriticalSection() 
{
    pthread_mutex_destroy(&FMutex);
    pthread_mutexattr_destroy(&FAttr);
}
//--------------------------------------------------------------------
void TCriticalSection::Enter() 
{
    pthread_mutex_lock(&FMutex);
}
//--------------------------------------------------------------------
void TCriticalSection::Leave() 
{
    pthread_mutex_unlock(&FMutex);
}
//--------------------------------------------------------------------
