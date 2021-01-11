#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
};

void *head = NULL;

struct block_meta *find(struct block_meta **last, size_t size) {
  struct block_meta *current = head;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

struct block_meta *request_mem(struct block_meta* last, size_t size) {
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + sizeof(struct block_meta));
  assert((void*)block == request);
  if (request == (void*) -1) {
    return NULL;
  }

  if (last) {
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->free = 0;
  return block;
}

void *malloc_f(size_t size) {
  struct block_meta *block;

  if (size <= 0) {
    return NULL;
  }

  if (!head) {
    block = request_mem(NULL, size);
    if (!block) {
      return NULL;
    }
    head = block;
  } else {
    struct block_meta *last = head;
    block = find(&last, size);
    if (!block) {
      block = request_mem(last, size);
      if (!block) {
        return NULL;
      }
    } else {
      block->free = 0;
    }
  }

  return(block+1);
}

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

void free_f(void *p) {
  if (!p) {
    return;
  }
  struct block_meta* block_ptr = get_block_ptr(p);
  assert(block_ptr->free == 0);
  block_ptr->free = 1;
}

void *realloc_f(void *p, size_t size) {
  if (!p) {
    return malloc_f(size);
  }
  struct block_meta* block_ptr = get_block_ptr(p);
  if (block_ptr->size >= size) {
    return p;
  }
  void *pointer;
  pointer = malloc_f(size);
  if (!pointer) {
    return NULL;
  }
  memcpy(pointer, p, block_ptr->size);
  free_f(p);
  return pointer;
}

void *calloc(size_t n, size_t l) {
  void *p = malloc_f(n * l);
  memset(p, 0, n * l);
  return p;
}

//https://danluu.com/malloc-tutorial/
#endif
