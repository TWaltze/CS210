// compiled on csa2 with the following: gcc -m32 -O2 struct.c -o struct
// objdump -d struct
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#ifdef __6502__
#include "cprintf.h"
#define NULL 0
#define printf cprintf
#define scanf cscanf
#define assert(cond)
#else
#include <stdio.h>
#include <assert.h>
#endif

// define the Employee Class
typedef uint32_t Unum;
#define EMPLOYEE_NAMELEN 80

// data members of and Employee Object
struct Employee {
  Unum          id;
  char          name[EMPLOYEE_NAMELEN];
  int32_t       salary;
};

// methods
Unum Employee_get_id(struct Employee *this)
{
  assert(this);
  return this->id;  
}

void Employee_set_id(struct Employee *this, Unum id)
{
  assert(this);
  this->id = id;
}

void Employee_get_name(struct Employee *this, char *name)
{
  int i;
  assert(this); assert(name);
  for (i=0;i<EMPLOYEE_NAMELEN; i++) name[i] = this->name[i];
}

void Employee_set_name(struct Employee *this, char *name)
{
  int i;
  assert(this); assert(name);
  for (i=0;i<EMPLOYEE_NAMELEN; i++) this->name[i] = name[i];

}

uint32_t Employee_get_salary(struct Employee *this)
{
  assert(this);
  return this->salary;
}

void Employee_set_salary(struct Employee *this, int32_t salary)
{
  assert(this);
  this->salary = salary;
}

void Employee_display(struct Employee *this)
{
  assert(this);
  printf("Employee: %" PRIxPTR ":  %" PRIu32 ": %s :", this, this->id, this->name);
  printf(" %" PRId32 "\n", this->salary);
}

// method that initialize a new employee object
void Employee_Employee(struct Employee *this, Unum id, char *name, int32_t salary)
{
  Employee_set_id(this, id);
  Employee_set_name(this, name);
  Employee_set_salary(this, salary);
}

// allocate memory for a new employee object -- with no initialization
struct Employee *Employee_new()
{
  struct Employee *e = malloc(sizeof(struct Employee));
  if (e == NULL) {
    printf("%s: ERROR: Memory allocation failed\n", __func__);
    assert(e);
    return NULL;
  }
  return e;
}

// destroy and employee object
void Employee_destroy(struct Employee *this)
{
  assert(this); 
  free(this);
}


// definition of List Class

/* Definition of a helper linked list node Class */
struct ListNode {
  struct Employee *emp;
  struct ListNode *next;
  struct ListNode *prev;
};

// function to display the contents of a List Node
/* display */
/* accepts a pointer to the node and displays the memory address of the node,  */
/* the number stored by the node, and the pointers to the previous and next nodes in the list */
void
ListNode_display(struct ListNode *this)
{
  if (this == NULL) {
    printf("NULL\n");
  } else {
    printf("List_Node: 0x%" PRIxPTR " : emp=0x%" PRIxPTR " next=0x%" PRIxPTR " prev=0x%" PRIxPTR "\n", 
	   this, this->emp, this->next, this->prev);
    Employee_display(this->emp);
  }
}

void
ListNode_ListNode(struct ListNode *this, struct Employee *e,
		  struct ListNode *n, struct ListNode *p)
{
  assert(this);
  this->emp = e; this->next = n; this->prev = p;
}

struct ListNode * 
ListNode_new()
{
  struct ListNode *n = malloc(sizeof(struct ListNode));
  if (n == NULL) {
    printf("%s: ERROR: Memory allocation failed\n", __func__);
    assert(n);
    return NULL;
  }
  return n;
}

void
ListNode_destroy(struct ListNode *this)
{
  assert(this);
  Employee_destroy(this->emp);
  free(this);
}

// The actual List Class
struct List {
  struct ListNode *cur;
  int32_t num;
};

// methods of the list class

/* insertbefore */
/* Accepts an int and modifies the list to include a new node inserted before the current node */
/* if the list is empty then a single node list is created with the new node */
/* the new node contains the int passed as an argument */
void 
List_insertbefore(struct List *this, struct Employee *e)
{
  struct ListNode *newNode;
  struct ListNode *previous;

  assert(this); assert(e);
  /* Allocate memory for the new node and initilize it*/
  newNode = ListNode_new();
  ListNode_ListNode(newNode, e, NULL, NULL);

  this->num++;

  /* If the list is empty, just set the list to have the new node */
  if (this->cur == NULL)  {
    this->cur = newNode;
    return;
  }

  previous = this->cur->prev;
  this->cur->prev = newNode;

  newNode->next = this->cur;
  newNode->prev = previous;

  /* if the node before the current node existed, then it now has to point forward to the new node */
  if (previous != NULL) {
    previous->next = newNode;
  }

}

/* insertafter */
/* Accepts an int and modifies the list to include a new node inserted after the current node */
/* if the list is empty then a single node list is created with the new node */
/* the new node contains the int passed as an argument */
void
List_insertafter(struct List *this, struct Employee *e)
{
  struct ListNode *newNode;
  struct ListNode *next;

  assert(this); assert(e);
  /* Allocate memory for the new node */
  newNode = ListNode_new();
  ListNode_ListNode(newNode, e, NULL, NULL);

  this->num++;
  /* If the list is empty, just set the list to have the new node */
  if (this->cur == NULL)  {
    this->cur = newNode;
    return;
  }

  next = this->cur->next;
  this->cur->next = newNode;

  newNode->next = next;
  newNode->prev = this->cur;

  /* if the node after the current node existed, then it now 
     has to point backward to the new node */
  if (next != NULL) {
    next->prev = newNode;
  }

}

