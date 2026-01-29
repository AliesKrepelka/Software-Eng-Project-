#ifndef PATRON_H
#define PATRON_H

#include <string>

// The Patron class represents a library patron, holding information about the patron's name,
// identification number, the amount of fines they owe, and the number of books they've checked out.
class Patron {
public:
    // Constructs a Patron with a name and ID. Initializes fine balance and book count to zero.
    Patron(std::string nm, int ID);

    // Default constructor for creating a blank Patron.
    Patron();

    // Getters for Patron attributes
    std::string getName() const;
    int getPatronID() const;
    float getFineBalance() const;
    int getNumBooks() const;

    // Setters for Patron attributes
    void setName(std::string nm);
    void setPatronID(int ID);
    void setFineBalance(float bal);
    void setNumBooks(int num);

private:
    std::string name;
    int patronID;
    float fineBalance;
    int numBooks;
};

#endif // PATRON_H
