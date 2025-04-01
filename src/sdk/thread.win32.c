#include <sdk/thread.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

export thread_t* thread_new(function_t handle, void* context) {
  return CreateThread(
    0,                              // default security attributes
    0,                              // use default stack size  
    (LPTHREAD_START_ROUTINE)handle, // thread function
    context,                        // argument to thread function 
    0,                              // use default creation flags 
    0                               // thread_id
  );
}
export void thread_await(thread_t* this) {
  WaitForSingleObject((HANDLE)this, INFINITE);
  CloseHandle(this);
}

void thread_mutex_init(thread_mutex_t* this) {
  InitializeCriticalSection(this);
}
export void thread_mutex_destroy(thread_mutex_t* this) {
  DeleteCriticalSection(this);
}
export void thread_mutex_lock(thread_mutex_t* this) {
  EnterCriticalSection(this);
}
export void thread_mutex_unlock(thread_mutex_t* this) {
  LeaveCriticalSection(this);
}

export thread_signal_t* thread_cond_new() {
  return (thread_signal_t*)CreateEventW(NULL, FALSE, FALSE, NULL);
}
export void thread_signal_free(thread_signal_t* this) {
  CloseHandle((HANDLE)this);
}
export void thread_signal_emit(thread_signal_t* this) {
  SetEvent((HANDLE)this);
}
export void thread_signal_await(thread_signal_t* this) {
  WaitForSingleObject((HANDLE)this, INFINITE);
}

#endif