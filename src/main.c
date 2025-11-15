#include <stddef.h>
#include <stdio.h>

#define FX_IMPLEMENTATION
#include "fx.h"

typedef struct Person {
  u32 id;
  u8 age;
  f32 height;
} Person;

void print_sizes(void) {
  /* Use printf here; the including .c should include <stdio.h> before defining
   * FX_IMPLEMENTATION */
  printf("sizeof(u8)   = %zu\n", sizeof(u8));
  printf("sizeof(u16)  = %zu\n", sizeof(u16));
  printf("sizeof(u32)  = %zu\n", sizeof(u32));
  printf("sizeof(u64)  = %zu\n", sizeof(u64));

  printf("sizeof(i8)   = %zu\n", sizeof(i8));
  printf("sizeof(i16)  = %zu\n", sizeof(i16));
  printf("sizeof(i32)  = %zu\n", sizeof(i32));
  printf("sizeof(i64)  = %zu\n", sizeof(i64));

  printf("sizeof(f32)  = %zu\n", sizeof(f32));
  printf("sizeof(f64)  = %zu\n", sizeof(f64));

  printf("sizeof(usize) = %zu\n", sizeof(usize));
  printf("sizeof(wchar) = %zu\n", sizeof(wchar));
  printf("sizeof(Str)   = %zu\n", sizeof(Str));
  printf("sizeof(Person)   = %zu\n", sizeof(Person));
}

void print_alignments(void) {
  /* Use printf here; the including .c should include <stdio.h> before defining
   * FX_IMPLEMENTATION */
  printf("alignof(u8)   = %zu\n", (usize)ALIGNOF(u8));
  printf("alignof(u16)  = %zu\n", (usize)ALIGNOF(u16));
  printf("alignof(u32)  = %zu\n", (usize)ALIGNOF(u32));
  printf("alignof(u64)  = %zu\n", (usize)ALIGNOF(u64));

  printf("alignof(i8)   = %zu\n", (usize)ALIGNOF(i8));
  printf("alignof(i16)  = %zu\n", (usize)ALIGNOF(i16));
  printf("alignof(i32)  = %zu\n", (usize)ALIGNOF(i32));
  printf("alignof(i64)  = %zu\n", (usize)ALIGNOF(i64));

  printf("alignof(f32)  = %zu\n", (usize)ALIGNOF(f32));
  printf("alignof(f64)  = %zu\n", (usize)ALIGNOF(f64));

  printf("alignof(usize) = %zu\n", (usize)ALIGNOF(usize));
  printf("alignof(wchar) = %zu\n", (usize)ALIGNOF(wchar));
  printf("alignof(Str)   = %zu\n", (usize)ALIGNOF(Str));
  printf("alignof(Person)   = %zu\n", (usize)ALIGNOF(Person));
}

int main(int argc, const char *argv[]) {

  Arena *arena = arena_create(4096, ALIGNOF(void *));
  Person *people = (Person *)arena_alloc_aligned(arena, 10 * sizeof(Person),
                                                 ALIGNOF(Person));
  for (u32 i = 0; i < 10; i++) {
    Person *p = &people[i];
    p->id = i;
    p->age = 20 + i;
    p->height = 5.5f + (f32)i * 0.1f;
    people[i] = *p;
  }

  for (u32 i = 0; i < 10; i++) {
    printf("Person %u: age=%u, height=%.2f\n", people[i].id, people[i].age,
           people[i].height);
  }
  arena_destroy(arena);

  u32 result = fx_example(21);
  printf("fx_example result: %d\n", result);
  print_alignments();
  print_sizes();
  return EXIT_SUCCESS;
}