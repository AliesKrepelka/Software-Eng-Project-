#include "Loans.h"

int Loans::nextLoanID = 1; // Initialize static member to track loan IDs

Loans::Loans(int bookID, int patronID, std::tm dueDate)
    : loanID(nextLoanID++), bookID(bookID), patronID(patronID), dueDate(dueDate), status(NORMAL) {}

int Loans::getLoanID() const { return loanID; }
int Loans::getBookID() const { return bookID; }
int Loans::getPatronID() const { return patronID; }
std::tm Loans::getDueDate() const { return dueDate; }
Loans::LoanStatus Loans::getStatus() const { return status; }

void Loans::setLoanID(int id) { loanID = id; }
void Loans::setBookID(int id) { bookID = id; }
void Loans::setPatronID(int id) { patronID = id; }
void Loans::setDueDate(const std::tm& date) { dueDate = date; }
void Loans::setStatus(LoanStatus status) { this->status = status; }
