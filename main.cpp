#include <iostream>
#include <string>
#include <limits>
#include <cctype>
using namespace std;

class Claim {
private:
    int claimId;
    string claimantName;
    string damageType;
    double damageAmount;

    static int totalClaims;

public:
    void setClaimantName(string
    claimantName) {
        this->claimantName =
    claimantName;
}
    Claim() {
        claimId = 0;
        claimantName = "";
        damageType = "";
        damageAmount = 0.0;
    }

    inline void input() {
        cout << "Enter Claim ID: ";
        cin >> claimId;

        cout << "Enter Claimant Name: ";
        string name;
        getline(cin >> ws, name);
        setClaimantName(name);

        cout << "Enter Damage Type (House/Crop): ";
        getline(cin >> ws, damageType);

        for (char &ch : damageType) {
            ch = static_cast<char>(
                tolower(static_cast<unsigned char>(ch))
            );
        }

        if (damageType == "house") {
            damageType = "House";
        } else if (damageType == "crop") {
            damageType = "Crop";
        }

        cout << "Enter Damage Amount: ";
        cin >> damageAmount;
    }

    bool isValid() const {
        return cin.good() &&
               claimId > 0 &&
               !claimantName.empty() &&
               (damageType == "House" || damageType == "Crop") &&
               damageAmount >= 0;
    }

    int getId() const {
        return claimId;
    }

    void display() const {
        cout << "\nClaim ID: " << claimId;
        cout << "\nClaimant Name: " << claimantName;
        cout << "\nDamage Type: " << damageType;
        cout << "\nDamage Amount: " << damageAmount << "\n";
    }

    static int getTotalClaims() {
        return totalClaims;
    }

    static void incrementTotalClaims() {
        totalClaims++;
    }

    friend void showClaimSummary(const Claim &c);
};

int Claim::totalClaims = 0;

void showClaimSummary(const Claim &c) {
    cout << "\n--- Claim Summary ---\n";
    cout << "Claim ID: " << c.claimId << "\n";
    cout << "Claimant: " << c.claimantName << "\n";
}

int main() {
    Claim claims[50];
    int count = 0;
    int choice;

    do {
        cout << "\n=== Flood Damage Claim Assessment ===\n";
        cout << "1. Create Claim\n";
        cout << "2. Read All Claims\n";
        cout << "3. Show Total Claims\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid menu input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            if (count >= 50) {
                cout << "Claim storage is full.\n";
                continue;
            }

            Claim temp;
            temp.input();

            if (!cin) {
                cout << "Invalid input. Please enter the correct data types.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            if (!temp.isValid()) {
                cout << "Invalid details. ID must be positive, name cannot be empty, "
                     << "damage type must be House or Crop, and amount cannot be negative.\n";
                continue;
            }

            bool duplicate = false;

            for (int i = 0; i < count; i++) {
                if (claims[i].getId() == temp.getId()) {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate) {
                cout << "Claim ID already exists. Use a unique ID.\n";
            } else {
                claims[count] = temp;
                count++;
                Claim::incrementTotalClaims();

                cout << "Claim created successfully.\n";
                showClaimSummary(claims[count - 1]);
            }

        } else if (choice == 2) {
            if (count == 0) {
                cout << "No claims available.\n";
            } else {
                for (int i = 0; i < count; i++) {
                    claims[i].display();
                }
            }

        } else if (choice == 3) {
            cout << "Total claims created: "
                 << Claim::getTotalClaims() << "\n";

        } else if (choice == 4) {
            cout << "Exiting application.\n";

        } else {
            cout << "Invalid menu choice. Try again.\n";
        }

    } while (choice != 4);

    return 0;
}
