#include <sdk/types.h>

#ifdef ERROR_H
#include <src/sdk/error.c>
#endif
#ifdef MEMORY_H
#include <src/sdk/memory.c>
#include <src/sdk/memory.win32.c>
#endif
#ifdef STRING_H
#include <src/sdk/string.c>
#endif
#ifdef QUEUE_H
#include <src/sdk/queue.c>
#endif
#ifdef NET_H
#include <src/sdk/net.c>
#include <src/sdk/net.win32.c>
#endif
#ifdef TCP_H
#include <src/sdk/tcp.c>
#include <src/sdk/tcp.win32.c>
#endif
#ifdef UDP_H
#include <src/sdk/udp.c>
#include <src/sdk/udp.win32.c>
#endif
#ifdef CONSOLE_H
#include <src/sdk/console.c>
#include <src/sdk/console.win32.c>
#endif
#ifdef EVENTS_H
#include <src/sdk/events.c>
#endif
#ifdef TIME_H
#include <src/sdk/time.c>
#include <src/sdk/time.win32.c>
#endif
#ifdef SNMP_H
#include <src/sdk/snmp.c>
#endif
#ifdef ASN1_H
#include <src/sdk/asn1.c>
#endif
#ifdef HASHSET_H
#include <src/sdk/hashset.c>
#endif
#ifdef MATH_H
#include <src/sdk/math.c>
#endif
#ifdef SNOWFLAKE_H
#include <src/sdk/snowflake.c>
#endif
#ifdef WINDOW_H
#include <src/sdk/window.c>
#include <src/sdk/window.win32.c>
#endif
#ifdef GFX_TEXT_H
#include <src/sdk/gfx.text.win32.c>
#endif
#ifdef GFX_COLOR_H
#include <src/sdk/gfx.color.win32.c>
#endif
#ifdef GFX_RECT_H
#include <src/sdk/gfx.rect.win32.c>
#endif
#ifdef GFX_STROKE_H
#include <src/sdk/gfx.stroke.win32.c>
#endif
#ifdef GFX_IMAGE_H
#include <src/sdk/gfx.image.win32.c>
#endif
#ifdef KEYBOARD_H
#include <src/sdk/keyboard.win32.c>
#endif
#ifdef THREAD_H
#include <src/sdk/thread.win32.c>
#endif
#ifdef OS_H
#include <src/sdk/os.win32.c>
#endif
#ifdef TASKMANAGER_H
#include <src/sdk/taskmanager.c>
#endif
#ifdef ASYNC_H
#include <src/sdk/async.win32.c>
#endif

#ifdef DEBUG
static inline void debug() {
  bool success = true;
  console_color(ANSI_FORE_LIGHTRED);
#ifdef MEMORY_H
  if (__memory_count != 0) {
    console_log("memory_count %lld", __memory_count);
    success = false;
  }
#endif
#ifdef NET_H
  if (__net_started == true) {
    console_log("call net_shutdown");
    success = false;
  }
  if (__socket_count != 0) {
    console_log("socket_count %lld", __socket_count);
    success = false;
  }
#endif
  if (success) {
    console_color(ANSI_FORE_LIGHTGREEN);
    console_log("SUCCESS");
  }
  console_color(ANSI_RESET);
}
#else
#define debug()
#endif