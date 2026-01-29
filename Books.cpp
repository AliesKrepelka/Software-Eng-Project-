#include "Books.h"

Books::Books(std::string author, std::string title, int isbn, int libraryID, float cost, BookStatus status)
    : author(author), title(title), isbn(isbn), libraryID(libraryID), cost(cost), bookStatus(status) {}

std::string Books::getAuthor() const { return author; }
std::string Books::getTitle() const { return title; }
int Books::getISBN() const { return isbn; }
int Books::getLibraryID() const { return libraryID; }
float Books::getCost() const { return cost; }
Books::BookStatus Books::getCurrentBookStatus() const { return bookStatus; }

void Books::setAuthor(const std::string& author) { this->author = author; }
void Books::setTitle(const std::string& title) { this->title = title; }
void Books::setISBN(int isbn) { this->isbn = isbn; }
void Books::setLibraryID(int libraryID) { this->libraryID = libraryID; }
void Books::setCost(float cost) { this->cost = cost; }
void Books::setCurrentBookStatus(BookStatus status) { this->bookStatus = status; }
