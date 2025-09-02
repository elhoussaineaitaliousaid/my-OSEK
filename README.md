Mini RTOS – STM32 (Priority Scheduler)
 Description

Ce projet implémente un mini système d’exploitation temps réel (RTOS) sur STM32.
Il repose sur :

SysTick pour la génération d’interruptions périodiques (tick timer).

PendSV pour le changement de contexte (context switching).

Table des tâches (os_taskTable) pour gérer les états, les piles et les priorités.

Le scheduler sélectionne toujours la tâche READY avec la priorité la plus élevée.

 Fonctionnalités

Gestion de plusieurs tâches (jusqu’à MAX_TASKS).

Ordonnancement par priorité (plus la valeur est élevée, plus la priorité est forte).

Sauvegarde et restauration automatique du contexte (registres R4–R11).

Support du time-slicing via SysTick.

Implémentation modulaire : facile à étendre (mutex, sémaphores, etc.).

 Architecture
1. Tâches

Chaque tâche est représentée par une structure :

typedef struct {
    void (*task_func)(void);   // Pointeur sur la fonction de la tâche
    uint32_t *stack_ptr;       // Pointeur de pile
    int state;                 // Etat : READY, RUNNING, BLOCKED
    int priority;              // Priorité (plus grand = plus prioritaire)
} os_task_t;

2. Ordonnancement

Le SysTick_Handler déclenche périodiquement l’ordonnancement.

Le PendSV_Handler sauvegarde le contexte de la tâche courante et restaure celui de la prochaine tâche choisie.

Le scheduler sélectionne la tâche READY la plus prioritaire.

3. Cycle d’exécution
[SysTick Interrupt] → Scheduler → Sélection tâche → PendSV → Changement de contexte → Nouvelle tâche s’exécute

🚀 Exemple d’utilisation
void task1(void) {
    while(1) {
        // Code de la tâche 1
    }
}

void task2(void) {
    while(1) {
        // Code de la tâche 2
    }
}

int main(void) {
    os_createTask(task1, priority=2);
    os_createTask(task2, priority=1);

    os_start();  // Lance le RTOS
    while(1);
}

 Ordonnancement

Exemple avec 3 tâches :

Tâche	État	Priorité	Résultat
T1	READY	3	Exécutée
T2	READY	2	Attente
T3	READY	1	Attente

 Le scheduler choisira T1 car elle a la priorité la plus haute.

 Organisation du projet
/Core
 ├── Inc/
 │   ├── os.h          # Déclarations du mini RTOS
 │   └── ...
 ├── Src/
 │   ├── os.c          # Implémentation du scheduler et gestion des tâches
 │   ├── main.c        # Exemple d’application utilisateur
 │   └── startup_stm32.s # Vecteurs d’interruptions

🔧 Compilation & Flash

Importer le projet dans STM32CubeIDE ou utiliser arm-none-eabi-gcc.

Compiler en mode Debug ou Release.

Flasher sur STM32 via ST-Link ou OpenOCD.
