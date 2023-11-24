/*  mailbox.h provides a very simple mailbox datatype.
 *  Gaius Mulley <gaius.southwales@gmail.com>.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "multiprocessor.h"
#define MAX_MAILBOX_DATA 100 // Constant variable to set number of total entries allowed to be stored in mailbox_data to 100

typedef struct triple_t {
	int result;
	int move_no;
	int positions_explored;
} triple;


typedef struct mailbox_t {
	triple data[MAX_MAILBOX_DATA]; // Array of elements set to size of MAX_MAILBOX_DATA (100 currently), each element is a triple used to store result, move_no and positions_explored
	int in; // Implements input index for bounded buffer, putting data in at this slot
	int out; // Implements output index for bounded buffer, sending data out from this slot
	sem_t* item_available;    /* is there data in the mailbox?  */
	sem_t* space_available;   /* is there space for more data in the mailbox?  */
	sem_t* mutex;             /* access to the mailbox.  */
	struct mailbox_t* prev;  /* pointer to previous mailbox.  */
} mailbox;


#if !defined(mailbox_h)
#  define mailbox_h
#  if defined(mailbox_c)
#     if defined(__GNUG__)
#        define EXTERN extern "C"
#     else /* !__GNUG__.  */
#        define EXTERN
#     endif /* !__GNUG__.  */
#  else /* !mailbox_c.  */
#     if defined(__GNUG__)
#        define EXTERN extern "C"
#     else /* !__GNUG__.  */
#        define EXTERN extern
#     endif /* !__GNUG__.  */
#  endif /* !mailbox_c.  */


/*
 *  init - create a single mailbox which can contain a single triple.
 */

EXTERN mailbox* mailbox_init(void);


/*
 *  send - send (result, move_no, positions_explored) to the mailbox mbox.
 */

EXTERN void mailbox_send(mailbox* mbox,
	int result, int move_no, int positions_explored);


/*
 *  rec - receive (result, move_no, positions_explored) from the
 *        mailbox mbox.
 */

EXTERN void mailbox_rec(mailbox* mbox,
	int* result, int* move_no,
	int* positions_explored);

#  undef EXTERN
#endif /* !mailbox_h.  */ 
