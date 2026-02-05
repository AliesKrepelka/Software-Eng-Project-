#include "BooksCollection.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>

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

BooksCollection::BooksCollection() = default;

BooksCollection::~BooksCollection() {
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

    std::cout << "Enter author: ";
    std::getline(std::cin, author);

    std::cout << "Enter ISBN (numbers only): ";
    std::cin >> isbn;

    std::cout << "Enter cost: ";
    std::cin >> cost;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int libraryID = ++Books::nextBookID;
    Books* book = new Books(author, title, isbn, libraryID, cost, Books::IN);
    booksList.push_back(book);
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
    std::cout << "Book updated.\n";
}

void BooksCollection::DeleteBook() {
    Books* b = PromptForSearchMechanism();
    if (!b) return;

    auto it = std::find(booksList.begin(), booksList.end(), b);
    if (it != booksList.end()) {
        delete *it;
        booksList.erase(it);
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
    for (auto* b : booksList) {
        if (b->getTitle() == title) return b;
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
