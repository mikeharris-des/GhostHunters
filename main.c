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
FILE:   main.c
FUNCTIONS:  main(), initBuilding(), initRoom(), initDoor(), initHunter(), initEvidence(), initJournal()
            connectRooms(), initAllThreads(), threads(), hunterThreadFunc(), hunterExit(), hunterFieldMod()
            hunterEvidence(), hunterMove(), printResults(), freeBuilding()
**********************************************************************************************/

#include "defs.h"

/*****************************************************************************************************     
  Function:   main()
  This program simulates a haunted building with hunters trying to find and banish a ghost.
  The main function initializes and populates the building, creates the ghost and places it
  in a random room, initializes the hunters and puts them in the same room 'van', runs the simulation 
  using threads, and prints the simulation live results and post conclusion summary. It frees up memory 
  resources allocated during the simulation.
  
  Algorithm:
  1. Seed random number generator with the current time.
  2. Initialize and populate the building with rooms, doors, and hunters.
  3. Initialize the building's journal to track evidence collected.
  4. Find a random room for the ghost to start in and display the room name.
  5. Initialize the ghost and place it in the selected room.
  6. Initialize and start threads for the simulation.
  7. Run the simulation and obtain the result (success or failure in banishing the ghost corresponding 
     to exit condition of all threads).
  8. Print the results of the simulation, including winner, hunter stats, and journal entries.
  9. Free up memory resources used in the simulation.

  returns: boolean C_TRUE for debugging purposes
 ****************************************************************************************************/

int main() { 
    srand(time(NULL));
    BuildingType* building;
    RoomType* ranRoom = NULL; 

    initBuilding(&building);
    populateBuilding(building);
    initJournal(&(building->journal));
    while(ranRoom==NULL){ranRoom = building->rooms[rand() % building->roomCount];}
    printf("GHOST STARTS IN [%s]\n",ranRoom->name);
    initGhost(&(building->ghost), ranRoom); 
    initAllThreads(building,MAX_HUNTERS);

    int result = threads(building);
    printResults(building,result);
    freeBuilding(&building);
    return C_TRUE;
}

/***************************************************************************************************
Function:   initBuilding()
Purpose:    Initialize a BuildingType struct by dynamically allocating memory and setting its 
            initial values. This function creates an empty building object, with no journal, ghost, 
            hunters, rooms or doors. journal and ghost are initialized to NULL and hunters, rooms, 
            and doors array have all elements initialized as NULL. The respective integer counts 
            are set to 0.

    out:     * building is a double pointer to a BuildingType struct. The function initializes the
               BuildingType struct and assigns the memory address to the pointer.
in: N/A
return: void
***************************************************************************************************/
void initBuilding(BuildingType** building){
    (*building) = (BuildingType*) malloc(sizeof(BuildingType));
    (*building)->journal = NULL;
    (*building)->ghost = NULL;

    for (int i = 0; i < MAX_HUNTERS; i++) {(*building)->hunters[i] = NULL;}
    for (int j = 0; j < MAX_ROOMS; j++) {(*building)->rooms[j] = NULL;}
    for (int k = 0; k < MAX_DOORS; k++) {(*building)->doors[k] = NULL;}

    (*building)->doorCount = 0; 
    (*building)->roomCount = 0;
    (*building)->hunterCount = 0;
}

/***************************************************************************************************
Function:   initRoom()
Purpose:    Initialize a RoomType struct by dynamically allocating memory for it and setting its 
            initial values. This function creates an empty room object, setting the ghost and 
            evidence field to NULL and hunter count to zero. All doors array elements are set to NULL, 
            and the room semaphore field is initialized for mutex protection. The room name string 
            is updated to what is passed in to the function

    out:    room is a double pointer to a RoomType struct. The function initializes a
            default RoomType and assigns the memory address to the (dereferenced) pointer. 
            Ghost, evidence, and doors (in each direction) are initialized to NULL indicating 
            none exists in room. 

    in:     * name is a character pointer representing the room name, which will update the
              RoomType struct field 'name' using strcpy()
return: void
****************************************************************************************************/
void initRoom(RoomType** room, char* name){
    *room = (RoomType*)malloc(sizeof(RoomType));
    strcpy((*room)->name,name);
    (*room)->ghost = NULL;
    (*room)->evidence = NULL; 
    (*room)->hunterCount = 0;
    for(int i = 0; i<DIRECTION_COUNT;i++){
        (*room)->doors[i] = NULL;
    }
    if (sem_init(&(*room)->mutex, SEM_PSHARED, SEM_VALUE) < SEM_OK) {
        printf("Error: on semaphore init.\n");
        exit(C_TRUE);
    }
}


