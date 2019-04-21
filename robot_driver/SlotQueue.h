#ifndef SLOT_QUEUE_H
#define SLOT_QUEUE_H

#define MAX_SLOTS 256 //god help you if this isn't big enough


//header for the slot queue class


struct Slot                 //object to store detected slots in the management queues
{
    unsigned long timestamp;  //time that this slot recieved glue
    long coordinate;          //position of this slot along the slide
    char target;              //current task to be performed on slot 'g':glue, 'p':press
};


class SlotQueue
{
public:
    SlotQueue();              //create an empty slot queue
    void enqueue(Slot* s);    //add a slot to the queue
    Slot* dequeue();          //return the next slot in the queue, and remove it from the queue
    Slot* get(int n=0);       //get the n'th slot in the queue (default first one)
    bool isempty();           //determine if the queue is empty
    int length();             //return the length of the queue

private:
    int start;
    int end;
    int num_slots;
    Slot* queue[MAX_SLOTS];      //allow up to MAX_SLOTS (256) slots in the queue
};


#endif
