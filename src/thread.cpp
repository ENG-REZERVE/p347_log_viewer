//--------------------------------------------------------------------
#include "thread.hpp"
//--------------------------------------------------------------------
void *TThread_ThreadProc(void* arg)
{
  if (arg != NULL)
  {
    ((TThread*)arg)->Execute();
    return NULL;
  }
  else
    return NULL;    
}
//--------------------------------------------------------------------
TThread::TThread(bool CreateSuspended)
{
  Terminated = false;
  Suspended = true;
  ReturnValue = 0;

  if (!CreateSuspended) Resume();
}
//--------------------------------------------------------------------
TThread::~TThread()
{
  Terminate();
  WaitFor();
}
//--------------------------------------------------------------------
int TThread::WaitFor()
{
  if (!Suspended) pthread_join(Fpt, NULL);
  return ReturnValue;
}
//--------------------------------------------------------------------
void TThread::Resume()
{
  if (Suspended) {
    Suspended = false;
	pthread_create(&Fpt, NULL, TThread_ThreadProc, this);
  }
}
//--------------------------------------------------------------------
void TThread::Terminate()
{
  Terminated = true;
}
//--------------------------------------------------------------------
