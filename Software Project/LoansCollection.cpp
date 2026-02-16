#include "LoansCollection.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>

/**
 * Gets the current system date and time
 *
 * @return std::tm - Structure containing current date/time information
 */
std::tm getCurrentDate() {
    // Get current time as time_t
    std::time_t t = std::time(nullptr);

    // Convert to local time structure
    std::tm tm = *std::localtime(&t);

    return tm;
}

/**
 * Converts a tm structure to a formatted date string (DD-MM-YYYY)
 *
 * @param date - The date structure to convert
 * @return std::string - Formatted date string
 */
std::string tmToString(const std::tm& date) {
    char buffer[11]; // Buffer to hold formatted date (10 chars + null terminator)

    // Format date as DD-MM-YYYY
    strftime(buffer, sizeof(buffer), "%d-%m-%Y", &date);

    return std::string(buffer);
}

/**
 * Calculates the number of days between two dates
 * Positive result means dueDate is in the future, negative means past due
 *
 * @param dueDate - The target/due date
 * @param currentDate - The reference date (typically today)
 * @return int - Number of days difference (positive = future, negative = overdue)
 */
int calculateDaysDifference(const std::tm& dueDate, const std::tm& currentDate) {
    // Create copies since mktime modifies the structure
    std::tm dueCopy = dueDate;
    std::tm curCopy = currentDate;

    // Convert tm structures to time_t (seconds since epoch)
    std::time_t due_time = std::mktime(&dueCopy);
    std::time_t current_time = std::mktime(&curCopy);

    // Define seconds in a day
    const double secondsPerDay = 60 * 60 * 24;

    // Calculate difference in days (difftime returns seconds)
    return static_cast<int>(std::difftime(due_time, current_time) / secondsPerDay);
}

/**
 * Extracts an integer value from a JSON object text using regex
 *
 * @param objectText - The JSON object as a string
 * @param key - The JSON key to search for
 * @param value - Reference to store the extracted integer value
 * @return bool - True if field found and extracted successfully, false otherwise
 */
static bool extractIntField(const std::string& objectText, const std::string& key, int& value) {
    // Create regex pattern to match: "key": number (including negative numbers)
    std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
    std::smatch match;

    // Search for pattern in object text
    if (!std::regex_search(objectText, match, pattern)) {
        return false; // Field not found
    }

    // Convert captured string to integer
    value = std::stoi(match[1].str());
    return true;
}

/**
 * Constructor - Initializes the LoansCollection and loads data from JSON file
 * Sets the default file path and attempts to load existing loan data
 */
LoansCollection::LoansCollection() : dataFilePath("loans.json") {
    // Load existing loan data from file if it exists
    LoadFromJsonFile(dataFilePath);
}

/**
 * Destructor - Saves data to JSON file and cleans up dynamically allocated memory
 * Ensures all loan data is persisted and no memory leaks occur
 */
LoansCollection::~LoansCollection() {
    // Save current loan data to file before destruction
    SaveToJsonFile(dataFilePath);

    // Delete all dynamically allocated Loans objects
    for (auto* loan : loansList) {
        delete loan;
    }

    // Clear the vector
    loansList.clear();
}

/**
 * Processes a book checkout for a patron
 * Validates patron eligibility (max 6 books, no outstanding fines) and book availability
 * Creates a new loan record with a 7-day due date
 *
 * @param allPatrons - Reference to PatronsCollection for patron lookup and updates
 * @param allBooks - Reference to BooksCollection for book lookup and status updates
 */
