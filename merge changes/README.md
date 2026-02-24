Merge Changes Summary (Maddie + Running Program 2.19 -> Software Project)

Target merged files:
- Software Project/main.cpp
- Software Project/LoansCollection.h
- Software Project/LoansCollection.cpp
- (PatronsCollection.cpp comments were preserved and not modified)

1) main.cpp merge updates
- Updated loan menu overdue action to call the overdue listing with full context:
  - from: loans.ListAllOverdueBooks();
  - to:   loans.ListAllOverdueBooks(patrons, books);
- Preserved existing Software Project commenting/doc style.

2) LoansCollection.h merge updates
- Updated method signature to support richer overdue reporting:
  - from: void ListAllOverdueBooks();
  - to:   void ListAllOverdueBooks(PatronsCollection &allPatrons, BooksCollection &allBooks);

3) LoansCollection.cpp merge updates
- Added include:
  - #include <cmath>

- Date difference logic improved (from merged Maddie behavior):
  - Normalizes both dates to noon before diff calculation to reduce time-of-day rollover issues.
  - Uses 86400.0 and std::floor-based day conversion.

- Check-in behavior enhanced:
  - On overdue return, computes overdue days and applies fine at $0.50/day.
  - Rounds fine balance to cents.
  - Prints fine and updated balance feedback.
  - Saves patrons data after fine update.

- Overdue listing enhanced:
  - Reports borrower name + patron ID and book title when available.
  - Falls back to "Unknown" if record missing.

4) Comments/style preservation
- Kept Software Project explanatory comments and structure while integrating merged behavior changes.

5) Build validation used for merged result
- g++ -std=c++17 -Wall -Wextra -pedantic "Software Project"/*.cpp -o /tmp/software_project_app
- Build succeeded.
