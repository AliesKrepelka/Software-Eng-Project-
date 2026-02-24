#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include "PatronsCollection.h"
#include "BooksCollection.h"
#include "LoansCollection.h"

// Box drawing characters (simple ASCII - no Unicode)
#define HORIZONTAL_LINE "-"
#define VERTICAL_LINE "|"
#define TOP_LEFT_CORNER "+"
#define TOP_RIGHT_CORNER "+"
#define BOTTOM_LEFT_CORNER "+"
#define BOTTOM_RIGHT_CORNER "+"
#define MIDDLE_LEFT_CORNER "+"
#define MIDDLE_RIGHT_CORNER "+"

const int BOX_WIDTH = 70;

// Helper function to print a horizontal line
void printHorizontalLine(const std::string& left, const std::string& middle, const std::string& right) {
    std::cout << left;
    for (int i = 0; i < BOX_WIDTH - 2; i++) {
        std::cout << middle;
    }
    std::cout << right << std::endl;
}

// Helper function to print a formatted box header
void printHeader(const std::string& title) {
    printHorizontalLine(TOP_LEFT_CORNER, HORIZONTAL_LINE, TOP_RIGHT_CORNER);
    std::cout << VERTICAL_LINE << " " << std::left << std::setw(BOX_WIDTH - 4) << title << " " << VERTICAL_LINE << std::endl;
    printHorizontalLine(MIDDLE_LEFT_CORNER, HORIZONTAL_LINE, MIDDLE_RIGHT_CORNER);
}

// Helper function to print a formatted box footer
void printFooter() {
    printHorizontalLine(BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER);
    std::cout << std::endl;
}

// Helper function to print a menu with box formatting
void printMenu(const std::string& title, const std::vector<std::string>& options) {
    printHeader(title);
    for (size_t i = 0; i < options.size(); i++) {
        std::string optionText = std::to_string(i + 1) + ". " + options[i];
        std::cout << VERTICAL_LINE << " " << std::left << std::setw(BOX_WIDTH - 4) << optionText << " " << VERTICAL_LINE << std::endl;
    }
    printHorizontalLine(MIDDLE_LEFT_CORNER, HORIZONTAL_LINE, MIDDLE_RIGHT_CORNER);
    std::cout << VERTICAL_LINE << " Enter choice:" << std::string(BOX_WIDTH - 16, ' ') << VERTICAL_LINE << std::endl;
    printHorizontalLine(BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER);
    std::cout << "> ";
}

// Helper function to print an info message in a box
void printInfo(const std::string& message) {
    printHorizontalLine(TOP_LEFT_CORNER, HORIZONTAL_LINE, TOP_RIGHT_CORNER);
    std::cout << VERTICAL_LINE << " " << std::left << std::setw(BOX_WIDTH - 4) << message << " " << VERTICAL_LINE << std::endl;
    printHorizontalLine(BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER);
    std::cout << std::endl;
}

// Helper function to print a success message in a box
void printSuccess(const std::string& message) {
    printHorizontalLine(TOP_LEFT_CORNER, HORIZONTAL_LINE, TOP_RIGHT_CORNER);
    std::cout << VERTICAL_LINE << " ✓ " << std::left << std::setw(BOX_WIDTH - 6) << message << " " << VERTICAL_LINE << std::endl;
    printHorizontalLine(BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER);
    std::cout << std::endl;
}

// Helper function to print an error message in a box
void printError(const std::string& message) {
    printHorizontalLine(TOP_LEFT_CORNER, HORIZONTAL_LINE, TOP_RIGHT_CORNER);
    std::cout << VERTICAL_LINE << " ✗ " << std::left << std::setw(BOX_WIDTH - 6) << message << " " << VERTICAL_LINE << std::endl;
    printHorizontalLine(BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER);
    std::cout << std::endl;
}

// Helper function to wait for user input
void waitForEnter() {
    std::cout << std::endl << VERTICAL_LINE << " Press Enter to continue... " << VERTICAL_LINE << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper function to safely read an integer from user input
int readChoice() {
    int choice;
    while (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        printError("Invalid input. Please enter a number.");
        std::cout << "> ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void patronOptions(PatronsCollection& patrons) {
    int choice;
    std::vector<std::string> options = {
        "Add Patron",
        "Edit Patron",
        "Delete Patron",
        "Print All Patrons",
        "Return to Main Menu"
    };

    do {
        printMenu("Patron Options", options);
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
            printInfo("Returning to Main Menu...");
            break;
        default:
            printError("Invalid choice. Please try again.");
        }
        if (choice != 5) {
            waitForEnter();
        }
    } while (choice != 5);
}

void bookOptions(BooksCollection& books) {
    int choice;
    std::vector<std::string> options = {
        "Add Book",
        "Edit Book",
        "Delete Book",
        "Print All Books",
        "Return to Main Menu"
    };

    do {
        printMenu("Book Options", options);
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
            printInfo("Returning to Main Menu...");
            break;
        default:
            printError("Invalid choice. Please try again.");
        }
        if (choice != 5) {
            waitForEnter();
        }
    } while (choice != 5);
}

void loanOptions(LoansCollection& loans, PatronsCollection& patrons, BooksCollection& books) {
    int choice;
    std::vector<std::string> options = {
        "Check Out Book",
        "Check In Book",
        "List All Overdue Books",
        "List All Checked Out Books",
        "Return to Main Menu"
    };

    do {
        printMenu("Loan Options", options);
        choice = readChoice();

        switch (choice) {
        case 1:
            loans.AutoUpdateLoanStatus();
            loans.CheckOutBook(patrons, books);
            break;
        case 2:
            loans.CheckInBook(patrons, books);
            loans.AutoUpdateLoanStatus();
            break;
        case 3:
            loans.AutoUpdateLoanStatus();
            loans.ListAllOverdueBooks(patrons, books);
            break;
        case 4:
            loans.AutoUpdateLoanStatus();
            loans.ListAllCheckedOutBooks(books);
            break;
        case 5:
            printInfo("Returning to Main Menu...");
            break;
        default:
            printError("Invalid choice. Please try again.");
        }
        if (choice != 5) {
            waitForEnter();
        }
    } while (choice != 5);
}

int main() {
    PatronsCollection patrons;
    BooksCollection books;
    LoansCollection loans;

    printInfo("Loaded saved data from JSON files (if available).");

    int choice;
    std::vector<std::string> mainOptions = {
        "Patron Options",
        "Book Options",
        "Loan Options",
        "Exit"
    };

    do {
        printMenu("Library Management System", mainOptions);
        choice = readChoice();

        switch (choice) {
        case 1:
            loans.AutoUpdateLoanStatus();
            patronOptions(patrons);
            break;
        case 2:
            bookOptions(books);
            break;
        case 3:
            loanOptions(loans, patrons, books);
            break;
        case 4:
            printSuccess("Exiting Library Management System. Goodbye!");
            break;
        default:
            printError("Invalid choice. Please try again.");
            waitForEnter();
        }
    } while (choice != 4);

    printInfo("Saving data to JSON files...");
    return 0;
}
