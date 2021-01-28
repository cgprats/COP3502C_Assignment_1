#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sp21_cop3502_as1.h"
#include "leak_detector_c.h"

// Function Prototypes
void remove_crlf(char *s); //Remove Carriage Return.
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length); //Get the Next Nonblank Line from Buffer
int find_region_total_population(region *currentRegion); //Calculate the Total Population of a Region
int calculate_captures(int monsterPopulation, int regionPopulation, int intendedCaptures); //Calculate the Amount of Captures of a Monster in a Region
void calculate_captures_output(FILE *ofp, trainer **trainersArray, int numTrainers); //Calculate the Output for the Trainer's Captures

// Constructor Prototypes
monster *new_monster(int id, char *name, char *element, int population); //Constructor for Monsters
monster **new_monsters_array(FILE *ifp, int *numMonsters); //Constructor for an Array of Monsters Created from a File
//region *new_region(char *name, int nmonsters, monster **monsters); //Constructor for Regions
region *new_region_from_file(FILE *ifp, char *name, int nmonsters, monster **monsters, int numMonsters); //Constructor for Regions with Information from File
region *new_region_for_itinerary(int index, region **regionsArray); //Constructor for Regions for Use in Itinerary Constructor
region **new_regions_array(FILE *ifp, int *numRegions, monster **monsterArray, int numMonsters); //Constructor for an Array of Regions Created from a File
//itinerary *new_itinerary(int nregions, region **regions, int captures); //Constructor for Itineraries
itinerary *new_itinerary_from_file(FILE *ifp, int nregions, int captures, region **regionsArray, int numRegions); //Constructor for Itineraries with Information from File
//trainer *new_trainer(char *name, itinerary *visits); //Constructor for Trainers
trainer *new_trainer_from_file(FILE *ifp, char *name, int nregions,int captures, region **regionsArray, int numRegions); //Constructor for Trainers with Information from File
trainer **new_trainers_array(FILE *ifp, int *numTrainers, region **regionsArray, int numRegions); //Constructor for an Array of Trainers Created from a File

// Destructor Prototypes
void dispose_monster(monster *delMonster); //Destructor for Monsters
void dispose_monsters_array(monster **delMonstersArray, int numMonsters); //Destructor for Monsters Array
void dispose_region(region *delRegion); //Destructor for Regions
void dispose_regions_array(region **delRegionsArray, int numRegions);
void dispose_itinerary(itinerary *delItinerary); //Destructor for Itineraries
void dispose_trainer(trainer *delTrainer); //Destructor for Trainers
void dispose_trainers_array(trainer **delTrainersArray, int numTrainers);

// The Main Function
int main() {
	//Report Memory Leaks When the Program Exits
	atexit(report_mem_leak);

	//Set the Input File to ifp and the Output File to ofp. If There is an Error, Print It.
	FILE *ifp = fopen("cop3502-as1-input.txt", "r");
	FILE *ofp = fopen("cop3502-as1-output-prats-christopher.txt", "w");

	//Create the Array of Monsters
	int numMonsters;
	monster **monsterArray = new_monsters_array(ifp, &numMonsters);

	//Create the Array of Regions
	int numRegions;
	region **regionsArray = new_regions_array(ifp, &numRegions, monsterArray, numMonsters);

	//Create the Array of Trainers
	int numTrainers;
	trainer **trainersArray = new_trainers_array(ifp, &numTrainers, regionsArray, numRegions);

	//Calculate the Captures for Each Trainer
	calculate_captures_output(ofp, trainersArray, numTrainers);

	//Dispose of Allocated Variables
	dispose_trainers_array(trainersArray, numTrainers);
	dispose_regions_array(regionsArray, numRegions);
	dispose_monsters_array(monsterArray, numMonsters);

	// Close the Input and Output Files Prior to Exit
	fclose(ifp);
	fclose(ofp);

	return 0;
}

/*
 * Remove Carriage Return. This function is from the lectures by the professor,
 * Matthew Gerber. We are permittied to use this code in our assignments.
 * Its Purpose is to Remove Carriage Returns and Line Feeds from the End of a String
 */
