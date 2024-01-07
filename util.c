/**********************************************************************************************
NAME: MICHAEL HARRIS ANASTASAKIS
STUDENT#: 101047439
CLASS: COMP2401-A | WINTER 2023
ASSIGNMENT: A5 "SIMULATING THE HUNT" | LOCKS AND THREADS
LANGUAGE:   C 
PURPOSE:    This program is a simulation of a haunted building using multithreading implemented with
            multiple semaphore mutex locks. The threads are various Ghost Hunters and a single ghost all 
            moving throughout the building concurrently. The building has rooms, doors, 
            hunters, ghosts, and evidence of the ghost's presence. The simulation runs 
            a series of actions, such as hunters moving between rooms, finding and collecting 
            evidence, and attempts to banish the ghost based on the evidence they found. The 
            program keeps track of hunter's fear and boredom levels, which affect their behavior 
            and exit conditions. The simulation determines whether the hunters are successful 
            in banishing the ghost or if the ghost prevails causing the hunters to flee in fear. 
            Upon completion of the simulation the program provides a summary of the simulation 
            results, including the victor (hunters or ghost), hunter stats, and all evidence 
            collected by hunters throughout the simulation.
FILE:   util.c
FILE PURPOSE:   contains utility functions that contain routine code multiple times throughout program
                organized here to make functions more compact and readable
FUNCTIONS:  slowThread(), typeFromEvidence(), evidenceToString(), getOppositeDirection(), 
            ghostToString(), pBreak()
**********************************************************************************************/

#include "defs.h"

/* 
    Function: slowThread
    Purpose: Sleeps the thread for a random amount of time, as specified
             in the assignment specification. Used at the top of the thread function.   
    Returns: void
*/
void slowThread() {
    usleep(MIN_WAIT + rand() % MAX_WAIT);
}

/* 
    Function: typeFromEvidence
    Purpose: Returns the GhostCategoryType of a journal
    Parameters: 
        in:     JournalType* evidence: The journal containing all evidence collected
    Returns: GhostCategoryType - the type of ghost, 

*/
GhostCategoryType typeFromEvidence(JournalType* evidence) {

    GhostCategoryType type = 0;
    int count = 0;
    EvidenceNodeType* current = evidence->head;

    // using bitwise OR to combine the evidence types
    // And our GhostCategoryType is setup to align with the bits
    while (current != NULL) {
        type |= current->evidence->type;
        current = current->next;
        count++;
    }

    if (count != 3) return UNKNOWN_GHOST;
    
    return type;
}

/*
    Function: evidenceToString
    Purpose: Converts an EvidenceCategoryType to a string
    Parameters:
        in:     EvidenceCategoryType evidence: The evidence to convert
        out:    char* buffer: The buffer to write the string to
    Returns: void
*/
void evidenceToString(EvidenceCategoryType evidence, char* buffer) {
    switch (evidence) {
        case EMF:
            strcpy(buffer, "EMF");
            break;
        case FINGERPRINTS:
            strcpy(buffer, "FINGERPRINTS");
            break;
        case TEMPERATURE:
            strcpy(buffer, "TEMPERATURE");
            break;
        case SOUND:
            strcpy(buffer, "SOUND");
            break;
        default:
            strcpy(buffer, "UNKNOWN");
            break;
    }
}

/*
    * Function: getOppositeDirection
    * Returns the opposite direction of the given direction from the Direction enum
    * Parameters:
    *   Input:  DirectionType direction - the direction to get the opposite of
    *   Output: none
    * Returns: DirectionType - the opposite direction, or DIRECTION_COUNT if the direction is invalid
*/
DirectionType getOppositeDirection(DirectionType direction) {
    switch (direction) {
        case NORTH:
            return SOUTH;
        case SOUTH:
            return NORTH;
        case EAST:
            return WEST;
        case WEST:
            return EAST;
        default:
            return DIRECTION_COUNT;
    }
}

/* * Function: ghostToString
    * Converts a ghost to a string
    * Parameters:
    *   Input:  GhostEnumType ghost - the ghost to convert
    *   Output: char* str - the string to convert to
    * Returns: void
*/
void ghostToString(GhostCategoryType ghost, char *str)
{ 
  switch (ghost) {
    case POLTERGEIST:
        strcpy(str, "POLTERGEIST GHOST");
        break;
    case BANSHEE:
        strcpy(str, "BANSHEE GHOST");
        break;
    case PHANTOM:
        strcpy(str, "PHANTOM GHOST");
        break;
    case BULLIES:
        strcpy(str, "BULLIES");
        break;
    default:
        strcpy(str, "AN UNKNOWN ENTITY");
        break;
  }
}

/***************************************************************************************************
 * Helper Function: pBreak()
 *         Purpose: prints a long line break for formatting and readability purposes
 * 	        in:     * type is the code for what type of consistent formated print break to print to terminal 
 *                  that is desiredL: long, short or empty 
 *          return: void
****************************************************************************************************/
void pBreak(int type){
    switch(type){
        case PRINT_LONG:
            printf(" ---------------------------------------------------\n");
            break;
        case PRINT_SHORT:
            printf(" ----------------------------------\n");
            break;
        case PRINT_EMPTY:
            printf("|\t\t\t\t\t\t    |\n");
            break;
    }
}
