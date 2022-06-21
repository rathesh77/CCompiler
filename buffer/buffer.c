/* vim: set tabstop=4:softtabstop=4:shiftwidth=4 */
#include "buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"

void buf_init(buffer_t *buffer, FILE *fd, int size) {
 // memset(buffer->content, 0, size);
  buffer->content = malloc(sizeof(char) * size);
  buffer->fd = fd;
  buffer->it = 0;
  buffer->end = 0;
  buffer->lock = 0;
  buffer->islocked = false;
  buffer->avail = 0;
  buffer->bytesread = 0;
  buffer->bytesreadsincelock = 0;
  buffer->currchar = 0;
  buffer->eof = false;
  buffer->size = size;
}

bool buf_eof_strict(buffer_t *buffer) {
  return buffer->avail == 0 && buffer->eof == true;
}

bool buf_eof(buffer_t *buffer) {
  buf_skipblank(buffer);
  return buf_eof_strict(buffer);
}

void buf_lock(buffer_t *buffer) {
  if (buffer->islocked) {
    fprintf(stderr, "Warning: lock was already set.\n");
    // print_backtrace();
  }
  buffer->bytesreadsincelock = 0;
  buffer->lock = buffer->it;
  buffer->islocked = true;
}

void buf_unlock(buffer_t *buffer) {
  if (!buffer->islocked) {
    fprintf(stderr, "Warning: lock was not set.\n");
    // print_backtrace();
  }
  buffer->bytesreadsincelock = 0;
  buffer->islocked = false;
}

static size_t buf_fread(buffer_t *buffer, size_t offset, size_t n) {
  // printf("read %lu bytes\n", n);
  size_t cnt = fread(&(buffer->content[offset]), 1, n, buffer->fd);
  if (cnt < n) {
    buffer->eof = true;
  }
  buffer->bytesread += cnt;

  return cnt;
}

static void buf_mod(size_t *val, size_t toadd) {
  *val = (*val + toadd);
}

static void buf_mod_backwards(size_t *val, size_t tosub) {
  *val = (*val - tosub);
}

static void buf_move_it(buffer_t *buffer, size_t cnt) {
  buf_mod(&buffer->it, cnt);
  buffer->currchar += cnt;
}

static void buf_move_it_bw(buffer_t *buffer, size_t cnt) {
  buf_mod_backwards(&buffer->it, cnt);
  buffer->currchar -= cnt;
}

char buf_getchar(buffer_t *buffer) {

  char ret = buffer->content[buffer->it];
  buf_move_it(buffer, 1);
  buffer->avail -= 1;
  return ret;

}

char buf_getchar_after_blank(buffer_t *buffer) {
  buf_skipblank(buffer);
  return buf_getchar(buffer);
}

void buf_forward(buffer_t *buffer, size_t n) {
  buf_move_it(buffer, n);
  buffer->avail -= n;
}

void buf_rollback(buffer_t *buffer, size_t n) {
  if (!buffer->islocked) {
    //fprintf(stderr, "Warning: rollback without lock.\n");
    // print_backtrace();
  }
  buf_move_it_bw(buffer, n);
  buffer->avail += n;
}

void buf_rollback_and_unlock(buffer_t *buffer, size_t n) {
  buf_rollback(buffer, n);
  buf_unlock(buffer);
}

size_t buf_skipblank(buffer_t *buffer) {
  size_t count = 0;
  char next = buf_getchar(buffer);
  while (ISBLANK(next)) {
    next = buf_getchar(buffer);
    count++;
  }
  buf_rollback(buffer, 1);
  if (next == '\0') {
    buffer->avail = 0;
  }
  return count;
}

char buf_getchar_rollback(buffer_t *buffer) {
  bool waslocked = true;
  buf_skipblank(buffer);
  char next = buf_getchar(buffer);
  buf_rollback(buffer, 1);
  return next;
}

void buf_print(buffer_t *buffer) {
  printf(COLOR_BLUE "#### <buffer> ####\n" COLOR_DEFAULT);
#ifdef WIN32
  printf(COLOR_GREEN "it: %u\nend: %u\navail: %u\nlock: %u\n" COLOR_DEFAULT,
         buffer->it, buffer->end, buffer->avail, buffer->lock);
#else
  printf(COLOR_GREEN "it: %zu\nend: %zu\navail: %zu\nlock: %zu\n" COLOR_DEFAULT,
         buffer->it, buffer->end, buffer->avail, buffer->lock);
#endif
  char *color = NULL;
  for (size_t i = 0; i < BUF_SIZE; i++) {
    if (buffer->it != i && buffer->end != i &&
        (!buffer->islocked || buffer->lock != i)) {
      printf("%c", buffer->content[i]);
      continue;
    }
    if (buffer->it == i)
      color = COLOR_BG_GREEN;
    else if (buffer->end == i)
      color = COLOR_BG_RED;
    else if (buffer->islocked && buffer->lock == i)
      color = COLOR_BG_BLUE;
    if (buffer->content[i] == '\n')
      printf("%s %s\n", color, COLOR_DEFAULT);
    else
      printf("%s%c" COLOR_DEFAULT, color, buffer->content[i]);
  }
  printf(COLOR_BLUE "#### </buffer> ####\n" COLOR_DEFAULT);
}