void remove_crlf(char *s) {
	/*
	 * Remember that in C, a string is an array of characters ending with a '\0' character.  We
	 * are given s, a pointer to this array.  We want to find the end of the string that s points
	 * to.  We know that strlen() will give us the *length* of s, so we add s's length to its
	 * location to find the end of s.
	 */

	char *t = s + strlen(s);

	//t is now at s's null terminator.

	t--;

	//t now points to the last character of s.  (Importantly, we haven't changed s.)

	/*
	 * \n is the UNIX-like newline character.  MS-DOS and Windows occasionally add \r, which is a
	 * separate "carriage return" character.  We need to find and clobber both.
	 */

	/*
	 * We have two cases where t can end up to the left of s: if we're passed a zero-length string,
	 * or if literally the entire string is made up of \n and/or \r.  Stop everything if t ends up
	 * to the left of s to prevent bad things from happening in that case.
	 */

	//Look at the contents of t as a single character.  As long as it's still a newline or a CR...

	while((t >= s) && (*t == '\n' || *t == '\r')) {
		*t = '\0';  // ...clobber it by overwriting it with null, and...
		t--;        // decrement t *itself (i.e., change the pointed location)

		/*
		 * t now still points to the last character of s, and we keep going until it's something
		 * other than a CR or newline.
		 */
	}
}

/*
 * This function will write the next line from ifp that isn't blank into buf, removing any
 * newline characters.  It writes a zero length string at eof, and only then.  It returns
 * true if it successfully read a line and false if it didn't. This function is from the
 * lectures by the professor, Matthew Gerber.
 */
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length) {
	buf[0] = '\0';

	while(!feof(ifp) && (buf[0] == '\0')) {
		fgets(buf, max_length, ifp);
		remove_crlf(buf);
	}

	if(buf[0] != '\0') {
		return 1;
	}
	else {
		return 0;
	}
}

// This Function will Calculate the Total Population of a Region
int find_region_total_population(region *currentRegion) {
	int total_population = 0;

	//Traverse Through the List of Monsters
	for (int i = 0; i < currentRegion->nmonsters; i++) {
		//Add the Commonality of the Monster to the Total Population
		total_population = total_population + currentRegion->monsters[i]->population;
	}
	//Return the Total Population
	return total_population;
}

// This Function Calculates the Amount of Captures of a Monster in a Region and Returns the Value
int calculate_captures(int monsterPopulation, int regionPopulation, int intendedCaptures) {
	double temp;
	int numCaptures = 0;

	//Perform the Calculation Algorithm
	temp = (double) monsterPopulation / (double) regionPopulation;
	temp *= intendedCaptures;
	numCaptures = round(temp);

	//Return the Number of Captures
	return numCaptures;
}

// This Function will Calculate the Output for Each Trainer
void calculate_captures_output(FILE *ofp, trainer **trainersArray, int numTrainers) {
	int tempCaptures;
	//Traverse Through the Trainers
	for (int i = 0; i < numTrainers; i++) {
		//Output the Current Trainer's Name
		fprintf(ofp, "%s\n", trainersArray[i]->name);

		//Traverse Through the Regions
		for (int j = 0; j < trainersArray[i]->visits->nregions; j++) {
			//Output the Current Region's Name
			fprintf(ofp, "%s\n", trainersArray[i]->visits->regions[j]->name);

			//Traverse Through the Monsters
			for (int k = 0; k < trainersArray[i]->visits->regions[j]->nmonsters; k++) {
				// Determine the Number of Captures
				tempCaptures = calculate_captures(trainersArray[i]->visits->regions[j]->monsters[k]->population, trainersArray[i]->visits->regions[j]->total_population, trainersArray[i]->visits->captures);

				//Output the Current Monster's Name and Number of Captures if it is NOT 0
				if (tempCaptures != 0) {
					fprintf(ofp, "%d %s\n", tempCaptures, trainersArray[i]->visits->regions[j]->monsters[k]->name);
				}
			}
		}

		//Output an Empty Line to Separate Trainers
		fprintf(ofp, "\n");
	}
}

/*
 * This section includes the functions for Constructing the various
 * objects used in this program.
 */

// This Function will Create and Return a New Montster from Specified Parametrs.
monster *new_monster(int id, char *name, char *element, int population) {
	monster *newMonster = malloc(sizeof(monster));
	newMonster->id = id;
	newMonster->name = strdup(name);
	newMonster->element = strdup(element);
	newMonster->population = population;
	return newMonster;
}

// This Function will Create and Return an Array of Monsters from an Input File
monster **new_monsters_array(FILE *ifp, int *numMonsters) {
	//Get the Number of Monsters
	char buf[256];
	get_next_nonblank_line(ifp, buf, 255);
	remove_crlf(buf);
	*numMonsters = atoi(buf);

	//Allocate the Array of Monsters with the Appropriate Size
	monster **newMonsterArray = calloc(*numMonsters, sizeof(monster *));

	//Initialize the Array of Monsters
	int tempID, tempPopulation;
	char tempName[256], tempElement[256];
	for (int i = 0; i < *numMonsters; i++) {
		//Read From Next Line into Buffer
		get_next_nonblank_line(ifp, buf, 255);
		remove_crlf(buf);

		//Get and Set the Temporary Values
		sscanf(buf, "%s %s %d", tempName, tempElement, &tempPopulation);
		tempID = i + 1;

		//Create the Monster and Add it to the Array
		monster *tempMonster = new_monster(tempID, tempName, tempElement, tempPopulation);
		newMonsterArray[i] = tempMonster;
	}

	//Return the Newly Created Array
	return newMonsterArray;
}

