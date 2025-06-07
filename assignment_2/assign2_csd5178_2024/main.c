/* =============================================== HY_345 assignment_2 ===================================================================*/
/* ==== csd5178 ASPRIDIS NIKOLAOS ================================================================================*/
/* ================================ 06/11/24 ====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define Stop_A 0
#define Bus 1
#define Stop_B 2
#define University 3

#define hasStudied 1
#define hasNotStudied 0

#define Math 0
#define Physics 1
#define Chemistry 2
#define CSD 3

/* student's info struct */
typedef struct student 
{
	int AM;
	int study_time;
	char *school;
	int place;
	int study_condition;

	struct student *next;
	struct student *prev;
}student;

sem_t sem_student_created;
sem_t sem_student_got_into_the_bus;
sem_t sem_student_got_out_of_the_bus;
sem_t sem_student_went_to_university; 
sem_t *sem_student;

pthread_t bus_th;

pthread_mutex_t lock;

int std_num; /* number of students */

student *Q_Stop_A; /* pointer to student queue (students waiting into stop A) */
student *Q_Stop_B; /* pointer to student queue (students waiting into stop B) */
student *Q_University; /* pointer to student queue (students studying in university) */
student *Q_Bus; /* pointer to students that are into the bus */


int enter_stud_num(); 
void student_struct_create(student *s, int student_id);
void *student_procedure(void *);
void *bus_procedure(void *);
void Enqueue(student *, student **);
void Remove(student *, student **);
void general_print();

int main() 
{
	pthread_t* students_th;
	student* students_st; 
	int i, err;
	
	Q_Stop_A = NULL; /* queues are empty in the beginning */
	Q_Stop_B = NULL;	
	Q_University = NULL;
	Q_Bus = NULL;

	srand(time(NULL)); /* sets different seed on every execution of the program */

	std_num = enter_stud_num(); /* user types students' number*/	

	/* memory allocation for student semaphores */
	sem_student = (sem_t *)malloc(std_num * sizeof(sem_t));
	if(!sem_student)
	{
		fprintf(stderr, "ERROR: Malloc failed (sem_student)!\n-1 is returned!\n\n");
		return -1;
	}

	/* initialise all student semaphores with value 0 */
	for(i=0; i<std_num; i++)
	{
		err = sem_init(sem_student + i, 0, 0);
		if(err)
		{
			fprintf(stderr, "ERROR: initialisation of sem_student[%d] failed with errno: %d\nProgram terminated with exit code %d\n\n", i, errno, errno);
			return errno;
		}
	}

	/* semaphore used from students in order to signal bus that they have
 	 * all arrived at stop A and are waiting for the bus to come (way of not
 	 * letting bus start boarding students before all of them have arrived to stop
 	 * A / have been created) */
	err = sem_init(&sem_student_created, 0, 0);
	if(err)
	{
		fprintf(stderr, "ERROR: initialisation of sem_student_created failed with errno: %d\nProgram terminated with exit code %d\n\n", errno, errno);
		return errno;
	}

	/* semaphore used from students in order to signal bus driver they have gotten in */
	err = sem_init(&sem_student_got_into_the_bus, 0, 0);
	if(err)
	{
		fprintf(stderr, "ERROR: initialisation of sem_student_got_into_the_bus failed with errno: %d\nProgram terminated with exit code %d\n\n", errno, errno);
		return errno;
	}	

	/* semaphore used from students to signal bus driver they have gotten out of the bus */
	err = sem_init(&sem_student_got_out_of_the_bus, 0, 0);
	if(err)
	{
		fprintf(stderr, "ERROR: initialisation of sem_student_got_out_of_the_bus failed with errno: %d\nProgram terminated with exit code %d\n\n", errno, errno);
		return errno;
	}

	/* semaphore used from students to signal bus driver they have gone to University */
	err = sem_init(&sem_student_went_to_university, 0, 0);
	if(err)
	{
		fprintf(stderr, "ERROR: initialisation of sem_student_went_to_university failed with errno: %d\nProgram terminated with exit code %d\n\n", errno, errno);
		return errno;
	}

	/*memory allocation for student threads */
	students_th = (pthread_t*)malloc(std_num * sizeof(pthread_t));
	if(!students_th)
	{
		fprintf(stderr, "ERROR: Malloc failed (students_th)!\n-1 is returned!\n\n");
		return -1;
	}

	/* memory allocation for student structs */
	students_st = (student*)malloc(std_num * sizeof(student));	
	if(!students_st)
	{
		fprintf(stderr, "ERROR: Malloc failed (students_st)!\n-1 is returned!\n\n");
		return -1;
	}

	/* initialise stopA's bus semaphore with value 0 (third argmnt) so as all students created to wait on Stop A (sem_wait(&sem1)) */
	/* err = sem_init(&sem1_stopA_waitTillTheBusComes, 0, 0);
	if(err)
	{
		fprintf(stderr, "ERROR: initialisation of sem1 failed with errno: %d\nProgram terminated with exit code %d\n\n", errno, errno);
		return errno;
	} */

	/* initialises lock mutex object */
	err = pthread_mutex_init(&lock, NULL);
	if(err)
	{
		fprintf(stderr, "ERROR: initialisation of lock failed with error number: %d\nProgram terminated with exit code %d\n\n", err, err);
		return err;
	}	

	/* students initialisation */
	for(i=0; i<std_num; i++)
	{
		/* 
		 * student struct creation 
		 * (CAUTION: the order in which the structs are created may differ from the order in which the students arrive
 		 * to stop A, because a thread that actually got created after another thread may "run" faster to stop A) 
		*/
		student_struct_create(students_st + i, i+1);	

		/* student thread is created and runs student_procedure, with parameter a pointer to student's struct */
		if(pthread_create(students_th + i, NULL, &student_procedure, students_st + i))
		{
			fprintf(stderr, "ERROR: pthread_create() for student thread %d failed!\n1 is returned!\n\n", i);
			return 1;
		}
	}

	/* bus initialisation */
	if(pthread_create(&bus_th, NULL, &bus_procedure, NULL))
	{
		fprintf(stderr, "ERROR: pthread_create for bus thread failed!\n1 is returned!\n\n");
		return 1;
	}

	/* wait till all student threads have terminated and only afterwards proceed */
	for(i=0; i<std_num; i++)
	{
		if(pthread_join(students_th[i], NULL))
		{
			fprintf(stderr, "ERROR: pthread_join() for student thread %d failed!\n-1 is returned!\n\n", i);
			return 1;
		}
	}

	/* now that all student threads have terminated, wait for the bus thread to terminate before proceeding to the execution of the following instructions */
	if(pthread_join(bus_th, NULL))
	{
		fprintf(stderr, "ERROR: pthread_join() for bus thread failed!\n-1 is returned!\n\n");
		return 1;
	}

	return 0;
}

