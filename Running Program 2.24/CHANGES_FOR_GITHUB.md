# GitHub Change Notes

1) Overdue Testing Option

Files Changed
- `main.cpp`
- `LoansCollection.h`
- `LoansCollection.cpp`

Summary
- Added a new Loan menu option:
  - `5. Mark Loan Overdue (Testing)`
- Added a new method:
  - `MarkLoanAsOverdueForTesting(PatronsCollection&, BooksCollection&)`
- The testing method finds an active loan and sets due date to 3 days in the past so overdue math can be tested quickly.
- Added status refresh before list views:
  - `List All Overdue Books`
  - `List All Checked Out Books`

2) Patron Fine Calculation Fix

Problem
- Overdue status was shown correctly, but `Patron` fines stayed at `$0`.

Files Changed
- `main.cpp`
- `LoansCollection.h`
- `LoansCollection.cpp`
- `PatronsCollection.h`
- `PatronsCollection.cpp`

Summary
- Added `LoansCollection::RecalculatePatronFines(PatronsCollection&)`:
  - Clears patron fines.
  - Recalculates fines from active overdue loans using:
    - `daysOverdue * kDailyFineAmount`
  - `kDailyFineAmount` is currently `$1.00/day`.
- Added `PatronsCollection` helpers:
  - `ResetAllFines()`
  - `AddFineToPatron(int patronID, float amount)`
- Hooked recalculation into loan and menu flows so patron fines are current when displayed:
  - before checkout validation
  - after check-in
  - before overdue/checked-out listings
  - after forcing overdue in testing mode
  - before entering Patron Options from main menu

Suggested Commit Messages
- `Add overdue testing menu option for loan due-date math validation`
- `Fix patron fine balances by recalculating from overdue loan days`
