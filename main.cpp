

/*
 * ============================================================
 *  PROJECT : Retail Micro-Payment Settlement Ledger
 *  PURPOSE : Applied DSA-I Demonstration (System Simulation)
 * ============================================================
 *
 *  DSA Concepts Used:
 *    1. Arrays              – Merchant storage
 *    2. Linked List          – Transaction history
 *    3. Stack (LIFO)         – Undo last transaction
 *    4. Queue (FIFO)         – Process payments in order
 *    5. Linear Search        – Search merchant by ID
 *    6. Binary Search        – Search merchant by ID (sorted)
 *    7. Bubble Sort          – Sort merchants by total amount
 *    8. Graph (Adj. Matrix)  – Bank-to-bank settlement network
 *    9. Dijkstra's Algorithm – Find cheapest transfer path
 *
 *  All data is stored in memory only. No files, no database.
 * ============================================================
 */

#include <iostream>
#include <cstring>   // for strcpy, strcmp
#include <climits>   // for INT_MAX
using namespace std;

// ============================================================
//  CONSTANTS
// ============================================================
const int MAX_MERCHANTS = 100;   // Max merchants (Array size)
const int MAX_BANKS     = 10;    // Max banks in the graph

// ============================================================
//  STRUCT: Merchant
//  Stored in an ARRAY (demonstrates Array usage)
// ============================================================
struct Merchant {
    int    id;
    char   name[50];
    double totalAmount;  // Running total of transactions
};

// ============================================================
//  STRUCT: TransactionNode (Linked List Node)
//  Linked List stores full transaction history
//  Time Complexity (Insert at head): O(1)
// ============================================================
struct TransactionNode {
    int    transactionId;
    int    merchantId;
    double amount;
    char   description[100];
    TransactionNode* next;
};

// ============================================================
//  STRUCT: StackNode (Stack using Linked List – LIFO)
//  Used for UNDO of the last transaction
//  Push: O(1), Pop: O(1)
// ============================================================
struct StackNode {
    int    transactionId;
    int    merchantId;
    double amount;
    char   description[100];
    StackNode* next;
};

// ============================================================
//  STRUCT: QueueNode (Queue using Linked List – FIFO)
//  Used for processing payments in order
//  Enqueue: O(1), Dequeue: O(1)
// ============================================================
struct QueueNode {
    int    transactionId;
    int    merchantId;
    double amount;
    QueueNode* next;
};

// ============================================================
//  GLOBAL DATA (All in-memory)
// ============================================================

// --- Array of Merchants ---
Merchant merchants[MAX_MERCHANTS];
int merchantCount = 0;

// --- Linked List head for Transaction History ---
TransactionNode* transactionHead = NULL;
int nextTransactionId = 1;

// --- Stack top pointer for Undo ---
StackNode* stackTop = NULL;

// --- Queue front and rear pointers for Payment Processing ---
QueueNode* queueFront = NULL;
QueueNode* queueRear  = NULL;

// --- Graph: Adjacency Matrix for Bank Network ---
//     graph[i][j] = cost of transfer from bank i to bank j
//     0 means no direct connection
int graph[MAX_BANKS][MAX_BANKS];
char bankNames[MAX_BANKS][30];
int bankCount = 0;

// ============================================================
//  FUNCTION DECLARATIONS
// ============================================================
void addMerchant();
void displayMerchants();
void addTransaction();
void viewTransactionHistory();
void processPayments();
void undoLastTransaction();
void searchMerchantById();
void sortMerchantsByAmount();
void simulateBankNetwork();
void findCheapestPath();

// Helper functions
void pushToStack(int txnId, int mId, double amt, const char* desc);
void popFromStack(int &txnId, int &mId, double &amt, char* desc);
bool isStackEmpty();
void enqueue(int txnId, int mId, double amt);
void dequeue(int &txnId, int &mId, double &amt);
bool isQueueEmpty();
int  linearSearch(int id);
int  binarySearch(int id);
void bubbleSort();

