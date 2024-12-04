#include <sdk/types.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/timer.h>
#include <sdk/error.h>
#include <windows.h>

export timer_t* timer_new(function_t callback, void* context, u32 timeout, u32 delay) {
  timer_t this;
  bool result = CreateTimerQueueTimer(&this, 0, (WAITORTIMERCALLBACK)callback, context, timeout, delay, 0);
  if (result == 0) {
    console_error("CreateTimerQueueTimer", GetLastError());
  }
  return this;
}
export void timer_set(timer_t* this, u32 timeout, u32 delay) {
  bool result = ChangeTimerQueueTimer(0, this, timeout, delay);
  if (result == 0) {
    console_error("ChangeTimerQueueTimer", GetLastError());
  }
}
export void timer_clear(timer_t* this) {
  i32 result = DeleteTimerQueueTimer(0, this, 0);
  if (result == 0) {
    result = GetLastError();
    if (result != ERR_IO_PENDING) {
      console_error("DeleteTimerQueueTimer", GetLastError());
    }
  }
}

#endif