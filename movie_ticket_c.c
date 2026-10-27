#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// DEFINE CONSTANTS FOR SYSTEM
#define MAX_EVENTS 3        
#define MAX_USERS 50        
#define MAX_BOOKINGS 50    
#define ROWS 5              
#define COLS 5             
#define MAX_SEATS 10        

// STRUCTURE TO STORE EVENT INFORMATION
typedef struct 
{
    char name[50];          
    char time[20];         
    float price;           
    char seats[ROWS][COLS]; 
} Event;

// STRUCTURE TO STORE USER INFORMATION
typedef struct 
{
    char username[50];     
    char password[50];     
} User;

// STRUCTURE TO STORE BOOKING INFORMATION
typedef struct 
{
    char username[50];     
    char event[50];        
    char time[20];         
    float total;           
    int seats;             
    char seat_numbers[MAX_SEATS][5]; 
} Booking;

// DIFFERENT ARRAYS USED IN PROGRAM
Event events[MAX_EVENTS];  
User users[MAX_USERS];     
Booking bookings[MAX_BOOKINGS]; 
int user_count = 0;        
int booking_count = 0;     

// ALL FUNCTIONS USED IN PROJECT
void init_system();        
void save_users();         
void load_users();         
void save_bookings();      
void load_bookings();      
void register_user();      
int login(char *current_user); 
void show_events();        
void show_seats(char *event_name); 
void book_ticket(char *username); 
void view_bookings(char *username); 
int is_seat_available(char *event_name, char *seat); 
void reserve_seats(char *event_name, char seats[][5], int count); 
void print_table_header(FILE *file); 

