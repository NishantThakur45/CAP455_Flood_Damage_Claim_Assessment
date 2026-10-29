#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <iomanip>
using namespace std;

const char DATA_FILE[] = "claims.dat";
const int NAME_SIZE = 100, TYPE_SIZE = 20, STATUS_SIZE = 20;

struct ClaimRecord {
    int id;
    char victim[NAME_SIZE];
    char type[TYPE_SIZE];
    char status[STATUS_SIZE];
    double amount;
};

void copyText(char* destination, size_t size, const string& source) {
    if (size == 0) return;
    strncpy(destination, source.c_str(), size - 1);
    destination[size - 1] = '\0';
}

class Claim {
protected:
    int claimId;
    string victim;
    double damageAmount;
    string status;
    static int nextClaimId;
    static int totalClaims;

    Claim(int id, const string& name, double amount, const string& claimStatus)
        : claimId(id), victim(name), damageAmount(amount), status(claimStatus) {}

public:
    Claim() : claimId(nextClaimId++), victim(""), damageAmount(0.0), status("Pending") {}
    Claim(const string& name, double amount)
        : claimId(nextClaimId++), victim(name), damageAmount(amount), status("Pending") {}
    Claim(const Claim& other)
        : claimId(other.claimId), victim(other.victim),
          damageAmount(other.damageAmount), status(other.status) {}
    virtual ~Claim() {}

    int getId() const { return claimId; }
    string getVictim() const { return victim; }
    double getAmount() const { return damageAmount; }
    string getStatus() const { return status; }
    void setVictim(const string& name) { victim = name; }
    void setAmount(double amount) { damageAmount = amount; }
    void setStatus(const string& value) { status = value; }

    virtual string type() const = 0;
    virtual void display() const = 0;

    ClaimRecord toRecord() const {
        ClaimRecord record{};
        record.id = claimId;
        copyText(record.victim, NAME_SIZE, victim);
        copyText(record.type, TYPE_SIZE, type());
        copyText(record.status, STATUS_SIZE, status);
        record.amount = damageAmount;
        return record;
    }

    bool isValid() const {
        return claimId > 0 && !victim.empty() && victim.size() < NAME_SIZE &&
               damageAmount >= 0.0 && status.size() < STATUS_SIZE;
    }

    bool operator>(const Claim& other) const {
        return damageAmount > other.damageAmount;
    }

    friend double operator+(const Claim& first, const Claim& second);
    friend void showClaimSummary(const Claim& claim);

    static int getTotalClaims() { return totalClaims; }
    static void setNextId(int id) { if (id > nextClaimId) nextClaimId = id; }
    static void setTotalClaims(int value) { totalClaims = value; }
    static void incrementTotalClaims() { totalClaims++; }
};

int Claim::nextClaimId = 1;
int Claim::totalClaims = 0;

double operator+(const Claim& first, const Claim& second) {
    return first.damageAmount + second.damageAmount;
}

void showClaimSummary(const Claim& claim) {
    cout << "\n--- Claim Summary ---\n"
         << "Claim ID: " << claim.claimId << "\n"
         << "Victim: " << claim.victim << "\n"
         << "Damage Type: " << claim.type() << "\n"
         << "Damage Amount: " << claim.damageAmount << "\n"
         << "Status: " << claim.status << "\n";
}

class House : public Claim {
public:
    House(const string& name, double amount) : Claim(name, amount) {}
    House(int id, const string& name, double amount, const string& status)
        : Claim(id, name, amount, status) {}
    string type() const override { return "House"; }
    void display() const override {
        cout << "\nClaim ID: " << claimId << "\nVictim: " << victim
             << "\nDamage Type: " << type() << "\nDamage Amount: "
             << fixed << setprecision(2) << damageAmount << "\nStatus: " << status << "\n";
    }
};

class Crop : public Claim {
public:
    Crop(const string& name, double amount) : Claim(name, amount) {}
    Crop(int id, const string& name, double amount, const string& status)
        : Claim(id, name, amount, status) {}
    string type() const override { return "Crop"; }
    void display() const override {
        cout << "\nClaim ID: " << claimId << "\nVictim: " << victim
             << "\nDamage Type: " << type() << "\nDamage Amount: "
             << fixed << setprecision(2) << damageAmount << "\nStatus: " << status << "\n";
    }
};

class Livestock : public Claim {
public:
    Livestock(const string& name, double amount) : Claim(name, amount) {}
    Livestock(int id, const string& name, double amount, const string& status)
        : Claim(id, name, amount, status) {}
    string type() const override { return "Livestock"; }
    void display() const override {
        cout << "\nClaim ID: " << claimId << "\nVictim: " << victim
             << "\nDamage Type: " << type() << "\nDamage Amount: "
             << fixed << setprecision(2) << damageAmount << "\nStatus: " << status << "\n";
    }
};