/***************************************************************************************************
Function:   initDoor()
Purpose:    Initialize a DoorType struct by dynamically allocating memory and setting its initial values.
            
            This function creates a door object, connecting two rooms in the following method:

                ' given roomA, roomB will connect in the given direction  '

            roomA and roomB for any door depends on how it is initilized and does not work both ways if 
            only one door is initialized for two rooms and can cause confusion. This is corrected using the 
            following comparison check ( See hunterMove() ):

                if(strcmp(theDoor->roomA->name,currRoom->name)==0){nextRoom = theDoor->roomB;} 
                else{nextRoom = theDoor->roomA;}

            Ideally two oposite doors should be initialized if two rooms are to be connected but this was
            not implemented with how connectRooms was called in populate building
            
            Function also initializes the DoorType mutex semaphore for shared resource protection

    out:    * door is a double pointer to a DoorType struct. The function initializes the
              DoorType struct and assigns the memory address to the pointer.
in:         * roomA is a pointer to a RoomType struct representing one room the door connects.
            * roomB is a pointer to a RoomType struct representing the other room the door connects.
return: void
***************************************************************************************************/
void initDoor(DoorType** door, RoomType* roomA, RoomType* roomB) {
    if (sem_init(&(*door)->mutex, SEM_PSHARED, SEM_VALUE) < SEM_OK) {
        printf("Error: on semaphore init.\n");
        exit(C_TRUE);
    }
    (*door)->roomA = roomA;
    (*door)->roomB = roomB;
}

/***************************************************************************************************
Function:   initHunter()
Purpose:    Initialize HunterType struct by dynamically allocating memory and setting its initial values.
            This function creates a hunter object, assigns the hunter's given name, places it in a 
            given room updating the room and hunter to have each others pointer, increments the 
            hunter count in the given room, assigns the shared journal, and sets its fear and boredom 
            attributes to global constant values.

out:        * hunter is a pointer to a pointer to a HunterType struct. The function initializes the
              HunterType struct and assigns the memory address to the pointer
in:         * name is a character pointer representing the hunter's name, which will update the name
              field in the respective HunterType struct using strcpy()
            * journal is a pointer to a shared JournalType struct, which is assigned to this hunter
in/out      * room is a pointer to a RoomType struct representing the room the hunter is placed in. 
              it is assigned to the hunters room field, and its own fields are updated to include
              hunter
return: void
**************************************************************************************************/
void initHunter(HunterType** hunter, char* name, RoomType* room, JournalType* journal){
    (*hunter) = (HunterType*)malloc(sizeof(HunterType));
    strcpy((*hunter)->name,name);
    
    sem_wait(&room->mutex);
    (*hunter)->room=room;
    room->hunters[room->hunterCount] = (*hunter);
    room->hunterCount = room->hunterCount + 1;
    sem_post(&room->mutex);
    
    (*hunter)->journal = journal;
    (*hunter)->fear = INIT_FEAR;
    (*hunter)->boredom = INIT_BORED;
}

/*****************************************************************************************************
Function:   initEvidence()
Purpose:    Initialize an EvidenceType struct by dynamically allocating memory and setting its initial values.
            This function creates an evidence object and sets its default values for its fields, 
            EvidenceType, EvidenceCategoryType and RoomType is set to what is passed in as parameters 
            (assumed called with default parameters). the founder field is initialized to NULL indicating 
            it was not found by a hunter.

    out:    * evidence is a double pointer to an EvidenceType struct. The function initializes the
              EvidenceType struct and assigns the memory address to the pointer.
in:         * type is an EvidenceCategoryType enumeration value representing the category of evidence.
            * origin is a pointer to a RoomType struct, representing the room where the evidence
              originates.
return: void
******************************************************************************************************/
void initEvidence(EvidenceType** evidence, EvidenceCategoryType type, RoomType* origin){
    (*evidence) = (EvidenceType*)malloc(sizeof(EvidenceType));
    (*evidence)->type = type;
    (*evidence)->founder = NULL;
    (*evidence)->origin = origin;
}

