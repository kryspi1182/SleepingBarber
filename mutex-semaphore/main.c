#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

struct List
{
    int client_id;
    struct List *next;
};

pthread_mutex_t chair = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitroom = PTHREAD_MUTEX_INITIALIZER;
int waiting_counter = 0;
int res_counter = 0;
int wait_limit = 10;
int debug = 0;
sem_t client_waiting;
sem_t barber_sleeping;
int finished = 0;
int next_client = 0;
struct timespec ts;

struct List *rejected = NULL;
struct List *waiting = NULL;

void write_rejected()
{
    struct List *temp = rejected;
    printf("Did not get in: ");
    while(temp!=NULL)
    {
        printf("%d ",temp->client_id);
        temp = temp->next;
    }
    printf("\n");
}
void write_waiting()
{
    struct List *temp = waiting;
    printf("Still waiting : ");
    while(temp!=NULL)
    {
        printf("%d " ,temp->client_id);
        temp = temp->next;
    }
    printf("\n");
}
void add_rejected(int x)
{
    struct List *temp = (struct List*)malloc(sizeof(struct List));
    temp->client_id = x;
    temp->next = rejected;
    rejected = temp;
    write_rejected();
}
void add_waiting(int x)
{
    struct List *temp = (struct List*)malloc(sizeof(struct List));
    temp->client_id = x;
    temp->next = waiting;
    waiting = temp;

    write_waiting();
}
void remove_waiting(int x)
{
    struct List *temp = waiting;
    struct List *pop = waiting;
    while (temp!=NULL)
    {
        if(temp->client_id==x)
        {
            if(temp->client_id==waiting->client_id)
            {
                waiting=waiting->next;
                free(temp);
            }
            else
            {
                pop->next=temp->next;
                free(temp);
            }
            break;
        }
        pop=temp;
        temp=temp->next;
    }
    write_waiting();

}

void wait(double number)
{
    double i = 0;
    double j = 0;
    for(i=0;i<number;i++)
    {
        for(j=0;j<number;j++)
            continue;
    }
}
void *Client(void *num)
{
    int nr = (int*)num;
    pthread_mutex_lock(&waitroom); // lock the waiting room, clients walk in one by one 
    if(waiting_counter > wait_limit)
    {
        res_counter++;
        waiting_counter--;
        if(debug==1)
            add_rejected(nr);

        pthread_mutex_unlock(&waitroom); // unlock the waiting room, this client gave up, so the next can walk in
        return;
    }
    sem_post(&client_waiting); // inform the barber that a client is waiting
    if(debug==1)
        add_waiting(nr);

    pthread_mutex_unlock(&waitroom); // unlock the waiting room, another client can walk through the door
    while(nr!=next_client)
    {
        wait(10000);
    }
    pthread_mutex_lock(&chair); // lock the chair, only one client at a time can have his hair cut
    
    sem_wait(&barber_sleeping); // waiting for the barber to wake up
    
    waiting_counter--;
    if(debug==1)
        remove_waiting(nr);

    printf("Resigned: %d | Waiting Room %d/%d | [Client number having his hair cut right now: %d]\n", res_counter, waiting_counter, wait_limit, nr);
    wait(30000);
    pthread_mutex_unlock(&chair); // unlock the chair, client had his hair cut
          
    return;
}

void *Barber()
{
    while(1)
    {
        if(finished == 1)
            return;
        sem_post(&barber_sleeping); // send info about the barber napping
        if(sem_timedwait(&client_waiting, &ts)==-1); // wait for a while for a client, if none appears, the loop goes into another iteration
            continue;
    }
}


int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        int bug = strcmp("-debug", argv[1]);
        if(bug == 0)
            debug = 1;
        else
        {
            printf("Incorrect parameter\n");
            return 0;
        }
        
    }
    else if(argc > 2)
    {
        printf("Incorrect number of parameters\n");
        return 0;
    }
    sem_init(&client_waiting, 0, 0);
    sem_init(&barber_sleeping, 0, 0);
    ts.tv_sec = 15;
    int n = 15;
    pthread_t threads[n];
    pthread_t barber;
    int i = 0;
    int j = 0;
    pthread_create(&barber, NULL, Barber, NULL);
    for(i=0;i<n;i++)
    {
        waiting_counter++;
        wait(3000);
        pthread_create(&threads[i], NULL, Client, (void *)i);  
        
    }
    for(j=0; j<n; j++)
    {
        pthread_join(threads[j], NULL);
        next_client++;
    }
    finished = 1;
    pthread_join(barber, NULL);
    printf("Finish\n");
    return 0;
}