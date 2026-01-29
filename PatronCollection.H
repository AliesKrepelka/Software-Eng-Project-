#ifndef PATRONSCOLLECTION_H
#define PATRONSCOLLECTION_H

#include <string>
#include <vector>
#include "Patron.h"

// The PatronsCollection class manages a collection of Patron objects.
// It provides functionalities to add, edit, delete, and search for patrons,
// as well as printing details for a single patron or all patrons and handling fine payments.
class PatronsCollection {
public:
    // Adds a new patron to the collection
    void AddPatron();

    // Edits an existing patron's details
    void EditPatron();

    // Deletes a patron from the collection
    void DeletePatron();

    // Prompts the user for a search mechanism (by name or ID) and returns the corresponding Patron object
    Patron* PromptForSearchMechanism();

    // Finds and returns a Patron object by name. Returns nullptr if not found.
    Patron* FindPatronByName(std::string name);

    // Finds and returns a Patron object by ID. Returns nullptr if not found.
    Patron* FindPatronByID(int id);

    // Prints details for all patrons in the collection
    void PrintAllPatrons() const;

    // Prints details of a specific patron
    void PrintPatron();

    // Handles the payment of fines for a specific patron
    void PayFine();

private:
    std::vector<Patron*> patronsList; // A vector to store pointers to Patron objects
};

#endif // PATRONSCOLLECTION_H
