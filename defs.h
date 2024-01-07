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
FILE:   defs.h
FILE PURPOSE:  a header file containing program definitions, data structures, enumerations, 
               function declarations used throughout the program. It organizes shared elements, 
               making the program easier to maintain and modify. The file includes C libraries 
               essential for the program, a logging function, typedefs, and forward declarations 
               for structs and enums, as well as the initialization of prototype functions.
**********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_HUNTERS     4       // change this value to determine how many hunters start in the simulation (program handles 0-4)

#define MAX_FEAR        100     // max fear level hunters experience see INIT_FEAR for range      (default 50-100)
#define MAX_BOREDOM     100     // max boredome level hunters experience see INIT_BORED for range (default 80-100)
#define INIT_FEAR       80      // initial fear field value of each hunter (IDEAL VALUE FOR MAX_HUNTERS = 4 & GHOST_EV_PROB = 30)
#define INIT_BORED      50      // initial bored field value of each hunter 

#define MAX_STR         64      // max characters in character array
#define MAX_ROOMS       16      // max rooms in a building
#define MAX_DOORS       16      // max doors in a building 
#define MIN_WAIT        1000    // minimum time of waiting in slowThread() eg: ' usleep(MIN_WAIT + rand() % MAX_WAIT) '
#define MAX_WAIT        5000    // maximum time of waiting in slowThread() eg: ' usleep(MIN_WAIT + rand() % MAX_WAIT) '
#define GHOST_WAIT      10000   // consistent wait time for ghost thread
#define GHOST_EV_PROB   30      // probability of ghost dropping evidence ( 30 = %30 )
#define LOGGING_ENABLED 1       // boolean true

#define C_FALSE         0       // boolean is false
#define C_TRUE          1       // boolean is true
#define PTHREAD_BAD     0       // upon creating or joining a thread if this value is returned there was an error in process
#define NEW_INT        -1       // initialize integer as negative for debugging when 0 values are valid in program

#define G_HUNTER_VAN    0       // building room array index 

#define G_HUNTER_1      0       // initialize hunter as first index of building hunters array 
#define G_HUNTER_2      1       // initialize hunter as second index of building hunters array 
#define G_HUNTER_3      2       // initialize hunter as third index of building hunters array 
#define G_HUNTER_4      3       // initialize hunter as fourth index of building hunters array 

#define PRINT_LONG      1       // indicates an long line break of consistent length is printed in a formatted table
#define PRINT_SHORT     2       // indicates an short line break of consistent length is printed in a formatted table
#define PRINT_EMPTY     3       // indicates an empty line of consistent length is printed in a formatted table

#define SEM_PSHARED     0       // indicates the semaphore is shared among all threads of a process
#define SEM_VALUE       1       // the initial value of the semaphore is the desired number of initial allowed concurrent accesses
#define SEM_OK          0       // if this return value is negative then semaphore is initialized with error


// updated 0404 NOLEAK 4 HUNTERS 1 GHOST | HAS EVIDENCE NOLEAK | BANISHMENT NOLEAKS HOTFIX3 | DONE

/*  By putting DIRECTION_COUNT at the end to define enum size
*/
enum Direction { NORTH, SOUTH, EAST, WEST, DIRECTION_COUNT };

/* By using bits like this, we can actually construct a GhostCategory from its evidence! */
enum EvidenceCategory {
    EMF                 = 1 << 0, // 0b0001
    FINGERPRINTS        = 1 << 1, // 0b0010
    TEMPERATURE         = 1 << 2, // 0b0100
    SOUND               = 1 << 3, // 0b1000
    UNKNOWN_EVIDENCE    = 0xF     // 0b1111
};

enum GhostCategory {
    BANSHEE         = EMF | TEMPERATURE | SOUND,            // 0b1101
    BULLIES         = EMF | FINGERPRINTS | SOUND,           // 0b1011
    PHANTOM         = FINGERPRINTS | TEMPERATURE | SOUND,   // 0b1110
    POLTERGEIST     = EMF | FINGERPRINTS | TEMPERATURE,     // 0b0111
    UNKNOWN_GHOST   = 0xFF                                  // 0b11111111    
};

/*  When working with lots of connected structures, forward declaring is often
    best to avoid circular dependencies. This just does the typedefs for us
    so that we can refer to the types before they've been defined!

    Also, the Ghost uses the full struct/enum names, so you can change the typedef 
    to use any naming convention you want, including camel_case. When working in VS Code,
    "F2" while selecting the Type name will let you rename it everywhere it's used.
*/
typedef struct Room RoomType;
typedef struct Building BuildingType;
typedef struct Door DoorType;
typedef struct Hunter HunterType;
typedef struct Ghost GhostType;
typedef struct Journal JournalType;
typedef struct Evidence EvidenceType;
typedef struct EvidenceNode EvidenceNodeType;
typedef enum Direction DirectionType;
typedef enum EvidenceCategory EvidenceCategoryType;
typedef enum GhostCategory GhostCategoryType;