// ============================================================
//  MAIN FUNCTION – Menu-Driven Program
// ============================================================
int main() {
    int choice;

    // Initialize graph to 0 (no connections)
    for (int i = 0; i < MAX_BANKS; i++)
        for (int j = 0; j < MAX_BANKS; j++)
            graph[i][j] = 0;

    cout << "=============================================" << endl;
    cout << "  RETAIL MICRO-PAYMENT SETTLEMENT LEDGER"      << endl;
    cout << "  (DSA-I System Simulation)"                    << endl;
    cout << "=============================================" << endl;

    do {
        cout << "\n---------- MAIN MENU ----------" << endl;
        cout << " 1.  Add Merchant"                  << endl;
        cout << " 2.  Display Merchants"             << endl;
        cout << " 3.  Add Transaction"               << endl;
        cout << " 4.  View Transaction History"      << endl;
        cout << " 5.  Process Payments (Queue)"      << endl;
        cout << " 6.  Undo Last Transaction (Stack)" << endl;
        cout << " 7.  Search Merchant by ID"         << endl;
        cout << " 8.  Sort Merchants by Amount"      << endl;
        cout << " 9.  Simulate Bank Network (Graph)" << endl;
        cout << "10.  Find Cheapest Transfer Path"   << endl;
        cout << "11.  Exit"                          << endl;
        cout << "-------------------------------"    << endl;
        cout << "Enter your choice: ";
        cin  >> choice;

        // Handle invalid input (non-integer)
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[ERROR] Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:  addMerchant();          break;
            case 2:  displayMerchants();     break;
            case 3:  addTransaction();       break;
            case 4:  viewTransactionHistory(); break;
            case 5:  processPayments();      break;
            case 6:  undoLastTransaction();  break;
            case 7:  searchMerchantById();   break;
            case 8:  sortMerchantsByAmount(); break;
            case 9:  simulateBankNetwork();  break;
            case 10: findCheapestPath();     break;
            case 11:
                cout << "\nExiting... Thank you!\n";
                break;
            default:
                cout << "[ERROR] Invalid choice. Try again.\n";
        }
    } while (choice != 11);

    // ----- CLEANUP: Free dynamically allocated memory -----

    // Free transaction linked list
    while (transactionHead != NULL) {
        TransactionNode* temp = transactionHead;
        transactionHead = transactionHead->next;
        delete temp;
    }

    // Free stack
    while (stackTop != NULL) {
        StackNode* temp = stackTop;
        stackTop = stackTop->next;
        delete temp;
    }

    // Free queue
    while (queueFront != NULL) {
        QueueNode* temp = queueFront;
        queueFront = queueFront->next;
        delete temp;
    }

    return 0;
}

// ============================================================
//  1. ADD MERCHANT
//  DSA Used : Array
//  Time     : O(1) per insertion
//  Space    : O(n) for n merchants stored in array
// ============================================================
void addMerchant() {
    if (merchantCount >= MAX_MERCHANTS) {
        cout << "[ERROR] Merchant list is full (max "
             << MAX_MERCHANTS << ").\n";
        return;
    }

    Merchant m;
    cout << "\n--- Add Merchant ---\n";
    cout << "Enter Merchant ID : ";
    cin  >> m.id;

    // Check for duplicate ID using linear search
    // Time Complexity: O(n)
    for (int i = 0; i < merchantCount; i++) {
        if (merchants[i].id == m.id) {
            cout << "[ERROR] Merchant ID " << m.id
                 << " already exists.\n";
            return;
        }
    }

    cin.ignore();  // Clear newline from buffer
    cout << "Enter Merchant Name: ";
    cin.getline(m.name, 50);

    m.totalAmount = 0.0;

    // Insert into array at the end – O(1)
    merchants[merchantCount] = m;
    merchantCount++;

    cout << "[SUCCESS] Merchant '" << m.name
         << "' added with ID " << m.id << ".\n";
}

