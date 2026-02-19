#include <iostream>
#include <limits>
#include "PatronsCollection.h"
#include "BooksCollection.h"
#include "LoansCollection.h"

// Helper function to safely read an integer from user input
int readChoice() {
    int choice;
    while (!(std::cin >> choice)) {
        // Clear error flag and input buffer
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please enter a number: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear remaining buffer
    return choice;
}

void patronOptions(PatronsCollection& patrons) {
    int choice;
    do {
        std::cout << "\n--- Patron Options ---\n";
        std::cout << "1. Add Patron\n";
        std::cout << "2. Edit Patron\n";
        std::cout << "3. Delete Patron\n";
        std::cout << "4. Print All Patrons\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1:
                patrons.AddPatron();
                break;
            case 2:
                patrons.EditPatron();
                break;
            case 3:
                patrons.DeletePatron();
                break;
            case 4:
                patrons.PrintAllPatrons();
                break;
            case 5:
                std::cout << "Returning to Main Menu...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5);
}

void bookOptions(BooksCollection& books) {
    int choice;
    do {
        std::cout << "\n--- Book Options ---\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. Edit Book\n";
        std::cout << "3. Delete Book\n";
        std::cout << "4. Print All Books\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1:
                books.AddBook();
                break;
            case 2:
                books.EditBook();
                break;
            case 3:
                books.DeleteBook();
                break;
            case 4:
                books.PrintAllBooks();
                break;
            case 5:
                std::cout << "Returning to Main Menu...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5);
}

void loanOptions(LoansCollection& loans, PatronsCollection& patrons, BooksCollection& books) {
    int choice;
    do {
        std::cout << "\n--- Loan Options ---\n";
        std::cout << "1. Check Out Book\n";
        std::cout << "2. Check In Book\n";
        std::cout << "3. List All Overdue Books\n";
        std::cout << "4. List All Checked Out Books\n";
        std::cout << "5. Mark Loan Overdue (Testing)\n";
        std::cout << "6. Return to Main Menu\n";
        std::cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1:
                loans.AutoUpdateLoanStatus();
                loans.RecalculatePatronFines(patrons);
                loans.CheckOutBook(patrons, books);
                break;
            case 2:
                loans.CheckInBook(patrons, books);
                loans.AutoUpdateLoanStatus();
                loans.RecalculatePatronFines(patrons);
                break;
            case 3:
                loans.AutoUpdateLoanStatus();
                loans.RecalculatePatronFines(patrons);
                loans.ListAllOverdueBooks();
                break;
            case 4:
                loans.AutoUpdateLoanStatus();
                loans.RecalculatePatronFines(patrons);
                loans.ListAllCheckedOutBooks(books);
                break;
            case 5:
                loans.MarkLoanAsOverdueForTesting(patrons, books);
                break;
            case 6:
                std::cout << "Returning to Main Menu...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 6);
}

int main() {
    PatronsCollection patrons;
    BooksCollection books;
    LoansCollection loans;

    std::cout << "Loaded saved data from JSON files (if available).\n";

    int choice;
    do {
        std::cout << "\n--- Library Management System ---\n";
        std::cout << "1. Patron Options\n";
        std::cout << "2. Book Options\n";
        std::cout << "3. Loan Options\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1:
                loans.AutoUpdateLoanStatus();
                loans.RecalculatePatronFines(patrons);
                patronOptions(patrons);
                break;
            case 2:
                bookOptions(books);
                break;
            case 3:
                loanOptions(loans, patrons, books);
                break;
            case 4:
                std::cout << "Exiting Library Management System. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    std::cout << "Saving data to JSON files...\n";
    return 0;
}
