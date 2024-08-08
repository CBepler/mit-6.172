// Copyright (c) 2012 MIT License by 6.172 Staff

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char * argv[]) {  // What is the type of argv?
  int i = 5;
  // The & operator here gets the address of i and stores it into pi
  int * pi = &i;
  // The * operator here dereferences pi and stores the value -- 5 --
  // into j.
  int j = *pi;

  char c[] = "6.172";
  char * pc = c;  // Valid assignment: c acts like a pointer to c[0] here.
  char d = *pc;
  printf("char d = %c\n", d);  // What does this print?

  // compound types are read right to left in C.
  // pcp is a pointer to a pointer to a char, meaning that
  // pcp stores the address of a char pointer.
  char ** pcp;
  pcp = argv;  // Why is this assignment valid?

  const char * pcc = c;  // pcc is a pointer to char constant
  char const * pcc2 = c;  // What is the type of pcc2?

  // For each of the following, why is the assignment:
  //*pcc = '7';  // invalid?   Pointing to const char so value at address can not change
  pcc = *pcp;  // valid?       Pointer itself is not const so it can be rebased to point at something else
  pcc = argv[0];  // valid?   Pointer itself is not const so it can be rebased to point at something else

  char * const cp = c;  // cp is a const pointer to char
  // For each of the following, why is the assignment:
  //cp = *pcp;  // invalid?   Pointer itself is const so can not be rebased to point at something else
  //cp = *argv;  // invalid?     Pointer itself is const so can not be rebased to point at something else
  *cp = '!';  // valid?     The value being pointed to is a non-const char so the value at the address can be changed

  const char * const cpc = c;  // cpc is a const pointer to char const
  // For each of the following, why is the assignment:
  //cpc = *pcp;  // invalid?    pointer itself is char no rebasing
  //cpc = argv[0];  // invalid?   pointer itself is char no rebasing
  //*cpc = '@';  // invalid?    Value being pointed to is const no changing value

  return 0;
}