// ============================================================
//  2. DISPLAY MERCHANTS
//  DSA Used : Array Traversal
//  Time     : O(n)
// ============================================================
void displayMerchants() {
    if (merchantCount == 0) {
        cout << "\n[INFO] No merchants registered yet.\n";
        return;
    }

    cout << "\n--- Merchant List (" << merchantCount << ") ---\n";
    cout << "----------------------------------------------\n";
    cout << "  ID  |  Name                  |  Total Amt\n";
    cout << "----------------------------------------------\n";

    // Traverse the array – O(n)
    for (int i = 0; i < merchantCount; i++) {
        cout << "  " << merchants[i].id << "\t|  "
             << merchants[i].name << "\t\t|  Rs."
             << merchants[i].totalAmount << endl;
    }

    cout << "----------------------------------------------\n";
}

// ============================================================
//  3. ADD TRANSACTION
//  DSA Used : Linked List (insert at head)
//  Also     : Stack (push for undo), Queue (enqueue for processing)
//  Time     : O(1) for insert at head
// ============================================================
void addTransaction() {
    if (merchantCount == 0) {
        cout << "\n[INFO] Add a merchant first.\n";
        return;
    }

    int mId;
    cout << "\n--- Add Transaction ---\n";
    cout << "Enter Merchant ID: ";
    cin  >> mId;

    // Find merchant using linear search – O(n)
    int index = linearSearch(mId);
    if (index == -1) {
        cout << "[ERROR] Merchant ID " << mId << " not found.\n";
        return;
    }

    double amount;
    cout << "Enter Transaction Amount (Rs.): ";
    cin  >> amount;

    if (amount <= 0) {
        cout << "[ERROR] Amount must be positive.\n";
        return;
    }

    cin.ignore();
    char desc[100];
    cout << "Enter Description: ";
    cin.getline(desc, 100);

    // --- Create new Linked List node (insert at head) – O(1) ---
    TransactionNode* newNode = new TransactionNode;
    newNode->transactionId = nextTransactionId;
    newNode->merchantId    = mId;
    newNode->amount        = amount;
    strcpy(newNode->description, desc);
    newNode->next          = transactionHead;
    transactionHead        = newNode;   // New head

    // --- Push onto Stack for undo capability – O(1) ---
    // Using Stack (LIFO) so the most recent transaction can be undone
    pushToStack(nextTransactionId, mId, amount, desc);

    // --- Enqueue into Queue for FIFO payment processing – O(1) ---
    // Using Queue (FIFO) so payments are processed in order
    enqueue(nextTransactionId, mId, amount);

    // Update merchant's running total
    merchants[index].totalAmount += amount;

    cout << "[SUCCESS] Transaction #" << nextTransactionId
         << " of Rs." << amount << " added for Merchant '"
         << merchants[index].name << "'.\n";

    nextTransactionId++;
}

// ============================================================
//  4. VIEW TRANSACTION HISTORY
//  DSA Used : Linked List Traversal
//  Time     : O(n) where n = number of transactions
// ============================================================
void viewTransactionHistory() {
    if (transactionHead == NULL) {
        cout << "\n[INFO] No transactions recorded yet.\n";
        return;
    }

    cout << "\n--- Transaction History (most recent first) ---\n";
    cout << "---------------------------------------------------\n";
    cout << "  TxnID | MerchantID | Amount    | Description\n";
    cout << "---------------------------------------------------\n";

    // Traverse the linked list from head to end – O(n)
    TransactionNode* current = transactionHead;
    while (current != NULL) {
        cout << "  " << current->transactionId
             << "\t|  " << current->merchantId
             << "\t     |  Rs." << current->amount
             << "\t  | " << current->description << endl;
        current = current->next;
    }

    cout << "---------------------------------------------------\n";
}

// ============================================================
//  5. PROCESS PAYMENTS (Queue – FIFO)
//  DSA Used : Queue (First-In, First-Out)
//  Time     : O(1) per dequeue
//  Payments are processed in the order they were added.
// ============================================================
void processPayments() {
    if (isQueueEmpty()) {
        cout << "\n[INFO] No payments pending in the queue.\n";
        return;
    }

    cout << "\n--- Processing Payments (FIFO Order) ---\n";

    int count = 0;
    // Dequeue all pending payments – each dequeue is O(1)
    while (!isQueueEmpty()) {
        int txnId, mId;
        double amt;
        dequeue(txnId, mId, amt);

        cout << "  [PROCESSED] TxnID: " << txnId
             << " | MerchantID: " << mId
             << " | Amount: Rs." << amt << endl;
        count++;
    }

    cout << "[DONE] " << count << " payment(s) processed.\n";
}

