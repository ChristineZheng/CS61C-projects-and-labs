/* 
 * CS61C Summer 2016
 * Name:
 * Login:
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flights.h"
#include "timeHM.h"


typedef struct flightSys {

    struct airport * front;
    
} flightSys;


typedef struct airport {
    
    char* name;
    struct entry * schedule;
    struct airport * next;
    
} airport;


typedef struct entry {
    
    struct airport * destination;
    timeHM_t * departure;
    timeHM_t * arrival;
    int * cost;
    struct entry * next;
    
} entry_t;



/*
   This should be called if memory allocation failed.
 */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}


/*
   Creates and initializes a flight system, which stores the flight schedules of several airports.
   Returns a pointer to the system created.
 */
flightSys_t* createSystem() {
    // Replace this line with your code
    
    flightSys_t* system = (flightSys_t*) calloc(1, sizeof(flightSys_t));    // initialize flight system
    if (system == NULL) {
        allocation_failed();
    }
    
    system->front = (airport_t*) calloc(1, sizeof(airport_t));   // initialize airport
    if (system->front == NULL) {
        allocation_failed();
    }
    system->front->name = NULL;
    system->front->schedule = calloc(1, sizeof(entry_t));
    if (system->front->schedule == NULL) {
        allocation_failed();
    }
    
    system->front->schedule->next = NULL;
    system->front->next = NULL;

    return system;
}


/*
   Frees all memory associated with this system; that's all memory you dynamically allocated in your code.
 */
void deleteEntries(airport_t* ap) {
    entry_t* ent = ap->schedule;
    while(ent->next) {
        entry_t* to_delete = ent;
        free(to_delete->destination);
        free(to_delete->departure);
        free(to_delete->arrival);
        free(to_delete->cost);
        ent = ent->next;
        free(to_delete);
    }
    if (ent) {
        free(ent->destination);
        free(ent->departure);
        free(ent->arrival);
        free(ent->cost);
        free(ent);
    }
}



void deleteSystem(flightSys_t* s) {
    // Replace this line with your code
    
    //flightSys_t* system = s;
    if (s == NULL) {
        return;
    }
    airport_t* curr = s->front;
    while(curr) {
	free(curr->name);
        deleteEntries(curr);
   	airport_t* temp = curr;
        curr = curr->next;
	free(temp);
    }
    free(s);
}


/*
   Adds a airport with the given name to the system. You must copy the string and store it.
   Do not store "name" (the pointer) as the contents it point to may change.
 */
void addAirport(flightSys_t* s, char* n) {
    // Replace this line with your code
    
    
    // initialize a airport
    airport_t* ptr = s->front;

    while(ptr->next) {
        ptr = ptr->next;
    }
    ptr->next = (airport_t*) calloc(1, sizeof(airport_t));
    if ((ptr->next) == NULL) {
        allocation_failed();
    }

    ptr->next->schedule = (entry_t*) calloc(1, sizeof(entry_t));
    if (ptr->next->schedule == NULL) {
        allocation_failed();
    }
    ptr->next->next = NULL;

    
    // copy airport name
    int lengthOfString = strlen(n);
    ptr->name = (char*) calloc(1, sizeof(char) * (1+lengthOfString));
    if ((ptr->name) == NULL) {
        allocation_failed();
    }
    strcpy(ptr->name, n);
    
}


/*
   Returns a pointer to the airport with the given name.
   If the airport doesn't exist, return NULL.
 */