/***************************************************************************************************
Function:   initJournal()
Purpose:    Initialize a JournalType struct dynamically allocating memory for it and setting its 
            initial values. This function creates an default empty journal object with size of 0. 
            The journal containsa singly linked list of evidenceNodes where the head field is set 
            to NULL indicating no evidenceNode exists. Guess field is assigned UNKNOWN_GHOST enum value,
            banished boolean is initialized to C_FALSE. Additionally, the semaphore field is initialized 
            for mutex protection.
out:          * journal is a double pointer to a JournalType struct. The function initializes the
                dereferenced JournalType struct fields and assigns the memory address to dereferenced pointer.
in: N/A
return: void
****************************************************************************************************/
void initJournal(JournalType** journal){
    (*journal) = (JournalType*) malloc(sizeof(JournalType));
    (*journal)->head=NULL; // add evidence, make a node
    (*journal)->size=0; // check using size
    (*journal)->guess=UNKNOWN_GHOST;
    (*journal)->banished=C_FALSE;
    if (sem_init(&(*journal)->mutex, SEM_PSHARED, SEM_VALUE) < SEM_OK) {
        printf("Error: on semaphore init.\n");
        exit(C_TRUE);
    }
}

/***************************************************************************************************
Function:   connectRooms()
Purpose:    Create a new DoorType struct to connect two RoomType structs in a specified direction
            and dynamically allocating memory for the door. This function creates a door object, 
            connecting two rooms in the following method:

                ' given roomA, roomB will connect in the given direction  '

            roomA and roomB for any door depends on how it is initilized and does not work both ways if 
            only one door is initialized for two rooms and can cause confusion. This is corrected using the 
            following comparison check ( See hunterMove() ):

                if(strcmp(theDoor->roomA->name,currRoom->name)==0){nextRoom = theDoor->roomB;} 
                else{nextRoom = theDoor->roomA;}

            Ideally two oposite doors should be initialized if two rooms are to be connected but this was
            not implemented with how connectRooms was called in populate building

            The rooms' doors arrays are updated accordingly in the given direction indext, and the function 
            returns a pointer to the new door.

in:         * roomA is a pointer to a RoomType struct representing one room the door connects.
            * direction is a DirectionType enumeration value representing the direction from roomA
              to roomB.
            * roomB is a pointer to a RoomType struct representing the other room the door connects.
out: N/A
return:     * A pointer to the new DoorType struct.
***********************************************************************************************/
DoorType* connectRooms(RoomType* roomA, DirectionType direction, RoomType* roomB) {
    DoorType* door = (DoorType*)malloc(sizeof(DoorType));

    // LOCK BOTH ROOMS
    sem_wait(&roomA->mutex);
    sem_wait(&roomB->mutex);
    initDoor(&door, roomA, roomB);
    roomA->doors[direction] = door;
    roomB->doors[getOppositeDirection(direction)] = door;

    // UNLOCK BOTH ROOMS
    sem_post(&roomA->mutex);
    sem_post(&roomB->mutex);

    return door;
}

/***************************************************************************************************
Function:   initAllThreads() 
Purpose:    Initialize all HunterType structs and assign them to the BuildingType struct.
            This function creates and initializes the hunters (Spengler, Venkman, Ray, and
            Winston), placing them in the starting room van (G_HUNTER_VAN) and assigning the building's
            shared journal to each of them. The hunters are then assigned to the building and the hunterCount
            of the building is set to the passed in hunterCount parameter (of the same name as the field). 
            To modify the number of hunters in the simulation change the value of the global constant 
            MAX_HUNTERS. This prigram will handle any number but 0-4 are the only valid options.

in/out:     * building is a pointer to a BuildingType struct, where the hunters will be assigned,
              the building is accessed for the room array field to assign the hunters to the correct
              starting room. The building's field journal pointer is also assigned to the hunter. This
              is technically an in/out parameter as the shared pointers in the building are being updated.
              
in:         * hunterCount is the number of hunter threads initialized for this program ( see main() )
out:        N/A
return:     void
***************************************************************************************************/
void initAllThreads(BuildingType* building, int hunterCount){
    printf("HUNTERS START IN %s\n",building->rooms[G_HUNTER_VAN]->name);
    if(hunterCount >0){
        HunterType* Spengler;
        initHunter(&(Spengler),"SPENGLER",building->rooms[G_HUNTER_VAN],building->journal);
        building->hunters[G_HUNTER_1] = Spengler;
    }
    if(hunterCount >1){
        HunterType* Venkman;
        initHunter(&(Venkman),"VENKMAN",building->rooms[G_HUNTER_VAN],building->journal);
        building->hunters[G_HUNTER_2] = Venkman;
    }
    if(hunterCount >2){
        HunterType* Ray;
        initHunter(&(Ray),"RAY",building->rooms[G_HUNTER_VAN],building->journal);
        building->hunters[G_HUNTER_3] = Ray;
    }
    if(hunterCount >3){
        HunterType* Winston;
        initHunter(&(Winston),"WINSTON",building->rooms[G_HUNTER_VAN],building->journal);
        building->hunters[G_HUNTER_4] = Winston;
    }
    building->hunterCount = hunterCount;
}

