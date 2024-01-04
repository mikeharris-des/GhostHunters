#include "defs.h"

/* 
    Function: populateBuilding
    Purpose: Populates a building with rooms and doors
    Parameters: 
        in-out: BuildingType* building: The building to populate
    Returns: void
*/
void populateBuilding(BuildingType* building) {
    /*
        * Requires that the building has been initialized with initBuilding()
        * Requires that initRoom and connectRooms have been implemented
        * You are allowed to modify this function, but the room connections
        * must remain the same. The overall map MUST be the same, with the same names
        * and the same connections. Failure to do so will result in major
        * deductions.
    */

    // Room names array
    char* room_names[] = {
        "VAN", "HALLWAY N", "HALLWAY S", "MASTER BEDROOM", "BOY'S BEDROOM", "BATHROOM",
        "KITCHEN", "LIVING ROOM", "GARAGE", "UTILITY ROOM"
    };
    int num_rooms = sizeof(room_names) / sizeof(room_names[0]);

    // Initialize rooms: It should be strcpy/strlcpy/strncpy the name
    for (int i = 0; i < num_rooms; i++) {
        initRoom(&(building->rooms[i]), room_names[i]);
    }

    // Connect rooms. The first room, in the direction, you will find the second room.
    // connectRooms() returns a pointer to the door that connects the two rooms.
    building->doors[0] = connectRooms(building->rooms[0], SOUTH, building->rooms[1]);
    building->doors[1] = connectRooms(building->rooms[1], EAST, building->rooms[4]);
    building->doors[2] = connectRooms(building->rooms[1], WEST, building->rooms[3]);
    building->doors[3] = connectRooms(building->rooms[1], SOUTH, building->rooms[2]);
    building->doors[4] = connectRooms(building->rooms[2], EAST, building->rooms[5]);
    building->doors[5] = connectRooms(building->rooms[2], SOUTH, building->rooms[6]);
    building->doors[6] = connectRooms(building->rooms[6], SOUTH, building->rooms[7]);
    building->doors[7] = connectRooms(building->rooms[6], WEST, building->rooms[8]);
    building->doors[8] = connectRooms(building->rooms[8], NORTH, building->rooms[9]);

    building->roomCount = num_rooms;
    building->doorCount = 9;
}