struct Ghost {
    RoomType*           room;
    GhostCategoryType   type;
    int                 boredom;
    int                 fear;
    pthread_t           thread;
};

struct Hunter {
    char                    name[MAX_STR];
    RoomType*               room;
    JournalType*            journal;
    int                     fear;
    int                     boredom;
    pthread_t               thread;
};

struct Evidence {
    EvidenceCategoryType        type;
    HunterType*                 founder;
    RoomType*            origin;
};

struct EvidenceNode {
    EvidenceType*           evidence;
    EvidenceNodeType*    next;
};

struct Journal {
    EvidenceNodeType*   head;
    int                 size;
    GhostCategoryType   guess;
    char                banished;
    sem_t               mutex;
};

struct Room {
    char                name[MAX_STR];
    GhostType*          ghost;
    EvidenceType*       evidence;
    HunterType*         hunters[MAX_HUNTERS];
    DoorType*           doors[DIRECTION_COUNT];
    int                 hunterCount;
    sem_t               mutex;
};

struct Door {
    RoomType*   roomA;
    RoomType*   roomB;
    sem_t       mutex;
};

struct Building {
    JournalType*    journal;
    GhostType*      ghost;
    HunterType*     hunters[MAX_HUNTERS];
    RoomType*       rooms[MAX_ROOMS];
    DoorType*       doors[MAX_DOORS];
    int             doorCount;
    int             roomCount;
    int             hunterCount;
};

// INITIALIZING FUNCTIONS

void initBuilding(BuildingType** building);
void populateBuilding(BuildingType* building);
void initRoom(RoomType** room, char* name);
void initDoor(DoorType** door, RoomType* roomA, RoomType* roomB);
void initHunter(HunterType** hunter, char* name, RoomType* room, JournalType* journal);
void initEvidence(EvidenceType** evidence, EvidenceCategoryType type, RoomType* origin);
void initJournal(JournalType** journal);

DoorType* connectRooms(RoomType*, DirectionType, RoomType*);

// HUNTER THREAD FUNCTIONS

void initAllThreads(BuildingType* building, int hunterCount);
int threads(BuildingType* building);
void* hunterThreadFunc(void* arg);
void hunterExit(HunterType* hunter);
int hunterEvidence(HunterType* hunter,RoomType* room,EvidenceType* evidence);
void hunterMove(HunterType* hunter);
RoomType* hunterFieldMod(HunterType* hunter);

// EXIT PROGRAM FUNCTIONS

void clearJournal(JournalType* journal);
void printResults(BuildingType* building, int results);
void freeBuilding(BuildingType** building);

/* Utility Functions: Feel free to modify */

void slowThread();
GhostCategoryType typeFromEvidence(JournalType* evidence);
void evidenceToString(EvidenceCategoryType evidence, char* buffer);
DirectionType getOppositeDirection(DirectionType dir);
void ghostToString(GhostCategoryType type, char* str);
void pBreak(int type);

/* Library Declarations: Do not make any modifications to the below definitions */
/* Ghost Functions Implemented in ghost.o */

void initGhost(GhostType** ghost, RoomType* room);
int sayGhostName(GhostType* ghost, GhostCategoryType category);
void cleanupGhost(GhostType* ghost);

/* Logging Functions & Struct*/
typedef enum LogAction {
    HUNTER = 1 << 15,
    GHOST = 1 << 14,
    MOVE = 1 << 13,
    EXIT = 1 << 12,
    EVIDENCE = 1 << 11,
    HAUNTING = 1 << 10,
    ROOM = 1 << 9,

    HUNTER_MOVE_TO = HUNTER | MOVE | ROOM,
    HUNTER_EXIT_AFRAID = HUNTER | EXIT | 1,
    HUNTER_EXIT_BORED = HUNTER | EXIT | 2,
    HUNTER_EXIT_BANISHED = HUNTER | EXIT | 3,
    HUNTER_EVIDENCE_COLLECT = HUNTER | EVIDENCE | ROOM | 1,
    HUNTER_EVIDENCE_DISCARD = HUNTER | EVIDENCE | ROOM | 2,
    GHOST_MOVE_TO = GHOST | MOVE | ROOM,
    GHOST_EXIT_AFRAID = GHOST | EXIT | 1,
    GHOST_EXIT_BORED = GHOST | EXIT | 2,
    GHOST_HAUNTING = GHOST | HAUNTING | ROOM,
} LogActionType;

void logAction(enum LogAction action, ...);