// This Function will Create and Return a New Region from Specified Parameters.
/*
region *new_region(char *name, int nmonsters, monster **monsters) {
	region *newRegion = malloc(sizeof(region));
	newRegion->name = strdup(name);
	newRegion->nmonsters = nmonsters;
	newRegion->monsters = monsters;
	newRegion->total_population = find_region_total_population(newRegion);
	return newRegion;
}*/

// This Function will Create and Return a New Region from a File and Specified Parameters.
region *new_region_from_file(FILE *ifp, char *name, int nmonsters, monster **monsters, int numMonsters) {
	region *newRegion = malloc(sizeof(region));
	newRegion->name = strdup(name);
	newRegion->nmonsters = nmonsters;
	newRegion->monsters = calloc(newRegion->nmonsters, sizeof(monster *));

	//Match the Data from the File to Create the Region's Monster Array
	char buf[256];
	for (int i = 0; i < newRegion->nmonsters; i++) {
		//Read the Next Line into the Buffer
		get_next_nonblank_line(ifp, buf, 255);
		remove_crlf(buf);

		//Find the Matching Monster from the Monsters Array
		for (int j = 0; j < numMonsters; j++) {
			if (!strcmp(buf, monsters[j]->name)) {
				newRegion->monsters[i] = new_monster(monsters[j]->id, monsters[j]->name, monsters[j]->element, monsters[j]->population);
			}
		}
	}

	//Calculate the Total Population of the Region
	newRegion->total_population = find_region_total_population(newRegion);

	// Return the Newly Created Region
	return newRegion;
}

// This Function will Create and Return a Region for Use in an Itinerary Constructor
region *new_region_for_itinerary(int index, region **regionsArray) {
	region *newRegion = malloc(sizeof(region));
	newRegion->name = strdup(regionsArray[index]->name);
	newRegion->nmonsters = regionsArray[index]->nmonsters;
	newRegion->total_population = regionsArray[index]->total_population;
	newRegion->monsters = calloc(newRegion->nmonsters, sizeof(monster *));

	//Match the Data from the Regions Array to Create the Local Region's Array
	for (int i = 0; i < newRegion->nmonsters; i++) {
		if (regionsArray[index]->monsters[i] != NULL) {
			newRegion->monsters[i] = new_monster(regionsArray[index]->monsters[i]->id, regionsArray[index]->monsters[i]->name, regionsArray[index]->monsters[i]->element, regionsArray[index]->monsters[i]->population);
		}
	}

	return newRegion;
}

// This Function will Create and Return an Array of Regions from an Input File
region **new_regions_array(FILE *ifp, int *numRegions, monster **monsterArray, int numMonsters) {
	//Get the Number of Regions
	char buf[256];
	get_next_nonblank_line(ifp, buf, 255);
	remove_crlf(buf);
	*numRegions = atoi(buf);

	//Allocate the Array of Regions with the Appropriate Size
	region **newRegionsArray = calloc(*numRegions, sizeof(region *));

	//Initialize the Array of Regions
	char tempName[256];
	int tempNMonsters;
	for (int i = 0; i < *numRegions; i++) {
		//Read Region Name from File
		get_next_nonblank_line(ifp, tempName, 255);
		remove_crlf(tempName);

		//Read Monsters Count from File
		get_next_nonblank_line(ifp, buf, 255);
		remove_crlf(buf);
		tempNMonsters = atoi(buf);

		//Create Region
		newRegionsArray[i] = new_region_from_file(ifp, tempName, tempNMonsters, monsterArray, numMonsters);
	}

	//Return the Newly Created Array
	return newRegionsArray;
}

// This Function will Create and Return a New Itinerary from Specified Parameters.
/*
itinerary *new_itinerary(int nregions, region **regions, int captures) {
	itinerary *newItinerary = malloc(sizeof(itinerary));
	newItinerary->nregions = nregions;
	newItinerary->regions = regions;
	newItinerary->captures = captures;
	return newItinerary;
}*/

