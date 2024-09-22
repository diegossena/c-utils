#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS

#ifdef SDK_NET_H
#include <winsock2.h>
#endif
#include <windows.h>

typedef BOOL(WINAPI* CancelIoEx_t)(HANDLE, LPOVERLAPPED);
CancelIoEx_t pCancelIoEx = 0;

SDK_EXPORT void __taskmanager_constructor(taskmanager_t* this) {
  HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
  pCancelIoEx = (CancelIoEx_t)GetProcAddress(hKernel32, "CancelIoEx");
  this->__iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
  if (this->__iocp == NULL) {
    error("CreateIoCompletionPort", GetLastError());
  }
}
SDK_EXPORT void __taskmanager_deconstructor(taskmanager_t* this) {
  CloseHandle(this->__iocp);
}
SDK_EXPORT void __taskmanager_pool(taskmanager_t* this) {
  DWORD bytes;
  task_t* task;
  OVERLAPPED* overlapped;
  i32 result = GetQueuedCompletionStatus(this->__iocp, &bytes, (PULONG_PTR)&task, &overlapped, 0);
  if (result) {
    result = 0;
  } else {
    result = GetLastError();
    if (result == ERR_WAIT_TIMEOUT)
      return;
  }
  task->callback(task->context, result, bytes);
}

SDK_EXPORT void _promise_post(task_t* this, i32 value) {
  PostQueuedCompletionStatus(this->taskmanager->__iocp, value, (ULONG_PTR)this, 0);
}

#endif