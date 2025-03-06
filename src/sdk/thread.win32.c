#include <sdk/thread.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

export thread_t* thread_new(function_t handle, void* context) {
  return CreateThread(
    null,     // default security attributes
    null,     // use default stack size  
    handle,   // thread function
    context,  // argument to thread function 
    null,     // use default creation flags 
    null      // thread_id
  );
}
export void thread_free(thread_t* this) {
  CloseHandle(this);
}
void thread_mutex_startup(thread_mutex_t* this) {
  InitializeCriticalSection(this);
}
export void thread_mutex_lock(thread_mutex_t* this) {
  EnterCriticalSection(this);
}
export void thread_mutex_unlock(thread_mutex_t* this) {
  LeaveCriticalSection(this);
}
export void thread_mutex_shutdown(thread_mutex_t* this) {
  DeleteCriticalSection(this);
}

#endif