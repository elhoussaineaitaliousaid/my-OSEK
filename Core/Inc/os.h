#ifndef INC_OS_H_
#define INC_OS_H_

#include <stdint.h>

// --- API Publique que les tâches peuvent appeler ---
void os_init(void);
void os_start(void);
void PendSV_Hndler(void) ;
void os_create_task(void (*task_func)(void), uint8_t priority);
void os_delay(uint32_t ticks);
void SysTick_Handler(void) ;
void os_schedule(void);

#endif /* INC_OS_H_ */
