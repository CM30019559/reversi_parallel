#define mailbox_c

#include "mailbox.h"

#define NO_MAILBOXES 30

static void* shared_memory = NULL;
static mailbox* freelist = NULL;  /* list of free mailboxes.  */


/*
 *  initialise the data structures of mailbox.  Assign prev to the
 *  mailbox prev field.
 */

static mailbox* mailbox_config(mailbox* mbox, mailbox* prev)
{
    mbox->in = 0; // My line of code
    mbox->out = 0; // My line of code
    // mbox->data.result = 0; // Unrequired line of code
    // mbox->data.move_no = 0; // Unrequired line of code
    // mbox->data.positions_explored = 0; // Unrequired line of code
    mbox->prev = prev;
    mbox->item_available = multiprocessor_initSem(0);
    mbox->space_available = multiprocessor_initSem(MAX_MAILBOX_DATA); // My line of code
    mbox->mutex = multiprocessor_initSem(1);
    return mbox;
}


/*
 *  init_memory - initialise the shared memory region once.
 *                It also initialises all mailboxes.
 */

static void init_memory(void)
{
    if (shared_memory == NULL)
    {
        mailbox* mbox;
        mailbox* prev = NULL;
        int i;
        _M2_multiprocessor_init();
        shared_memory = multiprocessor_initSharedMemory
        (NO_MAILBOXES * sizeof(mailbox));
        mbox = shared_memory;
        for (i = 0; i < NO_MAILBOXES; i++)
            prev = mailbox_config(&mbox[i], prev);
        freelist = prev;
    }
}


/*
 *  init - create a single mailbox which can contain a single triple.
 */

mailbox* mailbox_init(void)
{
    mailbox* mbox;

    init_memory();
    if (freelist == NULL)
    {
        printf("exhausted mailboxes\n");
        exit(1);
    }
    mbox = freelist;
    freelist = freelist->prev;
    return mbox;
}


/*
 *  kill - return the mailbox to the freelist.  No process must use this
 *         mailbox.
 */

mailbox* mailbox_kill(mailbox* mbox)
{
    mbox->prev = freelist;
    freelist = mbox;
    return NULL;
}


/*
 *  send - send (result, move_no, positions_explored) to the mailbox mbox.
 */

void mailbox_send(mailbox* mbox, int result, int move_no, int positions_explored) // mailbox_send is a producer to the shared buffer, sending the result, move_no and positions_explored to the shared buffer
{
    // My code
    multiprocessor_wait(mbox->space_available); // Uses function multiprocessor_wait to wait for semaphore space_available to be available stopping us from overflowing the mailbox
    multiprocessor_wait(mbox->mutex); // Uses function multiprocessor_wait to wait for semaphore mutex to be available to allow access to the mailbox

    mbox->data[mbox->in].result = result; // Adding the data through the in index to the shared buffer for the result value
    mbox->data[mbox->in].move_no = move_no; // Adding the data through the in index to the shared buffer for the move_no value
    mbox->data[mbox->in].positions_explored = positions_explored; // Adding the data through the in index to the shared buffer for the positions_explored value
    mbox->in = (mbox->in + 1) % MAX_MAILBOX_DATA; // This is a loop to increment in index by 1, divide by MAX_MAILBOX_DATA and place remainder into in index ensuring we don't overflow

    multiprocessor_signal(mbox->mutex); // Uses function multiprocessor_signal to signal that an item is being added to the buffer by the semaphore mutex and mutex is now available
    multiprocessor_signal(mbox->item_available); // Uses function multiprocessor_signal to signal that an item is now available in the buffer using the semaphore item_available
}


/*
 *  rec - receive (result, move_no, positions_explored) from the
 *        mailbox mbox.
 */

void mailbox_rec(mailbox* mbox, int* result, int* move_no, int* positions_explored) // mailbox_rec is a consumer from the shared buffer, retrieving the result, move_no and positions_explored from the shared buffer
{
    // My code
    multiprocessor_wait(mbox->item_available); // Uses function multiprocessor_wait to wait for semaphore item_available to say there is an item available
    multiprocessor_wait(mbox->mutex); // Uses function multiprocessor_wait to wait for semaphore mutex to be available

    *result = mbox->data[mbox->out].result; // Removing the data through the out index from the shared buffer for the result value
    *move_no = mbox->data[mbox->out].move_no; // Removing the data through the out index from the shared buffer for the move_no value
    *positions_explored = mbox->data[mbox->out].positions_explored; // Removing the data through the out index from the shared buffer for the positions_explored value
    mbox->out = (mbox->out + 1) % MAX_MAILBOX_DATA; // This is a loop to increment out index by 1, divide by MAX_MAILBOX_DATA and place remainder into out index ensuring we don't overflow

    multiprocessor_signal(mbox->mutex); // Uses function multiprocessor_signal to signal that an item has been added to the buffer by the semaphore mutex and mutex is now available
    multiprocessor_signal(mbox->space_available); // Uses function multiprocessor_signal to signal that space is now available in the buffer using the semaphore space_available
}