/* rm */
/* removes the current node from the list */
/* if the list is empty, then this function has no affect */
/* the memory of the removed node must be freed */
void
List_rm(struct List *this)
{
  struct ListNode *temp;

  assert(this);
  /*   if the list is empty, return */
  if (this->cur == NULL) {
    return;
  }

  if (this->cur->prev) {
    this->cur->prev->next = this->cur->next;
    temp = this->cur->prev;
  }
  if (this->cur->next) {
    this->cur->next->prev = this->cur->prev;
    temp = this->cur->next; 
  }
  if (this->cur->prev == NULL && this->cur->next == NULL) {
    temp = NULL;
  }
  ListNode_destroy(this->cur);
  this->cur = temp;
  this->num--;
}

/* clear */
/* removes all nodes from the list, freeing the memory */
/* calling this function will result in an empty list */
/* if the list is already empty, then this function has no effect */
void
List_clear(struct List *this)
{
  struct ListNode *temp;

  assert(this);
  /* if the list is empty then its already done */
  if (this->cur == NULL) {
    return;
  }

  /* move the current pointer to the beginning of the list */
  while (this->cur->prev != NULL) {
    this->cur = this->cur->prev;
  }

  /* Free the nodes from the list */
  while (this->cur != NULL) {
    temp = this->cur;
    this->cur = this->cur->next;
    ListNode_destroy(temp);
    this->num--;
  }
}

void
List_display(struct List *this)
{
  assert(this);
  ListNode_display(this->cur);
}

/* displayall */
/* For each node in the list, displays the memory address of the node,   */
/* the number stored by the node, and the pointers to the previous and next nodes in the list  */
/* if the list is empty, then this function has no effect */
void
List_displayall(struct List *this)
{
  struct ListNode *temp;
  
  assert(this);
  temp = this->cur;
  /* if the list is empty, nothing to display */
  if (temp == NULL) {
    return;
  }

  /* move the pointer to the beginning of the list */
  while (temp->prev) {
    temp = temp->prev;
  }

  printf("%" PRId32 " nodes on the list:\n", this->num);
  /* display each node in the list */
  while (temp != NULL) {
    ListNode_display(temp);
    temp = temp->next;
  }
}

/* forward */
/* moves the current pointer forward */
/* if the pointer is currently at the end of the list or the list is empty, */
/* this function has no effect */
void
List_forward(struct List *this)
{
  assert(this);
  if (this->cur != NULL && this->cur->next != NULL) {
    this->cur = this->cur->next;
  }
}

/* backward */
/* moves the current pointer backward */
/* if the pointer is currently at the beginning of the list or the list is empty, */
/* this function has no effect */
void
List_backward(struct List *this)
{
  assert(this);
  if (this->cur != NULL && this->cur->prev != NULL) {
    this->cur = this->cur->prev;
  }
}


// initialize a new list object
void
List_List(struct List *this)
{
  assert(this);
  this->cur = NULL;
  this->num = 0;   
}

// allocate the memory for a new list object
struct List *
List_new()
{
  struct List *list;
  list = (struct List *)malloc(sizeof(struct List));

  /* Check that the memory allocation worked */
  if (list == NULL) {
    printf("%s: ERROR: Memory allocation failed\n", __func__);
    return NULL;
  }
  return list;
}

void 
List_destroy(struct List *this)
{
  assert(this);
  if (this->num) {
    List_clear(this);
  }
  free(this);
}

struct Employee * read_employee()
{
  Unum id; 
  char name[EMPLOYEE_NAMELEN]; 
  int32_t salary;
  struct Employee *emp;

  scanf("%" SCNu32 " %79s %" SCNd32, &id, name, &salary);
  emp = Employee_new();
  Employee_Employee(emp, id, name, salary);
  return emp;
}

/* docmd */
/* accepts a character representing the command requested by the user */
/* and calls the appropriate function */
int 
docmd(char cmd, struct List *l)
{
  int rc = 1;

  switch (cmd) {
  case 'd':
    List_display(l);
    break;
  case 'D':
    List_displayall(l);
    break;
  case 'f':
    List_forward(l);
    break;
  case 'b': 
    List_backward(l);
    break;
  case 'I': {
    struct Employee *emp = read_employee();
    List_insertbefore(l,emp);
  }
    break;
  case 'i': {
    struct Employee *emp = read_employee();
    List_insertafter(l,emp);
  }
    break;
  case 'r':
    List_rm(l);
    break;
  case 'c':
    List_clear(l);
    break;
  case 'q':
    rc = 0;
    break;
  case '\n':
    break;
  default:
    printf("Unkown Command\n");
  }
  return rc;
}

/* main */
/* continuously reads a character from stdin and calls docmd to dispatch the command */
int
main(void)
{
  char cmd;
  int rc;

  struct List *theList = List_new();
  List_List(theList);

  while (1) {
    scanf("%c", &cmd);
    rc = docmd(cmd, theList);
    if (rc == 0) break;
  }
  return 0;
}
