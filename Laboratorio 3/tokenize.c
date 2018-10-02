#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int thc;    // thread count
int max = 100;  // size string
sem_t *sems;  // semaforo

void *tokenize(void *rank){
  int my_rank = (int)rank;
  int count;
  int next = (my_rank+1)%thc;

  char *fg_rv;
  char *my_line[max];
  char *my_string;

  sem_wait(&sems[my_rank]); 
  fg_rv = fgets(my_line, max, stdin);
  sem_post(&sems[next]);

  while(fg_rv != NULL){
    printf("Thread %ld > my line = %s", my_rank, my_line);

    count = 0; 
    my_string = strtok(my_line, " \t\n");
    while(my_string != NULL){
      count++;
      printf("Thread %ld > string %d = %s\n", my_rank, count, my_string);
      my_string = strtok(NULL, " \t\n");
    } 

    sem_wait(&sems[my_rank]); 
    fg_rv = fgets(my_line, max, stdin);
    sem_post(&sems[next]);
  }

  return NULL;
}

int main(int argc, char const *argv[]){
  pthread_t *threads;

  thc = 2;
  threads = (pthread_t*)malloc(thc*sizeof(pthread_t));
  sems = (sem_t*)malloc(thc*sizeof(sem_t));

  sem_init(&sems[0],0,1);

  int i;
  for(i=1; i<thc; i++){
    sem_init(&sems[i],0,0); 
  }

  // printf("Text: \t");
  for(i=0; i<thc; i++)
    pthread_create(&threads[i], (pthread_attr_t*), NULL,tokenize, (void*)i);

  for(i=0; i<thc; i++)
    pthread_join(threads[i], NULL);

  for(i=0; i<thc; i++)
    sem_destroy(&sems[i]);

  free(sems);
  free(threads);
  return 0;
}