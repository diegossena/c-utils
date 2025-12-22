#include <sdk/async.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

#define IOCP_SHUTDOWN -1

thread_t* _async_thread = 0;
HANDLE _iocp = 0;
u64 _async_count = 0;

void async_startup() {
  assert(_async_thread == 0);
  _async_thread = thread_new((function_t)_async_worker, null);
  _iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}
void async_shutdown() {
  assert(_async_thread != 0);
  PostQueuedCompletionStatus(_iocp, 0, IOCP_SHUTDOWN, 0);
  thread_wait(_async_thread);
#if DEBUG
  _async_thread = 0;
#endif
}

void _async_worker() {
  u32 bytes_count;
  OVERLAPPED* overlapped;
  u64 completion_key;
  while (true) {
    bool success = GetQueuedCompletionStatus(_iocp, (DWORD*)&bytes_count, &completion_key, &overlapped, INFINITE);
    if (success == false)
      continue;
    if (completion_key == IOCP_SHUTDOWN)
      break;
  }
  CloseHandle(_iocp);
#if DEBUG
  _iocp = 0;
#endif
}

#undef IOCP_SHUTDOWN
#endif