# Retail Micro-Payment Settlement Ledger

> **Course:** Data Structures and Algorithms — I (DSA-I)  
> **Language:** C++ (Terminal-Based, Menu-Driven)  
> **Source File:** `main.cpp`

---

## 1. Problem Statement

In today's digital economy, small retailers carry out hundreds of micro-payments every day — purchasing supplies from vendors, paying delivery partners, or settling dues with other merchants through various banks. Managing these tiny but frequent transactions manually is slow, error-prone, and hard to reverse when mistakes happen.

This project builds a **terminal-based settlement ledger** that lets a user register merchants, record transactions, undo mistakes, process payments in order, search and sort merchant records, and even find the cheapest bank-to-bank transfer route — all using core DSA-I concepts and no external libraries, databases, or graphical interfaces.

---

## 2. Objectives

- Apply fundamental data structures (Array, Linked List, Stack, Queue, Graph) to a realistic problem.
- Implement and compare searching algorithms (Linear Search and Binary Search).
- Demonstrate Bubble Sort on merchant records.
- Use Dijkstra's Algorithm on a weighted adjacency-matrix graph to find the cheapest settlement path between banks.
- Build a clean, menu-driven terminal application that a user can operate without any prior training.
- Strengthen understanding of time and space complexity through hands-on coding.

---

## 3. System Overview (Menu-Driven Flow)

When the program starts, it displays a banner and then enters an infinite loop that shows the **Main Menu** until the user chooses to exit.

```
=============================================
  RETAIL MICRO-PAYMENT SETTLEMENT LEDGER
  (DSA-I System Simulation)
=============================================

---------- MAIN MENU ----------
 1.  Add Merchant
 2.  Display Merchants
 3.  Add Transaction
 4.  View Transaction History
 5.  Process Payments (Queue)
 6.  Undo Last Transaction (Stack)
 7.  Search Merchant by ID
 8.  Sort Merchants by Amount
 9.  Simulate Bank Network (Graph)
10.  Find Cheapest Transfer Path
11.  Verify Transaction Token
12.  Currency Balancer Report
13.  Exit
-------------------------------
Enter your choice:
```

The user types a number (1–13) and the program runs the matching function. After the operation finishes, the menu re-appears. Choosing **13** frees all dynamically allocated memory and exits cleanly.

---

## 4. Data Structures Used (with Purpose)

### 4.1 Array

| Detail | Value |
|--------|-------|
| **What is stored** | `Merchant` structs (id, name, totalAmount) |
| **Max size** | 100 (`MAX_MERCHANTS`) |
| **Purpose** | Fast index-based access to merchant records; acts as the primary storage for all registered merchants. |

Each merchant is inserted at the end of the array in O(1) time. The array also supports traversal for display, linear search, and bubble sort.

### 4.2 Linked List (Singly Linked)

| Detail | Value |
|--------|-------|
| **Node struct** | `TransactionNode` (transactionId, merchantId, amount, description, next pointer) |
| **Purpose** | Stores the complete transaction history in insertion order. |

New transactions are inserted at the **head** of the list in O(1) time. The list is traversed to display history or to locate and remove a specific node during an undo operation.

### 4.3 Stack (LIFO — Linked List Based)

| Detail | Value |
|--------|-------|
| **Node struct** | `StackNode` (transactionId, merchantId, amount, description, next pointer) |
| **Purpose** | Enables **Undo Last Transaction**. Every new transaction is pushed onto the stack. When the user chooses undo, the top element is popped and the corresponding entry is removed from the linked list and the merchant's running total. |

Push and Pop both run in O(1).

### 4.4 Queue (FIFO — Linked List Based)

| Detail | Value |
|--------|-------|
| **Node struct** | `QueueNode` (transactionId, merchantId, amount, next pointer) |
| **Pointers** | `queueFront` and `queueRear` |
| **Purpose** | Payments are enqueued when a transaction is added and dequeued when the user selects **Process Payments**. This ensures payments are settled in the same order they were received. |

Enqueue and Dequeue both run in O(1).

### 4.5 Graph (Adjacency Matrix)

