#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS

#ifdef SDK_NET_H
#include <winsock2.h>
#endif
#include <windows.h>

#ifdef SDK_TASKMANAGER_PROMISES
typedef BOOL(WINAPI* CancelIoEx_t)(HANDLE, LPOVERLAPPED);
CancelIoEx_t pCancelIoEx = 0;
#endif
SDK_EXPORT void __taskmanager_constructor(taskmanager_t* this) {
#ifdef SDK_TASKMANAGER_PROMISES
  HMODULE hKernel32 = GetModuleHandleA("Kernel32.dll");
  pCancelIoEx = (CancelIoEx_t)GetProcAddress(hKernel32, "CancelIoEx");
  this->__iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
  if (this->__iocp == NULL) {
    error_log("CreateIoCompletionPort", GetLastError());
  }
#endif
}
SDK_EXPORT void __taskmanager_deconstructor(taskmanager_t* this) {
#ifdef SDK_TASKMANAGER_PROMISES
  CloseHandle(this->__iocp);
#endif
}
SDK_EXPORT void __taskmanager_pool(taskmanager_t* this) {
  i32 result;
#ifdef SDK_WINDOW_H
  MSG msg;
  result = PeekMessageA(&msg, 0, 0, 0, PM_REMOVE);
  if (result) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
    if (msg.message == WM_QUIT) {
      --this->tasks_count;
    }
  }
#endif
#ifdef SDK_TASKMANAGER_PROMISES
  DWORD bytes;
  task_t* task = 0;
  OVERLAPPED* overlapped;
  result = GetQueuedCompletionStatus(this->__iocp, &bytes, (PULONG_PTR)&task, &overlapped, 0);
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
#endif
}

#ifdef SDK_TASKMANAGER_PROMISES
SDK_EXPORT void _promise_post(task_t* this, i32 value) {
  PostQueuedCompletionStatus(this->taskmanager->__iocp, value, (ULONG_PTR)this, 0);
}
#endif

#endif