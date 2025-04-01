#include <sdk/async.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

thread_t* __async_thread = 0;
HANDLE __iocp = 0;

export void async_startup() {
  assert(__async_thread == 0);
  __async_thread = thread_new((function_t)__async_worker, null);
}
export void async_shutdown() {
  assert(__async_thread != 0);
  thread_free(__async_thread);
#if DEBUG
  __async_thread = 0;
#endif
}

void __async_worker() {
  DWORD bytes_count;
  OVERLAPPED* overlapped;
  ULONG_PTR completion_key;
  while (true) {
    BOOL result = GetQueuedCompletionStatus(__iocp, &bytes_count, &completion_key, &overlapped, INFINITE);
    if (!result) {

    }
  }
}

#endif