| Detail | Value |
|--------|-------|
| **Representation** | 2-D integer array `graph[MAX_BANKS][MAX_BANKS]` |
| **Max nodes** | 10 (`MAX_BANKS`) |
| **Edge weight** | Transfer fee between two banks (0 = no direct link) |
| **Purpose** | Models a bank-to-bank settlement network. Used by Dijkstra's algorithm to find the cheapest transfer route. |

The user is prompted to enter the number of banks, bank names, and the adjacency matrix (transfer fees) when they select **Simulate Bank Network**.

### 4.6 Hash Table (Array of Linked Lists for Chaining)

| Detail | Value |
|--------|-------|
| **Table size** | 101 |
| **Node struct** | `TokenNode` (token string, transactionId, merchantId, amount, description, currency, next pointer) |
| **Collision Resolution** | Chaining using Singly Linked Lists |
| **Purpose** | Instantly verifies whether a transaction authorization code is genuine and retrieves transaction details. |

Insert and Search both run in O(1) average time.

---

## 5. Algorithms Used

### 5.1 Linear Search

- Scans the merchant array from index 0 to `merchantCount - 1`.
- Returns the index if the merchant ID matches; returns -1 otherwise.
- **Time Complexity:** O(n)

### 5.2 Binary Search

- Works on the merchant array **after it has been sorted** by ID using Bubble Sort.
- Repeatedly halves the search space by comparing the target ID with the middle element.
- **Time Complexity:** O(log n)
- **Prerequisite:** The array must be sorted first; the program calls `bubbleSort()` before performing binary search.

### 5.3 Bubble Sort

- Sorts the merchant array in ascending order of `totalAmount`.
- Repeatedly compares adjacent elements and swaps them if they are in the wrong order.
- **Time Complexity:** O(n²) in the worst and average case.

### 5.4 Dijkstra's Algorithm

- Finds the shortest (cheapest) path from a source bank to all other banks in the adjacency-matrix graph.
- Uses a `visited[]` array and a `dist[]` array.
- In each iteration it picks the unvisited node with the smallest distance, marks it visited, and relaxes all its neighbours.
- **Time Complexity:** O(V²) where V = number of banks (since we use an adjacency matrix, not a priority queue).

### 5.5 Hash Table Insertion & Search (Hashing)

- String key is mapped to an array index in the range `[0, 100]` using a rolling polynomial hash.
- Collisions are handled using external chaining (linked lists).
- **Time Complexity:** O(1) average time.

---

## 6. Implementation Approach

The entire project is written in a **single file** (`main.cpp`) and follows a procedural style. Below is the high-level flow:

1. **Structs** are defined first — `Merchant`, `TransactionNode`, `TokenNode`, `StackNode`, `QueueNode`.
2. **Global variables** hold the merchant array, linked-list head, stack top, queue front/rear, `hashTable` array, graph matrix, and counters.
3. **Function prototypes** are declared before `main()`.
4. **`main()`** initialises the graph to all zeros, prints the banner, and enters the menu loop.
5. Each menu option calls a dedicated function:
   - `addMerchant()` — appends to the array.
   - `displayMerchants()` — traverses the array.
   - `addTransaction()` — prompts for currency, processes conversion, inserts into linked list head, pushes to stack, enqueues to queue, generates/inserts token, and updates merchant total.
   - `viewTransactionHistory()` — traverses the linked list showing currencies and converted INR amounts.
   - `processPayments()` — dequeues all entries and prints them.
   - `undoLastTransaction()` — pops from stack, searches and removes from linked list, subtracts from merchant total.
   - `searchMerchantById()` — runs both linear and binary search and prints results.
   - `sortMerchantsByAmount()` — calls `bubbleSort()` and displays the sorted array.
   - `simulateBankNetwork()` — takes user input to build the adjacency matrix.
   - `findCheapestPath()` — runs Dijkstra's algorithm on the graph.
   - `verifyTransactionToken()` — queries the Hash Table for O(1) token validation.
   - `currencyBalancerReport()` — scans transactions to print totals grouped by currency.