// ============================================================
//  6. UNDO LAST TRANSACTION (Stack – LIFO)
//  DSA Used : Stack (Last-In, First-Out)
//  Time     : O(1) for pop, O(n) to find & remove from linked list
//  The most recently added transaction is undone first.
// ============================================================
void undoLastTransaction() {
    if (isStackEmpty()) {
        cout << "\n[INFO] Nothing to undo.\n";
        return;
    }

    int txnId, mId;
    double amt;
    char desc[100];

    // Pop from stack – O(1) – gives us the most recent transaction
    popFromStack(txnId, mId, amt, desc);

    cout << "\n--- Undo Last Transaction ---\n";
    cout << "  Undoing TxnID: " << txnId
         << " | MerchantID: " << mId
         << " | Amount: Rs." << amt
         << " | Desc: " << desc << endl;

    // Subtract amount from merchant's total – O(n) search
    int index = linearSearch(mId);
    if (index != -1) {
        merchants[index].totalAmount -= amt;
    }

    // Remove the transaction node from the linked list – O(n)
    // We search the linked list for the matching transactionId
    if (transactionHead != NULL) {
        // Case 1: Head node is the one to remove
        if (transactionHead->transactionId == txnId) {
            TransactionNode* temp = transactionHead;
            transactionHead = transactionHead->next;
            delete temp;
        }
        // Case 2: Search in the rest of the list
        else {
            TransactionNode* current = transactionHead;
            while (current->next != NULL &&
                   current->next->transactionId != txnId) {
                current = current->next;
            }
            if (current->next != NULL) {
                TransactionNode* temp = current->next;
                current->next = temp->next;
                delete temp;
            }
        }
    }

    cout << "[SUCCESS] Transaction #" << txnId << " undone.\n";
}

// ============================================================
//  7. SEARCH MERCHANT BY ID
//  DSA Used : Linear Search – O(n)
//             Binary Search – O(log n) [requires sorted array]
// ============================================================
void searchMerchantById() {
    if (merchantCount == 0) {
        cout << "\n[INFO] No merchants to search.\n";
        return;
    }

    int id;
    cout << "\n--- Search Merchant ---\n";
    cout << "Enter Merchant ID to search: ";
    cin  >> id;

    // --- METHOD 1: Linear Search – O(n) ---
    cout << "\n[Linear Search - O(n)]" << endl;
    int linIndex = linearSearch(id);
    if (linIndex != -1) {
        cout << "  FOUND at index " << linIndex << ": "
             << merchants[linIndex].name
             << " | Total: Rs." << merchants[linIndex].totalAmount
             << endl;
    } else {
        cout << "  NOT FOUND.\n";
    }

    // --- METHOD 2: Binary Search – O(log n) ---
    // Binary search requires the array to be sorted by ID.
    // We sort a temporary copy so as not to disturb original order.
    cout << "\n[Binary Search - O(log n)] (sorting by ID first)" << endl;

    // Create temporary sorted copy – O(n) copy + O(n^2) sort
    Merchant temp[MAX_MERCHANTS];
    for (int i = 0; i < merchantCount; i++)
        temp[i] = merchants[i];

    // Bubble sort temp array by ID for binary search
    for (int i = 0; i < merchantCount - 1; i++) {
        for (int j = 0; j < merchantCount - 1 - i; j++) {
            if (temp[j].id > temp[j + 1].id) {
                Merchant swap = temp[j];
                temp[j]       = temp[j + 1];
                temp[j + 1]   = swap;
            }
        }
    }

    // Binary search on sorted temp array – O(log n)
    int low = 0, high = merchantCount - 1, found = -1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (temp[mid].id == id) {
            found = mid;
            break;
        } else if (temp[mid].id < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    if (found != -1) {
        cout << "  FOUND: " << temp[found].name
             << " | Total: Rs." << temp[found].totalAmount << endl;
    } else {
        cout << "  NOT FOUND.\n";
    }
}

// ============================================================
//  8. SORT MERCHANTS BY TOTAL AMOUNT
//  DSA Used : Bubble Sort – O(n^2) worst/average case
//  Sorts the merchant array in descending order of totalAmount.
// ============================================================
void sortMerchantsByAmount() {
    if (merchantCount == 0) {
        cout << "\n[INFO] No merchants to sort.\n";
        return;
    }

    // --- Bubble Sort: O(n^2) time, O(1) extra space ---
    // We compare adjacent elements and swap if needed.
    // Sorting in DESCENDING order of totalAmount.
    for (int i = 0; i < merchantCount - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < merchantCount - 1 - i; j++) {
            if (merchants[j].totalAmount < merchants[j + 1].totalAmount) {
                // Swap merchants[j] and merchants[j+1]
                Merchant temp = merchants[j];
                merchants[j]     = merchants[j + 1];
                merchants[j + 1] = temp;
                swapped = true;
            }
        }
        // Optimization: if no swap occurred, array is already sorted
        if (!swapped) break;
    }

    cout << "\n[SUCCESS] Merchants sorted by total amount (descending).\n";
    cout << "  Time Complexity : O(n^2) – Bubble Sort\n";
    cout << "  Space Complexity: O(1)   – In-place\n";

    // Display sorted list
    displayMerchants();
}