airport_t* getAirport(flightSys_t* s, char* n) {
    // Replace this line with your code
    
    // loop over the airport list and check the one has same airport name
    airport_t* ptr = s->front;
    while (ptr->next != NULL) {
        if (strcmp((ptr->name), n) == 0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    
    return NULL;
}


/*
   Print each airport name in the order they were added through addAirport, one on each line.
   Make sure to end with a new line. You should compare your output with the correct output
   in flights.out to make sure your formatting is correct.
 */
void printAirports(flightSys_t* s) {
    // Replace this line with your code
    
    airport_t* ptr = s->front;

    while (ptr->next != NULL) {
        printf("%s\n", ptr->name);
        ptr = ptr->next;
    }
}


/*
   Adds a flight to src's schedule, stating a flight will leave to dst at departure time and arrive at arrival time.
 */
void addFlight(airport_t* src, airport_t* dst, timeHM_t* dep, timeHM_t* arri, int cst) {
    // Replace this line with your code


    // look for the end of flight list
    entry_t* ptr = src->schedule;
    while (ptr->next) {
        ptr = ptr->next;
    }
    ptr->next = (entry_t*) calloc(1, sizeof(entry_t));
    if ((ptr->next) == NULL) {
        allocation_failed();
    }
    ptr->next->next = NULL;
    
    
    // create space for destination and store destination
    ptr->destination = (airport_t*) calloc(1, sizeof(airport_t));
    if ((ptr->destination) == NULL) {
        allocation_failed();
    }
    *(ptr->destination) = *dst;
    
    // create space for departure and store departure
    ptr->departure = (timeHM_t*) calloc(1, sizeof(timeHM_t));
    if ((ptr->departure) == NULL) {
        allocation_failed();
    }
    *(ptr->departure) = *dep;
    
    // create space for arrival and store arrival
    ptr->arrival = (timeHM_t*) calloc(1, sizeof(timeHM_t));
    if ((ptr->arrival) == NULL) {
        allocation_failed();
    }
    *(ptr->arrival) = *arri;
    
    // create space for cost and store cost
    ptr->cost = (int*) calloc(1, sizeof(int));
    if ((ptr->cost) == NULL) {
        allocation_failed();
    }
    *(ptr->cost) = cst;
    
//    printf("A flight will leave to %s at %d:%d, and arrive at %d:%d\n", dst->name, dep->hours, dep->minutes, arri->hours, arri->minutes);
    
}


/*
   Prints the schedule of flights of the given airport.

   Prints the airport name on the first line, then prints a schedule entry on each 
   line that follows, with the format: "destination_name departure_time arrival_time $cost_of_flight".

   You should use printTime (look in timeHM.h) to print times, and the order should be the same as 
   the order they were added in through addFlight. Make sure to end with a new line.
   You should compare your output with the correct output in flights.out to make sure your formatting is correct.
 */
void printSchedule(airport_t* s) {
    // Replace this line with your code
    
    printf("AIRPORT: %s\n", s->name);
    entry_t* ptr = s->schedule;
    while (ptr->next != NULL) {
        printf("%s ", ptr->destination->name);
        printTime(ptr->departure);
        printf("%s", " ");
        printTime(ptr->arrival);
        printf(" $%d\n", *(ptr->cost));
        ptr = ptr->next;
    }
    
}


/*
   Given a src and dst airport, and the time now, finds the next flight to take based on the following rules:
   1) Finds the cheapest flight from src to dst that departs after now.
   2) If there are multiple cheapest flights, take the one that arrives the earliest.

   If a flight is found, you should store the flight's departure time, arrival time, and cost in departure, arrival, 
   and cost params and return true. Otherwise, return false. 

   Please use the function isAfter() from time.h when comparing two timeHM_t objects.
 */
bool getNextFlight(airport_t* src, airport_t* dst, timeHM_t* now, timeHM_t* d, timeHM_t* a, int* c) {
    // Replace this line with your code
    
    
    entry_t* ptr = src->schedule;
    
    // create a flight variable as target
    entry_t* target = NULL;
    bool findSameDestination = false;
    
    // loop over all the flights
    while (ptr->next != NULL) {

        // check destination airport and departure time
        if ((strcmp(ptr->destination->name, dst->name) == 0) && isAfter(ptr->departure, now)) {
            
            if (findSameDestination == false) {
                findSameDestination = true;
                target = ptr;
            }
            
            // cheaper cost
            if (*(ptr->cost) < *(target->cost)) {
                target = ptr;
            }
            
            // same cost, arrive earlier
            else if (*(ptr->cost) == *(target->cost)) {
                if (!isAfter(ptr->arrival, target->arrival)) {
                    target = ptr;
                }
            }
            else {
                
            }
        }
        ptr = ptr->next;
                    
    }
    
    // store flight info
    if (findSameDestination) {

        *d = *(target->departure);
        *a = *(target->arrival);
        *c = *(target->cost);
        
        return true;
    }
    else {
        return false;
    }
}