6. **Helper functions** at the bottom implement stack push/pop, queue enqueue/dequeue, linear/binary search, bubble sort, and hash table routines.
7. On exit, all dynamically allocated linked-list, stack/queue, and hash table nodes are freed.

---

## 7. Time and Space Complexity Analysis

| Operation | Data Structure / Algorithm | Time Complexity | Space Complexity |
|-----------|---------------------------|-----------------|------------------|
| Add Merchant | Array (insert at end) | O(1) | O(1) per call |
| Display Merchants | Array traversal | O(n) | O(1) |
| Add Transaction | Linked List + Stack + Queue + Hash Table insert | O(1) average | O(1) per call |
| View Transaction History | Linked List traversal | O(n) | O(1) |
| Process Payments | Queue dequeue (all) | O(k), k = queue size | O(1) |
| Undo Last Transaction | Stack pop O(1) + Linked List search & delete O(n) | O(n) | O(1) |
| Linear Search | Array scan | O(n) | O(1) |
| Binary Search | Sorted array halving | O(log n) | O(1) |
| Bubble Sort | Adjacent-swap passes | O(n²) | O(1) |
| Simulate Bank Network | Adjacency Matrix input | O(V²) | O(V²) |
| Dijkstra's Algorithm | Adjacency Matrix + dist[] | O(V²) | O(V) |
| Verify Transaction Token | Hash Table Search | O(1) average | O(1) |
| Currency Balancer Report | Linked List traversal | O(n) | O(1) |

**Overall space:** O(MAX_MERCHANTS + T + V² + H) where T = transactions, V = banks, H = hash table nodes.

---

## 8. Execution Steps

### Prerequisites

- A C++ compiler (g++ recommended).
- A terminal or command prompt.

### Compile

```bash
g++ -o ledger main.cpp
```

### Run

```bash
./ledger
```

> **Windows users:** Use `ledger.exe` instead of `./ledger` after compiling.

The program will display the banner and main menu. Follow the on-screen prompts to interact.

---

## 9. Sample Input and Output

Below is an example of a typical session in the terminal.

### Adding a Merchant

```
Enter your choice: 1

--- Add Merchant ---
Enter Merchant ID   : 101
Enter Merchant Name : QuickMart

[OK] Merchant 'QuickMart' (ID: 101) added successfully.
```

### Adding a Transaction

```
Enter your choice: 3

--- Add Transaction ---
Enter Merchant ID: 101
Enter Currency:
INR/USD/EUR
USD
Enter Transaction Amount: 10
Original Amount: 10 USD
Converted Amount: Rs.830
Enter Description: Soda

[SUCCESS] Transaction #1 added for Merchant 'QuickMart'.
Authorization Token: TXN1001
```

### Undo Last Transaction

```
Enter your choice: 6

--- Undo Last Transaction ---
[UNDO] Reversed Transaction #1 | Merchant: 101 | Amount: 250.50
       Description: Weekly supply restock
       Merchant's total updated.
```

### Process Payments (Queue)

```
Enter your choice: 5

--- Process Payments ---
Processing TXN #2 | Merchant: 102 | Amount: 120.00
Processing TXN #3 | Merchant: 101 | Amount: 75.00
[DONE] 2 payment(s) processed.
```

### Search Merchant by ID

```
Enter your choice: 7

--- Search Merchant ---
Enter Merchant ID to search: 101

[Linear Search - O(n)]
  Found at index 0 : QuickMart (Total: 325.50)

[Binary Search - O(log n)]  (sorting by ID first...)
  Found at index 0 : QuickMart (Total: 325.50)
```

### Sort Merchants by Amount

```
Enter your choice: 8

--- Sort Merchants by Total Amount (Bubble Sort) ---
 ID: 103  | Name: FreshBites      | Total:  85.00
 ID: 102  | Name: PayCorner       | Total: 120.00
 ID: 101  | Name: QuickMart       | Total: 325.50
[DONE] Merchants sorted by amount.
```

### Simulate Bank Network & Find Cheapest Path

