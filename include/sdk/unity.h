#include <sdk/types.h>

#ifdef ERROR_H
#include <src/sdk/error.c>
#endif
#ifdef MEMORY_H
#include <src/sdk/memory.c>
#include <src/sdk/memory.win32.c>
#endif
#ifdef BUFFER_H
#include <src/sdk/buffer.c>
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
#ifdef DATE_H
#include <src/sdk/date.c>
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
#ifdef KEYBOARD_H
#include <src/sdk/keyboard.win32.c>
#endif
