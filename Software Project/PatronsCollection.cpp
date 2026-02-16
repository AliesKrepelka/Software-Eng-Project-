#include "PatronsCollection.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>
#include <sstream>

/**
 * Helper function to remove leading and trailing whitespace from a string
 *
 * @param s - The string to trim
 * @return std::string - The trimmed string without leading/trailing whitespace
 */
static std::string trim(const std::string& s) {
    // Find the first character that is not whitespace
    size_t start = s.find_first_not_of(" \t\r\n");

    // If no non-whitespace character found, return empty string
    if (start == std::string::npos) return "";

    // Find the last character that is not whitespace
    size_t end = s.find_last_not_of(" \t\r\n");

    // Return substring from first to last non-whitespace character
    return s.substr(start, end - start + 1);
}

/**
 * Helper function to check if a string contains only digit characters
 *
 * @param s - The string to check
 * @return bool - True if string is non-empty and contains only digits, false otherwise
 */
static bool isAllDigits(const std::string& s) {
    // Check that string is not empty AND all characters are digits
    return !s.empty() &&
        std::all_of(s.begin(), s.end(), ::isdigit);
}

/**
 * Escapes special JSON characters in a string (backslash and double quote)
 * Used when writing strings to JSON format
 *
 * @param input - The string to escape
 * @return std::string - The escaped string safe for JSON
 */
