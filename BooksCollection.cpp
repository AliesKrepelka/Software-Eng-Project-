#include "BooksCollection.h"
#include <iostream>
#include <limits> 
#include <algorithm> 
BooksCollection::BooksCollection() {
}

BooksCollection::~BooksCollection() {
    for (auto* book : booksList) {
        delete book;
    }
}

void BooksCollection::AddBook() {
    std::string title, author;
    int isbn, id = booksList.size();
    float cost;
    
    std::cout << "Enter book title: ";
    std::getline(std::cin, title);
    std::cout << "Enter author: ";
    std::getline(std::cin, author);
    std::cout << "Enter ISBN: ";
    std::cin >> isbn;
    std::cout << "Enter cost: ";
    std::cin >> cost;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character from the buffer

    Books* newBook = new Books(author, title, isbn, id, cost);
    booksList.push_back(newBook);
    
    std::cout << "Book added successfully.\n";
}

void BooksCollection::EditBook() {
    Books* book = PromptForSearchMechanism();
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    std::cout << "Editing Book: " << book->getTitle() << "\n";
    std::cout << "Select the attribute to edit: \n";
    std::cout << "1. Title\n";
    std::cout << "2. Author\n";
    std::cout << "3. ISBN\n";
    std::cout << "4. Cost\n";
    std::cout << "Enter choice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the newline character from the buffer

    switch (choice) {
        case 1: {
            std::string newTitle;
            std::cout << "Enter new title: ";
            std::getline(std::cin, newTitle);
            book->setTitle(newTitle);
            break;
        }
        case 2: {
            std::string newAuthor;
            std::cout << "Enter new author: ";
            std::getline(std::cin, newAuthor);
            book->setAuthor(newAuthor);
            break;
        }
        case 3: {
            int newISBN;
            std::cout << "Enter new ISBN: ";
            std::cin >> newISBN;
            book->setISBN(newISBN);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            break;
        }
        case 4: {
            float newCost;
            std::cout << "Enter new cost: ";
            std::cin >> newCost;
            book->setCost(newCost);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            break;
        }
        default:
            std::cout << "Invalid choice. Returning to main menu.\n";
    }

    std::cout << "Book updated successfully.\n";
}


void BooksCollection::DeleteBook() {
    Books* book = PromptForSearchMechanism();
    if (!book) {
        std::cout << "Book not found.\n";
        return;
    }

    auto it = std::find_if(booksList.begin(), booksList.end(), [book](const Books* b) { return b == book; });
    if (it != booksList.end()) {
        delete *it;
        booksList.erase(it);
        std::cout << "Book deleted successfully.\n";
    } else {
        std::cout << "Error deleting the book.\n";
    }
}

Books* BooksCollection::PromptForSearchMechanism() {
    std::cout << "Search by (1) Title, (2) ISBN, or (3) ID? ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    if (choice == 1) {
        std::string title;
        std::cout << "Enter title: ";
        std::getline(std::cin, title);
        return FindBookByTitle(title);
    } else if (choice == 2) {
        int isbn;
        std::cout << "Enter ISBN: ";
        std::cin >> isbn;
        return FindBookByISBN(isbn);
    } else if (choice == 3) {
        int id;
        std::cout << "Enter ID: ";
        std::cin >> id;
        return FindBookByID(id);
    } else {
        std::cout << "Invalid choice.\n";
        return nullptr;
    }
}

Books* BooksCollection::FindBookByTitle(const std::string& title) {
    for (auto* book : booksList) {
        if (book->getTitle() == title) return book;
    }
    return nullptr;
}

Books* BooksCollection::FindBookByISBN(int isbn) {
    for (auto* book : booksList) {
        if (book->getISBN() == isbn) return book;
    }
    return nullptr;
}

Books* BooksCollection::FindBookByID(int id) {
    for (auto* book : booksList) {
        if (book->getLibraryID() == id) return book;
    }
    return nullptr;
}

void BooksCollection::PrintAllBooks() const {
    for (const auto* book : booksList) {
        std::cout << "ID: " << book->getLibraryID() << ", Title: " << book->getTitle() 
                  << ", Author: " << book->getAuthor() << ", ISBN: " << book->getISBN() 
                  << ", Cost: $" << book->getCost() << std::endl;
    }
}

void BooksCollection::PrintBook() {
    Books* book = PromptForSearchMechanism();
    if (book) {
        std::cout << "ID: " << book->getLibraryID() << ", Title: " << book->getTitle() 
                  << ", Author: " << book->getAuthor() << ", ISBN: " << book->getISBN() 
                  << ", Cost: $" << book->getCost() << std::endl;
    } else {
        std::cout << "Book not found.\n";
    }
}
