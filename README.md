# Software-Eng-Project COSC 4301 01
Changes Summary
1. Loans.h — Added RETURNED status
Changed: enum LoanStatus { NORMAL, OVERDUE };
To: enum LoanStatus { NORMAL, OVERDUE, RETURNED };

2. LoansCollection.cpp — Fixed mktime mutation (line ~18)
Changed: std::mktime(const_cast<std::tm*>(&dueDate)) and const_cast on currentDate
To: Create copies first (std::tm dueCopy = dueDate; std::tm curCopy = currentDate;) then call mktime on copies

3. LoansCollection.cpp — Fixed use-after-erase bug in CheckInBook() (line ~63)
Changed:
loansList.erase(it);delete *it;
To:
Loans* toDelete = *it;loansList.erase(it);delete toDelete;

4. LoansCollection.cpp — Fixed overdue calculation in ListAllOverdueBooks() (line ~81)
Changed: if (calculateDaysDifference(dueDate, today) > 0)
To: if (calculateDaysDifference(dueDate, today) < 0)

5. LoansCollection.cpp — Added null check in ListBooksForPatron() (line ~95)
Added:
if (!book) {    std::cout << " - Book record missing for ID: " << loan->getBookID() << "\n";    continue;}
Before dereferencing book pointer

6. LoansCollection.cpp — Removed RETURNED check in ListBooksForPatronByID() (line ~115)
Changed: if (loan->getPatronID() == patronID && loan->getStatus() != Loans::LoanStatus::RETURNED)
To: if (loan->getPatronID() == patronID)

7. LoansCollection.cpp — Fixed overdue calculation in AutoUpdateLoanStatus() (line ~137)
Changed: if (calculateDaysDifference(loan->getDueDate(), today) > 0)
To: if (calculateDaysDifference(loan->getDueDate(), today) < 0)

8. LoansCollection.cpp — Added date normalization in EditLoan() (line ~169)
Added: std::mktime(&newDueDate); after newDueDate.tm_mday += 7;

9. Books.h — Added static ID member
Added: static int nextBookID; in the public section of Books class

10. Books.cpp — Initialized static member
Added: int Books::nextBookID = 0; at the top after includes

11. BooksCollection.cpp — Fixed ID collision in AddBook() (line ~16)
Changed: int isbn, id = booksList.size();
To: Use int id = Books::nextBookID++; instead of size()
