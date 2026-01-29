#include "Patron.h"
#include <iostream>

// Constructs a Patron with a name and ID. Initializes fine balance and book count to zero.
Patron::Patron(std::string nm, int ID) : name(nm), patronID(ID), fineBalance(0.0), numBooks(0) {}

// Default constructor definition for creating a blank Patron.
Patron::Patron() : patronID(0), fineBalance(0.0), numBooks(0) {}

// Get the Patron's name
std::string Patron::getName() const {
    return name;
}

// Get the Patron's ID
int Patron::getPatronID() const {
    return patronID;
}

// Get the Patron's fine balance
float Patron::getFineBalance() const {
    return fineBalance;
}

// Get the number of books the Patron has checked out
int Patron::getNumBooks() const {
    return numBooks;
}

// Set the Patron's name
void Patron::setName(std::string nm) {
    name = nm;
}

// Set the Patron's ID
void Patron::setPatronID(int ID) {
    patronID = ID;
}

// Set the Patron's fine balance
void Patron::setFineBalance(float bal) {
    fineBalance = bal;
}

// Set the number of books the Patron has checked out
void Patron::setNumBooks(int num) {
    numBooks = num;
}