void LoansCollection::CheckOutBook(PatronsCollection& allPatrons, BooksCollection& allBooks) {
    // Prompt user to find the patron checking out the book
    Patron* patron = allPatrons.PromptForSearchMechanism();

    // Validate patron conditions:
    // - Patron must exist
    // - Cannot have 6 or more books checked out
    // - Cannot have outstanding fines
    if (!patron || patron->getNumBooks() >= 6 || patron->getFineBalance() > 0) {
        std::cout << "Checkout conditions not met.\n";
        return;
    }

    // Prompt user to find the book to check out
    Books* book = allBooks.PromptForSearchMechanism();

    // Validate book conditions:
    // - Book must exist
    // - Book must be available (status = IN)
    if (!book || book->getCurrentBookStatus() != Books::IN) {
        std::cout << "Book not available.\n";
        return;
    }

    // Calculate due date (7 days from today)
    std::tm dueDate = getCurrentDate();
    dueDate.tm_mday += 7; // Add 7 days to current date
    mktime(&dueDate); // Normalize the date structure (handles month/year overflow)

    // Create new loan record
    Loans* loan = new Loans(book->getLibraryID(), patron->getPatronID(), dueDate);

    // Set loan details
    loan->setBookID(book->getLibraryID());
    loan->setPatronID(patron->getPatronID());

    // Add loan to collection
    loansList.push_back(loan);

    // Update book status to checked out
    book->setCurrentBookStatus(Books::OUT);

    // Increment patron's book count
    patron->setNumBooks(patron->getNumBooks() + 1);

    std::cout << "Book checked out successfully.\n";
}

/**
 * Processes a book check-in (return) for a patron
 * Finds the loan record, removes it, and updates book/patron status
 *
 * @param allPatrons - Reference to PatronsCollection for patron lookup and updates
 * @param allBooks - Reference to BooksCollection for book lookup and status updates
 */
void LoansCollection::CheckInBook(PatronsCollection& allPatrons, BooksCollection& allBooks) {
    // Prompt user to find the patron returning the book
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (patron == nullptr) {
        std::cout << "Patron not found.\n";
        return;
    }

    // Prompt user to find the book being returned
    Books* book = allBooks.PromptForSearchMechanism();
    if (book == nullptr) {
        std::cout << "Book not found.\n";
        return;
    }

    // Search for the loan record matching this patron and book
    // Using lambda function to check both patron ID and book ID
    auto it = std::find_if(loansList.begin(), loansList.end(), [&](Loans* loan) {
        return loan->getPatronID() == patron->getPatronID() && loan->getBookID() == book->getLibraryID();
        });

    // If loan record found, process the return
    if (it != loansList.end()) {
        // Store pointer to loan for deletion
        Loans* toDelete = *it;

        // Remove from collection
        loansList.erase(it);

        // Delete the loan object
        delete toDelete;

        // Update book status to available
        book->setCurrentBookStatus(Books::IN);

        // Decrement patron's book count
        patron->setNumBooks(patron->getNumBooks() - 1);

        std::cout << "Book checked in successfully.\n";
    }
    else {
        std::cout << "Loan record not found.\n";
    }
}

/**
 * Lists all books that are past their due date
 * Compares each loan's due date with current date
 */
void LoansCollection::ListAllOverdueBooks() {
    std::cout << "Overdue Books:\n";

    // Iterate through all loans
    for (auto* loan : loansList) {
        // Get the due date for this loan
        std::tm dueDate = loan->getDueDate();

        // Get current date
        std::tm today = getCurrentDate();

        // Calculate days difference (negative means overdue)
        if (calculateDaysDifference(dueDate, today) < 0) {
            std::cout << "Loan ID " << loan->getLoanID() << " is overdue.\n";
        }
    }
}

/**
 * Lists all currently checked out books with their due dates and status
 * Shows overdue status and days until/past due
 *
 * @param allBooks - Reference to BooksCollection to retrieve book details
 */