```
Enter your choice: 9

--- Simulate Bank Network ---
Enter number of banks (max 10): 3
Enter name for bank 0: SBI
Enter name for bank 1: HDFC
Enter name for bank 2: ICICI

Enter adjacency matrix (transfer fees, 0 = no link):
Row 0: 0 5 10
Row 1: 5 0 3
Row 2: 10 3 0

[OK] Bank network with 3 banks created.

Enter your choice: 10

--- Find Cheapest Transfer Path (Dijkstra) ---
Enter source bank index (0 to 2): 0

Shortest transfer cost from 'SBI':
  To HDFC  : cost = 5
  To ICICI : cost = 8   (via HDFC)
```

### Verify Transaction Token (Hash Table)

```
Enter your choice: 11

--- Verify Transaction Token ---
Enter Authorization Token: TXN1001

Token Found
Transaction ID: 1
Merchant ID: 101
Amount: 10 USD
Description: Soda

DSA Used: Hash Table
Average Time Complexity: O(1)
```

### Currency Balancer Report

```
Enter your choice: 12

--- Currency Balancer Report ---

INR Transactions: 0
USD Transactions: 1
EUR Transactions: 1

Total Settlement Value (INR):
Rs. 1280

DSA Used: Array Traversal
Time Complexity: O(n)
```

---

## 10. Screenshots

Screenshots demonstrating each menu option and its output are stored in the `screenshots/` folder located in the project directory.

The folder contains images showing:

- Program banner and main menu
- Adding merchants and transactions
- Viewing transaction history
- Processing payments via Queue
- Undoing a transaction via Stack
- Linear Search and Binary Search results
- Bubble Sort output
- Bank network adjacency matrix input
- Dijkstra's cheapest path output
- Program exit and memory cleanup

> Refer to the `screenshots/` folder for visual proof of correct execution.

---

## 11. Results and Observations

1. **Array** proved to be simple and efficient for storing a fixed number of merchants. Index-based access is O(1), and iteration is straightforward.

2. **Linked List** was the right choice for transaction history because the number of transactions is not known in advance. Insertion at the head is O(1), but deletion of a specific node requires O(n) traversal.

3. **Stack (LIFO)** made the undo feature very natural. The most recent transaction is always on top, so reversing it is a single pop operation.

4. **Queue (FIFO)** ensured fairness in payment processing — the first transaction added is the first one settled, which mirrors real-world payment queues.

5. **Linear Search** works on unsorted data but is slow for large arrays. **Binary Search** is much faster (O(log n)) but needs the array to be sorted first.

6. **Bubble Sort** is easy to understand and implement, but its O(n²) time makes it impractical for large datasets. For this project's scale (up to 100 merchants), it performs well enough.

7. **Dijkstra's Algorithm** successfully finds the cheapest bank-to-bank transfer route. Using an adjacency matrix keeps the code simple, and the O(V²) complexity is acceptable for up to 10 banks.

8. **Hash Table** allows O(1) average-time verification of transaction authorization tokens. It uses linked list chaining to resolve collisions.

9. **Currency Balancer** supports multiple currencies (INR, USD, EUR) using fixed conversion rates. It ensures all merchant balance modifications use converted INR value, maintaining total balance sanity.

10. All data is stored **in memory only** — there are no files, databases, or network calls. This keeps the project purely within the DSA-I syllabus.

---

## 12. Conclusion

This project demonstrates how core data structures and algorithms taught in the DSA-I course can be combined to solve a practical, real-world-inspired problem. By building a micro-payment settlement ledger from scratch in C++, we applied:

- **Arrays** for merchant storage,
- **Linked Lists** for dynamic transaction history,
- **Stacks** for undo functionality,
- **Queues** for ordered payment processing,
- **Graphs (Adjacency Matrix)** for modelling a bank network, and
- **Dijkstra's Algorithm** for finding the cheapest transfer path.

The menu-driven design makes the system interactive and easy to test. Every data structure and algorithm used is within the scope of DSA-I, and the project does not depend on any GUI framework, database, or web technology.

Through this project, we gained hands-on experience with pointer-based data structures, dynamic memory management, and algorithm analysis — skills that form the foundation for more advanced courses ahead.

---

> **Note:** This is a terminal-based C++ project developed strictly for academic purposes under the DSA-I curriculum.
