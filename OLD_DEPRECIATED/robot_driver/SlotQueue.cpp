#include "SlotQueue.h"
#include <Arduino.h>

//construct a SlotQueue object
SlotQueue::SlotQueue()
{
    //SlotQueue starts empty, and all pointers point to start
    start = 0;
    end = 0;
    num_slots = 0;
}

//adds a slot to the end of the queue
void SlotQueue::enqueue(Slot* s)
{
    if (num_slots >= MAX_SLOTS) 
        return; //if the queue is full, do nothing. This will never happen, unless the guitar has more than 256 slots.
    
    num_slots++;                            //update number of slots in queue
    queue[end++ % MAX_SLOTS] = s;           //insert slot at end of queue and increment the end pointer
}

//removes the first slot from the queue and returns it
Slot* SlotQueue::dequeue()
{
    if (num_slots <= 0) 
        return NULL;              //return an empty slot if none are left

    num_slots--;                            //update the number of slots in the queue
    return queue[start++ % MAX_SLOTS];      //return the slot at the head of the queue
}

//returns a slot from the queue without removing it
Slot* SlotQueue::get(int n)
{
    if (n > num_slots) 
        return NULL;              //return an empty slot
    
    return queue[(start + n) % MAX_SLOTS];  //return the selected slot
}

//returns whether or not the queue has any slots in it
bool SlotQueue::isempty()
{
    return (start == end);
}

//returns the number of slots in the queue
int SlotQueue::length()
{
    return num_slots;
}