template <typename T>
int totalClaims(const T& items) {
    return static_cast<int>(items.size());
}

vector<unique_ptr<Claim> > claims;

unique_ptr<Claim> makeClaim(const string& type, const string& name, double amount,
                            int id = 0, const string& status = "Pending") {
    if (id == 0) {
        if (type == "House") return unique_ptr<Claim>(new House(name, amount));
        if (type == "Crop") return unique_ptr<Claim>(new Crop(name, amount));
        if (type == "Livestock") return unique_ptr<Claim>(new Livestock(name, amount));
    } else {
        if (type == "House") return unique_ptr<Claim>(new House(id, name, amount, status));
        if (type == "Crop") return unique_ptr<Claim>(new Crop(id, name, amount, status));
        if (type == "Livestock") return unique_ptr<Claim>(new Livestock(id, name, amount, status));
    }
    throw invalid_argument("Invalid damage type.");
}

void saveAllClaims() {
    ofstream file(DATA_FILE, ios::binary | ios::trunc);
    if (!file) throw runtime_error("Cannot open claims.dat for writing.");
    for (size_t i = 0; i < claims.size(); ++i) {
        ClaimRecord record = claims[i]->toRecord();
        file.write(reinterpret_cast<const char*>(&record), sizeof(record));
        if (!file) throw runtime_error("Error writing claim record.");
    }
}

void loadClaims() {
    claims.clear();
    fstream file(DATA_FILE, ios::binary | ios::in);
    if (!file) {
        ofstream createFile(DATA_FILE, ios::binary);
        if (!createFile) throw runtime_error("Cannot create claims.dat.");
        return;
    }

    int maxId = 0;
    while (true) {
        ClaimRecord record{};
        streampos position = file.tellg();
        file.seekg(position, ios::beg);
        file.read(reinterpret_cast<char*>(&record), sizeof(record));
        if (file.gcount() == 0) break;
        if (file.gcount() != static_cast<streamsize>(sizeof(record)))
            throw runtime_error("Incomplete record in claims.dat.");

        string type(record.type), name(record.victim), status(record.status);
        unique_ptr<Claim> claim = makeClaim(type, name, record.amount, record.id, status);
        if (!claim->isValid()) throw runtime_error("Invalid record in claims.dat.");
        claims.push_back(move(claim));
        if (record.id > maxId) maxId = record.id;
    }
    Claim::setNextId(maxId + 1);
    Claim::setTotalClaims(static_cast<int>(claims.size()));
}

int findClaimIndex(int id) {
    vector<unique_ptr<Claim> >::iterator it =
        find_if(claims.begin(), claims.end(),
                [id](const unique_ptr<Claim>& item) { return item->getId() == id; });
    return it == claims.end() ? -1 : static_cast<int>(distance(claims.begin(), it));
}

void createClaim() {
    string name, type;
    double amount;

    cout << "Enter victim/claimant name: ";
    getline(cin >> ws, name);
    if (name.empty() || name.size() >= NAME_SIZE) {
        cout << "Invalid name. Use 1 to 99 characters.\n";
        return;
    }

    cout << "Damage type (House/Crop/Livestock): ";
    getline(cin, type);
    if (type == "house" || type == "HOUSE" || type == "House") type = "House";
    else if (type == "crop" || type == "CROP" || type == "Crop") type = "Crop";
    else if (type == "livestock" || type == "LIVESTOCK" || type == "Livestock") type = "Livestock";
    else {
        cout << "Invalid damage type.\n";
        return;
    }

    cout << "Enter damage amount: ";
    if (!(cin >> amount)) {
        cout << "Invalid amount.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (amount < 0) {
        cout << "Amount cannot be negative.\n";
        return;
    }

    try {
        unique_ptr<Claim> item = makeClaim(type, name, amount);
        if (!item->isValid()) throw invalid_argument("Invalid claim details.");
        claims.push_back(move(item));
        Claim::incrementTotalClaims();
        saveAllClaims();
        showClaimSummary(*claims.back());
        cout << "Claim created and saved successfully.\n";
    } catch (const exception& error) {
        cout << "Error: " << error.what() << "\n";
    }
}

void readAllClaims() {
    if (claims.empty()) {
        cout << "No claims available.\n";
        return;
    }
    for (vector<unique_ptr<Claim> >::const_iterator it = claims.begin(); it != claims.end(); ++it)
        (*it)->display();

    double total = 0;
    for (size_t i = 0; i < claims.size(); ++i) total += claims[i]->getAmount();
    cout << "\nTotal compensation amount: " << fixed << setprecision(2) << total << "\n";
    cout << "Total stored claims: " << totalClaims(claims) << "\n";
}