/***************************************************************************************************
Function:   threads() 
Purpose:    Create and join the threads for the ghost and hunters, and calculate the total number of
            attempts made by the hunters to banish the ghost. Resulting attempt value will only 
            be 1 or 0 with the default program but the the program handles an extreme case if the evidence
            indicates the wrong 'guess' is made. If this happens the attempt value will correctly return
            the number of banish attempts sucessful or otherwise.
            This function creates the hunters' threads using pthread_create, then joins the
            ghost thread and the hunters' threads using pthread_join. 

in/out:     * building is a pointer to the container BuildingType struct containing the hunters and 
              ghost fields and accesses and updates the structs in memory
out: N/A
return:     * totalAttempts is an integer representing the total number of attempts made by the hunters.
***************************************************************************************************/
int threads(BuildingType* building){
    int* attempts;
    int totalAttempts = 0;
    for(int i = 0; i<building->hunterCount;i++){
        if(pthread_create(&(building->hunters[i]->thread), NULL, &hunterThreadFunc, building->hunters[i]) != PTHREAD_BAD){
            printf("Error: pthread_create hunter.\n");
            exit(C_TRUE);
        } 
    }
    if(pthread_join(building->ghost->thread, NULL)!= PTHREAD_BAD){
        printf("Error: pthread_join ghost.\n");
        exit(C_TRUE);
    }
    for(int i = 0; i<building->hunterCount;i++){
        if(pthread_join(building->hunters[i]->thread, (void**) &attempts) != PTHREAD_BAD){
            printf("Error: pthread_join hunter.\n");
            exit(C_TRUE);
        }
        totalAttempts = totalAttempts + (*attempts);
        free(attempts);
    }
    return totalAttempts;
}

