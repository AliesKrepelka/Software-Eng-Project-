#include "PatronsCollection.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>
#include <sstream>

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static bool isAllDigits(const std::string& s) {
    return !s.empty() &&
           std::all_of(s.begin(), s.end(), ::isdigit);
}

static std::string escapeJson(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (char c : input) {
        if (c == '\\' || c == '"') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    return out;
}

static std::string unescapeJson(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 1 < input.size()) {
            ++i;
        }
        out.push_back(input[i]);
    }
    return out;
}

static bool extractStringField(const std::string& objectText, const std::string& key, std::string& value) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*\"((?:\\\\.|[^\"])*)\"");
    std::smatch match;
    if (!std::regex_search(objectText, match, pattern)) {
        return false;
    }
    value = unescapeJson(match[1].str());
    return true;
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

static bool extractFloatField(const std::string& objectText, const std::string& key, float& value) {
    std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    std::smatch match;
    if (!std::regex_search(objectText, match, pattern)) {
        return false;
    }
    value = std::stof(match[1].str());
    return true;
}

PatronsCollection::PatronsCollection() : dataFilePath("patrons.json") {
    LoadFromJsonFile(dataFilePath);
}

PatronsCollection::~PatronsCollection() {
    SaveToJsonFile(dataFilePath);
    for (auto* p : patronsList) {
        delete p;
    }
    patronsList.clear();
}

void PatronsCollection::AddPatron() {
    std::string first, last;
    std::cout << "Enter patron's first name: ";
    std::cin >> first;
    std::cout << "Enter patron's last name: ";
    std::cin >> last;

    int nextID = patronsList.empty() ? 1 : 1;
    for (auto* p : patronsList) {
        if (p->getPatronID() >= nextID) {
            nextID = p->getPatronID() + 1;
        }
    }

    Patron* newPatron = new Patron(first + " " + last, nextID);
    patronsList.push_back(newPatron);
    std::cout << "Patron added successfully. Patron ID: " << nextID << "\n";
}

void PatronsCollection::EditPatron() {
    Patron* p = PromptForSearchMechanism();
    if (!p) return;

    std::cout << "Editing patron: " << p->getName() << "\n";
    std::cout << "Current name: " << p->getName() << "\n";
    std::cout << "Enter new name (leave blank to keep): ";
    std::string name;
    std::getline(std::cin, name);
    name = trim(name);
    if (!name.empty()) p->setName(name);

    std::cout << "Patron updated.\n";
}

void PatronsCollection::DeletePatron() {
    Patron* p = PromptForSearchMechanism();
    if (!p) return;

    auto it = std::find(patronsList.begin(), patronsList.end(), p);
    if (it != patronsList.end()) {
        delete *it;
        patronsList.erase(it);
        std::cout << "Patron deleted.\n";
    }
}

void PatronsCollection::PrintAllPatrons() const {
    if (patronsList.empty()) {
        std::cout << "No patrons in the collection.\n";
        return;
    }
    for (const auto* p : patronsList) {
        std::cout << "Name: " << p->getName() << " | ID: " << p->getPatronID() 
                  << " | Fines: $" << p->getFineBalance() << " | Books: " << p->getNumBooks() << "\n";
    }
}

Patron* PatronsCollection::FindPatronByID(int id) {
    for (auto* p : patronsList) {
        if (p->getPatronID() == id)
            return p;
    }
    return nullptr;
}

Patron* PatronsCollection::FindPatronByName(std::string name) {
    name = trim(name);
    for (auto* p : patronsList) {
        if (p->getName() == name)
            return p;
    }
    return nullptr;
}

Patron* PatronsCollection::PromptForSearchMechanism() {
    while (true) {
        std::cout << "Enter patron NAME or ID (0 to cancel): ";
        std::string input;
        std::getline(std::cin, input);
        input = trim(input);

        if (input == "0") return nullptr;

        if (isAllDigits(input)) {
            Patron* p = FindPatronByID(std::stoi(input));
            if (!p) std::cout << "Patron not found.\n";
            else return p;
        } else {
            Patron* p = FindPatronByName(input);
            if (!p) std::cout << "Patron not found.\n";
            else return p;
        }
    }
}

void PatronsCollection::PrintPatron() {
    Patron* p = PromptForSearchMechanism();
    if (!p) return;
    std::cout << "Patron: " << p->getName() << "\n";
    std::cout << "ID: " << p->getPatronID() << "\n";
    std::cout << "Fines: $" << p->getFineBalance() << "\n";
    std::cout << "Books Checked Out: " << p->getNumBooks() << "\n";
}

void PatronsCollection::PayFine() {
    Patron* p = PromptForSearchMechanism();
    if (!p) return;

    float balance = p->getFineBalance();
    if (balance <= 0) {
        std::cout << "No fines to pay.\n";
        return;
    }

    std::cout << "Current fine balance: $" << balance << "\n";
    std::cout << "Enter amount to pay: $";
    float payment = 0.0f;
    std::cin >> payment;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (payment > 0) {
        float newBalance = balance - payment;
        if (newBalance < 0) newBalance = 0;
        p->setFineBalance(newBalance);
        std::cout << "Payment processed. New balance: $" << newBalance << "\n";
    }
}

void PatronsCollection::ResetAllFines() {
    for (auto* p : patronsList) {
        p->setFineBalance(0.0f);
    }
}

void PatronsCollection::AddFineToPatron(int patronID, float amount) {
    if (amount <= 0.0f) {
        return;
    }

    Patron* p = FindPatronByID(patronID);
    if (!p) {
        return;
    }

    p->setFineBalance(p->getFineBalance() + amount);
}

bool PatronsCollection::SaveToJsonFile(const std::string& filePath) const {
    std::ofstream out(filePath, std::ios::trunc);
    if (!out) {
        return false;
    }

    out << "{\n";
    out << "  \"patrons\": [\n";
    for (size_t i = 0; i < patronsList.size(); ++i) {
        const Patron* p = patronsList[i];
        out << "    {\n";
        out << "      \"patronID\": " << p->getPatronID() << ",\n";
        out << "      \"name\": \"" << escapeJson(p->getName()) << "\",\n";
        out << "      \"fineBalance\": " << p->getFineBalance() << ",\n";
        out << "      \"numBooks\": " << p->getNumBooks() << "\n";
        out << "    }";
        if (i + 1 < patronsList.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    return true;
}

bool PatronsCollection::LoadFromJsonFile(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in) {
        return false;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string content = buffer.str();

    for (auto* p : patronsList) {
        delete p;
    }
    patronsList.clear();

    std::regex objectPattern("\\{[^\\{\\}]*\\}");
    auto begin = std::sregex_iterator(content.begin(), content.end(), objectPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        const std::string objectText = it->str();

        int patronID = 0;
        int numBooks = 0;
        float fineBalance = 0.0f;
        std::string name;

        bool ok = extractIntField(objectText, "patronID", patronID)
            && extractIntField(objectText, "numBooks", numBooks)
            && extractFloatField(objectText, "fineBalance", fineBalance)
            && extractStringField(objectText, "name", name);

        if (!ok) {
            continue;
        }

        Patron* loadedPatron = new Patron(name, patronID);
        loadedPatron->setNumBooks(numBooks);
        loadedPatron->setFineBalance(fineBalance);
        patronsList.push_back(loadedPatron);
    }

    return true;
}
