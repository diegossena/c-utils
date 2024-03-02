#include "internal/console.h"
#include "internal/net.h"

#include "sdk/application.h"

void app_inicialize() {
  console_inicialize();
  net_inicialize();
}

void app_shutdown() {
  net_shutdown();
}