#include "LoansCollection.h"
#include <iostream>
#include <ctime>
#include <algorithm>

std::tm getCurrentDate() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    return tm;
}

std::string tmToString(const std::tm& date) {
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y", &date);
    return std::string(buffer);
}

int calculateDaysDifference(const std::tm& dueDate, const std::tm& currentDate) {
    std::time_t due_time = std::mktime(const_cast<std::tm*>(&dueDate));
    std::time_t current_time = std::mktime(const_cast<std::tm*>(&currentDate));
    const double secondsPerDay = 60 * 60 * 24;
    return static_cast<int>(std::difftime(due_time, current_time) / secondsPerDay);
}

void LoansCollection::CheckOutBook(PatronsCollection &allPatrons, BooksCollection &allBooks) {
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron || patron->getNumBooks() >= 6 || patron->getFineBalance() > 0) {
        std::cout << "Checkout conditions not met.\n";
        return;
    }

    Books* book = allBooks.PromptForSearchMechanism();
    if (!book || book->getCurrentBookStatus() != Books::IN) {
        std::cout << "Book not available.\n";
        return;
    }

    std::tm dueDate = getCurrentDate(); 
    dueDate.tm_mday += 7; 
    mktime(&dueDate); 
    Loans* loan = new Loans(book->getLibraryID(), patron->getPatronID(), dueDate);

    loan->setBookID(book->getLibraryID());
    loan->setPatronID(patron->getPatronID());
    loansList.push_back(loan);

    book->setCurrentBookStatus(Books::OUT);
    patron->setNumBooks(patron->getNumBooks() + 1);

    std::cout << "Book checked out successfully.\n";
}

void LoansCollection::CheckInBook(PatronsCollection &allPatrons, BooksCollection &allBooks) {
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (patron == nullptr) {
        std::cout << "Patron not found.\n";
        return;
    }

    Books* book = allBooks.PromptForSearchMechanism();
    if (book == nullptr) {
        std::cout << "Book not found.\n";
        return;
    }

    auto it = std::find_if(loansList.begin(), loansList.end(), [&](Loans* loan) {
        return loan->getPatronID() == patron->getPatronID() && loan->getBookID() == book->getLibraryID();
    });

    if (it != loansList.end()) {
        loansList.erase(it);
        delete *it; // Assuming dynamic allocation of loans
        book->setCurrentBookStatus(Books::IN);
        patron->setNumBooks(patron->getNumBooks() - 1);
        std::cout << "Book checked in successfully.\n";
    } else {
        std::cout << "Loan record not found.\n";
    }
}

void LoansCollection::ListAllOverdueBooks() {
    std::cout << "Overdue Books:\n";
    for (auto* loan : loansList) {
        std::tm dueDate = loan->getDueDate();
        std::tm today = getCurrentDate();
        if (calculateDaysDifference(dueDate, today) > 0) {
            std::cout << "Loan ID " << loan->getLoanID() << " is overdue.\n";
        }
    }
}

void LoansCollection::ListBooksForPatron(PatronsCollection &allPatrons, BooksCollection &allBooks) {
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron) {
        std::cout << "Patron not found.\n";
        return;
    }

    std::cout << "Books checked out by " << patron->getName() << ":\n";
    for (auto* loan : loansList) {
        if (loan->getPatronID() == patron->getPatronID()) {
            Books* book = allBooks.FindBookByID(loan->getBookID());
            std::cout << "Title: " << book->getTitle() << ", Due Date: " << tmToString(loan->getDueDate()) << "\n";
        }
    }
}

void LoansCollection::ListBooksForPatronByID(PatronsCollection &allPatrons, BooksCollection &allBooks, int patronID) {
    Patron* patron = allPatrons.FindPatronByID(patronID);
    if (patron == nullptr) {
        std::cout << "Patron not found.\n";
        return;
    }

    std::cout << "Books checked out by " << patron->getName() << " (ID: " << patronID << "):\n";
    bool found = false;
    for (auto* loan : loansList) {
        if (loan->getPatronID() == patronID && loan->getStatus() != Loans::LoanStatus::RETURNED) {
            Books* book = allBooks.FindBookByID(loan->getBookID());
            if (book) {
                std::cout << " - Title: " << book->getTitle() << ", Due Date: " << tmToString(loan->getDueDate())
                          << ", Status: " << (loan->getStatus() == Loans::OVERDUE ? "Overdue" : "Checked Out") << "\n";
                found = true;
            }
        }
    }
    if (!found) {
        std::cout << "No books currently checked out by this patron.\n";
    }
}

void LoansCollection::AutoUpdateLoanStatus() {
    std::tm today = getCurrentDate();
    for (auto* loan : loansList) {
        if (calculateDaysDifference(loan->getDueDate(), today) > 0) {
            loan->setStatus(Loans::OVERDUE);
        }
    }
}


void LoansCollection::EditLoan(PatronsCollection &allPatrons, BooksCollection &allBooks) {
    std::cout << "\n--- Editing a Loan Record ---\n";
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron) {
        std::cout << "Patron not found.\n";
        return;
    }

    Books* book = allBooks.PromptForSearchMechanism();
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    auto it = std::find_if(loansList.begin(), loansList.end(), [&](Loans* loan) {
        return loan->getPatronID() == patron->getPatronID() && loan->getBookID() == book->getLibraryID();
    });

    if (it == loansList.end()) {
        std::cout << "No active loan found for this book and patron combination.\n";
        return;
    }

    std::tm newDueDate = getCurrentDate();
    newDueDate.tm_mday += 7; // Extending the due date by 7 days
    (*it)->setDueDate(newDueDate);

    std::cout << "Loan record updated. New due date: " << tmToString(newDueDate) << ".\n";
}

void LoansCollection::ReportLost(PatronsCollection &allPatrons, BooksCollection &allBooks) {
    std::cout << "\n--- Reporting a Book as Lost ---\n";
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron) {
        std::cout << "Patron not found.\n";
        return;
    }

    Books* book = allBooks.PromptForSearchMechanism();
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    auto it = std::find_if(loansList.begin(), loansList.end(), [&](Loans* loan) {
        return loan->getBookID() == book->getLibraryID();
    });

    if (it != loansList.end()) {
        book->setCurrentBookStatus(Books::LOST);
        std::cout << "Book marked as lost.\n";
    } else {
        std::cout << "Loan record for the book not found.\n";
    }
}