void *bus_procedure(void *s)
{
	int i, sl_CSD, sl_Chemistry, sl_Physics, sl_Math;
	student *ptr, *prev;
	
	/* sl_[school] vars are initialised with no. 3 , because in the beginning the bus is empty */
	sl_CSD = 3;
	sl_Chemistry = 3;
	sl_Physics = 3;
	sl_Math = 3;

	/* bus waits until all students have gone to stop A (until all student threads have been created) */
	for(i=0; i<std_num; i++)
	{
		sem_wait(&sem_student_created);
	}

	while(1)
	{
		/* bus starts its journey */
		if(Q_Stop_A)
		{
			/* there is at least one student waiting in stop A */
			ptr = Q_Stop_A;
			while(ptr)
			{
				prev = ptr;
				ptr = ptr->next;
				
				if(!strcmp(prev->school,"Math") && sl_Math)
				{
					/* student studies mathematics and there is an available seat for math students */
					/* bus driver lets that student get into the bus */
					sl_Math--;
					sem_post(&sem_student[(prev->AM)-1]);
					/* bus driver waits till student tells him: "ok! I got in!"*/
					sem_wait(&sem_student_got_into_the_bus);
				}
				else if(!strcmp(prev->school,"Physics") && sl_Physics)
				{
					/* student studies physics and there is an available seat for physics students */
					/* bus driver lets that student get into the bus */
					sl_Physics--;
					sem_post(&sem_student[(prev->AM)-1]);
					/* bus driver waits till student tells him: "ok! I got in!"*/
					sem_wait(&sem_student_got_into_the_bus);	
				}
				else if(!strcmp(prev->school,"Chemistry") && sl_Chemistry)
				{
					/* student studies chemistry and there is an available seat for chemistry students */
					/* bus driver lets that student get into the bus */
					sl_Chemistry--;
					sem_post(&sem_student[(prev->AM)-1]);
					/* bus driver waits till student tells him: "ok! I got in!"*/
					sem_wait(&sem_student_got_into_the_bus);
				}
				else if(!strcmp(prev->school,"CSD") && sl_CSD)
				{
					/* student studies computer science and there is an available seat for csd students */
					/* bus driver lets that student get into the bus */
					sl_CSD--;
					sem_post(&sem_student[(prev->AM)-1]);
					/* bus driver waits till student tells him: "ok! I got in!"*/
					sem_wait(&sem_student_got_into_the_bus);
				}
				else
				{
					/* student cannot enter the bus */
					printf("Student %d (%s) cannot enter the Bus\n", prev->AM, prev->school);
				}
			}	
		}
		/* bus goes from stop A to stop B */
		printf("\n\nBus is on the way to University...!\n");
		sleep(10);	/* 10 secs are needed for the bus to move from stop A to stop B */
		printf("Bus arrived at University!\n");
		
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);
	
		if(Q_Bus || Q_Stop_B)
		{
			/* there is at least one student into the bus or waiting at stop B, so bus has to stop at stop B */
			/* first , bus is emptied . Students hop off the bus
 * with the order in which they had gotten into the bus (FIFO) and go from
 * stop B to
 * University */
			ptr = Q_Bus;
			while(ptr)
			{
				prev = ptr;
				ptr = ptr->next;

				if(!strcmp(prev->school, "Math"))
				{
					/* student that hops off the bus studies math */
					sl_Math++;
				}
				else if(!strcmp(prev->school, "Physics"))
				{
					/* student that hops off the bus studies physics */		
					sl_Physics++;									
				}
				else if(!strcmp(prev->school, "Chemistry"))
				{
					/* student that hops off the bus studies chemistry */
					sl_Chemistry++;
				}
				else
				{
					/* student that hops off the bus studies computer science */
					sl_CSD++;
				}
				/* bus driver signals student to hop off the bus */
				sem_post(&sem_student[(prev->AM)-1]);
				/* bus driver waits till student tells him: "ok, I got out!" */	
				sem_wait(&sem_student_got_out_of_the_bus);
			}

			/* secondly, students that have just gotten off to
 * stop B are sent to University */				
			/* checks one by one students at stop B and whoever
 * has not studied yet is sent to the University */
			ptr = Q_Stop_B;			
			while(ptr)
			{
				prev = ptr;
				ptr = ptr->next;

				if(!(prev->study_condition))
				{
					/* student has not studied yet, so tell him to go to University */
					sem_post(&sem_student[(prev->AM)-1]);
					/* waits till student says: "ok, I
					 * went to University!" (because we want bus to start boarding the students
 					 * already waiting for it at stop B only after it has been emptied) */
					sem_wait(&sem_student_went_to_university);
				}	
			}
	
			/* lastly, students waiting on stop B get into the bus */
			ptr = Q_Stop_B;
			while(ptr)
			{
				prev = ptr;
				ptr = ptr->next;

				if(prev->study_condition)
				{
					/* student has finished studying and is waiting at stop B, so let him get into the bus */
					sem_post(&sem_student[(prev->AM)-1]);
					/* driver waits until student tells him: "ok, I got inside!" */
					sem_wait(&sem_student_got_into_the_bus);
				}
			} 
			
			/* bus goes from stop B to stop A */
			printf("Bus is heading to Stop A\n");
			sleep(10);	/* it takes 10 seconds for it to move from stop B to stop A */
			printf("\n\nBus arrived to Stop A\n");
		
			/* bus is emptied */
			if(Q_Bus)
			{
				/* there is at least one student into the bus that wants to hop off */
				ptr = Q_Bus;
				while(ptr)
				{
					prev = ptr;
					ptr = ptr->next;

					/* tell student to get off the bus */
					sem_post(&sem_student[(prev->AM)-1]);
					/* wait till student says: "ok, I got off the bus!" */
					sem_wait(&sem_student_got_out_of_the_bus);
				}		
			}
		}
		
		/* checks if there are no students left waiting for the bus at a bus stop */
		if(!Q_Stop_A && !Q_Stop_B)
		{
			/* bus stops and gets destroyed */
			break;
		}		
	}
		

	return NULL;
}

