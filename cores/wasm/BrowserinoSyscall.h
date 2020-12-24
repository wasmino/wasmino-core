#ifndef BrowserinoSyscall_h
#define BrowserinoSyscall_h

#ifdef __cplusplus
extern "C" {
#endif

// syscall(BROWSERINO_SYSCALL_WRITE, void *ptr, size_t size)
#define BROWSERINO_SYSCALL_WRITE 1
// syscall(BROWSERINO_SYSCALL_WRITE, void *ptr, size_t size)
#define BROWSERINO_SYSCALL_READ  2

extern long __browserino_syscall(long number, ...);

#define browserinoSyscall(...) __browserino_syscall(...)

#ifdef __cplusplus
} // extern "C"
#endif

#endif
