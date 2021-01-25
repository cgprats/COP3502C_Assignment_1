#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sp21_cop3502_as1.h"
#include "leak_detector_c.h"
// Function Constructors
void remove_crlf(char *s); //Remove Carriage Return.
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length); //Get the Next Nonblank Line from Buffer
monster *new_monster(int id, char *name, char *element, int population); //Constructor for Monsters
region *new_region(char *name, int nmonsters, int total_population, monster **monsters); //Constructor for Regions
itinerary *new_itinerary(int nregions, region **regions, int captures); //Constructor for Itineraries
trainer *new_trainer(char *name, itinerary *visits); //Constructor for Trainers
void dispose_monster(monster *delMonster); //Destructor for Monsters
void dispose_region(region *delRegion); //Destructor for Regions
void dispose_itinerary(itinerary *delItinerary); //Destructor for Itineraries
void dispose_trainer(trainer *delTrainer); //Destructor for Trainers

// The Main Function
int main() {
	//Report Memory Leaks When the Program Exits
	atexit(report_mem_leak);
	return 0;
}

/* 
 * Remove Carriage Return. This function is from the lectures by the professor,
 * Matthew Gerber. We are permittied to use this code in our assignments.
 * Its Purpose is to Remove Carriage Returns and Line Feeds from the End of a String
 */
void remove_crlf(char *s)
{
    /* Remember that in C, a string is an array of characters ending with a '\0' character.  We
       are given s, a pointer to this array.  We want to find the end of the string that s points
       to.  We know that strlen() will give us the *length* of s, so we add s's length to its
       location to find the end of s. */

    char *t = s + strlen(s);

    /* t is now at s's null terminator. */

    t--;

    /* t now points to the last character of s.  (Importantly, we haven't changed s.) */

    /* \n is the UNIX-like newline character.  MS-DOS and Windows occasionally add \r, which is a
       separate "carriage return" character.  We need to find and clobber both. */

    /* We have two cases where t can end up to the left of s: if we're passed a zero-length string,
       or if literally the entire string is made up of \n and/or \r.  Stop everything if t ends up
       to the left of s to prevent bad things from happening in that case. */

    /* Look at the contents of t as a single character.  As long as it's still a newline or a CR... */

    while((t >= s) && (*t == '\n' || *t == '\r')) {
        *t = '\0';  // ...clobber it by overwriting it with null, and...
        t--;        // decrement t *itself (i.e., change the pointed location)

        /* t now still points to the last character of s, and we keep going until it's something
           other than a CR or newline. */
    }
}

/* This function will write the next line from ifp that isn't blank into buf, removing any
 * newline characters.  It writes a zero length string at eof, and only then.  It returns
 * true if it successfully read a line and false if it didn't. This function is from the
 * lectures by the professor, Matthew Gerber.
 */
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length)
{
    buf[0] = '\0';

    while(!feof(ifp) && (buf[0] == '\0'))
    {
        fgets(buf, max_length, ifp);
        remove_crlf(buf);
    }

    if(buf[0] != '\0') {
        return 1;
    } else {
        return 0;
    }
}

// This Function will Create and Return a New Montster from Specified Parametrs.
monster *new_monster(int id, char *name, char *element, int population) {
	monster *newMonster = malloc(sizeof(monster));
	newMonster->id = id;
	newMonster->name = name;
	newMonster->element = element;
	newMonster->population = population;
	return newMonster;
}

// This Function will Create and Return a New Region from Specified Parameters.
region *new_region(char *name, int nmonsters, int total_population, monster **monsters) {
	region *newRegion = malloc(sizeof(region));
	newRegion->name = name;
	newRegion->nmonsters = nmonsters;
	newRegion->total_population = total_population;
	newRegion->monsters = monsters;
	return newRegion;
}

// This Function will Create and Return a New Itinerary from Specified Parameters.
itinerary *new_itinerary(int nregions, region **regions, int captures) {
	itinerary *newItinerary = malloc(sizeof(itinerary));
	newItinerary->nregions = nregions;
	newItinerary->regions = regions;
	newItinerary->captures = captures;
	return newItinerary;
}

// This Function will Create and Return a New Trainer from Specified Parameters.
trainer *new_trainer(char *name, itinerary *visits) {
	trainer *newTrainer = malloc(sizeof(trainer));
	newTrainer->name = name;
	newTrainer->visits = visits;
	return newTrainer;
}

// This Function will Destroy a Monster that was Constructed.
void dispose_monster(monster *delMonster) {
	free(delMonster->name);
	free(delMonster->element);
	free(delMonster);
}

// This Function will Destroy a Region that was Constructed.
// TODO: Deal with **monsters double pointer
void dispose_region(region *delRegion) {
	free(delRegion->name);
	free(delRegion);
}

// This Function will Destroy a Itinerary that was Constructed.
// TODO: Deal with **regions double pointer
void dispose_itinerary(itinerary *delItinerary) {
	free(delItinerary);
}

// This Function will Destroy a Trainer that was Constructed.
void dispose_trainer(trainer *delTrainer) {
	free(delTrainer->name);
	dispose_itinerary(delTrainer->visits);
	free(delTrainer);
}