/*****************************************************************************************************************
Function:   hunterThreadFunc() 
Purpose:    This function is executed by each hunter thread and is responsible for the hunter's actions
            such as searching for evidence, attempting to banish a ghost, moving around the rooms, and
            managing their boredom and fear levels. The function returns the number of attempts made
            by the hunter to banish the ghost.

            The most complex part of the function occurs in this thread function. thread Algorithm is as follows:
                1.  slowThread() to simulate the hunter is preforming a time consuming task in the current 
                    room 

                2.  hunterExit() is called to check if the hunter thread meets valid exit conditions, if
                    valid the thread loop breaks and function returns.

                3.  If exit condition is not valid thread loop progresses with Hunters actions where
                    hunterFieldMod() will initially be called to update the fear or boredom levels based
                    on if the ghost is present in the room

                4.  slowThread() is again called to help avoid race conditions in the next steps as alot of
                    mutex locking and unlocking occurs but can simulate the hunter searching the room for
                    a ghost or evidence

                5.  this step conditionally checks if evidence should be collected by a hunter in this room:
                        a.  if evidence is present in room
                        b.  if a ghost is not present in room
                        c.  if hunters shared evidence count (difference) is less than 3 unique pieces 
                            of evidence
                    If all 3 conditions are valid then the respective hunter proceeds to analyze the evidence
                    where hunterEvidence() is called to determine if evidence should be collected or discarded
                    ( see hunterEvidence() )

                6.  if the conditions for step 5 is not valid then this step conditionally checks if the hunters 
                    should banish the building's ghost:  
                        a.  if the current room has a ghost
                        b.  if the hunters are prepared to banish the ghost where timeToBanish = C_TRUE if 
                            hunters have exactly 3 pieces of evidence
                        c.  the number of hunters in this room is less than 3
                    If all 3 conditions are valid then the respective hunter proceeds wait in the room for a 
                    banishing of the ghost to occur:
                        6i.     if number of hunters in this room is exactly 2 
                                    ->  the hunters will attempt to banish the ghost calling typeFromEvidence(), 
                                        ghostToString() and sayGhostName() functions to determine if all final 
                                        conditions are valid for a successful banishing of the ghost by these 
                                        two present hunters, if so the successfull banished exit condition will 
                                        be updated to valid for all hunters
                                    ->  if the final banishing conditions are invalid the journal will be emptied, all evidence
                                        will be cleared and the hunters will have to collect evidence again 
                        6ii.    if number of hunters in this room is less than 2 (==1) the hunter will wait in the
                                current room for another hunter to enter the room. 

                7.  if the conditions for step 5 and 6 are not valid the hunter proceeds to move to another room 
                    calling hunterMove()

in/out:     * arg is a pointer to a HunterType struct containing the hunter's fields to be accessed 
              and updated 
out: N/A
return:     * attempts is a pointer to an integer representing the number of attempts made by the hunters
              to banish the ghost. Works if the edge case occurs when initial attempt to banish the
              ghost does not work. 
*****************************************************************************************************************/
void* hunterThreadFunc(void *arg) {
    HunterType* hunter = (HunterType*)arg;
    int* attempts = malloc(sizeof(int));
    *attempts = 0;
    int timeToBanish = C_FALSE;
    GhostCategoryType guess = UNKNOWN_GHOST;
    while(C_TRUE){
        slowThread();
        RoomType* thisRoom = NULL;                  
		if(hunter->boredom>=MAX_BOREDOM || hunter->fear>= MAX_FEAR || hunter->journal->banished==C_TRUE){
            hunterExit(hunter);
		    break;
		} else{
            thisRoom = hunterFieldMod(hunter);
			slowThread();
			sem_wait(&hunter->room->mutex);
            sem_wait(&hunter->journal->mutex);

            int difference = hunter->journal->size;
            EvidenceType* weGotOne = thisRoom->evidence;

            if(weGotOne!=NULL && thisRoom->ghost==NULL && difference<3){ // has evidence and no ghost	
                timeToBanish = hunterEvidence(hunter,thisRoom,weGotOne);
                if(difference!=hunter->journal->size){printf(" * EVIDENCE COUNT IS NOW AT SIZE OF %d \n\n",difference + 1);} 
                else{printf(" * EVIDENCE COUNT UNCHANGED AT SIZE OF %d \n\n",difference);}
                thisRoom->evidence = NULL; // 4. Set the evidence pointer in the room to NULL. // freeing at end? 
                sem_post(&hunter->journal->mutex); 
                sem_post(&hunter->room->mutex);

            } else if(thisRoom->ghost!=NULL && timeToBanish && thisRoom->hunterCount<=2){ // has ghost and the journal size is 3
            	printf("*[%s] IS WAITING FOR BACKUP IN %s\n",hunter->name,thisRoom->name);
                if(thisRoom->hunterCount==2){
				    printf("\n**** HUNTERS [%s] AND [%s] WILL NOW ATTEMPT TO BANISH THE GHOST IN %s ****\n",thisRoom->hunters[0]->name,thisRoom->hunters[1]->name,thisRoom->name);
                    guess = typeFromEvidence(hunter->journal);
                    char ghostName[MAX_STR];
                    ghostToString(guess, ghostName);
                    if(sayGhostName(thisRoom->ghost, guess)){
                        hunter->journal->banished = C_TRUE;
                        hunter->journal->guess = guess;          
                        printf("**** HUNTERS [%s] AND [%s] SUCCESSFULLY BANISHED THE GHOST MENACE CORRECTLY CALLING TO THE %s ****\n\n",thisRoom->hunters[0]->name,thisRoom->hunters[1]->name,ghostName);
                    } else{
                        printf("**** HUNTERS [%s] AND [%s] FAILED IN THEIR BANISH ATTEMPT FALSELY CALLING TO THE %s ****\n",thisRoom->hunters[0]->name,thisRoom->hunters[1]->name,ghostName);
                        printf("\t*THE HUNTERS ERASE THEIR ENTRIES AND START AGAIN\n\n");
                        timeToBanish = C_FALSE;
                        clearJournal(hunter->journal); //THROW THE BOOK OUT START AGAIN
                        *attempts = *attempts + 1;
                    }
                } 
                sem_post(&hunter->room->mutex);
                sem_post(&hunter->journal->mutex);
            }
            else{                           // if ghost is in room and not enough evidence to banish or no evidence is in the room 
            	sem_post(&hunter->journal->mutex);
            	sem_post(&hunter->room->mutex);
                hunterMove(hunter);
            }
		}
	}
    return (void*) attempts;
}

