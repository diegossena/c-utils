#include <sdk/taskmanager.h>
#ifdef PLATFORM_WINDOWS
#include <windows.h>


export void __taskmanager_startup(taskmanager_t* this) {
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  this->threads_count = system_info.dwNumberOfProcessors - 1;
  this->threads = memory_alloc(sizeof(void*) * this->threads_count);
  console_log("threads_count %d", this->threads_count);
  for (u8 i = 0; i < this->threads_count; i++) {
    this->threads[i] = CreateThread(
      0,                        // default security attributes
      0,                        // use default stack size  
      (void*)__taskmanager_run, // thread function name
      &this->tasks,             // argument to thread function 
      0,                        // use default creation flags 
      0                         // thread_id
    );
  }
}
export void __taskmanager_shutdown(taskmanager_t* this) {
  for (u8 i = 0; i < this->threads_count; i++) {
    CloseHandle(this->threads[i]);
  }
}

#endif