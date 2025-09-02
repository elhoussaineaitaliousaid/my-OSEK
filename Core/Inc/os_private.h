#ifndef OS_PRIVATE_H
#define OS_PRIVATE_H

#include <stdint.h>

// --- Constantes de Compilation ---
#define MAX_TASKS 3    // Nombre MAXIMAL de tâches que l'OS peut gérer
#define STACK_SIZE 128 // Taille de la pile de CHAQUE tâche (en words uint32_t)
#define set_PSP(sp)        __set_PSP((uint32_t)(sp))
#define get_PSP()          __get_PSP()
#define get_CONTROL()      __get_CONTROL()
#define set_CONTROL(x)     __set_CONTROL(x)
#define _ISB()             __ISB()

typedef void (*os_TaskFunc)(void);

typedef enum {
    OS_TASK_SUSPENDED,
    OS_TASK_READY,
    OS_TASK_RUNNING,
    OS_TASK_WAITING
} os_TaskState;

typedef struct {
    uint32_t* stack_ptr;
    os_TaskFunc task_func;
    os_TaskState state;
    uint8_t priority;
    uint32_t delay;
} os_TCB_t;


extern os_TCB_t os_taskTable[MAX_TASKS];
extern uint32_t os_taskStacks[MAX_TASKS][STACK_SIZE];
extern volatile int8_t os_currentTaskID;

#endif
