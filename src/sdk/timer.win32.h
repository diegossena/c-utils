#include <sdk/timer.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>

SDK_EXPORT timer_t* timer_new(function_t callback, void* context, u32 timeout, u32 delay) {
  timer_t this;
  bool result = CreateTimerQueueTimer(&this, 0, (WAITORTIMERCALLBACK)callback, context, timeout, delay, 0);
  if (result == 0) {
    error("CreateTimerQueueTimer", GetLastError());
  }
  return this;
}
SDK_EXPORT void timer_set(timer_t* this, u32 timeout, u32 delay) {
  bool result = ChangeTimerQueueTimer(0, this, delay, delay);
  if (result == 0) {
    error("ChangeTimerQueueTimer", GetLastError());
  }
}
SDK_EXPORT void timer_clear(timer_t* this) {
  i32 result = DeleteTimerQueueTimer(0, this, 0);
  if (result == 0) {
    result = GetLastError();
    if (result != ERR_IO_PENDING) {
      error("DeleteTimerQueueTimer", GetLastError());
    }
  }
}

#endif