void searchClaim() {
    int id;
    cout << "Enter Claim ID to search: ";
    if (!(cin >> id)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int index = findClaimIndex(id);
    if (index == -1) cout << "Claim not found.\n";
    else claims[index]->display();
}

void updateClaim() {
    int id;
    cout << "Enter Claim ID to update: ";
    if (!(cin >> id)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int index = findClaimIndex(id);
    if (index == -1) {
        cout << "Claim not found.\n";
        return;
    }

    string name, status;
    double amount;
    cout << "Enter new victim name: ";
    getline(cin, name);
    if (name.empty() || name.size() >= NAME_SIZE) {
        cout << "Invalid name.\n";
        return;
    }
    cout << "Enter new damage amount: ";
    if (!(cin >> amount) || amount < 0) {
        cout << "Invalid amount.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Status (Pending/Assessed/Closed): ";
    getline(cin, status);
    if (status != "Pending" && status != "Assessed" && status != "Closed") {
        cout << "Invalid status.\n";
        return;
    }

    claims[index]->setVictim(name);
    claims[index]->setAmount(amount);
    claims[index]->setStatus(status);

    try {
        ClaimRecord record = claims[index]->toRecord();
        fstream file(DATA_FILE, ios::binary | ios::in | ios::out);
        if (!file) throw runtime_error("Cannot open data file for update.");
        file.seekp(static_cast<streamoff>(index) * sizeof(ClaimRecord), ios::beg);
        file.write(reinterpret_cast<const char*>(&record), sizeof(record));
        if (!file) throw runtime_error("Could not write updated record.");
        file.close();
        saveAllClaims();
        cout << "Claim updated successfully.\n";
    } catch (const exception& error) {
        cout << "Error: " << error.what() << "\n";
    }
}

void deleteClaim() {
    int id;
    cout << "Enter Claim ID to delete: ";
    if (!(cin >> id)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int index = findClaimIndex(id);
    if (index == -1) {
        cout << "Claim not found.\n";
        return;
    }

    claims.erase(claims.begin() + index);
    Claim::setTotalClaims(static_cast<int>(claims.size()));
    try {
        saveAllClaims();
        cout << "Claim deleted successfully.\n";
    } catch (const exception& error) {
        cout << "Error: " << error.what() << "\n";
    }
}

void compareClaims() {
    if (claims.size() < 2) {
        cout << "Create at least two claims first.\n";
        return;
    }
    int firstId, secondId;
    cout << "Enter first Claim ID: ";
    if (!(cin >> firstId)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cout << "Enter second Claim ID: ";
    if (!(cin >> secondId)) {
        cout << "Invalid ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int first = findClaimIndex(firstId), second = findClaimIndex(secondId);
    if (first == -1 || second == -1) {
        cout << "One or both claims were not found.\n";
        return;
    }
    cout << "Combined damage amount: " << (*claims[first] + *claims[second]) << "\n";
    if (*claims[first] > *claims[second]) cout << "First claim has higher damage.\n";
    else if (*claims[second] > *claims[first]) cout << "Second claim has higher damage.\n";
    else cout << "Both claims have equal damage.\n";
}

void sortClaimsByAmount() {
    sort(claims.begin(), claims.end(),
         [](const unique_ptr<Claim>& a, const unique_ptr<Claim>& b) {
             return a->getAmount() > b->getAmount();
         });
    try {
        saveAllClaims();
        cout << "Claims sorted by damage amount.\n";
        readAllClaims();
    } catch (const exception& error) {
        cout << "Error: " << error.what() << "\n";
    }
}

int main() {
    try {
        loadClaims();
    } catch (const exception& error) {
        cout << "Startup error: " << error.what() << "\n";
        cout << "Keep a backup of claims.dat before changing data files.\n";
        return 1;
    }

    int choice = 0;
    do {
        cout << "\n=== Flood Damage Claim Assessment System ===\n"
             << "1. Create Claim\n"
             << "2. Read All Claims\n"
             << "3. Search Claim by ID\n"
             << "4. Update Claim\n"
             << "5. Delete Claim\n"
             << "6. Show Total Claims\n"
             << "7. Compare Two Claims\n"
             << "8. Sort Claims by Damage Amount\n"
             << "9. Exit\n"
             << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid menu input. Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        try {
            switch (choice) {
                case 1: createClaim(); break;
                case 2: readAllClaims(); break;
                case 3: searchClaim(); break;
                case 4: updateClaim(); break;
                case 5: deleteClaim(); break;
                case 6:
                    cout << "Total stored claims: " << totalClaims(claims) << "\n";
                    cout << "Static claim count: " << Claim::getTotalClaims() << "\n";
                    break;
                case 7: compareClaims(); break;
                case 8: sortClaimsByAmount(); break;
                case 9: cout << "Exiting application.\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } catch (const exception& error) {
            cout << "Operation error: " << error.what() << "\n";
        }
    } while (choice != 9);

    return 0;
}
