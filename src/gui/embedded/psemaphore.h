/* posix semaphore header with local struct and macro for few element to
   compativle with system V. */
#ifndef PSEMAPHORE_H
#  define PSEMAPHORE_H
#  include <semaphore.h>
   typedef struct {
      sem_t *sem_id_e[3];
     int nid; // 1 or 3
     const char *fname; // nid == 1 -> it has fname, else unnamed semaphore
   } semId_type;

#  define SEMID_FAILED(ID)  \
    ((ID)->nid == 1 ?(ID)->sem_id_e[0] == SEM_FAILED :	\
     (ID)->sem_id_e[0] == SEM_FAILED ||			\
     (ID)->sem_id_e[1] == SEM_FAILED ||			\
     (ID)->sem_id_e[2] == SEM_FAILED )

#endif