// This Function will Create and Return a New Itinerary from a File and Specified Parameters
itinerary *new_itinerary_from_file(FILE *ifp, int nregions, int captures, region **regionsArray, int numRegions) {
	itinerary *newItinerary = malloc(sizeof(itinerary));
	newItinerary->nregions = nregions;
	newItinerary->captures = captures;
	newItinerary->regions = calloc(nregions, sizeof(region *));

	//Create the Itinerary's Region Array
	char tempName[256];
	for (int i = 0; i < newItinerary->nregions; i++) {
		//Read Region Name from File
		get_next_nonblank_line(ifp, tempName, 255);
		remove_crlf(tempName);

		for (int j = 0; j < numRegions; j++) {
			if (!strcmp(tempName, regionsArray[j]->name)) {
				newItinerary->regions[i] = new_region_for_itinerary(j, regionsArray);
			}
		}
	}

	//Return the Newly Created Itinerary
	return newItinerary;
}

// This Function will Create and Return a New Trainer from Specified Parameters.
/*
trainer *new_trainer(char *name, itinerary *visits) {
	trainer *newTrainer = malloc(sizeof(trainer));
	newTrainer->name = strdup(name);
	newTrainer->visits = visits;
	return newTrainer;
}*/

// This Function will Create and Return a New Trainer from a File and Specified Parameters.
trainer *new_trainer_from_file(FILE *ifp, char *name, int nregions, int captures, region **regionsArray, int numRegions) {
	trainer *newTrainer = malloc(sizeof(trainer));
	newTrainer->name = strdup(name);
	newTrainer->visits = new_itinerary_from_file(ifp, nregions, captures, regionsArray, numRegions);

	// Return the Newly Created Trainer
	return newTrainer;
}

// This Function will Create and Return an Array of Regions from an Input File
trainer **new_trainers_array(FILE *ifp, int *numTrainers, region **regionsArray, int numRegions) {
	//Get the Number of Trainers
	char buf[256];
	get_next_nonblank_line(ifp, buf, 255);
	remove_crlf(buf);
	*numTrainers = atoi(buf);

	//Allocate the Number of Trainers with the Appropriate Size
	trainer **newTrainersArray = calloc(*numTrainers, sizeof(trainer *));

	//Initialize the Array of Trainers
	char tempName[256];
	int tempNumCaptures, tempNumRegions;
	for (int i =0; i < *numTrainers; i++) {
		//Read Trainer Name from File
		get_next_nonblank_line(ifp, tempName, 255);
		remove_crlf(tempName);

		//Read Capture Count from File
		get_next_nonblank_line(ifp, buf, 255);
		remove_crlf(buf);
		tempNumCaptures = atoi(buf);

		//Read Region Count from File
		get_next_nonblank_line(ifp, buf, 255);
		remove_crlf(buf);
		tempNumRegions = atoi(buf);

		//Create Trainer
		newTrainersArray[i] = new_trainer_from_file(ifp, tempName, tempNumRegions, tempNumCaptures, regionsArray, numRegions);
	}

	//Return the Newly Created Array
	return newTrainersArray;
}

/*
 * This section includes the functions for Destroying the various
 * objects used in this program.
 */

// This Function will Destroy a Monster that was Constructed.
void dispose_monster(monster *delMonster) {
	free(delMonster);
}

// This Function will Destroy an Array of Monsters that was Constructed
void dispose_monsters_array(monster **delMonstersArray, int numMonsters) {
	for (int i = 0; i < numMonsters; i++) {
		if(delMonstersArray[i] != NULL) {
			dispose_monster(delMonstersArray[i]);
		}
	}
	free(delMonstersArray);
}

// This Function will Destroy a Region that was Constructed.
void dispose_region(region *delRegion) {
	dispose_monsters_array(delRegion->monsters, delRegion->nmonsters);
	free(delRegion);
}

// This Function will Destroy an Array of Regions that was Constructed
void dispose_regions_array(region **delRegionsArray, int numRegions) {
	for (int i = 0; i < numRegions; i++) {
		if (delRegionsArray[i] != NULL) {
			dispose_region(delRegionsArray[i]);
		}
	}
	free(delRegionsArray);
}

// This Function will Destroy a Itinerary that was Constructed.
void dispose_itinerary(itinerary *delItinerary) {
	dispose_regions_array(delItinerary->regions, delItinerary->nregions);
	free(delItinerary);
}

// This Function will Destroy a Trainer that was Constructed.
void dispose_trainer(trainer *delTrainer) {
	dispose_itinerary(delTrainer->visits);
	free(delTrainer);
}

// This Function will Destroy an Array of Traienrs that was Constructed
void dispose_trainers_array(trainer **delTrainersArray, int numTrainers) {
	for (int i = 0; i < numTrainers; i++) {
		if (delTrainersArray[i] != NULL) {
			dispose_trainer(delTrainersArray[i]);
		}
	}
	free(delTrainersArray);
}