/***************************************************************************************************
Function:   hunterExit()
Purpose:    Handles the hunter thread exit of the thread loop based on their boredom, fear, or successful
            banishing of the ghost. Updates the room's hunter count and removes the hunter from the
            room's hunters array. It updates the room's data to reflect the hunter's exit and logs 
            the reason for the exit. Note the fear exit condition has a unique way of removing the
            hunter to correctly implement the desired simulation:
                a/c.    if max boredom of a hunter is reached or if the hunters successfully banished the ghost
                        the hunter is removed from the room and thread is joined 
                b.      if max fear of a hunter is reached this is because a ghost is in the room
                        and the hunter must leave this room before being removed completely or the ghost 
                        will bug out as the ghost reacts to a hunter in the room and vice versa 
                        therefore hunterMove is called then the hunter is removed from the 
                        room and thread is joined 
in/out:     *   hunter is a pointer to a HunterType struct containing the hunter field's which is accessed
                and updated
out: N/A
return: void
**************************************************************************************************/
void hunterExit(HunterType* hunter){
    if(hunter->boredom>=MAX_BOREDOM){logAction(HUNTER_EXIT_BORED, hunter);} 
    else if(hunter->fear>= MAX_FEAR){
        hunterMove(hunter); // need to move before flee or inf loop ghost haunt :(
        logAction(HUNTER_EXIT_AFRAID, hunter);
    }
    else if(hunter->journal->banished==C_TRUE){logAction(HUNTER_EXIT_BANISHED, hunter);}
    sem_wait(&hunter->room->mutex); // Lock the room the hunter is exiting.
    RoomType* thisRoom = hunter->room;
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (thisRoom->hunters[i] == hunter) {
            if(thisRoom->hunterCount>1){
                thisRoom->hunters[i] = thisRoom->hunters[thisRoom->hunterCount-1];
                thisRoom->hunters[thisRoom->hunterCount-1] = NULL;
            } else{thisRoom->hunters[i]=NULL;}
            thisRoom->hunterCount = thisRoom->hunterCount - 1;
            break;
        }
    }
    sem_post(&hunter->room->mutex); // Unlock the room the hunter is exiting.
}

/***************************************************************************************************
Function:   hunterFieldMod()
Purpose:    checks whether a ghost is present in the room the hunter is in. If a ghost is present, 
            it increases the hunter's fear field by 1 and resets boredom field to 0. 
            If no ghost is present, it increases the hunter's boredom by 1.

in/out:     * hunter is a pointer to a HunterType struct containing the hunter's data, the hunter's
              pointer is unchange but the hunter's fields are modified
in & out:   N/A
return:     * thisRoom is a pointer to a RoomType struct representing the room the hunter is currently in
***************************************************************************************************/
RoomType* hunterFieldMod(HunterType* hunter){
    int tempFear = NEW_INT;
    int tempBore = NEW_INT;
    sem_wait(&hunter->room->mutex);
    RoomType* thisRoom = hunter->room;
    if(thisRoom->ghost != NULL){ 
        tempFear = hunter->fear;
        tempFear = tempFear + 1;
        hunter->fear = tempFear;
        hunter->boredom = 0; // reset boredom
    } else{
        tempBore = hunter->boredom;
        tempBore = tempBore + 1;
        hunter->boredom = tempBore;
    }
    sem_post(&hunter->room->mutex);
    return thisRoom;
}

