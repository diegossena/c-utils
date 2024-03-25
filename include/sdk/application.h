#ifndef SDK_APPLICATION_H
#define SDK_APPLICATION_H

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct application_t {
  queue_t __tasks; // event_listener_t
} application_t;

void app_new(application_t*);
/**
 * @brief run main loop and free on exit
 * @return exit code
 */
i32 app_run(application_t*);


#endif