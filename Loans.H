#ifndef LOANS_H
#define LOANS_H

#include <string>
#include <ctime>

class Loans {
public:
    enum LoanStatus { NORMAL, OVERDUE };
    
    Loans(int bookID, int patronID, std::tm dueDate);

    int getLoanID() const;
    int getBookID() const;
    int getPatronID() const;
    std::tm getDueDate() const;
    LoanStatus getStatus() const;

    void setLoanID(int id);
    void setBookID(int id);
    void setPatronID(int id);
    void setDueDate(const std::tm& date);
    void setStatus(LoanStatus status);

private:
    static int nextLoanID; // Static member to track the next available loan ID
    int loanID;
    int bookID;
    int patronID;
    std::tm dueDate;
    LoanStatus status;
};

#endif // LOANS_H