/***************************************************************************************************
Function:   hunterEvidence()
Purpose:    This function is called when a hunter finds new evidence in a room. It checks if the
            evidence is unique (not already present in the hunter's journal) and then adds it to the
            journal if it is, otherwise it discards the evidence. It also logs the actions of the
            hunter and returns a flag indicating if the hunter has enough evidence to attempt a
            banishing of the ghost.
in/out:     * hunter is a pointer to a HunterType struct containing the hunter's fields. These fields
              are accessed and updated.
            * newEvidence is a pointer to an EvidenceType struct representing the evidence found by the 
              hunter. newEvidence is accessed and updated depending on its contents. 
int         * thisRoom is a pointer to a RoomType struct representing the room the hunter is currently in.
out:        N/A
return:     * timeToBanish is an integer flag (C_TRUE or C_FALSE) indicating if the hunter has enough
            evidence to attempt a banishing of the ghost.
*************************************************************************************************/
int hunterEvidence(HunterType* hunter, RoomType* thisRoom, EvidenceType* newEvidence){
    int timeToBanish = C_FALSE;
    EvidenceNodeType* journalPage = hunter->journal->head;
    EvidenceNodeType* lastPage = NULL;
    char unique = C_TRUE;
    while(journalPage!=NULL){
        if(journalPage->evidence->type==newEvidence->type){
            unique = C_FALSE;
            break;
        }
        lastPage = journalPage;
        journalPage = journalPage->next;
    }
    if(unique){
        newEvidence->founder=hunter; // 1. Set the hunter as the founder of the evidence in the Evidence struct.
        EvidenceNodeType* newPage = (EvidenceNodeType*) malloc(sizeof(EvidenceNodeType));
        newPage->evidence = newEvidence;
        newPage->next = hunter->journal->head;
        hunter->journal->head = newPage;
        if (lastPage == NULL) {newPage->next = NULL;}
        hunter->journal->size = hunter->journal->size +1; // 3. Increment the journal's evidence count.
        if(hunter->journal->size==3){timeToBanish = C_TRUE;} 
        printf("\n");
        logAction(HUNTER_EVIDENCE_COLLECT,hunter,newEvidence->type,thisRoom); // 5. Log the evidence as collected with the provided logAction() function.
    } else{
    	printf("\n");
        logAction(HUNTER_EVIDENCE_DISCARD,hunter,newEvidence->type,thisRoom); // 2. Log the evidence as discarded using the logAction() function.
        free(newEvidence);
    }
    return timeToBanish;
}


/***************************************************************************************************
Function:   hunterMove()
Purpose:    Handles the movement of the hunter from their current room to an adjacent room.
            This function is called when a hunter wants to move to a new room. It selects a valid
            door at random, updates the hunter's room and the rooms' hunter counts, and logs the
            action using the logAction() function. It also ensures proper mutex locking and
            unlocking to maintain thread safety during the movement.

in/out:     * hunter is a pointer to a HunterType struct containing the hunter's fields. These fields
              are accessed and updated according the various conditions and values of the fields
out: N/A
return: void 
************************************************************************************************/

void hunterMove(HunterType* hunter){
    RoomType* currRoom = hunter->room;
    int findDoor = NEW_INT;
    RoomType* nextRoom = NULL;
    DoorType* theDoor = NULL; 
    
    while(theDoor==NULL){//                                          __FIND VALID DOOR__
        findDoor = rand() % DIRECTION_COUNT;
        sem_wait(&currRoom->mutex);                                 //      CURROOM LOCK
        if(currRoom->doors[findDoor]!=NULL){
            theDoor = currRoom->doors[findDoor];
            sem_post(&currRoom->mutex);                             //  1 CURROOM UNLOCK  
            sem_wait(&currRoom->doors[findDoor]->mutex);            //         DOOR LOCK  
            if(strcmp(theDoor->roomA->name,currRoom->name)==0){nextRoom = theDoor->roomB;} 
            else{nextRoom = theDoor->roomA;}
            sem_post(&currRoom->doors[findDoor]->mutex);            //       DOOR UNLOCK   
        } else{sem_post(&currRoom->mutex);}                         //  2 CURROOM UNLOCK                 
    }
    sem_wait(&theDoor->mutex); // 1. Lock the door between the two rooms.
    sem_wait(&currRoom->mutex); // 2. Lock the room the hunter is exiting.
    // 3. remove hunter
    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (currRoom->hunters[i] == hunter) {
            if(currRoom->hunterCount>1){
                currRoom->hunters[i] = currRoom->hunters[currRoom->hunterCount-1];
                currRoom->hunters[currRoom->hunterCount-1] = NULL;
            } else{currRoom->hunters[i]=NULL;}
            currRoom->hunterCount = currRoom->hunterCount - 1;
            break;
        }
    }
    sem_wait(&nextRoom->mutex); //  5. Lock the room the hunter is entering.
    nextRoom->hunters[nextRoom->hunterCount] = hunter; // 6. Add the hunter to the room they are entering.
    hunter->room = nextRoom;
    nextRoom->hunterCount = nextRoom->hunterCount + 1;
    logAction(HUNTER_MOVE_TO,hunter,nextRoom); // 9. Log the movement using the provided logAction() function
    sem_post(&nextRoom->mutex); // 7. Unlock the room the hunter is entering.
    sem_post(&currRoom->mutex);
    sem_post(&theDoor->mutex); // 8. Unlock the door between the two rooms.
}

