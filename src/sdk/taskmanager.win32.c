#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

SDK_EXPORT void _task_resolve(task_t* this, u32 value) {
  PostQueuedCompletionStatus(this->taskmanager->__iocp, value, (ULONG_PTR)this, 0);
}
SDK_EXPORT void __taskmanager_constructor(taskmanager_t* this) {
  this->__iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
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
  bool result = GetQueuedCompletionStatus(this->__iocp, &bytes, (PULONG_PTR)&task, &overlapped, 0);
  if (result) {
    task->handle(task->context, bytes);
  }
}

#endif