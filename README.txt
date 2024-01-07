*******************************************************************************************************************

NAME: 		MICHAEL HARRIS ANASTASAKIS
LANGUAGE:   C
PURPOSE:    This program is a simulation of a haunted building using multithreading implemented
            with multiple semaphore mutex locks. The threads are various Ghost Hunters and a
	    single ghost all moving throughout the building concurrently. The building has rooms,
            doors, hunters, ghosts, and evidence of the ghost's presence. The simulation runs a
            series of actions, such as hunters moving between rooms, finding and collecting
            evidence, and attempts to banish the ghost based on the evidence they found. The
            program keeps track of hunter's fear and boredom levels, which affect their behaviour
            and exit conditions. The simulation determines whether the hunters are successful
            in banishing the ghost or if the ghost prevails causing the hunters to flee in fear.
            Upon completion of the simulation the program provides a summary of the simulation
            results, including the victor (hunters or ghost), hunter stats, and all evidence
            collected by hunters throughout the simulation.

PROGRAM FILES:  	building.c  defs.h  ghost.o  logger.o  main.c  Makefile  README.txt  util.c

PROGRAM REQUIREMENTS: 	linux based operating system (Tested on UBUNTU 22.04)

ACKNOWLEDGEMENTS:
    I would like to express my gratitude to Professor Connor Hillen for providing the initial outline and 
    specifications for several C assignments during my time at University. These assignments served as the 
    foundation for the final project for this class and the project presented here is the result of his
    specification requirements and his inclusion of the logger.o (console logging implementation) and ghost.o
    (ghost thread entity) files, as well as the design of the base map layout.

*******************************************************************************************************************

FILE:		main.c

					[MAIN PROGRAM FILE]

FUNCTIONS:	main(), initBuilding(), initRoom(), initDoor(), initHunter(), initEvidence(), initJournal()
          	connectRooms(), initAllThreads(), threads(), hunterThreadFunc(), hunterExit(), hunterFieldMod()
          	hunterEvidence(), hunterMove(), printResults(), freeBuilding()

PURPOSE: 	Execution of Program calling all primary functions in desired order within main() function.
			Majority of all functions in program are also in this file for convenience, these functions are
			the most critical to the program however the program needs all functions to execute.
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
				7. Run the simulation and obtain the result (success or failure in banishing the
                                ghost corresponding to exit condition of all threads).
				8. Print the results of the simulation, including winner, hunter stats, and
                                journal entries.
				9. Free up memory resources used in the simulation.

*******************************************************************************************************************

FILE:		building.c

FUNCTION:	populateBuilding()

PURPOSE: 	The purpose of this file is to implement the populateBuilding() function.
		This function is critical in creating the building structure by initializing
		rooms with their respective names, creating connections between the rooms using
		doors, and storing the number of rooms and doors in the building. This function
		creates a pre-defined layout of the building, ensuring that all rooms are connected
		properly, which is essential for the program to function correctly. The layout
		and connections must be well thought out before being implemented or the program
		may fail in execution.

*******************************************************************************************************************

FILE: 		defs.h

FILE PURPOSE:	a header file containing program definitions, data structures, enumerations,
		function declarations used throughout the program. It organizes shared elements,
		making the program easier to maintain and modify. The file includes C libraries
		essential for the program, a logging function, typedefs, and forward declarations
		for structs and enums, as well as the initialization of prototype functions.

*******************************************************************************************************************

FILE: 		Makefile

FILE PURPOSE:	utilizes the 'make' utility from the OS to compile and link all files / warning
		suppressors / commands necessary to execute this program and is consistent, organized and
		convenient for any user

