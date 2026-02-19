#include "BooksCollection.h"
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

BooksCollection::BooksCollection() : dataFilePath("books.json") {
    LoadFromJsonFile(dataFilePath);
}

BooksCollection::~BooksCollection() {
    SaveToJsonFile(dataFilePath);
    for (auto* b : booksList) {
        delete b;
    }
    booksList.clear();
}

void BooksCollection::AddBook() {
    std::string title, author;
    int isbn = 0;
    float cost = 0.0f;

    std::cout << "Enter book title: ";
    std::getline(std::cin, title);
    if (title.empty()) std::getline(std::cin, title);
    title = trim(title);

    std::cout << "Enter author: ";
    std::getline(std::cin, author);
    author = trim(author);

    std::cout << "Enter ISBN (numbers only): ";
    std::cin >> isbn;

    std::cout << "Enter cost: ";
    std::cin >> cost;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int libraryID = ++Books::nextBookID;
    Books* book = new Books(author, title, isbn, libraryID, cost, Books::IN);
    booksList.push_back(book);
    SaveToJsonFile(dataFilePath);
    std::cout << "Book added successfully. Library ID: " << libraryID << "\n";
}

void BooksCollection::EditBook() {
    Books* b = PromptForSearchMechanism();
    if (!b) return;

    std::cout << "Editing book ID " << b->getLibraryID() << "\n";
    std::cout << "Current title: " << b->getTitle() << "\n";
    std::cout << "Enter new title (leave blank to keep): ";
    std::string title;
    std::getline(std::cin, title);
    title = trim(title);
    if (!title.empty()) b->setTitle(title);

    std::cout << "Current author: " << b->getAuthor() << "\n";
    std::cout << "Enter new author (leave blank to keep): ";
    std::string author;
    std::getline(std::cin, author);
    author = trim(author);
    if (!author.empty()) b->setAuthor(author);

    std::cout << "Current ISBN: " << b->getISBN() << "\n";
    std::cout << "Enter new ISBN (0 to keep): ";
    int isbn = 0;
    std::cin >> isbn;
    if (isbn != 0) b->setISBN(isbn);

    std::cout << "Current cost: " << b->getCost() << "\n";
    std::cout << "Enter new cost (-1 to keep): ";
    float cost = -1.0f;
    std::cin >> cost;
    if (cost >= 0.0f) b->setCost(cost);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    SaveToJsonFile(dataFilePath);
    std::cout << "Book updated.\n";
}

void BooksCollection::DeleteBook() {
    Books* b = PromptForSearchMechanism();
    if (!b) return;

    auto it = std::find(booksList.begin(), booksList.end(), b);
    if (it != booksList.end()) {
        delete *it;
        booksList.erase(it);
        SaveToJsonFile(dataFilePath);
        std::cout << "Book deleted.\n";
    }
}

Books* BooksCollection::PromptForSearchMechanism() {
    while (true) {
        std::cout << "Enter book TITLE, ISBN, or ID (0 to cancel): ";
        std::string input;
        std::getline(std::cin, input);
        input = trim(input);

        if (input == "0") return nullptr;

        if (isAllDigits(input)) {
            int value = std::stoi(input);
            Books* byId = FindBookByID(value);
            if (byId) return byId;
            Books* byIsbn = FindBookByISBN(value);
            if (byIsbn) return byIsbn;
            std::cout << "Book not found.\n";
        } else {
            Books* b = FindBookByTitle(input);
            if (!b) std::cout << "Book not found.\n";
            else return b;
        }
    }
}

Books* BooksCollection::FindBookByTitle(const std::string& title) {
    const std::string targetTitle = trim(title);
    for (auto* b : booksList) {
        if (trim(b->getTitle()) == targetTitle) return b;
    }
    return nullptr;
}

Books* BooksCollection::FindBookByISBN(int isbn) {
    for (auto* b : booksList) {
        if (b->getISBN() == isbn) return b;
    }
    return nullptr;
}

Books* BooksCollection::FindBookByID(int id) {
    for (auto* b : booksList) {
        if (b->getLibraryID() == id) return b;
    }
    return nullptr;
}

void BooksCollection::PrintAllBooks() const {
    if (booksList.empty()) {
        std::cout << "No books in the collection.\n";
        return;
    }
    for (auto* b : booksList) {
        std::cout << "ID: " << b->getLibraryID()
                  << " | Title: " << b->getTitle()
                  << " | Author: " << b->getAuthor()
                  << " | ISBN: " << b->getISBN()
                  << " | Status: " << (b->getCurrentBookStatus() == Books::IN ? "IN" :
                                      b->getCurrentBookStatus() == Books::OUT ? "OUT" : "LOST")
                  << "\n";
    }
}

void BooksCollection::PrintBook() {
    Books* b = PromptForSearchMechanism();
    if (!b) return;
    std::cout << "ID: " << b->getLibraryID()
              << " | Title: " << b->getTitle()
              << " | Author: " << b->getAuthor()
              << " | ISBN: " << b->getISBN()
              << " | Cost: " << b->getCost()
              << " | Status: " << (b->getCurrentBookStatus() == Books::IN ? "IN" :
                                  b->getCurrentBookStatus() == Books::OUT ? "OUT" : "LOST")
              << "\n";
}

bool BooksCollection::SaveToJsonFile(const std::string& filePath) const {
    std::ofstream out(filePath, std::ios::trunc);
    if (!out) {
        return false;
    }

    out << "{\n";
    out << "  \"books\": [\n";
    for (size_t i = 0; i < booksList.size(); ++i) {
        const Books* b = booksList[i];
        out << "    {\n";
        out << "      \"libraryID\": " << b->getLibraryID() << ",\n";
        out << "      \"title\": \"" << escapeJson(b->getTitle()) << "\",\n";
        out << "      \"author\": \"" << escapeJson(b->getAuthor()) << "\",\n";
        out << "      \"isbn\": " << b->getISBN() << ",\n";
        out << "      \"cost\": " << b->getCost() << ",\n";
        out << "      \"status\": " << static_cast<int>(b->getCurrentBookStatus()) << "\n";
        out << "    }";
        if (i + 1 < booksList.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    return true;
}

bool BooksCollection::LoadFromJsonFile(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in) {
        return false;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    const std::string content = buffer.str();

    for (auto* b : booksList) {
        delete b;
    }
    booksList.clear();

    int maxID = 0;
    std::regex objectPattern("\\{[^\\{\\}]*\\}");
    auto begin = std::sregex_iterator(content.begin(), content.end(), objectPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        const std::string objectText = it->str();

        int libraryID = 0;
        int isbn = 0;
        int status = 0;
        float cost = 0.0f;
        std::string title;
        std::string author;

        bool ok = extractIntField(objectText, "libraryID", libraryID)
            && extractIntField(objectText, "isbn", isbn)
            && extractIntField(objectText, "status", status)
            && extractFloatField(objectText, "cost", cost)
            && extractStringField(objectText, "title", title)
            && extractStringField(objectText, "author", author);

        if (!ok) {
            continue;
        }

        Books::BookStatus bookStatus = Books::IN;
        if (status == static_cast<int>(Books::OUT)) {
            bookStatus = Books::OUT;
        } else if (status == static_cast<int>(Books::LOST)) {
            bookStatus = Books::LOST;
        }

        booksList.push_back(new Books(author, title, isbn, libraryID, cost, bookStatus));
        if (libraryID > maxID) {
            maxID = libraryID;
        }
    }

    Books::nextBookID = maxID;
    return true;
}