void *student_procedure(void *s)
{
	/* student thread is created and waits at stop A till the bus comes (sem1) */
	
	/* the reason why I am using the following mutex is that while into
	 * main function's for loop there is a predefined order in which
	 * threads are actually created, the speed with which each one of them
	 * is going to run is undefined! As a result, a thread that was
	 * actually created later may run faster than a thread that was
	 * actually created earlier, like in screenshot 1, where student 5
	 * surpasses student 4 and arrives to stop A earlier. */
	pthread_mutex_lock(&lock);
		/* we keep the order in which students arrive on stop A by putting student's struct into a queue */
		Enqueue((student *)s, &Q_Stop_A);
		printf("\n\nStudent %d (%s) created.\n", ((student *)s)->AM, ((student *)s)->school);
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);	
	pthread_mutex_unlock(&lock);

	/* student tells bus that he has been created . The reason behind this
 	 * is that I don't want bus to start boarding students before all students
 	 * have been created (and gone to stop A). Firstly, all students must
 	 * be created and afterwards , bus can be allowed to start picking
 	 * them up */
	sem_post(&sem_student_created);

	/* student (thread) waits until bus lets him board to it */
	sem_wait(sem_student + (((student *)s)->AM) - 1);

	/* student leaves stop A and boards to the bus */
		printf("\n\nStudent %d (%s) boarded to the bus.\n", ((student *)s)->AM, ((student *)s)->school);
		/* student leaves stop A */
		Remove((student *)s, &Q_Stop_A);
		/* and gets into the bus */
		Enqueue((student *)s, &Q_Bus);
		((student *)s)->place = Bus;
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);
		/* student tells bus driver that he got in */
		sem_post(&sem_student_got_into_the_bus);		

	/* student (thread) stays into the bus until it arrives at stop B */
	sem_wait(sem_student + (((student *)s)->AM) - 1);	

	/* student gets off the bus and goes to stop B */
		printf("\n\nStudent %d (%s) got off the bus.\n", ((student *)s)->AM, ((student *)s)->school);
		/* student leaves bus */
		Remove((student *)s, &Q_Bus);
		/* and goes to stop B */
		Enqueue((student *)s, &Q_Stop_B);
		((student *)s)->place = Stop_B;
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);	
		/* student tells bus driver that he got out of the bus */
		sem_post(&sem_student_got_out_of_the_bus);

	/* student (thread) stays at stop B until all students have gotten off the bus and have arrived at stop B */	
	sem_wait(sem_student + (((student *)s)->AM) - 1);	

	/* student leaves stop B, goes to University, starts studying,
 	 * finishes studying and returns to stop B, where he waits for the bus to come
 	 * again */
	/* I am going to use a mutex again, because more than one student
 	 * threads may try to move from stop B to University at the same (almost) time */
	pthread_mutex_lock(&lock);
		printf("Student %d (%s) went to University.\n", ((student *)s)->AM, ((student *)s)->school);	
		/* student leaves stop B */
		Remove((student *)s, &Q_Stop_B);
		/* and goes to University */
		Enqueue((student *)s, &Q_University);
		((student *)s)->place = University;
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);
		/* student tells bus that he went to University */
		sem_post(&sem_student_went_to_university);
	pthread_mutex_unlock(&lock);
		
		/* student starts studying */
		sleep(((student *)s)->study_time);
		/* student stops studying after his study time goes by */
		((student *)s)->study_condition = 1;	/* shows that he has studied */
	
	pthread_mutex_lock(&lock);	
		/* student leaves University */	
		printf("\n\nStudent %d (%s) studied for %d seconds, and now is heading to stop B.\n", ((student *)s)->AM, ((student *)s)->school, ((student *)s)->study_time);		
		Remove((student *)s, &Q_University);
		/* and goes to stop B */
		Enqueue((student *)s, &Q_Stop_B);
		((student *)s)->place = Stop_B;
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);	
	pthread_mutex_unlock(&lock);

	/* student (thread) waits at stop B until bus comes to stop B again */
	sem_wait(sem_student + (((student *)s)->AM) - 1);	
	
	/* student leaves stop B and boards to the bus */
		printf("\n\nStudent %d (%s) boarded to the bus.\n", ((student *)s)->AM, ((student *)s)->school);
		/* student leaves stop B */
		Remove((student *)s, &Q_Stop_B);
		/* and gets into the bus */
		Enqueue((student *)s, &Q_Bus);
		((student *)s)->place = Bus;
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);
		/* student tells bus driver that he got in */
		sem_post(&sem_student_got_into_the_bus);		

	/* student (thread) stays into the bus until he gets home */
	sem_wait(sem_student + (((student *)s)->AM) - 1);	
	
	/* student (thread) gets off the bus and goes home */
		printf("\n\nStudent %d (%s) went home.\n", ((student *)s)->AM, ((student *)s)->school);
		/* student gets off the bus */
		Remove((student *)s, &Q_Bus);
		/* general print */
		general_print(Q_Stop_A, Q_Stop_B, Q_Bus, Q_University);
		/* tells bus driver that he got off the bus */
		sem_post(&sem_student_got_out_of_the_bus);
		/* and goes home */	

	return NULL;
}

