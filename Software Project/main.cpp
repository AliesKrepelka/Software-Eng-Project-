#include <iostream>
#include <limits>
#include "PatronsCollection.h"
#include "BooksCollection.h"
#include "LoansCollection.h"

/**
 * Helper function to safely read an integer from user input
 * This function validates input and handles non-numeric entries
 *
 * @return int - The validated integer entered by the user
 */
int readChoice() {
    int choice;

    // Loop until valid integer input is received
    while (!(std::cin >> choice)) {
        // Clear the error flag on cin (occurs when non-numeric input is entered)
        std::cin.clear();

        // Ignore/discard invalid input from the buffer up to newline character
        // std::numeric_limits<std::streamsize>::max() ensures all remaining input is cleared
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Prompt user to try again with valid input
        std::cout << "Invalid input. Please enter a number: ";
    }

    // Clear any remaining characters in the input buffer (like extra newlines)
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return choice;
}

/**
 * Displays and handles the Patron management submenu
 * Allows adding, editing, deleting, and viewing patrons
 *
 * @param patrons - Reference to PatronsCollection object for managing library patrons
 */
void patronOptions(PatronsCollection& patrons) {
    int choice;

    // Loop until user chooses to return to main menu (option 5)
    do {
        // Display patron management menu options
        std::cout << "\n--- Patron Options ---\n";
        std::cout << "1. Add Patron\n";
        std::cout << "2. Edit Patron\n";
        std::cout << "3. Delete Patron\n";
        std::cout << "4. Print All Patrons\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "Enter choice: ";

        // Read and validate user's choice
        choice = readChoice();

        // Execute action based on user's selection
        switch (choice) {
        case 1:
            // Add a new patron to the collection
            patrons.AddPatron();
            break;
        case 2:
            // Edit an existing patron's information
            patrons.EditPatron();
            break;
        case 3:
            // Remove a patron from the collection
            patrons.DeletePatron();
            break;
        case 4:
            // Display all patrons in the system
            patrons.PrintAllPatrons();
            break;
        case 5:
            // Exit patron menu and return to main menu
            std::cout << "Returning to Main Menu...\n";
            break;
        default:
            // Handle invalid menu selections
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5); // Continue until user selects option 5
}

/**
 * Displays and handles the Book management submenu
 * Allows adding, editing, deleting, and viewing books
 *
 * @param books - Reference to BooksCollection object for managing library books
 */
void bookOptions(BooksCollection& books) {
    int choice;

    // Loop until user chooses to return to main menu (option 5)
    do {
        // Display book management menu options
        std::cout << "\n--- Book Options ---\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. Edit Book\n";
        std::cout << "3. Delete Book\n";
        std::cout << "4. Print All Books\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "Enter choice: ";

        // Read and validate user's choice
        choice = readChoice();

        // Execute action based on user's selection
        switch (choice) {
        case 1:
            // Add a new book to the collection
            books.AddBook();
            break;
        case 2:
            // Edit an existing book's information
            books.EditBook();
            break;
        case 3:
            // Remove a book from the collection
            books.DeleteBook();
            break;
        case 4:
            // Display all books in the library
            books.PrintAllBooks();
            break;
        case 5:
            // Exit book menu and return to main menu
            std::cout << "Returning to Main Menu...\n";
            break;
        default:
            // Handle invalid menu selections
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5); // Continue until user selects option 5
}

/**
 * Displays and handles the Loan management submenu
 * Manages book checkouts, check-ins, and loan status tracking
 *
 * @param loans - Reference to LoansCollection object for managing book loans
 * @param patrons - Reference to PatronsCollection for accessing patron information
 * @param books - Reference to BooksCollection for accessing book information
 */
void loanOptions(LoansCollection& loans, PatronsCollection& patrons, BooksCollection& books) {
    int choice;

    // Loop until user chooses to return to main menu (option 5)
    do {
        // Display loan management menu options
        std::cout << "\n--- Loan Options ---\n";
        std::cout << "1. Check Out Book\n";
        std::cout << "2. Check In Book\n";
        std::cout << "3. List All Overdue Books\n";
        std::cout << "4. List All Checked Out Books\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "Enter choice: ";

        // Read and validate user's choice
        choice = readChoice();

        // Execute action based on user's selection
        switch (choice) {
        case 1:
            // Process a book checkout (requires patron and book information)
            loans.CheckOutBook(patrons, books);
            break;
        case 2:
            // Process a book return/check-in (requires patron and book information)
            loans.CheckInBook(patrons, books);
            break;
        case 3:
            // Display all books that are past their due date
            loans.ListAllOverdueBooks();
            break;
        case 4:
            // Display all currently checked out books (requires book collection for details)
            loans.ListAllCheckedOutBooks(books);
            break;
        case 5:
            // Exit loan menu and return to main menu
            std::cout << "Returning to Main Menu...\n";
            break;
        default:
            // Handle invalid menu selections
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5); // Continue until user selects option 5
}

/**
 * Main entry point for the Library Management System
 * Initializes collections and displays the main menu
 *
 * @return int - Exit status code (0 for successful execution)
 */
int main() {
    // Initialize collection objects to manage patrons, books, and loans
    // Constructors automatically load data from JSON files if they exist
    PatronsCollection patrons;
    BooksCollection books;
    LoansCollection loans;

    // Inform user that saved data has been loaded
    std::cout << "Loaded saved data from JSON files (if available).\n";

    int choice;

    // Main program loop - continues until user chooses to exit (option 4)
    do {
        // Display main menu options
        std::cout << "\n--- Library Management System ---\n";
        std::cout << "1. Patron Options\n";
        std::cout << "2. Book Options\n";
        std::cout << "3. Loan Options\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice: ";

        // Read and validate user's choice
        choice = readChoice();

        // Direct user to appropriate submenu based on selection
        switch (choice) {
        case 1:
            // Enter patron management submenu
            patronOptions(patrons);
            break;
        case 2:
            // Enter book management submenu
            bookOptions(books);
            break;
        case 3:
            // Enter loan management submenu
            loanOptions(loans, patrons, books);
            break;
        case 4:
            // Exit the program
            std::cout << "Exiting Library Management System. Goodbye!\n";
            break;
        default:
            // Handle invalid menu selections
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4); // Continue until user selects option 4 (Exit)

    // Inform user that data is being saved before program terminates
    // Actual saving is likely handled by destructors of collection objects
    std::cout << "Saving data to JSON files...\n";

    return 0; // Return success status
}
