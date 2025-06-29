#include <sdk/async.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

#define IOCP_SHUTDOWN -1

thread_t* __async_thread = 0;
HANDLE __iocp = 0;
u64 __async_count = 0;

void async_startup() {
  assert(__async_thread == 0);
  __async_thread = thread_new((function_t)__async_worker, null);
  __iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}
void async_shutdown() {
  assert(__async_thread != 0);
  PostQueuedCompletionStatus(__iocp, 0, IOCP_SHUTDOWN, 0);
  thread_wait(__async_thread);
#if DEBUG
  __async_thread = 0;
#endif
}

void __async_worker() {
  u32 bytes_count;
  OVERLAPPED* overlapped;
  u64 completion_key;
  while (true) {
    bool success = GetQueuedCompletionStatus(__iocp, (DWORD*)&bytes_count, &completion_key, &overlapped, INFINITE);
    if (success == false)
      continue;
    if (completion_key == IOCP_SHUTDOWN)
      break;
  }
  CloseHandle(__iocp);
#if DEBUG
  __iocp = 0;
#endif
}

#undef IOCP_SHUTDOWN
#endif