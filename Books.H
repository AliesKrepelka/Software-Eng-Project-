#ifndef BOOKS_H
#define BOOKS_H

#include <string>

class Books {
public:
    enum BookStatus { IN, OUT, LOST };

    Books(std::string author, std::string title, int isbn, int libraryID, float cost, BookStatus status = IN);

    std::string getAuthor() const;
    std::string getTitle() const;
    int getISBN() const;
    int getLibraryID() const;
    float getCost() const;
    BookStatus getCurrentBookStatus() const;

    void setAuthor(const std::string& author);
    void setTitle(const std::string& title);
    void setISBN(int isbn);
    void setLibraryID(int libraryID);
    void setCost(float cost);
    void setCurrentBookStatus(BookStatus status);

private:
    std::string author;
    std::string title;
    int isbn;
    int libraryID;
    float cost;
    BookStatus bookStatus;
};

#endif // BOOKS_H