*******************************************************************************************************************


	[ EXTRACTION | COMPILATION | EXECUTION | INSTRUCTIONS ]


	--> TO EXTRACT TAR FILE FROM TERMINAL BE IN DIRECTORY OF DOWNLOADED TAR FILE & TYPE THE FOLLOWING CMDS:
			tar -xvf ANASTASAKIS_MICHAEL_101047439_A5.tar

	--> TO COMPILE IN TERMINAL BE IN DIRECTORY OF DOWNLOADED UNTAR'd FILES & TYPE THE FOLLOWING CMD:
			make

	--> THE FOLLOWING SHOULD BE PRINTED TO TERMINAL UPON 'make' CMD
			gcc -g -Wall -Wextra -Werror -fstack-protector -c main.c
			gcc -g -Wall -Wextra -Werror -fstack-protector -c building.c
			gcc -g -Wall -Wextra -Werror -fstack-protector -c util.c
			gcc -g -Wall -Wextra -Werror -fstack-protector -o final main.o building.o util.o ghost.o
			logger.o -lpthread

	--> TO EXECUTE IN TERMINAL BE IN DIRECTORY OF DOWNLOADED UNTAR'd FILES & TYPE THE FOLLOWING CMDS:
    		./final > final.txt


	[ OTHER OPTIONS ]

	--> TO EXECUTE WITH VALGRIND MEMORY/LEAK CHECK BE IN DIR OF FILES AND IN TERMINAL TYPE THE FOLLOWING CMDS:
    		valgrind --track-origins=yes --leak-check=full ./final

	--> TO CLEAN DIRECTORY OF UNUSED FILES BE IN DIR OF FILES AND IN TERMINAL TYPE THE FOLLOWING CMDS:
	    	make clean


