#include<iostream>
#include<string>
#include<vector>
#include <limits> 
#include <algorithm> 
#include "PatronsCollection.h"
#include "Patron.h"

using namespace std;

// This is included as a reference if you wish to use this type of template style coding for hw4 or beyond.
template<typename T>
T getNumericInput(const string& prompt) {
    T input;
    cout << prompt;
    while (!(cin >> input)) {
        cin.clear(); // Clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the line
        cout << "Invalid input. Please enter a valid number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard any extra input to prepare for next input
    return input;
}

// Function to get a string input from the user with a prompt
string getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin >> ws, input); // Properly handles leading whitespace
    return input;
}

// Function to get integer input from the user with a prompt
int getIntInput(const string& prompt) {
    int input;
    cout << prompt;
    while(!(cin >> input)) {
        cout << "Invalid input, please enter a number: ";
        cin.clear(); // clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard input
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard any remaining input
    return input;
}

// Adds a new patron to the collection
void PatronsCollection::AddPatron() {
    cout << "\n--- Add a New Patron ---\n";
    string firstName = getStringInput("Enter patron's first name: ");
    string lastName = getStringInput("Enter patron's last name: ");
    int ID = patronsList.size(); // ID is the next index in the vector
    
    string fullName = firstName + " " + lastName;
    auto* patron = new Patron(fullName, ID);
    patronsList.push_back(patron);
    cout << "Patron added successfully.\n";
}

// Prompts the user to choose a search mechanism and returns the corresponding Patron
Patron* PatronsCollection::PromptForSearchMechanism() {
    while (true) {
        string method = getStringInput("Search by name or ID? (name/id): ");
        if (method == "name") {
            string name = getStringInput("Enter the patron's full name: ");
            return FindPatronByName(name);
        } else if (method == "id") {
            int id = getIntInput("Enter the patron's ID: ");
            return FindPatronByID(id);
        } else {
            cout << "Invalid option. Please type 'name' or 'id'.\n";
        }
    }
}

// Finds a patron by name
Patron* PatronsCollection::FindPatronByName(string name) {
    for (auto* patron : patronsList) {
        if (patron->getName() == name) {
            return patron;
        }
    }
    return nullptr; // Patron not found
}

// Finds a patron by ID
Patron* PatronsCollection::FindPatronByID(int id) {
    for (auto* patron : patronsList) {
        if (patron->getPatronID() == id) {
            return patron;
        }
    }
    return nullptr; // Patron not found
}

// Prints all patrons in the collection
void PatronsCollection::PrintAllPatrons() const {
    cout << "\n--- List of All Patrons ---\n";
    for (const auto* patron : patronsList) {
        cout << "ID: " << patron->getPatronID() << ", Name: " << patron->getName() << ", Fines: $" << patron->getFineBalance() << ", Books Checked Out: " << patron->getNumBooks() << endl;
    }
}

// Edits an existing patron's details
void PatronsCollection::EditPatron() {
    cout << "\n--- Edit a Patron ---\n";
    Patron* patron = PromptForSearchMechanism();
    if (patron != nullptr) {
        string firstName = getStringInput("Enter patron's new first name: ");
        string lastName = getStringInput("Enter patron's new last name: ");
        patron->setName(firstName + " " + lastName);
        cout << "Patron updated successfully.\n";
    } else {
        cout << "Patron not found.\n";
    }
}

// Deletes a patron from the collection
void PatronsCollection::DeletePatron() {
    cout << "\n--- Delete a Patron ---\n";
    Patron* patron = PromptForSearchMechanism();
    if (patron != nullptr) {
        auto it = find(patronsList.begin(), patronsList.end(), patron);
        if (it != patronsList.end()) {
            delete *it; // Free the memory
            patronsList.erase(it); // Remove from the list
            cout << "Patron deleted successfully.\n";
        }
    } else {
        cout << "Patron not found.\n";
    }
}

// Prints details of a specific patron
void PatronsCollection::PrintPatron() {
    cout << "\n--- Print a Patron's Details ---\n";
    Patron* patron = PromptForSearchMechanism();
    if (patron != nullptr) {
        cout << "ID: " << patron->getPatronID() << ", Name: " << patron->getName() << ", Fines: $" << patron->getFineBalance() << ", Books Checked Out: " << patron->getNumBooks() << endl;
    } else {
        cout << "Patron not found.\n";
    }
}

// Handles fine payment for a patron
void PatronsCollection::PayFine() {
    cout << "\n--- Pay a Patron's Fine ---\n";
    Patron* patron = PromptForSearchMechanism();
    if (patron != nullptr) {
        cout << "Current Fine: $" << patron->getFineBalance() << endl;
        float amount = getNumericInput<float>("Enter payment amount: $");
        if (amount > 0) {
            float newBalance = max(0.0f, patron->getFineBalance() - amount);
            patron->setFineBalance(newBalance);
            cout << "New Fine Balance: $" << newBalance << endl;
        } else {
            cout << "Invalid payment amount.\n";
        }
    } else {
        cout << "Patron not found.\n";
    }
}
