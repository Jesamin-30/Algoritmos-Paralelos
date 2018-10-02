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
	pthread_mutex_t mutex;
	struct node* next;
};

struct node *head = NULL;
pthread_mutex_t head_mutex;

int max;
int thc, nopt;
int ninsert, nsearch, ndelete;


void pinit(struct node **curr, struct node **pred){
	*pred = NULL;
	pthread_mutex_lock(&head_mutex);
	*curr = head;
	if(*curr != NULL)
		pthread_mutex_lock(&((*curr)->mutex));
}

void padvance(struct node **curr, struct node **pred){
	struct node *tcurr = *curr;
	struct node *tpred = *pred;

	if(tcurr == NULL){
		if(tpred == NULL)
			pthread_mutex_unlock(&head_mutex);
	}
	else{
		if(tcurr->next != NULL)
			pthread_mutex_lock(&(tcurr->next->mutex));

		if(tpred != NULL)	pthread_mutex_unlock(&(tpred->mutex));
		else	pthread_mutex_unlock(&head_mutex);

		*pred = tcurr;
		*curr = tcurr->next;
	}
}


bool linsert(int value){
	struct node *curr, *pred, *tmp;

	pinit(&curr, &pred);

	while(curr!=NULL && curr->data<value){
		padvance(&curr, &pred);
	}

	if(curr==NULL || curr->data>value){
		tmp = malloc(sizeof(struct node));
		pthread_mutex_init(&(tmp->mutex), NULL);
		tmp->data = value;
		tmp->next = curr;

		if(curr!=NULL)
			pthread_mutex_unlock(&(curr->mutex));
		if(pred == NULL){
			head = tmp;
			pthread_mutex_unlock(&head_mutex);
		}
		else{
			pred->next = tmp;
			pthread_mutex_unlock(&(pred->mutex));
		}
	}
	else{
		if(curr != NULL)	pthread_mutex_unlock(&(curr->mutex));
		if(pred != NULL)	pthread_mutex_unlock(&(pred->mutex));
		else	pthread_mutex_unlock(&head_mutex);

		return false;
	}
	return true;
}

bool lsearch(int value) {
	struct node *tmp, *ltmp;

	pthread_mutex_lock(&head_mutex);
	tmp = head;

	if(tmp != NULL)	pthread_mutex_lock(&(tmp->mutex));

	pthread_mutex_unlock(&head_mutex);
	while(tmp != NULL && tmp->data<value){
		if(tmp->next != NULL) 
			pthread_mutex_lock(&(tmp->next->mutex));
		ltmp = tmp;
		tmp = tmp->next;
		pthread_mutex_unlock(&(ltmp->mutex));
	}

	if(tmp == NULL || tmp->data>value) {
		if(tmp != NULL) 
			pthread_mutex_unlock(&(tmp->mutex));
		return false;
	}
	else{
	  	pthread_mutex_unlock(&(tmp->mutex));
	  	return true;
	}
}

bool lempty(){
	return (head == NULL)? true: false;
}

void lfree(){
	struct node *curr, *foll;

	if(lempty())	return;

	curr = head;
	foll = curr->next;

	while(foll != NULL){
		free(curr);
		curr = foll;
		foll = curr->next;
	}
	free(curr);
}

bool ldelete(int value) {
	struct node *curr, *pred;

	pinit(&curr, &pred);

	while(curr != NULL && curr->data < value)
		padvance(&curr, &pred);

	if(curr != NULL && curr->data==value){
		if(pred == NULL){
			head = curr->next;
			pthread_mutex_unlock(&head_mutex);
			pthread_mutex_unlock(&(curr->mutex));
			pthread_mutex_destroy(&(curr->mutex));
			free(curr);
		}
		else{
			pred->next = curr->next;
			pthread_mutex_unlock(&(pred->mutex));
			pthread_mutex_unlock(&(curr->mutex));
			pthread_mutex_destroy(&(curr->mutex));
			free(curr);
		}
	}
	else{
		if(pred != NULL)  pthread_mutex_unlock(&(pred->mutex));
		if(curr != NULL)  pthread_mutex_unlock(&(curr->mutex));
		if(curr == head)  pthread_mutex_unlock(&head_mutex);
		return false;
	}

	return true;
}

void* thread_opt(void *rank){
	int my_rank = (int)rank;
	int thopt = nopt/thc;

	int i, opt, val;
	int lcinsert, lcdelete, lcsearch;
	for(i=0; i<thopt; i++){
		opt = rand()%nopt;
		val = rand()%max;

		if(opt < nsearch){
			lsearch(val);
			lcsearch++;
		}
		else if(opt < nsearch+ninsert){
			linsert(val);
			lcinsert++;
		}
		else{
			ldelete(val);
			lcdelete++;
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
		
	pthread_mutex_init(&head_mutex, NULL);
	int i;
	double time;
    struct timeval ti, tf;
    
    pthread_t *threads = malloc(thc*sizeof(pthread_t));

    gettimeofday(&ti, NULL);	
	for(i=0; i<thc; i++)
		pthread_create(&threads[i], NULL, thread_opt, (void *)i);

	for(i=0; i<thc; i++)
		pthread_join(threads[i], NULL);	
	gettimeofday(&tf, NULL);

    time = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000;   
    printf("Time: %.6lf s\n",time/1000);

	lfree();
	pthread_mutex_destroy(&head_mutex);
	free(threads);
	return 0;
}