void Enqueue(student *stdnt, student **Q)
{
	student *ptr;

	/* stdnt == NULL */
	if(!stdnt)
	{
		fprintf(stderr, "In function Enqueue(): No pointer to student struct parsed as a parameter!\nProcess terminated!\n\n");
		exit(-1);
	}

	/* Q == NULL */
	if(!Q)
	{
		fprintf(stderr, "In function Enqueue(): No pointer to pointer to queue parsed as a parameter!\nProcess terminated!\n\n");
		exit(-1);
	}

	/* (*Q)==NULL ---> This is going to be the first node of the queue (first student) */
	if(!(*Q))
	{
		*Q = stdnt;
		return;
	}
	/* this is not the first node into the list */
	ptr = *Q;
	while(ptr->next)
	{
		ptr = ptr->next;
	}
	/* stdnt is added in the end of the queue */
	ptr->next = stdnt;
	ptr->next->prev = ptr;

}


void Remove(student *stdnt, student **Q)
{

	if(!stdnt)
	{
		fprintf(stderr, "In function Dequeue(): No pointer to student struct parsed as a parameter!\nProcess terminated!\n\n");
		exit(-1);
	}

	if(!Q)
	{
		fprintf(stderr, "In function Dequeue(): No pointer to queue parsed as a parameter!\nProcess terminated!\n\n");
		exit(-1);
	}	

	/* (*Q)==NULL ---> empty queue */
	if(!(*Q))
	{
		printf("Queue is empty!\n");
		return;
	}

	if(stdnt == *Q)
	{
		/* student to be removed is the first one into queue */
		*Q = (*Q)->next;
	}
	else if(stdnt->next)
	{
		/* student to be removed is neither the first one, nor the last one into queue */
		stdnt->next->prev = stdnt->prev;
		stdnt->prev->next = stdnt->next;
	}
	else
	{
		/* student to be removed is the last one into the queue */
		stdnt->prev->next = NULL;
	}

	stdnt->prev = NULL;
	stdnt->next = NULL;
	/* free(stdnt);*/
}


