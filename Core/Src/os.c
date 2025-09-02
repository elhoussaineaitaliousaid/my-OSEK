#include "os.h"
#include "os_private.h"
#include <string.h>
#include "stm32f1xx.h"

#define PSR_INIT 0x01000000
#define SYST_LOAD 1
#define tick_hz  1000

extern uint32_t cpu_hz;
extern uint8_t use_ahb_div;

os_TCB_t os_taskTable[MAX_TASKS];
uint32_t os_taskStacks[MAX_TASKS][STACK_SIZE];
volatile int8_t os_currentTaskID = -1;  // correction : signé

void os_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        os_taskTable[i].stack_ptr = NULL;
        os_taskTable[i].task_func = NULL;
        os_taskTable[i].state = OS_TASK_SUSPENDED;
        os_taskTable[i].priority = 0;
        os_taskTable[i].delay = 0;
    }
}

void PendSV_Hndler(void) {
    // sauvegarde du contexte de la tâche courante
    uint32_t *sp = os_taskTable[os_currentTaskID].stack_ptr;
    asm volatile(
        "mrs r0, psp\n"
        "stmdb r0!, {r4-r11}\n"
        "str r0, [%0]\n"
        : : "r"(&os_taskTable[os_currentTaskID].stack_ptr) : "r0"
    );

    // sélectionner la prochaine tâche READY
    int next = (os_currentTaskID + 1) % MAX_TASKS;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (os_taskTable[next].state == OS_TASK_READY) {
            os_currentTaskID = next;
            break;
        }
        next = (next + 1) % MAX_TASKS;
    }

    // restaurer le contexte de la prochaine tâche
    sp = os_taskTable[os_currentTaskID].stack_ptr;
    asm volatile(
        "ldr r0, [%0]\n"
        "ldmia r0!, {r4-r11}\n"
        "msr psp, r0\n"
        "bx lr\n"
        : : "r"(&os_taskTable[os_currentTaskID].stack_ptr) : "r0"
    );
}


void os_create_task(void (*task_func)(void), uint8_t priority) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (os_taskTable[i].state == OS_TASK_SUSPENDED &&
            os_taskTable[i].stack_ptr == NULL) {

            os_taskTable[i].task_func = task_func;
            os_taskTable[i].priority  = priority;
            os_taskTable[i].state     = OS_TASK_READY;

            uint32_t *sp = &os_taskStacks[i][STACK_SIZE - 1];
            *(--sp) = PSR_INIT;              // xPSR
            *(--sp) = (uint32_t)task_func;   // PC
            *(--sp) = 0xFFFFFFFD;            // LR
            *(--sp) = 0; // R12
            *(--sp) = 0; // R3
            *(--sp) = 0; // R2
            *(--sp) = 0; // R1
            *(--sp) = 0; // R0

            for (int r = 0; r < 8; r++) {
                *(--sp) = 0;
            }

            os_taskTable[i].stack_ptr = sp;
            return;
        }
    }
    while (1) { } // si aucun slot trouvé
}

void os_start(void) {
    SCB->SHP[10] = 0xFF; // PendSV priorité la plus basse

    uint32_t clk = use_ahb_div ? (cpu_hz / 8U) : cpu_hz;
    uint32_t ticks = (clk / tick_hz) - 1U;
    SysTick->LOAD = ticks;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;

    os_currentTaskID = -1;
    int highestPriority = -1;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (os_taskTable[i].state == OS_TASK_READY) {
            if (os_taskTable[i].priority > highestPriority) {
                highestPriority = os_taskTable[i].priority;
                os_currentTaskID = i;
            }
        }
    }


    if (os_currentTaskID == -1) while(1); // aucune tâche READY

    uint32_t *stack_top = os_taskTable[os_currentTaskID].stack_ptr;
    set_PSP(stack_top);

    uint32_t control = get_CONTROL();
    control |= (1 << 1); // utiliser PSP
    set_CONTROL(control);

    _ISB();

    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // déclenche le premier switch

    while(1) {
        __WFI(); // attend interruption (SysTick / PendSV)
    }
}


void os_delay(uint32_t ticks) {
    os_taskTable[os_currentTaskID].state = OS_TASK_SUSPENDED; // correction
    os_taskTable[os_currentTaskID].delay = ticks;

    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void SysTick_Handler(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (os_taskTable[i].state == OS_TASK_SUSPENDED && os_taskTable[i].delay > 0) {
            os_taskTable[i].delay--;

            if (os_taskTable[i].delay == 0) {
                os_taskTable[i].state = OS_TASK_READY;
            }
        }
    }
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
