#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
using namespace std;

/* ===================== CONSTANTS ===================== */
#define MAX_SCREENS 3
#define MAX_SEATS 10
#define MAX_QUEUE 10
#define MAX_STACK 20
#define HASH_SIZE 50

/* ===================== SCREEN ===================== */
int getValidatedInt(string prompt, int minVal, int maxVal);
struct Screen {
    int screenId;
    string movieName;
    int seats[MAX_SEATS];   // 0 = Free, 1 = Booked
};

Screen screens[MAX_SCREENS];

/* ===================== RESERVATION (LINKED LIST) ===================== */
struct Reservation {
    int bookingId;
    string customerName;
    int screenId;
    int seatNo;
    string seatType;
    int price;
    Reservation* next;
};

Reservation* head = NULL;
int bookingCounter = 1001;

/* ===================== PRINT TICKET ===================== */
void printTicket(Reservation* r) {
    cout << "\n========= TICKET =========\n";
    cout << "Booking ID : " << r->bookingId << endl;
    cout << "Name       : " << r->customerName << endl;
    cout << "Movie      : " << screens[r->screenId - 1].movieName << endl;
    cout << "Screen     : " << r->screenId << endl;
    cout << "Seat No    : " << r->seatNo << endl;
    cout << "Seat Type  : " << r->seatType << endl;
    cout << "Price      : " << r->price << " Birr\n";
    cout << "==========================\n";

    string filename = "ticket_" + to_string(r->bookingId) + ".txt";
    ofstream file(filename);
    if(file.is_open()) {
        file << "--------------------------------\n";
        file << "        MOVIE TICKET\n";
        file << "--------------------------------\n";
        file << "Booking ID : " << r->bookingId << endl;
        file << "Name       : " << r->customerName << endl;
        file << "Movie      : " << screens[r->screenId - 1].movieName << endl;
        file << "Screen     : " << r->screenId << endl;
        file << "Seat No    : " << r->seatNo << endl;
        file << "Seat Type  : " << r->seatType << endl;
        file << "Price      : " << r->price << " Birr\n";
        file << "--------------------------------\n";
        file << "Thank you for booking!\n";
        file << "--------------------------------\n";
        file.close();
        cout << "Ticket saved as " << filename << endl;
    } else {
        cout << "Error saving ticket to file.\n";
    }
}
/* ===================== HASHING ===================== */
struct HashNode {
    int bookingId;
    Reservation* reservation;
    HashNode* next;
};

HashNode* hashTable[HASH_SIZE] = {NULL};

int hashFunction(int key) {
    return key % HASH_SIZE;
}

void insertIntoHash(Reservation* r) {
    int index = hashFunction(r->bookingId);
    HashNode* node = new HashNode;
    node->bookingId = r->bookingId;
    node->reservation = r;
    node->next = hashTable[index];
    hashTable[index] = node;
}

void searchBookingById() {
    int id = getValidatedInt("Enter Booking ID: ", 1000, 99999);
    int index = hashFunction(id);

    HashNode* temp = hashTable[index];
    while(temp != NULL) {
        if(temp->bookingId == id) {
            printTicket(temp->reservation);
            return;
        }
        temp = temp->next;
    }

    cout << "Booking not found.\n";
}
/* ===================== QUEUE ===================== */
struct Queue {
    string names[MAX_QUEUE];
    int front = 0;
    int rear = -1;
} waitingQueue;

/* ===================== STACK ===================== */
struct Stack {
    int seatNo[MAX_STACK];
    int screenId[MAX_STACK];
    int top = -1;
} undoStack;

/* ===================== GRAPH ===================== */
int graph[MAX_SCREENS][MAX_SCREENS] = {
    {0,1,1},
    {1,0,1},
    {1,1,0}
};

/* ===================== INITIALIZE SCREENS ===================== */
void initializeScreens() {
    string movieNames[MAX_SCREENS] = {
        "Avengers: Endgame",
        "The Batman",
        "Frozen II"
    };
    for(int i = 0; i < MAX_SCREENS; i++) {
        screens[i].screenId = i + 1;
        screens[i].movieName = movieNames[i];
        for(int j = 0; j < MAX_SEATS; j++)
            screens[i].seats[j] = 0;
    }
}

