#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <threadpoolapiset.h>

void tashmanager_push(function_t callback, void* context) {
#ifdef DEBUG
  WINBOOL result = QueueUserWorkItem((LPTHREAD_START_ROUTINE)callback, context, 0);
  assert(result);
#else
  QueueUserWorkItem((LPTHREAD_START_ROUTINE)callback, context, 0);
#endif
}


#endif