#ifndef LIST_HPP
#define LIST_HPP


#ifndef NULL
#define NULL 0
#endif


struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

static inline void INIT_LIST_HEAD(struct list_head* list)
{
    list->prev = list;
    list->next = list;
}


static inline struct list_head* list_head_remove(struct list_head* node)
{
    if (node->prev == node || node->next == node) {
        return NULL;
    }
    struct list_head* prev_node = node->prev;
    struct list_head* next_node = node->next;
    prev_node->next = next_node;
    next_node->prev = prev_node;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

static inline void list_head_insert(struct list_head* node, struct list_head* new_node)
{
    struct list_head* next_node = node->next;
    new_node->prev = node;
    new_node->next = next_node;
    node->next = new_node;
    next_node->prev = new_node;
}


static inline struct list_head* list_head_remove_tail(struct list_head* head)
{
    
}

static inline void list_head_remove_head(struct list_head* head)
{
}

static inline void list_head_empty(struct list_head* head)
{
}




#endif // LIST_HPP