void LoansCollection::ListAllCheckedOutBooks(BooksCollection& allBooks) {
    // Check if there are any loans
    if (loansList.empty()) {
        std::cout << "No books currently checked out.\n";
        return;
    }

    std::cout << "\n--- All Checked Out Books ---\n";

    // Iterate through all loans
    for (auto* loan : loansList) {
        // Find the book record for this loan
        Books* book = allBooks.FindBookByID(loan->getBookID());

        // Handle case where book record is missing
        if (!book) {
            std::cout << "Book ID: " << loan->getBookID() << " (Record Missing)\n";
            continue;
        }

        // Get due date and calculate days until due
        std::tm dueDate = loan->getDueDate();
        std::tm today = getCurrentDate();
        int daysUntilDue = calculateDaysDifference(dueDate, today);

        // Display book information
        std::cout << "Book: " << book->getTitle() << " | Author: " << book->getAuthor()
            << " | Due: " << tmToString(dueDate);

        // Display due status with appropriate message
        if (daysUntilDue < 0) {
            // Book is overdue (negative days)
            std::cout << " (OVERDUE by " << (-daysUntilDue) << " days)";
        }
        else if (daysUntilDue == 0) {
            // Book is due today
            std::cout << " (Due Today)";
        }
        else {
            // Book is not yet due (positive days)
            std::cout << " (Due in " << daysUntilDue << " days)";
        }
        std::cout << "\n";
    }
}

/**
 * Lists all books currently checked out by a specific patron
 * Prompts user to search for patron first
 *
 * @param allPatrons - Reference to PatronsCollection for patron lookup
 * @param allBooks - Reference to BooksCollection to retrieve book details
 */
void LoansCollection::ListBooksForPatron(PatronsCollection& allPatrons, BooksCollection& allBooks) {
    // Prompt user to find the patron
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron) {
        std::cout << "Patron not found.\n";
        return;
    }

    std::cout << "Books checked out by " << patron->getName() << ":\n";

    // Iterate through all loans to find this patron's books
    for (auto* loan : loansList) {
        // Check if this loan belongs to the selected patron
        if (loan->getPatronID() == patron->getPatronID()) {
            // Find the book record
            Books* book = allBooks.FindBookByID(loan->getBookID());

            // Handle missing book record
            if (!book) {
                std::cout << " - Book record missing for ID: " << loan->getBookID() << "\n";
                continue;
            }

            // Display book information and due date
            std::cout << "Title: " << book->getTitle() << ", Due Date: " << tmToString(loan->getDueDate()) << "\n";
        }
    }
}

/**
 * Lists all books checked out by a patron using their patron ID directly
 * Does not prompt user - uses provided ID parameter
 *
 * @param allPatrons - Reference to PatronsCollection for patron lookup
 * @param allBooks - Reference to BooksCollection to retrieve book details
 * @param patronID - The ID of the patron to query
 */
void LoansCollection::ListBooksForPatronByID(PatronsCollection& allPatrons, BooksCollection& allBooks, int patronID) {
    // Find the patron by ID
    Patron* patron = allPatrons.FindPatronByID(patronID);
    if (patron == nullptr) {
        std::cout << "Patron not found.\n";
        return;
    }

    std::cout << "Books checked out by " << patron->getName() << " (ID: " << patronID << "):\n";

    bool found = false; // Track if any books were found

    // Iterate through all loans
    for (auto* loan : loansList) {
        // Check if this loan belongs to the specified patron
        if (loan->getPatronID() == patronID) {
            // Find the book record
            Books* book = allBooks.FindBookByID(loan->getBookID());

            if (book) {
                // Display detailed book information including status
                std::cout << " - Title: " << book->getTitle() << ", Due Date: " << tmToString(loan->getDueDate())
                    << ", Status: " << (loan->getStatus() == Loans::OVERDUE ? "Overdue" : "Checked Out") << "\n";
                found = true;
            }
        }
    }

    // Inform user if patron has no books checked out
    if (!found) {
        std::cout << "No books currently checked out by this patron.\n";
    }
}

/**
 * Automatically updates the status of all loans
 * Sets status to OVERDUE for loans past their due date
 * Should be called periodically to keep loan statuses current
 */