/***************************************************************************************************
Function:   printResults()
Purpose:    This function prints the simulation results, including the winner, hunter stats, and
            all evidence collected. It provides an overview of the results of the simulation in a
            formatted printout.
    in:     * building is a pointer to a BuildingType struct containing the building's data.
            * results is an integer that represents the total number of attempts the hunters made to
              banish the ghost.
out: N/A
return: void 
************************************************************************************************/
void printResults(BuildingType* building, int results){
    // DISPLAY WINNERS 
    char ghostName[MAX_STR];
    char victor[MAX_STR];
    char evidenceName[MAX_STR];
	printf("\n SIMULATION RESULTS\n");
    pBreak(PRINT_LONG);
    printf("|                 AFTER ACTION REPORT               |\n");
    pBreak(PRINT_LONG);
    pBreak(PRINT_EMPTY);
    if(building->journal->banished){
    	strcpy(victor,"*** THE HUNTERS ***");
    	results = results +1;
    } 
    else{strcpy(victor,"*** THE GHOST ***  ");}
    printf("| THE VICTOR: %15s  %-17s  |\n","",victor);
    pBreak(PRINT_EMPTY);  
    printf("| *HUNTERS TOTAL ATTEMPTS TO BANISH: \t\t%2d  |\n",results);
    pBreak(PRINT_EMPTY);
    ghostToString(building->journal->guess,ghostName);
    printf("| *HUNTERS CONCLUDE GHOST IS:    %17s  |\n",ghostName);
    pBreak(PRINT_EMPTY);
    pBreak(PRINT_LONG);					   
    printf("\n FINAL HUNTER STATS \n");
    pBreak(PRINT_SHORT);
	printf("|%11s   | %6s  | %6s |\n","HUNTER","FEAR","BOREDOM");
    pBreak(PRINT_SHORT);
	for (int i = 0; i < building->hunterCount; i++) {printf("|%11s   |   %3d   |   %3d   |\n",building->hunters[i]->name,building->hunters[i]->fear,building->hunters[i]->boredom);}
    pBreak(PRINT_SHORT);
    printf("\n LAST JOURNAL ENTRIES\n");
    pBreak(PRINT_LONG);
    EvidenceNodeType* journalPage = building->journal->head;
    EvidenceNodeType* lastPage = journalPage;
    printf("| %11s   | %10s   | %16s   |\n","EVIDENCE","FOUNDER","ROOM");
    pBreak(PRINT_LONG);
    while(journalPage!=NULL){
        journalPage = journalPage->next;
        evidenceToString(lastPage->evidence->type,evidenceName);
        printf("| %12s  | %10s   | %16s   |\n",evidenceName,lastPage->evidence->founder->name,lastPage->evidence->origin->name); 
        lastPage = journalPage;
    }
    pBreak(PRINT_LONG);
}
void clearJournal(JournalType* journal){
    EvidenceNodeType* journalPage = journal->head;
    EvidenceNodeType* lastPage = journalPage;
    while(journalPage!=NULL){
        journalPage = journalPage->next;
        free(lastPage->evidence);
        free(lastPage);
        lastPage = journalPage;
        journal->size = journal->size - 1;
    }
    journal->head = NULL;
}

/***************************************************************************************************
Function: freeBuilding()
Purpose:    This function frees the memory dynamically allocated for the building throughout the 
            entire program and all its components. The freeing of memory is preformed on the 
            entire hunters array, ghosts array, journal evidence linked list, doors array, and rooms
            array.
in/out:     ** building double pointer to a BuildingType container that contains the building data
            to be deallocated from memory.
out: N/A
return: void 
**************************************************************************************************/

void freeBuilding(BuildingType** building) {
    // FREE HUNTERS
    for (int i = 0; i < (*building)->hunterCount; i++) {free((*building)->hunters[i]);}
    // FREE GHOSTS
    free((*building)->ghost);
    // FREE JOURNAL
    sem_destroy(&(*building)->journal->mutex);  
    clearJournal((*building)->journal);
    free((*building)->journal);
    // FREE DOORS
    for (int i = 0; i < (*building)->doorCount; i++) {
        sem_destroy(&(*building)->doors[i]->mutex);
        free((*building)->doors[i]);
    }
    // FREE ROOMS
    for (int i = 0; i < (*building)->roomCount; i++) {
        RoomType* room = (*building)->rooms[i];
        sem_destroy(&room->mutex);
        if (room->evidence != NULL) {free(room->evidence);}
        for (int j = 0; j < room->hunterCount; j++) {free(room->hunters[j]);}
		free(room);
    }
    // FREE BUILDING
    free(*building);
}