void general_print(student *Q_Stop_A, student *Q_Stop_B, student *Q_Bus, student *Q_University)
{
	student *ptr;	

	printf("\n\nStop A: ");
	/* prints student info for stop A */
	ptr = Q_Stop_A;
	while(ptr)
	{
		printf("[%d, %s] ", ptr->AM, ptr->school);		
		ptr = ptr->next;	
	}

	printf("\nBus: ");
	/* prints student info for Bus */
	ptr = Q_Bus;
	while(ptr)
	{
		printf("[%d, %s] ", ptr->AM, ptr->school);
		ptr = ptr->next;
	}

	printf("\nUniversity: ");
	/* prints student info for University */
	ptr = Q_University;
	while(ptr)
	{
		printf("[%d, %s] ", ptr->AM, ptr->school);
		ptr = ptr->next;
	}

	printf("\nStop B: ");
	/* prints student info for Stop B */
	ptr = Q_Stop_B;
	while(ptr)
	{
		printf("[%d, %s] ", ptr->AM, ptr->school);
		ptr = ptr->next;
	}
	
	printf("\n\n\n");

}


void student_struct_create(student *s, int student_id)
{
	int school_name;

	s->AM = student_id;
	s->study_time = 5 + rand()%11; /* random number between 5 and 15 secs */
	school_name = rand()%4; /* random number between 0 and 3 (0 for Math, 1 for Physics, 2 for Chemistry and 3 for CSD) */ 
	s->place = Stop_A; /* student starts from Stop A */
	s->study_condition = 0; /* student has not studied yet */

	/* next and prev are initialised with NULL */
	s->next = NULL;
	s->prev = NULL;

	switch(school_name)
	{
		case 0:
			s->school = "Math";
			break;
		case 1:
			s->school = "Physics";
			break;
		case 2:
			s->school = "Chemistry";
			break;
		default:
			s->school = "CSD";
	}
}



int enter_stud_num()
{
	int num;

	printf("Enter the number of students:\n");
	scanf("%d", &num);
	while(!num)
	{
		printf("Give a number greater than zero:\n");
		scanf("%d", &num);
	}	

	return num;
}


