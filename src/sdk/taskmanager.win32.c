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

#endif