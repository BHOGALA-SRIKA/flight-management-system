# flight-management-system
Implements a queue-based flight scheduling system combined with a directed 
graph to manage connecting routes and delays.Follows a multi-file modular 
design with proper dynamic memory handling and no global variables.



Technical Implementation

This project moves beyond basic programming by integrating three core computer science pillars:

Graph Theory: Uses an Adjacency List to represent airports as nodes and flights as directed edges. Implements Depth First Search (DFS) to find all available connecting routes between two cities.

Linear Data Structures: Implements a Linked-List based Queue to manage flight records dynamically, ensuring efficient O(1) insertion.

Data Persistence: Features a custom File I/O engine that serializes flight data into a pipe-delimited (|) format, allowing data to persist across sessions



Key Features
Multi-Role Access: Separate interfaces for Users (View & Search) and Employees (Admin control with secure login).

Route Discovery: Automatically builds a flight graph and searches for multi-leg journeys.

Real-time Delay Tracking: Calculates actual arrival times based on scheduled data and live delay inputs.

Automated Cleanup: Logic to remove "expired" flights based on the current system time.

Memory Management: Clean implementation with zero global variables and full dynamic memory deallocation (free).



Project Structure

main.c              → Main program (menus, user/employee modes)

flight.c            → Queue and file operations

flight.h            → Flight structure & function declarations

graph.c             → Graph implementation (connecting routes)

graph.h             → Graph structure & function declarations

flight_all.txt      → Stores all flight records (auto-updated)

admin_login.txt     → Stores employee login credentials



Prerequisites

GCC Compiler (MinGW for Windows, or native GCC for Linux/Mac).

Compilation

Run the following command in your terminal:
Bash
gcc main.c flight.c graph.c -o flight_system

Run the program using the following command
./flight_system



Default Admin Credentials: ID: abcd | Password: 1234



Sample Output:

==== Flight Management System ====

U = User | E = Employee | Q = Quit

Enter role: U

--- User Menu ---
1. Find connecting flights
2. Display flights by departure
3. Track flight delays
4. Exit User Menu
Choice: 1
Enter Source: Bengaluru

Enter Destination: Chennai

Searching routes from Bengaluru to Chennai ...

Route: Bengaluru -> Kolkata -> Delhi -> Mumbai -> Goa -> Chennai

   Flights: AI606 (Dep:1200 Arr:1430 Delay:0) -> AI607 (Dep:1600 Arr:1900 Delay:30) -> AI101 (Dep:1030 Arr:1215 Delay:15) -> AI203 (Dep:1330 Arr:1510 Delay:10) -> AI603 (Dep:1900 Arr:2050 Delay:10)
   
   Total delay across route: 65 minutes
   
Route: Bengaluru -> Goa -> Chennai

   Flights: AI602 (Dep:1700 Arr:1830 Delay:5) -> AI603 (Dep:1900 Arr:2050 Delay:10)
   
   Total delay across route: 15 minutes


This is a robust, terminal-based application designed to manage airline schedules, route optimization, and passenger logistics using advanced C programming concepts.
