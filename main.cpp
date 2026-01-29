#include <iostream>
#include <limits>
#include "PatronsCollection.h"
#include "BooksCollection.h"
#include "LoansCollection.h"

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
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer

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
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer

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
        std::cout << "4. Return to Main Menu\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer

        switch (choice) {
            case 1:
                loans.CheckOutBook(patrons, books);
                break;
            case 2:
                loans.CheckInBook(patrons, books);
                break;
            case 3:
                loans.ListAllOverdueBooks();
                break;
            case 4:
                std::cout << "Returning to Main Menu...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);
}

int main() {
    PatronsCollection patrons;
    BooksCollection books;
    LoansCollection loans;

    int choice;
    do {
        std::cout << "\n--- Library Management System ---\n";
        std::cout << "1. Patron Options\n";
        std::cout << "2. Book Options\n";
        std::cout << "3. Loan Options\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer

        switch (choice) {
            case 1:
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

    return 0;
}