int main() {
    init_system();          // Initialize system data
    char current_user[50] = ""; 
    int choice;
    
//LOOP FOR MAIN PROGRAM CONSIST OF DIFFERENT OPTIONS 
    while(1) {
        printf("\nTicket Booking System\n");
        printf("1. Register\n2. Login\n3. Book Ticket\n4. View Bookings\n5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch(choice)
		{
            case 1: 
                register_user(); 
                break;
            case 2: 
                if(login(current_user)) 
                    printf("Welcome, %s!\n", current_user); 
                break;
            case 3: 
                if(strlen(current_user) == 0) printf("Please login first.\n");
                else book_ticket(current_user);
                break;
            case 4: 
                if(strlen(current_user) == 0) printf("Please login first.\n");
                else view_bookings(current_user);
                break;
            case 5: 
                save_users();     
                save_bookings();
                printf("Goodbye!\n");
                return 0;
            default: 
                printf("Invalid choice!\n");
        }
    }
}

// PRINT fORMATTED TABLE FOR BOOKING DISPLAY TO MAKE IT MORE ATTRACTIVE AND CLEAR
void print_table_header(FILE *file) 
{
    fprintf(file, "+----------------------+----------------------+----------------+--------+---------------------+-----------------+\n");
    fprintf(file, "| %-20s | %-20s | %-14s | %-6s | %-19s | %-15s |\n", 
           "Username", "Event", "Time", "Price", "Seats", "Total");
    fprintf(file, "+----------------------+----------------------+----------------+--------+---------------------+-----------------+\n");
}

// INITIALIZING DIFFERENT EVENTS AND LOAD SAVED DATA
void init_system()
{
    strcpy(events[0].name, "Chhichhore");
    strcpy(events[0].time, "10:00 AM");
    events[0].price = 500.0;
    memset(events[0].seats, 'A', sizeof(events[0].seats)); 
    
    strcpy(events[1].name, "Jab We Met");
    strcpy(events[1].time, "02:30 PM");
    events[1].price = 500.0;
    memset(events[1].seats, 'A', sizeof(events[1].seats));
    
    strcpy(events[2].name, "Student of the Year");
    strcpy(events[2].time, "06:45 PM");
    events[2].price = 500.0;
    memset(events[2].seats, 'A', sizeof(events[2].seats));
    
    load_users();
    load_bookings();
}

// SAVE USER DATA TO FILE
void save_users() 
{
    FILE *file = fopen("users.txt", "w");
    if(file) 
	{
        int i;
        for(i = 0; i < user_count; i++) 
		{
            fprintf(file, "%s,%s\n", users[i].username, users[i].password);
        }
        fclose(file);
    }
}

// LOAD USER DATA FROM FILE
void load_users() 
{
    FILE *file = fopen("users.txt", "r");
    if(file) 
	{
        char line[100];
        while(fgets(line, sizeof(line), file) && user_count < MAX_USERS)
		{
            char *token = strtok(line, ",");
            if(token) strcpy(users[user_count].username, token);
            token = strtok(NULL, "\n");
            if(token) strcpy(users[user_count].password, token);
            user_count++;
        }
        fclose(file);
    }
}

// SAVE BOOKING DATA TO FILE IN TABLE FORMAT
void save_bookings() 
{
    FILE *file = fopen("bookings.txt", "w");
    if(file)
	{
        print_table_header(file); 
        int i,j;
        for(i = 0; i < booking_count; i++)
		{
            char all_seats[100] = "";
            for(j = 0; j < bookings[i].seats; j++) 
			{
                strcat(all_seats, bookings[i].seat_numbers[j]);
                if(j < bookings[i].seats - 1) strcat(all_seats, ", ");
            }
            fprintf(file, "| %-20s | %-20s | %-14s | %6.2f | %-19s | %15.2f |\n",
                   bookings[i].username,
                   bookings[i].event,
                   bookings[i].time,
                   bookings[i].total/bookings[i].seats, // Price per seat
                   all_seats,
                   bookings[i].total);
        }
        if(booking_count > 0) 
		{
            fprintf(file, "+----------------------+----------------------+----------------+--------+---------------------+-----------------+\n");
        }
        fclose(file);
    }
}

// LOAD BOOKING DATA FROM FILE
void load_bookings() 
{
    FILE *file = fopen("bookings.txt", "r");
    if(file) 
	{
        char line[256];
        while(fgets(line, sizeof(line), file)) 
		{
            if(line[0] == '|' && line[1] == ' ')
			{
                char username[50], event[50], time[20], seats_str[100];
                float price, total;
                sscanf(line, "| %49[^|] | %49[^|] | %19[^|] | %f | %99[^|] | %f |",
                      username, event, time, &price, seats_str, &total);
                
                username[strcspn(username, " ")] = '\0';
                event[strcspn(event, " ")] = '\0';
                time[strcspn(time, " ")] = '\0';
                
                strcpy(bookings[booking_count].username, username);
                strcpy(bookings[booking_count].event, event);
                strcpy(bookings[booking_count].time, time);
                bookings[booking_count].total = total;
                
                char *token = strtok(seats_str, ",");
                int seat_count = 0;
                while(token != NULL && seat_count < MAX_SEATS)
				{
                    while(*token == ' ') token++; 
                    strcpy(bookings[booking_count].seat_numbers[seat_count], token);
                    seat_count++;
                    token = strtok(NULL, ",");
                }
                bookings[booking_count].seats = seat_count;
                
                booking_count++;
            }
        }
        fclose(file);
    }
}

//FUNCTION TO REGISTER USER
void register_user() 
{
    if(user_count >= MAX_USERS) 
	{
        printf("Max users reached!\n");
        return;
    }
    
    printf("Username: ");
    fgets(users[user_count].username, 50, stdin);
    users[user_count].username[strcspn(users[user_count].username, "\n")] = '\0';
    
    int i;
    for(i = 0; i < user_count; i++) 
	{
        if(strcmp(users[i].username, users[user_count].username) == 0) 
		{
            printf("Username taken!\n");
            return;
        }
    }
    
    printf("Password: ");
    fgets(users[user_count].password, 50, stdin);
    users[user_count].password[strcspn(users[user_count].password, "\n")] = '\0';
    
    user_count++;
    save_users(); 
    printf("Registration successful!\n");
}

// FUNCTION FOR USER TO LOGIN 
int login(char *current_user)
{
    char username[50], password[50];
    
    printf("Username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = '\0';
    
    printf("Password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = '\0';
    
    int i;
    for(i = 0; i < user_count; i++)
	{
        if(strcmp(users[i].username, username) == 0 && 
           strcmp(users[i].password, password) == 0) {
            strcpy(current_user, username); 
            return 1; 
        }
    }
    
    printf("Invalid credentials!\n");
    return 0; 
}

//FUNCTION TO DISPLAY ALL AVAILABLE EVENTS 
void show_events()
{
    printf("\nAvailable Events:\n");
    printf("ID\tName\t\t\tTime\tPrice\n");
    int i;
    for(i = 0; i < MAX_EVENTS; i++) 
	{
        printf("%d\t%-20s\t%s\t%.2f\n", 
              i+1, events[i].name, events[i].time, events[i].price);
    }
}

// FUNCTION TO DISPLAY SEAT MAP FOR AN PARTICULAR EVENT
void show_seats(char *event_name) 
{
    int event_index = -1;
    int i;
    for(i = 0; i < MAX_EVENTS; i++)
	{
        if(strcmp(events[i].name, event_name) == 0) {
            event_index = i;
            break;
        }
    }
    
    if(event_index == -1) return; 
    
    printf("\nSeat Map (%s):\n", event_name);
    printf("  ");
    int col,row;
    for(col = 1; col <= COLS; col++) 
	{
        printf("%2d ", col);
    }
    printf("\n");
    
    for(row = 0; row < ROWS; row++) 
	{
        printf("%c ", 'A'+row); 
        for(col = 0; col < COLS; col++) 
		{
            printf(" %c ", events[event_index].seats[row][col]);
        }
        printf("\n");
    }
    printf("A = Available, X = Booked\n");
}

// FUNCTION TO BOOK TICKETS FOR AN EVENT 
void book_ticket(char *username)
{
    int event_id, seat_count;
    char seat_numbers[MAX_SEATS][5];
    
    show_events(); 
    printf("Select event (1-%d): ", MAX_EVENTS);
    scanf("%d", &event_id);
    getchar();
    
    if(event_id < 1 || event_id > MAX_EVENTS) 
	{
        printf("Invalid event!\n");
        return;
    }
    
    show_seats(events[event_id-1].name); 
    printf("Number of seats (max %d): ", MAX_SEATS);
    scanf("%d", &seat_count);
    getchar();
    
    if(seat_count < 1 || seat_count > MAX_SEATS) 
	{
        printf("Invalid seat count!\n");
        return;
    }
    
    printf("Enter seat numbers (e.g., A1 B2): ");
    int i;
    for(i = 0; i < seat_count; i++) 
	{
        scanf("%4s", seat_numbers[i]);
        getchar();
        
        if(strlen(seat_numbers[i]) != 2 || 
           seat_numbers[i][0] < 'A' || seat_numbers[i][0] > 'A'+ROWS-1 ||
           seat_numbers[i][1] < '1' || seat_numbers[i][1] > '0'+COLS) 
		   {
            printf("Invalid seat: %s\n", seat_numbers[i]);
            return;
           }
        
        if(!is_seat_available(events[event_id-1].name, seat_numbers[i])) 
		{
            printf("Seat %s is already booked!\n", seat_numbers[i]);
            return;
        }
    }
    
    // CREATE NEW BOOKING RECORD
    strcpy(bookings[booking_count].username, username);
    strcpy(bookings[booking_count].event, events[event_id-1].name);
    strcpy(bookings[booking_count].time, events[event_id-1].time);
    bookings[booking_count].total = seat_count * events[event_id-1].price;
    bookings[booking_count].seats = seat_count;
    
    for(i = 0; i < seat_count; i++) 
	{
        strcpy(bookings[booking_count].seat_numbers[i], seat_numbers[i]);
    }
    booking_count++;
    
    reserve_seats(events[event_id-1].name, seat_numbers, seat_count);
    
    save_bookings(); 
    printf("Booking confirmed! Total: %.2f\n", bookings[booking_count-1].total);
}

// FUNCTION TO CHECK IF SEAT IS AVAILABLE OR NOT 
int is_seat_available(char *event_name, char *seat)
{
    int event_index = -1;
    int i;
    for(i = 0; i < MAX_EVENTS; i++)
	{
        if(strcmp(events[i].name, event_name) == 0) 
		{
            event_index = i;
            break;
        }
    }
    
    if(event_index == -1) return 0; 
    
    int row = seat[0] - 'A';
    int col = seat[1] - '1';
    
    
    if(row < 0 || row >= ROWS || col < 0 || col >= COLS) return 0;
    
    return events[event_index].seats[row][col] == 'A';
}

// FUNCTION FOR RESERVED SEATS 
void reserve_seats(char *event_name, char seats[][5], int count) 
{
    int event_index = -1;
    int i;
    for(i = 0; i < MAX_EVENTS; i++) 
	{
        if(strcmp(events[i].name, event_name) == 0) 
		{
            event_index = i;
            break;
        }
    }
    
    if(event_index == -1) return; 
    
    for(i = 0; i < count; i++) 
	{
        int row = seats[i][0] - 'A';
        int col = seats[i][1] - '1';
        if(row >= 0 && row < ROWS && col >= 0 && col < COLS) 
		{
            events[event_index].seats[row][col] = 'X';
        }
    }
}

// FUNCTION TO ALL BOOKINGS THAT ARE MADE BY THE USER
void view_bookings(char *username) 
{
    printf("\nYour Bookings:\n");
    print_table_header(stdout); 
    
    int found = 0;
    int i,j;
    for(i = 0; i < booking_count; i++) 
	{
        if(strcmp(bookings[i].username, username) == 0) 
		{
            found = 1;
            
            char all_seats[100] = "";
            for(j = 0; j < bookings[i].seats; j++) 
			{
                strcat(all_seats, bookings[i].seat_numbers[j]);
                if(j < bookings[i].seats - 1) strcat(all_seats, ", ");
            }
            
            printf("| %-20s | %-20s | %-14s | %6.2f | %-19s | %15.2f |\n",
                  bookings[i].username,
                  bookings[i].event,
                  bookings[i].time,
                  bookings[i].total/bookings[i].seats,
                  all_seats,
                  bookings[i].total);
        }
    }
    
// PRINT APPROPRIATE FOOTER BASED ON WHETHER BOOKINGS WERE FOUND
    if(found) 
	{
        printf("+----------------------+----------------------+----------------+--------+---------------------+-----------------+\n");
    }
	else 
	{
        printf("| No bookings found.                                                                                         |\n");
        printf("+------------------------------------------------------------------------------------------------------------+\n");
    }
}
