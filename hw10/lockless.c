struct pointer_t {
    node_t* ptr;
    unsigned int count;
};

struct node_t {
    data_t value;
    pointer_t next;
};

struct queue_t {
    pointer_t head;
    pointer_t tail;
};

void initialize(queue_t* q, data_t value) {
    node_t* node = new_node(); // Allocate a new node
    node->value = value; 
    node->next.ptr = NULL; // Set the next pointer to NULL
    q->head.ptr = node; // Set the head and tail to the new node
    q->tail.ptr = node;
}
void enqueue(queue_t* q, data_t value) {
    node_t* node = new_node(); //allocate a new node
    node->value = value; //set value
    node->next.ptr = NULL; //set next to null
    pointer_t tail; //make node pointer
    while (true) {
        tail = q->tail;     //set tail to queue tail
        pointer_t next = tail.ptr->next;    //create a new node pointer pointing to the next of tail
        if (tail == q->tail) {  //check that tail is still the same
            if (next.ptr == NULL) {     //check that tail next is null
                if (CAS(&tail.ptr->next, next, (struct pointer_t) {node, next.count + 1 })) {   //If tail and next values are still correct with count aligned then swap
                break;
                }
            } else {
                CAS(&q->tail, tail, (struct pointer_t) {next.ptr, tail.count + 1 });  //If tail next was not NULL then CAS to set tail to tail->next
            }
        }
    }
    CAS(&q->tail, tail, (struct pointer_t) { node, tail.count + 1 });   //CAS to set new tail
}

bool dequeue(queue_t* q, data_t* pvalue) {
    pointer_t head; //make head pointer
    while (true) {
        head = q->head;
        pointer_t tail = q->tail;
        pointer_t next = head.ptr->next;
        if (head == q->head) { //check head still holds right value
            if (head.ptr == tail.ptr) {
                if (next.ptr == NULL) {
                return false; //empty queue
                }
                CAS(&q->tail, tail, (struct pointer_t) { next.ptr, tail.count + 1}); //set tail to tail->next
            } else {
                *pvalue = next.ptr->value; //get value of item we are removing
                if (CAS(&q->head, head, (struct pointer_t) { next.ptr, head.count + 1})) { //check head still has right value and if so set head to head->next
                    break;
                }
            }
        }
    }
    free_node(head.ptr); //free pld head
    return true; //successful dequeue
}