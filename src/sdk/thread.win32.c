#include <sdk/thread.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

inline thread_t* thread_new(function_t handle, void* context) {
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
inline void thread_free(thread_t* this) { CloseHandle(this); }
inline void thread_wait(thread_t* this) { WaitForSingleObject((HANDLE)this, INFINITE); }
inline void thread_wait_all(thread_t** threads, u64 count) { WaitForMultipleObjects(count, (HANDLE*)threads, true, INFINITE); }

inline void mutex_init(mutex_t* this) { InitializeCriticalSection(this); }
inline void mutex_destroy(mutex_t* this) { DeleteCriticalSection(this); }
inline void mutex_lock(mutex_t* this) { EnterCriticalSection(this); }
inline void mutex_unlock(mutex_t* this) { LeaveCriticalSection(this); }

inline sync_t* sync_new() { return (sync_t*)CreateEventW(NULL, FALSE, FALSE, NULL); }
inline void sync_free(sync_t* this) { CloseHandle((HANDLE)this); }
inline void sync_signal(sync_t* this) { SetEvent((HANDLE)this); }
inline void sync_wait(sync_t* this) { WaitForSingleObject((HANDLE)this, INFINITE); }

#endif