#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

SDK_EXPORT void __taskmanager_constructor_platform(taskmanager_t* this) {
  this->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
  if (this->iocp == NULL) {
    error("CreateIoCompletionPort", GetLastError());
  }
}
SDK_EXPORT void __taskmanager_deconstructor_platform(taskmanager_t* this) {
  CloseHandle(this->iocp);
}
SDK_EXPORT void __taskmanager_pool(taskmanager_t* this) {
  task_t* task;
  DWORD bytes;
  ULONG_PTR key;
  WINBOOL result;
  OVERLAPPED* overlapped;
  result = GetQueuedCompletionStatus(this->iocp, &bytes, (PULONG_PTR)&task, &overlapped, 0);
  if (result) {
    function_t handle = task->handle;
    task->handle = task->destroy;
    handle(task->context);
  }
}

#endif