static std::string escapeJson(const std::string& input) {
    std::string out;
    out.reserve(input.size()); // Pre-allocate memory for efficiency

    // Iterate through each character
    for (char c : input) {
        // If character is backslash or quote, add escape character
        if (c == '\\' || c == '"') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    return out;
}

/**
 * Removes escape characters from a JSON string
 * Used when reading strings from JSON format
 *
 * @param input - The escaped JSON string
 * @return std::string - The unescaped string
 */
static std::string unescapeJson(const std::string& input) {
    std::string out;
    out.reserve(input.size()); // Pre-allocate memory for efficiency

    // Iterate through each character
    for (size_t i = 0; i < input.size(); ++i) {
        // If backslash found and not at end, skip it (it's an escape character)
        if (input[i] == '\\' && i + 1 < input.size()) {
            ++i; // Skip the escape character
        }
        out.push_back(input[i]);
    }
    return out;
}

/**
 * Extracts a string value from a JSON object text using regex
 *
 * @param objectText - The JSON object as a string
 * @param key - The JSON key to search for
 * @param value - Reference to store the extracted value
 * @return bool - True if field found and extracted successfully, false otherwise
 */
static bool extractStringField(const std::string& objectText, const std::string& key, std::string& value) {
    // Create regex pattern to match: "key": "value"
    // Pattern captures escaped characters and non-quote characters in the value
    std::regex pattern("\"" + key + "\"\\s*:\\s*\"((?:\\\\.|[^\"])*)\"");
    std::smatch match;

    // Search for pattern in object text
    if (!std::regex_search(objectText, match, pattern)) {
        return false; // Field not found
    }

    // Extract and unescape the captured value (first capture group)
    value = unescapeJson(match[1].str());
    return true;
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
 * Extracts a float value from a JSON object text using regex
 *
 * @param objectText - The JSON object as a string
 * @param key - The JSON key to search for
 * @param value - Reference to store the extracted float value
 * @return bool - True if field found and extracted successfully, false otherwise
 */
static bool extractFloatField(const std::string& objectText, const std::string& key, float& value) {
    // Create regex pattern to match: "key": number (including decimals and negative numbers)
    std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    std::smatch match;

    // Search for pattern in object text
    if (!std::regex_search(objectText, match, pattern)) {
        return false; // Field not found
    }

    // Convert captured string to float
    value = std::stof(match[1].str());
    return true;
}

/**
 * Constructor - Initializes the PatronsCollection and loads data from JSON file
 * Sets the default file path and attempts to load existing patron data
 */
PatronsCollection::PatronsCollection() : dataFilePath("patrons.json") {
    // Load existing patron data from file if it exists
    LoadFromJsonFile(dataFilePath);
}

/**
 * Destructor - Saves data to JSON file and cleans up dynamically allocated memory
 * Ensures all patron data is persisted and no memory leaks occur
 */
PatronsCollection::~PatronsCollection() {
    // Save current patron data to file before destruction
    SaveToJsonFile(dataFilePath);

    // Delete all dynamically allocated Patron objects
    for (auto* p : patronsList) {
        delete p;
    }

    // Clear the vector
    patronsList.clear();
}

/**
 * Prompts user for patron information and adds a new patron to the collection
 * Automatically assigns a unique patron ID
 */
void PatronsCollection::AddPatron() {
    std::string first, last;

    // Get patron's name from user
    std::cout << "Enter patron's first name: ";
    std::cin >> first;
    std::cout << "Enter patron's last name: ";
    std::cin >> last;

    // Calculate next available ID by finding the highest existing ID
    int nextID = patronsList.empty() ? 1 : 1; // Start at 1 if list is empty
    for (auto* p : patronsList) {
        // If current patron's ID is >= nextID, increment nextID
        if (p->getPatronID() >= nextID) {
            nextID = p->getPatronID() + 1;
        }
    }

    // Create new Patron object with concatenated full name and generated ID
    Patron* newPatron = new Patron(first + " " + last, nextID);

    // Add to collection
    patronsList.push_back(newPatron);

    std::cout << "Patron added successfully. Patron ID: " << nextID << "\n";
}

/**
 * Allows editing of an existing patron's information
 * Prompts user to search for a patron and then update their name
 */
void PatronsCollection::EditPatron() {
    // Search for the patron to edit
    Patron* p = PromptForSearchMechanism();

    // If patron not found or user cancelled, return
    if (!p) return;

    std::cout << "Editing patron: " << p->getName() << "\n";
    std::cout << "Current name: " << p->getName() << "\n";
    std::cout << "Enter new name (leave blank to keep): ";

    // Get new name from user
    std::string name;
    std::getline(std::cin, name);
    name = trim(name); // Remove leading/trailing whitespace

    // Only update if user entered a non-empty name
    if (!name.empty()) p->setName(name);

    std::cout << "Patron updated.\n";
}

/**
 * Removes a patron from the collection
 * Prompts user to search for a patron and then deletes them from memory and collection
 */
void PatronsCollection::DeletePatron() {
    // Search for the patron to delete
    Patron* p = PromptForSearchMechanism();

    // If patron not found or user cancelled, return
    if (!p) return;

    // Find the patron in the vector
    auto it = std::find(patronsList.begin(), patronsList.end(), p);

    if (it != patronsList.end()) {
        // Delete the dynamically allocated Patron object
        delete* it;

        // Remove from vector
        patronsList.erase(it);

        std::cout << "Patron deleted.\n";
    }
}

/**
 * Displays information for all patrons in the collection
 * Shows name, ID, fine balance, and number of books checked out
 */
void PatronsCollection::PrintAllPatrons() const {
    // Check if collection is empty
    if (patronsList.empty()) {
        std::cout << "No patrons in the collection.\n";
        return;
    }

    // Iterate through and display each patron's information
    for (const auto* p : patronsList) {
        std::cout << "Name: " << p->getName() << " | ID: " << p->getPatronID()
            << " | Fines: $" << p->getFineBalance() << " | Books: " << p->getNumBooks() << "\n";
    }
}

/**
 * Searches for a patron by their unique ID number
 *
 * @param id - The patron ID to search for
 * @return Patron* - Pointer to the patron if found, nullptr otherwise
 */
Patron* PatronsCollection::FindPatronByID(int id) {
    // Linear search through patron list
    for (auto* p : patronsList) {
        if (p->getPatronID() == id)
            return p;
    }

    // Patron not found
    return nullptr;
}

/**
 * Searches for a patron by their exact name
 *
 * @param name - The patron's name to search for (trimmed automatically)
 * @return Patron* - Pointer to the patron if found, nullptr otherwise
 */
Patron* PatronsCollection::FindPatronByName(std::string name) {
    // Remove leading/trailing whitespace from search name
    name = trim(name);

    // Linear search through patron list
    for (auto* p : patronsList) {
        if (p->getName() == name)
            return p;
    }

    // Patron not found
    return nullptr;
}

/**
 * Prompts user to search for a patron by name or ID
 * Automatically detects whether input is numeric (ID) or text (name)
 * Allows user to cancel by entering 0
 *
 * @return Patron* - Pointer to found patron, or nullptr if cancelled/not found
 */
Patron* PatronsCollection::PromptForSearchMechanism() {
    // Loop until valid patron found or user cancels
    while (true) {
        std::cout << "Enter patron NAME or ID (0 to cancel): ";
        std::string input;
        std::getline(std::cin, input);
        input = trim(input); // Remove whitespace

        // Check if user wants to cancel
        if (input == "0") return nullptr;

        // Determine if input is numeric (ID) or text (name)
        if (isAllDigits(input)) {
            // Search by ID
            Patron* p = FindPatronByID(std::stoi(input));
            if (!p) std::cout << "Patron not found.\n";
            else return p;
        }
        else {
            // Search by name
            Patron* p = FindPatronByName(input);
            if (!p) std::cout << "Patron not found.\n";
            else return p;
        }
    }
}

/**
 * Displays detailed information for a single patron
 * Prompts user to search for patron first
 */
void PatronsCollection::PrintPatron() {
    // Search for the patron to display
    Patron* p = PromptForSearchMechanism();

    // If patron not found or user cancelled, return
    if (!p) return;

    // Display all patron information
    std::cout << "Patron: " << p->getName() << "\n";
    std::cout << "ID: " << p->getPatronID() << "\n";
    std::cout << "Fines: $" << p->getFineBalance() << "\n";
    std::cout << "Books Checked Out: " << p->getNumBooks() << "\n";
}

/**
 * Allows a patron to make a payment towards their fine balance
 * Prompts for patron search and payment amount
 */
void PatronsCollection::PayFine() {
    // Search for the patron who is paying
    Patron* p = PromptForSearchMechanism();

    // If patron not found or user cancelled, return
    if (!p) return;

    // Get current fine balance
    float balance = p->getFineBalance();

    // Check if patron has any fines to pay
    if (balance <= 0) {
        std::cout << "No fines to pay.\n";
        return;
    }

    // Display current balance and prompt for payment amount
    std::cout << "Current fine balance: $" << balance << "\n";
    std::cout << "Enter amount to pay: $";
    float payment = 0.0f;
    std::cin >> payment;

    // Clear input buffer after reading payment
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Process payment if amount is positive
    if (payment > 0) {
        // Calculate new balance after payment
        float newBalance = balance - payment;

        // Ensure balance doesn't go negative
        if (newBalance < 0) newBalance = 0;

        // Update patron's fine balance
        p->setFineBalance(newBalance);

        std::cout << "Payment processed. New balance: $" << newBalance << "\n";
    }
}

/**
 * Saves all patron data to a JSON file
 * Creates a properly formatted JSON structure with all patron information
 *
 * @param filePath - The path to the JSON file to save to
 * @return bool - True if save successful, false if file couldn't be opened
 */
bool PatronsCollection::SaveToJsonFile(const std::string& filePath) const {
    // Open file for writing (truncate mode - overwrites existing content)
    std::ofstream out(filePath, std::ios::trunc);

    // Check if file opened successfully
    if (!out) {
        return false;
    }

    // Write JSON structure manually
    out << "{\n";
    out << "  \"patrons\": [\n";

    // Write each patron as a JSON object
    for (size_t i = 0; i < patronsList.size(); ++i) {
        const Patron* p = patronsList[i];

        // Write patron object with proper indentation
        out << "    {\n";
        out << "      \"patronID\": " << p->getPatronID() << ",\n";
        out << "      \"name\": \"" << escapeJson(p->getName()) << "\",\n"; // Escape special characters
        out << "      \"fineBalance\": " << p->getFineBalance() << ",\n";
        out << "      \"numBooks\": " << p->getNumBooks() << "\n";
        out << "    }";

        // Add comma between objects (but not after the last one)
        if (i + 1 < patronsList.size()) {
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
 * Loads patron data from a JSON file
 * Parses JSON manually using regex to extract patron information
 * Clears existing patrons before loading
 *
 * @param filePath - The path to the JSON file to load from
 * @return bool - True if load successful, false if file couldn't be opened
 */
bool PatronsCollection::LoadFromJsonFile(const std::string& filePath) {
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

    // Delete all existing patrons before loading new data
    for (auto* p : patronsList) {
        delete p;
    }
    patronsList.clear();

    // Use regex to find all JSON objects (delimited by curly braces)
    std::regex objectPattern("\\{[^\\{\\}]*\\}");

    // Create iterator to find all matches
    auto begin = std::sregex_iterator(content.begin(), content.end(), objectPattern);
    auto end = std::sregex_iterator();

    // Iterate through each JSON object found
    for (auto it = begin; it != end; ++it) {
        const std::string objectText = it->str();

        // Variables to store extracted field values
        int patronID = 0;
        int numBooks = 0;
        float fineBalance = 0.0f;
        std::string name;

        // Extract all required fields from the JSON object
        // All fields must be present for the object to be valid
        bool ok = extractIntField(objectText, "patronID", patronID)
            && extractIntField(objectText, "numBooks", numBooks)
            && extractFloatField(objectText, "fineBalance", fineBalance)
            && extractStringField(objectText, "name", name);

        // Skip this object if any field extraction failed
        if (!ok) {
            continue;
        }

        // Create new Patron object with loaded data
        Patron* loadedPatron = new Patron(name, patronID);

        // Set additional fields that aren't set by constructor
        loadedPatron->setNumBooks(numBooks);
        loadedPatron->setFineBalance(fineBalance);

        // Add to collection
        patronsList.push_back(loadedPatron);
    }

    return true;
}