// ============================================================
//  9. SIMULATE BANK NETWORK (Graph – Adjacency Matrix)
//  DSA Used : Graph represented as Adjacency Matrix
//  Space    : O(V^2) where V = number of banks
// ============================================================
void simulateBankNetwork() {
    cout << "\n--- Simulate Bank-to-Bank Settlement Network ---\n";

    // Reset the graph for fresh setup
    bankCount = 0;
    for (int i = 0; i < MAX_BANKS; i++)
        for (int j = 0; j < MAX_BANKS; j++)
            graph[i][j] = 0;

    cout << "Enter number of banks (max " << MAX_BANKS << "): ";
    cin  >> bankCount;

    if (bankCount <= 0 || bankCount > MAX_BANKS) {
        cout << "[ERROR] Invalid number of banks.\n";
        bankCount = 0;
        return;
    }

    cin.ignore();
    for (int i = 0; i < bankCount; i++) {
        cout << "Enter name of Bank " << i << ": ";
        cin.getline(bankNames[i], 30);
    }

    int edges;
    cout << "Enter number of connections (edges): ";
    cin  >> edges;

    cout << "(Enter each edge as: fromBank toBank cost)\n";
    for (int i = 0; i < edges; i++) {
        int from, to, cost;
        cout << "  Edge " << (i + 1) << ": ";
        cin >> from >> to >> cost;

        if (from < 0 || from >= bankCount ||
            to < 0   || to >= bankCount   || cost < 0) {
            cout << "  [ERROR] Invalid edge. Skipping.\n";
            continue;
        }

        // Undirected graph: set both directions
        graph[from][to] = cost;
        graph[to][from] = cost;
    }

    // Display adjacency matrix
    cout << "\n--- Adjacency Matrix (Transfer Costs) ---\n";
    cout << "\t";
    for (int i = 0; i < bankCount; i++)
        cout << bankNames[i] << "\t";
    cout << endl;

    for (int i = 0; i < bankCount; i++) {
        cout << bankNames[i] << "\t";
        for (int j = 0; j < bankCount; j++) {
            cout << graph[i][j] << "\t";
        }
        cout << endl;
    }

    cout << "\n[SUCCESS] Bank network created with "
         << bankCount << " banks.\n";
    cout << "  DSA: Graph (Adjacency Matrix)\n";
    cout << "  Space Complexity: O(V^2)\n";
}