/* ===================== INPUT VALIDATION ===================== */
int getValidatedInt(string prompt, int minVal, int maxVal) {
    int val;
    while(true) {
        cout << prompt;
        cin >> val;
        if(cin.fail() || val < minVal || val > maxVal) {
            cout << "Invalid input. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
    }
}

/* ===================== SHOW AVAILABLE MOVIES ===================== */
void showAvailableMovies() {
    cout << "\n===== Available Movies =====\n";
    for(int i = 0; i < MAX_SCREENS; i++) {
        cout << "Screen " << screens[i].screenId << ": " << screens[i].movieName << endl;
    }
}

/* ===================== GRAPHICAL SEAT LAYOUT ===================== */
void showSeatsGraphical(int screenId) {
    cout << "\nScreen " << screenId << ": " << screens[screenId - 1].movieName << endl;
    cout << "Seats Layout (O = Free, X = Booked):\n";

    for(int i = 0; i < MAX_SEATS; i++) {
        cout << "[" << i+1 << "] ";
        if(screens[screenId - 1].seats[i] == 0)
            cout << "O  "; // Free seat
        else
            cout << "X  "; // Booked seat

        if((i+1) % 5 == 0) cout << endl; // 5 seats per row
    }
    cout << endl;
}

/* ===================== ADD RESERVATION ===================== */
Reservation* addReservation(string name, int screenId, int seatNo, string seatType, int price, bool saveToFile=true) {
    Reservation* temp = new Reservation;
    temp->bookingId = bookingCounter++;
    temp->customerName = name;
    temp->screenId = screenId;
    temp->seatNo = seatNo;
    temp->seatType = seatType;
    temp->price = price;
    temp->next = head;
    head = temp;
    // Save to file
    if(saveToFile) {
        ofstream file("reservations.txt", ios::app);
        if(file.is_open()) {
            file << temp->bookingId << "," << temp->customerName << "," << temp->screenId << ","
                 << temp->seatNo << "," << temp->seatType << "," << temp->price << "\n";
            file.close();
        }
    }

    return temp;
}



/* ===================== LOAD RESERVATIONS ===================== */
void loadReservations() {
    ifstream file("reservations.txt");
    string line;
    int maxId = 1000;
    while(getline(file, line)) {
        stringstream ss(line);
        string token;
        Reservation* temp = new Reservation;
        getline(ss, token, ','); temp->bookingId = stoi(token);
        getline(ss, temp->customerName, ',');
        getline(ss, token, ','); temp->screenId = stoi(token);
        getline(ss, token, ','); temp->seatNo = stoi(token);
        getline(ss, temp->seatType, ',');
        getline(ss, token, ','); temp->price = stoi(token);
        temp->next = head;
        head = temp;
insertIntoHash(temp);
        // Mark seat as booked
        screens[temp->screenId - 1].seats[temp->seatNo - 1] = 1;

        if(temp->bookingId > maxId) maxId = temp->bookingId;
    }
    bookingCounter = maxId + 1;
    file.close();
}

/* ===================== BOOK MULTIPLE SEATS ===================== */
void bookSeat() {
    string name, seatType;
    int screenId, choice, price, seatCount;

    cout << "\nEnter customer name: ";
    getline(cin, name);

    showAvailableMovies();
    screenId = getValidatedInt("Enter screen number: ", 1, MAX_SCREENS);

    showSeatsGraphical(screenId); // <-- graphical layout

    choice = getValidatedInt("Seat Type (1.Normal 100, 2.VIP 200): ", 1, 2);
    seatType = (choice == 2) ? "VIP" : "Normal";
    price = (choice == 2) ? 200 : 100;

    seatCount = getValidatedInt("How many seats do you want to book? ", 1, MAX_SEATS);

    Reservation* bookedSeats[MAX_SEATS];
    int bookedIndex = 0;

    for(int i = 0; i < seatCount; i++) {
        int seatNo = getValidatedInt("Enter seat number: ", 1, MAX_SEATS);
        if(screens[screenId - 1].seats[seatNo - 1] == 1) {
            cout << "Seat already booked. Choose another.\n";
            i--;
            continue;
        }

        screens[screenId - 1].seats[seatNo - 1] = 1;
        Reservation* r = addReservation(name, screenId, seatNo, seatType, price);
        bookedSeats[bookedIndex++] = r;

        undoStack.seatNo[++undoStack.top] = seatNo;
        undoStack.screenId[undoStack.top] = screenId;
    }

    cout << "\nBooking successful!\n";
    int printOpt = getValidatedInt("Print all tickets now? (1.Yes 0.No): ", 0, 1);
    if(printOpt == 1) {
        for(int i = 0; i < bookedIndex; i++)
            printTicket(bookedSeats[i]);
    }
}
/* ===================== CANCEL BOOKING ===================== */
void cancelBooking() {
    string name;
    int screenId, seatNo;
    bool found = false;

    cout << "\nEnter customer name: ";
    getline(cin, name);

    screenId = getValidatedInt("Enter screen number: ", 1, MAX_SCREENS);
    seatNo = getValidatedInt("Enter seat number: ", 1, MAX_SEATS);

    // Search for reservation
    Reservation* temp = head;
    Reservation* prev = NULL;

    while(temp) {
        if(temp->customerName == name &&
           temp->screenId == screenId &&
           temp->seatNo == seatNo) {
            found = true;
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    if(!found) {
        cout << "No reservation found for this customer on this seat.\n";
        return;
    }

    // Free the seat
    screens[screenId - 1].seats[seatNo - 1] = 0;

    // Remove reservation from linked list
    if(prev == NULL) head = temp->next;
    else prev->next = temp->next;
    delete temp;

    // Push to undo stack
    undoStack.seatNo[++undoStack.top] = seatNo;
    undoStack.screenId[undoStack.top] = screenId;

    // Check waiting list
    if(waitingQueue.front <= waitingQueue.rear) {
        string waitName = waitingQueue.names[waitingQueue.front++];
        screens[screenId - 1].seats[seatNo - 1] = 1;
        addReservation(waitName, screenId, seatNo, "Normal", 100);
        cout << "Waiting customer booked automatically.\n";
    } else {
        cout << "Booking cancelled successfully.\n";
    }
}


/* ===================== BOOKING SUMMARY ===================== */
void bookingSummary() {
    string name;
    cout << "\nEnter customer name: ";
    getline(cin, name);

    Reservation* temp = head;
    bool found = false;

    cout << "\n===== BOOKING SUMMARY =====\n";
    while(temp) {
        if(temp->customerName == name) {
            printTicket(temp);
            found = true;
        }
        temp = temp->next;
    }

    if(!found)
        cout << "No booking found for this customer.\n";
}

/* ===================== MERGE SORT ===================== */
Reservation* merge(Reservation* a, Reservation* b) {
    if(!a) return b;
    if(!b) return a;
    if(a->bookingId <= b->bookingId) {
        a->next = merge(a->next, b);
        return a;
    } else {
        b->next = merge(a, b->next);
        return b;
    }
}

void split(Reservation* source, Reservation** front, Reservation** back) {
    Reservation* slow = source;
    Reservation* fast = source->next;
    while(fast) {
        fast = fast->next;
        if(fast) { slow = slow->next; fast = fast->next; }
    }
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

void mergeSort(Reservation** headRef) {
    if(!*headRef || !(*headRef)->next) return;
    Reservation *a, *b;
    split(*headRef, &a, &b);
    mergeSort(&a);
    mergeSort(&b);
    *headRef = merge(a, b);
}

/* ===================== ADMIN DISPLAY ===================== */
void displayAllReservationsAdmin() {
    Reservation* temp = head;
    cout << "\n--- ALL RESERVATIONS ---\n";
    while(temp) {
        cout << "ID: " << temp->bookingId
             << " Name: " << temp->customerName
             << " Screen: " << temp->screenId
             << " Seat: " << temp->seatNo
             << " Type: " << temp->seatType
             << " Price: " << temp->price << " Birr\n";
        temp = temp->next;
    }
}

/* ===================== UPDATE MOVIES ===================== */
void updateMovies() {
    cout << "\n--- Update Movies on Screens ---\n";
    for(int i = 0; i < MAX_SCREENS; i++) {
        cout << "Screen " << screens[i].screenId
             << " current movie: " << screens[i].movieName << endl;
        cout << "Enter new movie name: ";
        string newMovie;
        getline(cin, newMovie);
        if(!newMovie.empty()) screens[i].movieName = newMovie;
    }
    cout << "Movies updated successfully!\n";
}

/* ===================== SHOW GRAPH ===================== */
void showGraph() {
    cout << "\nScreen Connectivity Graph\n";
    for(int i = 0; i < MAX_SCREENS; i++) {
        for(int j = 0; j < MAX_SCREENS; j++)
            cout << graph[i][j] << " ";
        cout << endl;
    }
}
void showTotalIncome() {
     cout << "\nTotal Income\n";
    int total = 0;
    Reservation* temp = head;

    while(temp != NULL) {
        total += temp->price;
        temp = temp->next;
    }

    cout << "\n===== TOTAL INCOME =====\n";
    cout << "Total Revenue: " << total << " Birr\n";
}
/* ===================== MAIN ===================== */
int main() {
    initializeScreens();
    loadReservations(); // Load previous bookings

    int role, choice;
    cout << "Welcome to Theater Reservation System\n";
    role = getValidatedInt("1.Customer\n2.Admin\nChoice: ", 1, 3);

    if(role == 1) {
        do {
            cout << "\n1.Book Seat\n2.Booking Summary\n3.Cancel Reservation\n0.Exit\n";
            choice = getValidatedInt("Choice: ", 0, 3);
            if(choice == 1) bookSeat();
            if(choice == 2) bookingSummary();
            if(choice == 3) cancelBooking();
        } while(choice != 0);
    } else {
        string pass;
        cout << "Enter admin password: ";
        getline(cin, pass);
        if(pass != "1234") return 0;

        do {
            cout << "\n1.Sort Reservations\n2.Display Reservations\n3.Show Graph\n4.Update Movies\n5.Total Income\n6.Search Reservation\n0.Exit\n";
            choice = getValidatedInt("Choice: ", 0, 6);
            if(choice == 1) { mergeSort(&head); cout << "Sorted using Merge Sort.\n"; }
            if(choice == 2) displayAllReservationsAdmin();
            if(choice == 3) showGraph();
            if(choice == 4) updateMovies();
            if(choice == 5) showTotalIncome();
            if(choice == 6) searchBookingById();
        } while(choice != 0);
    }
    return 0;
}
