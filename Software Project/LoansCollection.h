#ifndef LOANSCOLLECTION_H
#define LOANSCOLLECTION_H

#include <vector>
#include "Loans.h"
#include "PatronsCollection.h"
#include "BooksCollection.h"

// Manages a collection of Loans, facilitating operations such as checking out
// and checking in books, listing overdue books
class LoansCollection {
public:
    LoansCollection();
    ~LoansCollection();

    // Checks out a book for a patron
    void CheckOutBook(PatronsCollection &allPatrons, BooksCollection &allBooks);

    // Checks in a book from a patron
    void CheckInBook(PatronsCollection &allPatrons, BooksCollection &allBooks);

    // Lists all overdue books with patron and book details
    void ListAllOverdueBooks(PatronsCollection &allPatrons, BooksCollection &allBooks);

    // Lists all currently checked out books
    void ListAllCheckedOutBooks(BooksCollection &allBooks);

    // Lists all books checked out to a specific patron
    void ListBooksForPatron(PatronsCollection &allPatrons, BooksCollection &allBooks);

    // Lists all books checked out to a patron by their ID
    void ListBooksForPatronByID(PatronsCollection &allPatrons, BooksCollection &allBooks, int patronID);

    // Updates the loan status based on the current date
    void AutoUpdateLoanStatus();

    // Edits a loan, allowing for rechecks
    void EditLoan(PatronsCollection &allPatrons, BooksCollection &allBooks);

    // Reports a book as lost
    void ReportLost(PatronsCollection &allPatrons, BooksCollection &allBooks);
    bool LoadFromJsonFile(const std::string& filePath);
    bool SaveToJsonFile(const std::string& filePath) const;

private:
    std::vector<Loans*> loansList; // Stores pointers to Loans
    std::string dataFilePath;
};

#endif // LOANSCOLLECTION_H