void LoansCollection::AutoUpdateLoanStatus() {
    // Get current date for comparison
    std::tm today = getCurrentDate();

    // Check each loan
    for (auto* loan : loansList) {
        // If due date has passed (negative days difference), mark as overdue
        if (calculateDaysDifference(loan->getDueDate(), today) < 0) {
            loan->setStatus(Loans::OVERDUE);
        }
    }
}

/**
 * Allows editing of a loan record (extends due date by 7 days)
 * Prompts for patron and book to find the specific loan
 *
 * @param allPatrons - Reference to PatronsCollection for patron lookup
 * @param allBooks - Reference to BooksCollection for book lookup
 */
void LoansCollection::EditLoan(PatronsCollection& allPatrons, BooksCollection& allBooks) {
    std::cout << "\n--- Editing a Loan Record ---\n";

    // Prompt user to find the patron
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron) {
        std::cout << "Patron not found.\n";
        return;
    }

    // Prompt user to find the book
    Books* book = allBooks.PromptForSearchMechanism();
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    // Find the loan record matching this patron and book combination
    auto it = std::find_if(loansList.begin(), loansList.end(), [&](Loans* loan) {
        return loan->getPatronID() == patron->getPatronID() && loan->getBookID() == book->getLibraryID();
        });

    // Check if loan record exists
    if (it == loansList.end()) {
        std::cout << "No active loan found for this book and patron combination.\n";
        return;
    }

    // Calculate new due date (7 days from today)
    std::tm newDueDate = getCurrentDate();
    newDueDate.tm_mday += 7; // Extending the due date by 7 days
    std::mktime(&newDueDate); // Normalize the date structure

    // Update the loan's due date
    (*it)->setDueDate(newDueDate);

    std::cout << "Loan record updated. New due date: " << tmToString(newDueDate) << ".\n";
}

/**
 * Marks a book as lost in the system
 * Updates the book's status to LOST
 *
 * @param allPatrons - Reference to PatronsCollection for patron lookup
 * @param allBooks - Reference to BooksCollection for book lookup and status update
 */
void LoansCollection::ReportLost(PatronsCollection& allPatrons, BooksCollection& allBooks) {
    std::cout << "\n--- Reporting a Book as Lost ---\n";

    // Prompt user to find the patron who lost the book
    Patron* patron = allPatrons.PromptForSearchMechanism();
    if (!patron) {
        std::cout << "Patron not found.\n";
        return;
    }

    // Prompt user to find the lost book
    Books* book = allBooks.PromptForSearchMechanism();
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    // Find the loan record for this book
    auto it = std::find_if(loansList.begin(), loansList.end(), [&](Loans* loan) {
        return loan->getBookID() == book->getLibraryID();
        });

    // If loan record exists, mark book as lost
    if (it != loansList.end()) {
        // Update book status to LOST
        book->setCurrentBookStatus(Books::LOST);
        std::cout << "Book marked as lost.\n";
    }
    else {
        std::cout << "Loan record for the book not found.\n";
    }
}

/**
 * Saves all loan data to a JSON file
 * Creates a properly formatted JSON structure with all loan information
 * Stores due date as separate year/month/day fields
 *
 * @param filePath - The path to the JSON file to save to
 * @return bool - True if save successful, false if file couldn't be opened
 */
