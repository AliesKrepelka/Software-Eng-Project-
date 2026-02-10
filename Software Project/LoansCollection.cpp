#include "LoansCollection.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>

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
    std::tm dueCopy = dueDate;
    std::tm curCopy = currentDate;
    std::time_t due_time = std::mktime(&dueCopy);
    std::time_t current_time = std::mktime(&curCopy);
    const double secondsPerDay = 60 * 60 * 24;
    return static_cast<int>(std::difftime(due_time, current_time) / secondsPerDay);
}

static bool extractIntField(const std::string& objectText, const std::string& key, int& value) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    if (!std::regex_search(objectText, match, pattern)) {
        return false;
    }
    value = std::stoi(match[1].str());
    return true;
}

LoansCollection::LoansCollection() : dataFilePath("loans.json") {
    LoadFromJsonFile(dataFilePath);
}

LoansCollection::~LoansCollection() {
    SaveToJsonFile(dataFilePath);
    for (auto* loan : loansList) {
        delete loan;
    }
    loansList.clear();
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
        Loans* toDelete = *it;
        loansList.erase(it);
        delete toDelete;
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
        if (calculateDaysDifference(dueDate, today) < 0) {
            std::cout << "Loan ID " << loan->getLoanID() << " is overdue.\n";
        }
    }
}

void LoansCollection::ListAllCheckedOutBooks(BooksCollection &allBooks) {
    if (loansList.empty()) {
        std::cout << "No books currently checked out.\n";
        return;
    }

    std::cout << "\n--- All Checked Out Books ---\n";
    for (auto* loan : loansList) {
        Books* book = allBooks.FindBookByID(loan->getBookID());
        if (!book) {
            std::cout << "Book ID: " << loan->getBookID() << " (Record Missing)\n";
            continue;
        }
        
        std::tm dueDate = loan->getDueDate();
        std::tm today = getCurrentDate();
        int daysUntilDue = calculateDaysDifference(dueDate, today);
        
        std::cout << "Book: " << book->getTitle() << " | Author: " << book->getAuthor()
                  << " | Due: " << tmToString(dueDate);
        
        if (daysUntilDue < 0) {
            std::cout << " (OVERDUE by " << (-daysUntilDue) << " days)";
        } else if (daysUntilDue == 0) {
            std::cout << " (Due Today)";
        } else {
            std::cout << " (Due in " << daysUntilDue << " days)";
        }
        std::cout << "\n";
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
            if (!book) {
                std::cout << " - Book record missing for ID: " << loan->getBookID() << "\n";
                continue;
            }
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
        if (loan->getPatronID() == patronID) {
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
        if (calculateDaysDifference(loan->getDueDate(), today) < 0) {
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
    std::mktime(&newDueDate); // Normalize the date
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

bool LoansCollection::SaveToJsonFile(const std::string& filePath) const {
    std::ofstream out(filePath, std::ios::trunc);
    if (!out) {
        return false;
    }

    out << "{\n";
    out << "  \"loans\": [\n";
    for (size_t i = 0; i < loansList.size(); ++i) {
        const Loans* loan = loansList[i];
        std::tm dueDate = loan->getDueDate();

        out << "    {\n";
        out << "      \"loanID\": " << loan->getLoanID() << ",\n";
        out << "      \"bookID\": " << loan->getBookID() << ",\n";
        out << "      \"patronID\": " << loan->getPatronID() << ",\n";
        out << "      \"dueYear\": " << (dueDate.tm_year + 1900) << ",\n";
        out << "      \"dueMonth\": " << (dueDate.tm_mon + 1) << ",\n";
        out << "      \"dueDay\": " << dueDate.tm_mday << ",\n";
        out << "      \"status\": " << static_cast<int>(loan->getStatus()) << "\n";
        out << "    }";
        if (i + 1 < loansList.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    return true;
}

bool LoansCollection::LoadFromJsonFile(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in) {
        return false;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string content = buffer.str();

    for (auto* loan : loansList) {
        delete loan;
    }
    loansList.clear();

    int maxLoanID = 0;
    std::regex objectPattern("\\{[^\\{\\}]*\\}");
    auto begin = std::sregex_iterator(content.begin(), content.end(), objectPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        const std::string objectText = it->str();

        int loanID = 0;
        int bookID = 0;
        int patronID = 0;
        int dueYear = 0;
        int dueMonth = 0;
        int dueDay = 0;
        int status = 0;

        bool ok = extractIntField(objectText, "loanID", loanID)
            && extractIntField(objectText, "bookID", bookID)
            && extractIntField(objectText, "patronID", patronID)
            && extractIntField(objectText, "dueYear", dueYear)
            && extractIntField(objectText, "dueMonth", dueMonth)
            && extractIntField(objectText, "dueDay", dueDay)
            && extractIntField(objectText, "status", status);

        if (!ok) {
            continue;
        }

        std::tm dueDate = {};
        dueDate.tm_year = dueYear - 1900;
        dueDate.tm_mon = dueMonth - 1;
        dueDate.tm_mday = dueDay;
        std::mktime(&dueDate);

        Loans* loadedLoan = new Loans(bookID, patronID, dueDate);
        loadedLoan->setLoanID(loanID);
        loadedLoan->setStatus(static_cast<Loans::LoanStatus>(status));
        loansList.push_back(loadedLoan);

        if (loanID > maxLoanID) {
            maxLoanID = loanID;
        }
    }

    Loans::setNextLoanID(maxLoanID + 1);
    return true;
}