*******************************************************************************************************************


	[ EXECUTION ]

	--> UPON EXECUTION NO USER INPUT IS REQUIRED

	--> THE PROGRAM WILL RUN THEN PRINT OUT DETAILED LOGS OF ALL ACTIONS THE ENTITIES HAVE IN THE SIMULATION

	--> WHEN THE PROGRAM FINISHES A CONCLUDING SIMULATION SUMMARY WILL PRINT TO THE TERMINAL


	[ PROGRAM EXAMPLE OUTPUT ]

	--> THE FOLLOWING IS TERMINAL PRINTOUT EXAMPLES OF THIS PROGRAM:


		[ SIMULATION START EXAMPLE ]
	"
		GHOST STARTS IN [UTILITY ROOM]
		HUNTERS START IN VAN
		[HUNTER MOVE] [WINSTON] has moved into [HALLWAY N]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY N]
		[HUNTER MOVE] [RAY] has moved into [HALLWAY N]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY N]
		[HUNTER MOVE] [WINSTON] has moved into [BOY'S BEDROOM]
		[GHOST EVIDENCE] Left [TEMPERATURE] in [UTILITY ROOM]
		[HUNTER MOVE] [VENKMAN] has moved into [VAN]
		[HUNTER MOVE] [WINSTON] has moved into [HALLWAY N]
		[HUNTER MOVE] [RAY] has moved into [BOY'S BEDROOM]
		[HUNTER MOVE] [WINSTON] has moved into [MASTER BEDROOM]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY S]
		[GHOST MOVE] Ghost has moved into [GARAGE]
		....
		....
		....
																"

	[ SIMULATION EVIDENCE EXAMPLE ]

	"	....
		....
		....
		[GHOST MOVE] Ghost has moved into [GARAGE]
		[GHOST MOVE] Ghost has moved into [UTILITY ROOM]

		[HUNTER EVIDENCE] [VENKMAN] found [TEMPERATURE] in [KITCHEN] and [DISCARDED]
		 * EVIDENCE COUNT UNCHANGED AT SIZE OF 2

		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY N]
		[GHOST MOVE] Ghost has moved into [GARAGE]
		[GHOST MOVE] Ghost has moved into [UTILITY ROOM]
		[HUNTER MOVE] [SPENGLER] has moved into [BOY'S BEDROOM]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY S]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY N]
		[HUNTER MOVE] [VENKMAN] has moved into [KITCHEN]
		[GHOST MOVE] Ghost has moved into [GARAGE]
		[HUNTER MOVE] [VENKMAN] has moved into [GARAGE]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY S]
		[HUNTER MOVE] [VENKMAN] has moved into [KITCHEN]
		[GHOST MOVE] Ghost has moved into [KITCHEN]
		[GHOST MOVE] Ghost has moved into [HALLWAY S]
		[GHOST MOVE] Ghost has moved into [KITCHEN]
		[GHOST MOVE] Ghost has moved into [HALLWAY S]
		[HUNTER MOVE] [VENKMAN] has moved into [LIVING ROOM]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY N]

		[HUNTER EVIDENCE] [VENKMAN] found [FINGERPRINTS] in [LIVING ROOM] and [COLLECTED]
		 * EVIDENCE COUNT IS NOW AT SIZE OF 3

		[HUNTER MOVE] [SPENGLER] has moved into [MASTER BEDROOM]
		[GHOST MOVE] Ghost has moved into [HALLWAY N]
		....
		....
		....
														"

		[ HUNTER EXIT FROM FEAR EXAMPLE ]

	"	....
		....
		....
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY S]
		[HUNTER MOVE] [SPENGLER] has moved into [MASTER BEDROOM]
		[GHOST MOVE] Ghost has moved into [HALLWAY S]
		[GHOST MOVE] Ghost has moved into [BATHROOM]
		[GHOST MOVE] Ghost has moved into [HALLWAY S]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY N]
		*[VENKMAN] IS WAITING FOR BACKUP IN HALLWAY S
		[HUNTER MOVE] [SPENGLER] has moved into [VAN]
		*[VENKMAN] IS WAITING FOR BACKUP IN HALLWAY S
		[GHOST HAUNTING] Haunting the room [HALLWAY S]		<--- note the ghost haunts in same room
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY N]
		[HUNTER EXIT] [VENKMAN] exited because [AFRAID]		<--- note the hunter moved then exit
		....
		....
		....

														"
		[ HUNTERS BANISH GHOST EXAMPLE ]

	"	....
		....
		....
		[GHOST MOVE] Ghost has moved into [HALLWAY N]
		[HUNTER MOVE] [VENKMAN] has moved into [BOY'S BEDROOM]
		[HUNTER MOVE] [WINSTON] has moved into [HALLWAY S]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY N]
		[HUNTER MOVE] [RAY] has moved into [KITCHEN]
		[HUNTER MOVE] [WINSTON] has moved into [BATHROOM]
		[HUNTER MOVE] [SPENGLER] has moved into [GARAGE]
		[GHOST HAUNTING] Haunting the room [HALLWAY N]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY S]
		[HUNTER MOVE] [WINSTON] has moved into [HALLWAY S]
		[HUNTER MOVE] [RAY] has moved into [HALLWAY S]
		[HUNTER MOVE] [WINSTON] has moved into [KITCHEN]
		[HUNTER MOVE] [SPENGLER] has moved into [KITCHEN]
		[GHOST EVIDENCE] Left [SOUND] in [HALLWAY N]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY N]
		[HUNTER MOVE] [RAY] has moved into [HALLWAY N]
		[HUNTER MOVE] [WINSTON] has moved into [LIVING ROOM]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY S]
		[HUNTER MOVE] [SPENGLER] has moved into [GARAGE]
		[GHOST HAUNTING] Haunting the room [HALLWAY N]
		*[RAY] IS WAITING FOR BACKUP IN HALLWAY N
		[HUNTER MOVE] [SPENGLER] has moved into [KITCHEN]
		[HUNTER MOVE] [WINSTON] has moved into [KITCHEN]
		[HUNTER MOVE] [VENKMAN] has moved into [BATHROOM]
		*[RAY] IS WAITING FOR BACKUP IN HALLWAY N
		[GHOST HAUNTING] Haunting the room [HALLWAY N]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY S]
		[HUNTER MOVE] [SPENGLER] has moved into [HALLWAY S]
		[HUNTER MOVE] [WINSTON] has moved into [HALLWAY S]
		[HUNTER MOVE] [VENKMAN] has moved into [HALLWAY N]

		**** HUNTERS [RAY] AND [VENKMAN] WILL NOW ATTEMPT TO BANISH THE GHOST IN HALLWAY N ****
		**** HUNTERS [RAY] AND [VENKMAN] SUCCESSFULLY BANISHED THE GHOST MENACE CORRECTLY CALLING TO THE
		BULLIES ****

		[HUNTER MOVE] [SPENGLER] has moved into [BATHROOM]
		[GHOST EXIT] Exited because [BANISHED]
		[HUNTER MOVE] [WINSTON] has moved into [KITCHEN]
		[HUNTER EXIT] [VENKMAN] exited because [BANISHED]
		[HUNTER EXIT] [RAY] exited because [BANISHED]
		[HUNTER EXIT] [WINSTON] exited because [BANISHED]
		[HUNTER EXIT] [SPENGLER] exited because [BANISHED]
		....
		....
		....											"





		[ SIMULATION CONCLUSION HUNTERS WIN EXAMPLE ]

		"
		....
		....
		....

		 SIMULATION RESULTS
		 ---------------------------------------------------
		|                 AFTER ACTION REPORT               |
		 ---------------------------------------------------
		|			                            |
		| THE VICTOR:                  *** THE HUNTERS ***  |
		|						    |
		| *HUNTERS TOTAL ATTEMPTS TO BANISH: 	    1  	    |
		|						    |
		| *HUNTERS CONCLUDE GHOST IS:              BULLIES  |
		|						    |
		 ---------------------------------------------------

		 FINAL HUNTER STATS
		 ----------------------------------
		|     HUNTER   |   FEAR  | BOREDOM |
		 ----------------------------------
		|   SPENGLER   |    87   |     7   |
		|    VENKMAN   |    86   |     3   |
		|        RAY   |    89   |     0   |
		|    WINSTON   |    82   |    40   |
		 ----------------------------------

		 LAST JOURNAL ENTRIES
		 ---------------------------------------------------
		|    EVIDENCE   |    FOUNDER   |             ROOM   |
		 ---------------------------------------------------
		| FINGERPRINTS  |        RAY   |      LIVING ROOM   |
		|          EMF  |    VENKMAN   |    BOY'S BEDROOM   |
		|        SOUND  |    VENKMAN   |        HALLWAY S   |
		 ---------------------------------------------------
		....
		....
		....												"


		[ SIMULATION CONCLUSION GHOST WINS EXAMPLE ]

		"
		....
		....
		....
		[GHOST MOVE] Ghost has moved into [KITCHEN]
		[GHOST EXIT] Exited because [BORED]

		 SIMULATION RESULTS
		 ---------------------------------------------------
		|                 AFTER ACTION REPORT               |
		 ---------------------------------------------------
		|						    |
		| THE VICTOR:                  *** THE GHOST ***    |
		|						    |
		| *HUNTERS TOTAL ATTEMPTS TO BANISH: 		 0  |
		|						    |
		| *HUNTERS CONCLUDE GHOST IS:    AN UNKNOWN ENTITY  |
		|						    |
		 ---------------------------------------------------

		 FINAL HUNTER STATS
		 ----------------------------------
		|     HUNTER   |   FEAR  | BOREDOM |
		 ----------------------------------
		|   SPENGLER   |   100   |     0   |
		|    VENKMAN   |   100   |     0   |
		|        RAY   |   100   |     0   |
		|    WINSTON   |   100   |     0   |
		 ----------------------------------

		 LAST JOURNAL ENTRIES
		 ---------------------------------------------------
		|    EVIDENCE   |    FOUNDER   |             ROOM   |
		 ---------------------------------------------------
		|          EMF  |   SPENGLER   |          KITCHEN   |
		| FINGERPRINTS  |    WINSTON   |        HALLWAY N   |
		|  TEMPERATURE  |        RAY   |        HALLWAY N   |
		 ---------------------------------------------------
														"



Enjoy.
