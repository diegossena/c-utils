#include <sdk/promise.h>
#ifdef PLATFORM_WINDOWS

#include <windows.h>
void* __global_iocp = 0;

SDK_EXPORT void _promise_post(task_t* this, i32 value) {
  PostQueuedCompletionStatus(__global_iocp, value, (ULONG_PTR)this, 0);
}
SDK_EXPORT void __promise_startup() {
  __global_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
  if (__global_iocp == NULL) {
    error_log("CreateIoCompletionPort", GetLastError());
  }
}
SDK_EXPORT void _promise_shutdown() { CloseHandle(__global_iocp); }
SDK_EXPORT void _promise_run() {
  i32 result;
  DWORD bytes;
  task_t* task = 0;
  OVERLAPPED* overlapped;
  result = GetQueuedCompletionStatus(__global_iocp, &bytes, (PULONG_PTR)&task, &overlapped, 0);
  if (result) {
    result = 0;
  } else {
    result = GetLastError();
    if (result == ERR_WAIT_TIMEOUT)
      return;
  }
  if (task) {
    task->callback(task->context, result, bytes);
  }
}

#endif