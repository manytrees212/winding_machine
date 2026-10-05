#include <stddef.h>
#include <stdint.h>

// ============================================
// 1. PURE VIRTUAL CALL HANDLER
// ============================================

/**
 * Called when a pure virtual function is called.
 * This should never happen in correct code.
 * If it does, halt execution for debugging.
 */
extern "C" void __cxa_pure_virtual() {
    while (1) {
        __asm volatile("bkpt 1");
    }
}

// ============================================
// 2. STATIC INITIALIZATION GUARDS (Single-threaded)
// ============================================

/**
 * These functions are used by the compiler to ensure thread-safe
 * initialization of static local variables. On bare-metal single-threaded
 * systems, we can provide empty implementations.
 *
 * WARNING: If you use an RTOS with multiple threads, you MUST implement
 * these with proper mutex/semaphore protection!
 */
extern "C" int __cxa_guard_acquire(uint32_t* guard) {
    // Check if already initialized (first byte = 0)
    // Return 1 to indicate we should proceed with initialization
    return *guard == 0;
}

extern "C" void __cxa_guard_release(uint32_t* guard) {
    // Mark as initialized
    *guard = 1;
}

// Abort guard - called if initialization throws an exception
extern "C" void __cxa_guard_abort() {
}

// ============================================
// 3. ATEXIT / DESTRUCTOR SUPPORT
// ============================================

/**
 * Called to register a destructor function.
 * With -fno-use-cxa-atexit, destructors are handled by __libc_init_array,
 * so this can be a dummy function.
 */
extern "C" int __cxa_atexit(void (*destructor)(void*), void* arg, void* dso) {
    if(destructor || arg || dso){}
    return 0;
}

/**
 * Alternative for GCC.
 */
extern "C" int __aeabi_atexit(void* object, void (*destructor)(void*), void* dso_handle) {
    if(object || destructor || dso_handle){}
    return 0;
}

