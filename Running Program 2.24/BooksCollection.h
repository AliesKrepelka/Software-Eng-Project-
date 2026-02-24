#ifndef BOOKSCOLLECTION_H
#define BOOKSCOLLECTION_H

#include <vector>
#include "Books.h"

class BooksCollection {
public:
    BooksCollection();
    ~BooksCollection();

    void AddBook();
    void EditBook();
    void DeleteBook();
    Books* PromptForSearchMechanism();
    Books* FindBookByTitle(const std::string& title);
    Books* FindBookByISBN(int isbn);
    Books* FindBookByID(int id);
    void PrintAllBooks() const;
    void PrintBook();
    bool LoadFromJsonFile(const std::string& filePath);
    bool SaveToJsonFile(const std::string& filePath) const;

private:
    std::vector<Books*> booksList;
    std::string dataFilePath;
};

#endif // BOOKSCOLLECTION_H