bool LoansCollection::SaveToJsonFile(const std::string& filePath) const {
    // Open file for writing (truncate mode - overwrites existing content)
    std::ofstream out(filePath, std::ios::trunc);

    // Check if file opened successfully
    if (!out) {
        return false;
    }

    // Write JSON structure manually
    out << "{\n";
    out << "  \"loans\": [\n";

    // Write each loan as a JSON object
    for (size_t i = 0; i < loansList.size(); ++i) {
        const Loans* loan = loansList[i];

        // Get due date to extract individual date components
        std::tm dueDate = loan->getDueDate();

        // Write loan object with proper indentation
        out << "    {\n";
        out << "      \"loanID\": " << loan->getLoanID() << ",\n";
        out << "      \"bookID\": " << loan->getBookID() << ",\n";
        out << "      \"patronID\": " << loan->getPatronID() << ",\n";

        // Store date as separate fields (tm_year is years since 1900, tm_mon is 0-11)
        out << "      \"dueYear\": " << (dueDate.tm_year + 1900) << ",\n";
        out << "      \"dueMonth\": " << (dueDate.tm_mon + 1) << ",\n";
        out << "      \"dueDay\": " << dueDate.tm_mday << ",\n";

        // Store status as integer value
        out << "      \"status\": " << static_cast<int>(loan->getStatus()) << "\n";
        out << "    }";

        // Add comma between objects (but not after the last one)
        if (i + 1 < loansList.size()) {
            out << ",";
        }
        out << "\n";
    }

    // Close JSON structure
    out << "  ]\n";
    out << "}\n";

    return true;
}

/**
 * Loads loan data from a JSON file
 * Parses JSON manually using regex to extract loan information
 * Clears existing loans before loading and tracks highest loan ID
 *
 * @param filePath - The path to the JSON file to load from
 * @return bool - True if load successful, false if file couldn't be opened
 */
bool LoansCollection::LoadFromJsonFile(const std::string& filePath) {
    // Open file for reading
    std::ifstream in(filePath);

    // If file doesn't exist or can't be opened, return false
    if (!in) {
        return false;
    }

    // Read entire file content into a string
    std::stringstream buffer;
    buffer << in.rdbuf(); // Read file buffer
    const std::string content = buffer.str();

    // Delete all existing loans before loading new data
    for (auto* loan : loansList) {
        delete loan;
    }
    loansList.clear();

    // Track the highest loan ID to set next ID properly
    int maxLoanID = 0;

    // Use regex to find all JSON objects (delimited by curly braces)
    std::regex objectPattern("\\{[^\\{\\}]*\\}");

    // Create iterator to find all matches
    auto begin = std::sregex_iterator(content.begin(), content.end(), objectPattern);
    auto end = std::sregex_iterator();

    // Iterate through each JSON object found
    for (auto it = begin; it != end; ++it) {
        const std::string objectText = it->str();

        // Variables to store extracted field values
        int loanID = 0;
        int bookID = 0;
        int patronID = 0;
        int dueYear = 0;
        int dueMonth = 0;
        int dueDay = 0;
        int status = 0;

        // Extract all required fields from the JSON object
        // All fields must be present for the object to be valid
        bool ok = extractIntField(objectText, "loanID", loanID)
            && extractIntField(objectText, "bookID", bookID)
            && extractIntField(objectText, "patronID", patronID)
            && extractIntField(objectText, "dueYear", dueYear)
            && extractIntField(objectText, "dueMonth", dueMonth)
            && extractIntField(objectText, "dueDay", dueDay)
            && extractIntField(objectText, "status", status);

        // Skip this object if any field extraction failed
        if (!ok) {
            continue;
        }

        // Reconstruct the tm structure from individual date components
        std::tm dueDate = {};
        dueDate.tm_year = dueYear - 1900;  // tm_year is years since 1900
        dueDate.tm_mon = dueMonth - 1;     // tm_mon is 0-11
        dueDate.tm_mday = dueDay;          // tm_mday is 1-31
        std::mktime(&dueDate);              // Normalize the date structure

        // Create new Loans object with loaded data
        Loans* loadedLoan = new Loans(bookID, patronID, dueDate);

        // Set additional fields that aren't set by constructor
        loadedLoan->setLoanID(loanID);
        loadedLoan->setStatus(static_cast<Loans::LoanStatus>(status));

        // Add to collection
        loansList.push_back(loadedLoan);

        // Track the maximum loan ID seen
        if (loanID > maxLoanID) {
            maxLoanID = loanID;
        }
    }

    // Set the next loan ID to be one more than the highest loaded ID
    // This prevents ID conflicts when creating new loans
    Loans::setNextLoanID(maxLoanID + 1);

    return true;
}
