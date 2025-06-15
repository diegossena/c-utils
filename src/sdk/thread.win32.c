#include <sdk/thread.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

thread_t* thread_new(function_t handle, void* context) {
  HANDLE this = CreateThread(
    0,                              // default security attributes
    0,                              // use default stack size  
    (LPTHREAD_START_ROUTINE)handle, // thread function
    context,                        // argument to thread function 
    0,                              // use default creation flags 
    0                               // thread_id
  );
  return this;
}
void thread_free(thread_t* this) {
  CloseHandle(this);
}
void thread_wait(thread_t* this) {
  WaitForSingleObject((HANDLE)this, INFINITE);
}
void thread_wait_all(thread_t** threads, u64 count) {
  WaitForMultipleObjects(count, (HANDLE*)threads, true, INFINITE);
}

void mutex_init(mutex_t* this) {
  InitializeCriticalSection(this);
}
void mutex_destroy(mutex_t* this) {
  DeleteCriticalSection(this);
}
void mutex_lock(mutex_t* this) {
  EnterCriticalSection(this);
}
void mutex_unlock(mutex_t* this) {
  LeaveCriticalSection(this);
}

sync_t* sync_new() {
  return (sync_t*)CreateEventW(NULL, FALSE, FALSE, NULL);
}
void sync_free(sync_t* this) {
  CloseHandle((HANDLE)this);
}
void sync_signal(sync_t* this) {
  SetEvent((HANDLE)this);
}
void sync_wait(sync_t* this) {
  WaitForSingleObject((HANDLE)this, INFINITE);
}

#endif