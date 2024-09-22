#include <sdk/types.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/timer.h>
#include <sdk/error.h>
#include <windows.h>

SDK_EXPORT timer_t* timer_new(function_t callback, void* context, u32 timeout, u32 delay) {
  timer_t this;
  bool result = CreateTimerQueueTimer(&this, 0, (WAITORTIMERCALLBACK)callback, context, timeout, delay, 0);
  if (result == 0) {
    error_log("CreateTimerQueueTimer", GetLastError());
  }
  return this;
}
SDK_EXPORT void timer_set(timer_t* this, u32 timeout, u32 delay) {
  bool result = ChangeTimerQueueTimer(0, this, timeout, delay);
  if (result == 0) {
    error_log("ChangeTimerQueueTimer", GetLastError());
  }
}
SDK_EXPORT void timer_clear(timer_t* this) {
  i32 result = DeleteTimerQueueTimer(0, this, 0);
  if (result == 0) {
    result = GetLastError();
    if (result != ERR_IO_PENDING) {
      error_log("DeleteTimerQueueTimer", GetLastError());
    }
  }
}

#endif