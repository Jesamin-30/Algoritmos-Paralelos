#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

typedef int bool;
#define true 1
#define false 0

struct node{
  int data;
  struct node* next;
};

struct node *head = NULL;

int max;
int thc, nopt;
int ninsert, nsearch, ndelete;

pthread_rwlock_t rwlock;

bool insert(int value){
  struct node *curr = head;
  struct node *pred = NULL;
  struct node *tmp;

  while(curr!=NULL && curr->data<value){
    pred = curr;
    curr = curr->next;
  }

  if(curr==NULL || curr->data>value){
    tmp = malloc(sizeof(struct node));
    tmp->data = value;
    tmp->next = curr;

    if(pred==NULL)  head = tmp;
    else  pred->next = tmp;
    return true;
  } 
  return false;
}

bool search(int value){
  struct node *tmp;

  tmp = head;
  while(tmp!=NULL && tmp->data<value){
    tmp = tmp->next;
  }

  return (tmp==NULL || tmp->data>value)? false: true;
}

bool delete(int value) {
  struct node *curr = head;
  struct node *pred = NULL;

  while(curr != NULL && curr->data < value){
    pred = curr;
    curr = curr->next;
  }

  if(curr != NULL && curr->data==value){
    if(pred == NULL){
      head = curr->next;      
      free(curr);
    }
    else{
      pred->next = curr->next;      
      free(curr);
    }

    return true;
  } 

  return false;
}

bool lempty(){
  return (head == NULL)? true: false;
}

void lfree(){
  struct node *curr, *foll;

  if(lempty())  return;

  curr = head;
  foll = curr->next;

  while(foll != NULL){
    free(curr);
    curr = foll;
    foll = curr->next;
  }
  free(curr);
}

void* operacion(void *rank){
  int my_rank = (int)rank;
  int thopt = nopt/thc;

  int i, opt, val;
  for(i=0; i<thopt; i++){
    opt = rand()%nopt;
    val = rand()%max;

    if(opt < nsearch){
      pthread_rwlock_rdlock(&rwlock);
      search(val);
      pthread_rwlock_unlock(&rwlock);
    }
    else if(opt < nsearch+ninsert){
      pthread_rwlock_wrlock(&rwlock);
      insert(val);
      pthread_rwlock_unlock(&rwlock);
    }
    else{
      pthread_rwlock_wrlock(&rwlock);
      delete(val);
      pthread_rwlock_unlock(&rwlock);
    }
  }
  return NULL;
}

int main(int argc, char const *argv[]){
  thc = 4;
  ninsert = 20000;
  ndelete = 100;
  nsearch = 100;
  nopt = ninsert+ndelete+nsearch;
  max = nopt*10;
    
  int i;
  double time;
    struct timeval ti, tf;
    
    pthread_t *threads = malloc(thc*sizeof(pthread_t));
  pthread_rwlock_init(&rwlock, NULL);

    gettimeofday(&ti, NULL);  
  for(i=0; i<thc; i++)
    pthread_create(&threads[i], NULL, operacion, (void *)i);

  for(i=0; i<thc; i++)
    pthread_join(threads[i], NULL); 
  gettimeofday(&tf, NULL);

    time = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000;   
    printf("tiempo: %.6lf s\n",time/1000);

  lfree();
  pthread_rwlock_destroy(&rwlock);
  free(threads);
  return 0;
}