// ============================================================
//  10. FIND CHEAPEST TRANSFER PATH (Dijkstra's Algorithm)
//  DSA Used : Dijkstra's Algorithm (Greedy approach)
//  Time     : O(V^2) with adjacency matrix
//  Finds shortest (cheapest) path from source to destination bank.
// ============================================================
void findCheapestPath() {
    if (bankCount == 0) {
        cout << "\n[INFO] Set up bank network first (Option 9).\n";
        return;
    }

    int source, destination;
    cout << "\n--- Find Cheapest Transfer Path (Dijkstra) ---\n";
    cout << "Available Banks:\n";
    for (int i = 0; i < bankCount; i++)
        cout << "  " << i << " - " << bankNames[i] << endl;

    cout << "Enter source bank index     : ";
    cin  >> source;
    cout << "Enter destination bank index: ";
    cin  >> destination;

    if (source < 0 || source >= bankCount ||
        destination < 0 || destination >= bankCount) {
        cout << "[ERROR] Invalid bank index.\n";
        return;
    }

    if (source == destination) {
        cout << "[INFO] Source and destination are the same. Cost = 0.\n";
        return;
    }

    // --- Dijkstra's Algorithm ---
    // dist[i]    = shortest distance from source to bank i
    // visited[i] = whether bank i has been finalized
    // parent[i]  = previous bank in the shortest path

    int dist[MAX_BANKS];
    bool visited[MAX_BANKS];
    int parent[MAX_BANKS];

    // Initialize – O(V)
    for (int i = 0; i < bankCount; i++) {
        dist[i]    = INT_MAX;  // Infinity
        visited[i] = false;
        parent[i]  = -1;
    }
    dist[source] = 0;

    // Main loop – runs V times – O(V^2) total
    for (int count = 0; count < bankCount - 1; count++) {

        // GREEDY STEP: Pick the unvisited bank with smallest dist
        // This is the greedy choice – O(V) scan
        int u = -1;
        int minDist = INT_MAX;
        for (int i = 0; i < bankCount; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;  // Remaining banks are unreachable
        visited[u] = true;

        // Update distances of adjacent banks – O(V)
        for (int v = 0; v < bankCount; v++) {
            // graph[u][v] > 0 means there is an edge
            if (!visited[v] && graph[u][v] > 0 &&
                dist[u] != INT_MAX &&
                dist[u] + graph[u][v] < dist[v]) {

                dist[v]   = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    // --- Display Result ---
    if (dist[destination] == INT_MAX) {
        cout << "\n[RESULT] No path exists from "
             << bankNames[source] << " to "
             << bankNames[destination] << ".\n";
    } else {
        cout << "\n[RESULT] Cheapest transfer cost from "
             << bankNames[source] << " to "
             << bankNames[destination] << " = Rs."
             << dist[destination] << endl;

        // Reconstruct and print the path using parent array
        // We use a simple array as a stack to reverse the path
        int path[MAX_BANKS];
        int pathLen = 0;
        int node = destination;
        while (node != -1) {
            path[pathLen++] = node;
            node = parent[node];
        }

        cout << "  Path: ";
        for (int i = pathLen - 1; i >= 0; i--) {
            cout << bankNames[path[i]];
            if (i > 0) cout << " --> ";
        }
        cout << endl;
    }

    cout << "\n  DSA: Dijkstra's Algorithm (Greedy)\n";
    cout << "  Time Complexity : O(V^2)\n";
    cout << "  Space Complexity: O(V)\n";
}

// ============================================================
//  HELPER: Stack Operations (Linked List based – LIFO)
// ============================================================

// Push – O(1): Insert at top of stack
void pushToStack(int txnId, int mId, double amt, const char* desc) {
    StackNode* newNode = new StackNode;
    newNode->transactionId = txnId;
    newNode->merchantId    = mId;
    newNode->amount        = amt;
    strcpy(newNode->description, desc);
    newNode->next = stackTop;
    stackTop      = newNode;   // New top
}

// Pop – O(1): Remove from top of stack
void popFromStack(int &txnId, int &mId, double &amt, char* desc) {
    if (stackTop == NULL) return;
    txnId = stackTop->transactionId;
    mId   = stackTop->merchantId;
    amt   = stackTop->amount;
    strcpy(desc, stackTop->description);

    StackNode* temp = stackTop;
    stackTop = stackTop->next;
    delete temp;
}

// Check if stack is empty – O(1)
bool isStackEmpty() {
    return (stackTop == NULL);
}

// ============================================================
//  HELPER: Queue Operations (Linked List based – FIFO)
// ============================================================

// Enqueue – O(1): Insert at rear
void enqueue(int txnId, int mId, double amt) {
    QueueNode* newNode = new QueueNode;
    newNode->transactionId = txnId;
    newNode->merchantId    = mId;
    newNode->amount        = amt;
    newNode->next          = NULL;

    if (queueRear == NULL) {
        // Queue is empty – both front and rear point to new node
        queueFront = queueRear = newNode;
    } else {
        queueRear->next = newNode;
        queueRear       = newNode;
    }
}

// Dequeue – O(1): Remove from front
void dequeue(int &txnId, int &mId, double &amt) {
    if (queueFront == NULL) return;
    txnId = queueFront->transactionId;
    mId   = queueFront->merchantId;
    amt   = queueFront->amount;

    QueueNode* temp = queueFront;
    queueFront = queueFront->next;

    // If queue becomes empty, reset rear too
    if (queueFront == NULL)
        queueRear = NULL;

    delete temp;
}

// Check if queue is empty – O(1)
bool isQueueEmpty() {
    return (queueFront == NULL);
}

// ============================================================
//  HELPER: Linear Search – O(n)
//  Scans the merchant array sequentially.
// ============================================================
int linearSearch(int id) {
    for (int i = 0; i < merchantCount; i++) {
        if (merchants[i].id == id)
            return i;   // Found at index i
    }
    return -1;   // Not found
}

// ============================================================
//  HELPER: Binary Search – O(log n)
//  Requires sorted array. Used in Option 7 on a temp copy.
// ============================================================
int binarySearch(int id) {
    int low = 0, high = merchantCount - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (merchants[mid].id == id)
            return mid;
        else if (merchants[mid].id < id)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// ============================================================
//  HELPER: Bubble Sort – O(n^2)
//  Sorts merchants array by totalAmount in descending order.
// ============================================================
void bubbleSort() {
    for (int i = 0; i < merchantCount - 1; i++) {
        for (int j = 0; j < merchantCount - 1 - i; j++) {
            if (merchants[j].totalAmount < merchants[j + 1].totalAmount) {
                Merchant temp = merchants[j];
                merchants[j]     = merchants[j + 1];
                merchants[j + 1] = temp;
            }
        }
    }
}

/*
 * ============================================================
 *  DSA CONCEPT SUMMARY (for Viva Reference)
 * ============================================================
 *
 *  1. ARRAY
 *     - Merchant storage in a fixed-size array.
 *     - Access: O(1), Search: O(n), Insert at end: O(1)
 *
 *  2. LINKED LIST
 *     - Transaction history stored as singly linked list.
 *     - Insert at head: O(1), Traversal: O(n)
 *
 *  3. STACK (LIFO)
 *     - Implemented using linked list (no fixed size limit).
 *     - Used for undoing the last transaction.
 *     - Push: O(1), Pop: O(1)
 *
 *  4. QUEUE (FIFO)
 *     - Implemented using linked list with front & rear ptrs.
 *     - Used for processing payments in order received.
 *     - Enqueue: O(1), Dequeue: O(1)
 *
 *  5. LINEAR SEARCH
 *     - Sequential scan through merchant array.
 *     - Time: O(n), Space: O(1)
 *
 *  6. BINARY SEARCH
 *     - Requires sorted array. Halves search space each step.
 *     - Time: O(log n), Space: O(1)
 *
 *  7. BUBBLE SORT
 *     - Compares and swaps adjacent elements repeatedly.
 *     - Time: O(n^2) worst/average, O(n) best (with flag)
 *     - Space: O(1) – in-place
 *
 *  8. GRAPH (Adjacency Matrix)
 *     - Bank-to-bank network as a 2D array.
 *     - Space: O(V^2), Edge lookup: O(1)
 *
 *  9. DIJKSTRA'S ALGORITHM (Greedy)
 *     - Finds cheapest (shortest) path in weighted graph.
 *     - Greedy choice: always pick nearest unvisited node.
 *     - Time: O(V^2) with adj. matrix, Space: O(V)
 *
 * ============================================================
 */
