//Programmed by ONTORIA, SOPHIA CHLOE T. & RAMOS, RHON JACOB B. - BSIS-2B
//Project#1 : ATM Simulation

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>   //used to build the birthday string from separate M/D/Y fields
#include <cstdlib>
#include <cctype>
#include <fstream>   //needed to read/write the "pin.code" file on the flash drive (ATM card)
#include <vector>    //used to hold the list of detected removable drives
#include <limits>    //used for cin.ignore() when clearing the input buffer
#include <windows.h>
#include <conio.h>

using namespace std;

const string CARD_FILENAME = "pin.code";   //the file that is stored on the flash drive and acts as the ATM card

const string LINE = string(64, '-');

//ADT List Node - holds one ATM Account record
struct Account
{
    int accountNo;        // 5-digit account number
    string pin;            // ENCRYPTED pin
    string name;
    string birthday;       //MM/DD/YYYY
    string contact;        // contact number
    double balance;
};

//Node used by the SINGLY LINKED LIST
struct Node
{
    Account data;
    Node *next;
    Node(Account x)
    {
        data = x;
        next = NULL;
    }
};

const int PIN_SHIFT_KEY = 3;
const int KEY_ESC = 27; //used everywhere to detect "cancel transaction"

//Converts all the string to lowercase which makes the program case-insensitive
//Allows the user to log in using either all caps or lower caps
string ToLower(const string &s)
{
    string result = s;
    for (size_t i = 0; i < result.length(); i++)
        result[i] = (char)tolower((unsigned char)result[i]);
    return result;
}

string EncryptPin(const string &pin)
{
    string encrypted = "";
    for (size_t i = 0; i < pin.length(); i++)
    {
        char digit = pin[i];
        char enc = ((digit - '0' + PIN_SHIFT_KEY) % 10) + '0';
        encrypted += enc;
    }
    return encrypted;
}

string DecryptPin(const string &encryptedPin)
{
    string decrypted = "";
    for (size_t i = 0; i < encryptedPin.length(); i++)
    {
        char digit = encryptedPin[i];
        char dec = ((digit - '0' - PIN_SHIFT_KEY + 10) % 10) + '0';
        decrypted += dec;
    }
    return decrypted;
}

int InputValidInt(const string &prompt,
                   int minVal = numeric_limits<int>::min(),
                   int maxVal = numeric_limits<int>::max())
{
    int value;
    while (true)
    {
        cout << prompt;
        if (!(cin >> value))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Numbers only, please try again.\n";
            continue;
        }
        // discard any leftover characters on the same line (e.g. "12abc")
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (value < minVal || value > maxVal)
        {
            cout << "\nInvalid input. Please enter a number between "
                 << minVal << " and " << maxVal << ".\n";
            continue;
        }
        return value;
    }
}

double InputValidDouble(const string &prompt,
                         double minVal = -numeric_limits<double>::max(),
                         double maxVal = numeric_limits<double>::max())
{
    double value;
    while (true)
    {
        cout << prompt;
        if (!(cin >> value))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Numbers only, please try again.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (value < minVal || value > maxVal)
        {
            cout << "\nInvalid input. Please enter a value between "
                 << minVal << " and " << maxVal << ".\n";
            continue;
        }
        return value;
    }
}

string InputPin(string input)
{
    cout << input;
    string pin = "";
    char ch;

    while (true)
    {
        ch = _getch();

        if (ch == '\r') // ENTER key
        {
            if (pin.length() >= 4)
                break;              // 4-6 digits typed -> ENTER submits
            else
                continue;           // not enough digits yet, ignore ENTER
        }
        else if (ch == '\b')        // BACKSPACE
        {
            if (!pin.empty())
            {
                pin.pop_back();
                cout << "\b \b";
            }
        }
        else if (isdigit((unsigned char)ch) && pin.length() < 6)
        {
            pin.push_back(ch);
            cout << '*';
            if (pin.length() == 6)
                break;              // 6 digits reached -> auto-submit
        }
        // any other key is ignored
    }
    cout << endl;
    return pin;
}

string InputMobileNumber(string input)
{
    string number;
    while (true)
    {
        cout << input;
        cin >> number;

        bool validDigits = !number.empty() && number.length() <= 11;
        for (size_t i = 0; i < number.length() && validDigits; i++)
        {
            if (!isdigit((unsigned char)number[i]))
                validDigits = false;
        }

        if (validDigits)
            break;

        cout << "\nInvalid mobile number. Digits only, max 11 digits.\n";
    }
    return number;
}

string InputContactNumber(string input)
{
    string number;
    while (true)
    {
        cout << input;
        cin >> number;

        bool validDigits = (number.length() == 11);
        for (size_t i = 0; i < number.length() && validDigits; i++)
        {
            if (!isdigit((unsigned char)number[i]))
                validDigits = false;
        }

        if (validDigits)
            break;

        cout << "\nInvalid contact number. Numbers only, must be exactly 11 digits.\n";
    }
    return number;
}

//Returns false immediately if ESC is pressed by the user; true (with "result" filled) on pressing the ENTER key.
bool ReadNumericWithEsc(const string &prompt, string &result, bool allowDecimal)
{
    cout << prompt;
    string buffer;

    while (true)
    {
        char ch = _getch();

        if (ch == KEY_ESC)
        {
            cout << "\n";
            return false;
        }
        else if (ch == '\r') // ENTER
        {
            cout << "\n";
            break;
        }
        else if (ch == '\b') // BACKSPACE
        {
            if (!buffer.empty())
            {
                buffer.pop_back();
                cout << "\b \b";
            }
        }
        else if (isdigit((unsigned char)ch))
        {
            buffer.push_back(ch);
            cout << ch;
        }
        else if (allowDecimal && ch == '.' && !buffer.empty() &&
                 buffer.find('.') == string::npos)
        {
            buffer.push_back(ch);
            cout << ch;
        }
        // any other key is ignored
    }

    result = buffer;
    return true;
}

//Cancelable integer input (e.g. target account no., network choice)
bool InputIntCancelable(const string &prompt, int minVal, int maxVal, int &value)
{
    while (true)
    {
        string raw;
        if (!ReadNumericWithEsc(prompt, raw, false))
            return false; // ESC pressed -> cancel

        if (raw.empty())
        {
            cout << "\nInvalid input. Numbers only. (ESC to cancel)\n";
            continue;
        }

        int val = atoi(raw.c_str());
        if (val < minVal || val > maxVal)
        {
            cout << "\nInvalid input. Enter a number between " << minVal
                 << " and " << maxVal << ". (ESC to cancel)\n";
            continue;
        }

        value = val;
        return true;
    }
}

//Cancelable double input (e.g. withdraw/deposit/transfer/load amounts)
bool InputDoubleCancelable(const string &prompt, double minVal, double maxVal, double &value)
{
    while (true)
    {
        string raw;
        if (!ReadNumericWithEsc(prompt, raw, true))
            return false; // ESC pressed -> cancel

        if (raw.empty() || raw == ".")
        {
            cout << "\nInvalid input. Numbers only. (ESC to cancel)\n";
            continue;
        }

        double val = atof(raw.c_str());
        if (val < minVal || val > maxVal)
        {
            cout << "\nInvalid input. Enter a value between " << minVal
                 << " and " << maxVal << ". (ESC to cancel)\n";
            continue;
        }

        value = val;
        return true;
    }
}

//Cancelable mobile number input (digits only, max 11)
bool InputMobileNumberCancelable(const string &prompt, string &number)
{
    while (true)
    {
        string raw;
        if (!ReadNumericWithEsc(prompt, raw, false))
            return false; // ESC pressed -> cancel

        if (!raw.empty() && raw.length() <= 11)
        {
            number = raw;
            return true;
        }

        cout << "\nInvalid mobile number. Digits only, max 11 digits. (ESC to cancel)\n";
    }
}

//Cancelable PIN input, same rules as InputPin() (4-6 digits) but ESC aborts.
bool InputPinCancelable(const string &prompt, string &pin)
{
    cout << prompt;
    string buffer;

    while (true)
    {
        char ch = _getch();

        if (ch == KEY_ESC)
        {
            cout << "\n";
            return false;
        }
        else if (ch == '\r') // ENTER
        {
            if (buffer.length() >= 4)
                break;
            else
                continue; // not enough digits yet
        }
        else if (ch == '\b') // BACKSPACE
        {
            if (!buffer.empty())
            {
                buffer.pop_back();
                cout << "\b \b";
            }
        }
        else if (isdigit((unsigned char)ch) && buffer.length() < 6)
        {
            buffer.push_back(ch);
            cout << '*';
            if (buffer.length() == 6)
                break;
        }
    }

    cout << endl;
    pin = buffer;
    return true;
}

//Flash Drive as ATM Card which stores the encrypted pin in pin.code
string NormalizeDrivePath(string drivePath)
{
    if (!drivePath.empty() && drivePath.back() != '\\' && drivePath.back() != '/')
        drivePath += '\\';
    return drivePath;
}

//Checks whether the pin.code file (the "card") exists on the given drive
bool CardIsInserted(const string &drivePath)
{
    string fullPath = NormalizeDrivePath(drivePath) + CARD_FILENAME;
    ifstream cardFile(fullPath.c_str());
    return cardFile.good();
}

// Scans all drive letters (A-Z) and returns the ones Windows reports as DRIVE_REMOVABLE a.k.a the flashdrive
vector<string> GetRemovableDrives()
{
    vector<string> drives;
    DWORD driveMask = GetLogicalDrives();

    for (int i = 0; i < 26; i++)
    {
        if (driveMask & (1 << i))
        {
            string drivePath = string(1, (char)('A' + i)) + ":\\";
            UINT type = GetDriveTypeA(drivePath.c_str());
            if (type == DRIVE_REMOVABLE)
                drives.push_back(drivePath);
        }
    }
    return drives;
}

//Out of the currently plugged-in removable drives, find the first one that already has a pin.code file on it.
bool FindDriveWithCard(string &foundDrivePath)
{
    vector<string> drives = GetRemovableDrives();
    for (size_t i = 0; i < drives.size(); i++)
    {
        if (CardIsInserted(drives[i]))
        {
            foundDrivePath = drives[i];
            return true;
        }
    }
    return false;
}

//Writes the account number and encrypted pin onto the flash drive's pin.code
bool SaveCardToFlashDrive(const string &drivePath, int accountNo, const string &encryptedPin)
{
    string fullPath = NormalizeDrivePath(drivePath) + CARD_FILENAME;
    ofstream cardFile(fullPath.c_str());
    if (!cardFile.is_open())
        return false;

    cardFile << accountNo << endl;
    cardFile << encryptedPin << endl;
    cardFile.close();
    return true;
}

// Reads the account number and encrypted pin stored on the pin.code file
bool ReadCardFromFlashDrive(const string &drivePath, int &accountNo, string &encryptedPin)
{
    string fullPath = NormalizeDrivePath(drivePath) + CARD_FILENAME;
    ifstream cardFile(fullPath.c_str());
    if (!cardFile.is_open())
        return false;

    cardFile >> accountNo;
    cardFile >> encryptedPin;
    cardFile.close();
    return true;
}

//Detects if a flashdrive is inserted
string WaitForFlashDriveInsertion(bool requireCard)
{
    cout << "\nPlease insert your flash drive"
         << (requireCard ? " (ATM card)" : "")
         << ". Waiting for detection... (press ESC to cancel)\n"; //User can cancel the transaction by pressing ESC

    string drivePath;
    while (true)
    {
        // Allow the user to cancel without unplugging anything
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == KEY_ESC) // ESC key
                return "";
        }

        if (requireCard)
        {
            if (FindDriveWithCard(drivePath))
            {
                cout << "\nCard detected on drive " << drivePath << "\n";
                return drivePath;
            }
        }
        else
        {
            vector<string> drives = GetRemovableDrives();
            if (!drives.empty())
            {
                drivePath = drives[0]; // first removable drive found
                cout << LINE << "\n\n";
                cout << "\nFlash drive detected: " << drivePath << "\n\n";
                cout << LINE << "\n\n";
                return drivePath;
            }
        }

        Sleep(500); // poll twice a second instead of hammering the CPU
    }
}

class LinkList
{
    private:
        Node *head;
        Node *tail;
        int nextAccountNo;      // auto-generates 5-digit account numbers
    public:
        LinkList() : head(NULL), tail(NULL), nextAccountNo(10001) {}

        ~LinkList()
        {
            Node *p;
            while (head != NULL)
            {
                p = head;
                head = head->next;
                delete p;
            }
            tail = NULL;
        }

        int Insert(string name, string birthday, string contact,
                   string pin, double initialDeposit)
        {
            Account newAcc;
            newAcc.accountNo = nextAccountNo++;   // 5-digit, auto-incremented
            newAcc.pin = EncryptPin(pin);          // store ENCRYPTED pin only
            newAcc.name = name;
            newAcc.birthday = birthday;
            newAcc.contact = contact;
            newAcc.balance = initialDeposit;

            Node *newNode = new Node(newAcc);

            if (head == NULL)
            {
                head = tail = newNode;
            }
            else
            {
                tail->next = newNode;
                tail = newNode;
            }
            return newAcc.accountNo;
        }

        //ADT LIST OPERATION: Search (login -> account no. + pin)
        Node* Search(int accNo, string pin)
        {
            string encryptedPin = EncryptPin(pin);
            Node *temp = head;
            while (temp != NULL)
            {
                if (temp->data.accountNo == accNo && temp->data.pin == encryptedPin)
                    return temp;
                temp = temp->next;
            }
            return NULL;
        }

        //Search for account using name, acc no., and pin
        Node* SearchByNameAccNoPin(string name, int accNo, string pin)
        {
            string encryptedPin = EncryptPin(pin);
            string lowerName = ToLower(name);
            Node *temp = head;
            while (temp != NULL)
            {
                if (temp->data.accountNo == accNo &&
                    temp->data.pin == encryptedPin &&
                    ToLower(temp->data.name) == lowerName)
                    return temp;
                temp = temp->next;
            }
            return NULL;
        }

        //(used for Fund Transfer -- "for enrolled accounts only",
        Node* SearchByAccountNo(int accNo)
        {
            Node *temp = head;
            while (temp != NULL)
            {
                if (temp->data.accountNo == accNo)
                    return temp;
                temp = temp->next;
            }
            return NULL; //not found / not enrolled
        }

        bool IsEmpty()
        {
            return head == NULL;
        }

        //Transaction Module: Balance Inquiry
        void BalanceInquiry(Node *acc)
        {
            cout << fixed << setprecision(2);
            cout << "\n----------------------------------";
            cout << "\n        BALANCE INQUIRY";
            cout << "\n----------------------------------";
            cout << "\nAccount No. : " << acc->data.accountNo;
            cout << "\nAccount Name: " << acc->data.name;
            cout << "\nBalance     : PHP " << acc->data.balance;
            cout << "\n----------------------------------\n";
        }

        //Transaction Module: Withdraw
        void Withdraw(Node *acc, double amount)
        {
            if (amount <= 0)
            {
                cout << "\nInvalid amount. Please enter a positive value.\n";
                return;
            }
            if (amount > acc->data.balance)
            {
                cout << "\nInsufficient balance!\n";
                return;
            }
            acc->data.balance -= amount;
            cout << fixed << setprecision(2);
            cout << "\n" << LINE;
            cout << "\nWithdraw successful!";
            cout << "\n" << LINE;
            cout << "\nNew balance: PHP " << acc->data.balance;
            cout << "\n" << LINE << "\n";
        }

        //Transaction Module: Deposit
        void Deposit(Node *acc, double amount)
        {
            if (amount <= 0)
            {
                cout << "\nInvalid amount. Please enter a positive value.\n";
                return;
            }
            acc->data.balance += amount;
            cout << fixed << setprecision(2);
            cout << "\n" << LINE;
            cout << "\nDeposit successful!";
            cout << "\n" << LINE;
            cout << "\nNew balance: PHP " << acc->data.balance;
            cout << "\n" << LINE << "\n";
        }

        //Transaction Module: Fund Transfer (enrolled accounts only)
        void FundTransfer(Node *sender, int targetAccNo, double amount)
        {
            if (amount <= 0)
            {
                cout << "\nInvalid amount. Please enter a positive value.\n";
                return;
            }
            if (targetAccNo == sender->data.accountNo)
            {
                cout << "\nYou cannot transfer funds to your own account.\n";
                return;
            }

            Node *receiver = SearchByAccountNo(targetAccNo);
            if (receiver == NULL)
            {
                cout << "\nFund transfer failed: Account No. " << targetAccNo
                     << " is not an enrolled account.\n";
                return;
            }
            if (amount > sender->data.balance)
            {
                cout << "\nInsufficient balance!\n";
                return;
            }

            sender->data.balance -= amount;
            receiver->data.balance += amount;

            cout << fixed << setprecision(2);
            cout << "\n" << LINE;
            cout << "\nFund transfer successful!";
            cout << "\n" << LINE;
            cout << "\nTransferred PHP " << amount << " to " << receiver->data.name
                 << " (Acct No. " << receiver->data.accountNo << ")";
            cout << "\nYour new balance: PHP " << sender->data.balance;
            cout << "\n" << LINE << "\n";
        }

        //Changes the pin code of the program as well as the pin.code in the flashdrive
        //Cancelable: pressing ESC at any PIN prompt aborts with no changes made.
        void ChangePin(Node *acc)
        {
            string oldPinEntered;
            if (!InputPinCancelable("\nEnter your CURRENT PIN: ", oldPinEntered))
            {
                cout << "\nPIN change cancelled.\n";
                return;
            }
            if (EncryptPin(oldPinEntered) != acc->data.pin)
            {
                cout << "\nIncorrect current PIN. PIN change cancelled.\n";
                return;
            }

            string newPin1, newPin2;
            while (true)
            {
                if (!InputPinCancelable("Enter your NEW PIN (4-6 digits): ", newPin1))
                {
                    cout << "\nPIN change cancelled.\n";
                    return;
                }
                if (!InputPinCancelable("Re-enter your NEW PIN to confirm: ", newPin2))
                {
                    cout << "\nPIN change cancelled.\n";
                    return;
                }

                if (newPin1 == newPin2)
                    break;

                cout << "\nPIN codes do not match. Please try again.\n";
            }
            acc->data.pin = EncryptPin(newPin1);
            cout << "\n" << LINE;
            cout << "\nPIN changed successfully!";
            cout << "\n" << LINE << "\n";

            // Update the card (pin.code file) so it matches the new pin
            cout << "\nPlease re-insert your flash drive to update your card.";
            string drivePath = WaitForFlashDriveInsertion(true);
            if (drivePath.empty())
            {
                cout << "\nCard update cancelled. NOTE: your old card is now out of "
                        "sync with your account. Update it before your next login.\n";
                return;
            }
            SaveCardToFlashDrive(drivePath, acc->data.accountNo, acc->data.pin);
            cout << "\n" << LINE;
            cout << "\nCard updated successfully!";
            cout << "\n" << LINE << "\n";
        }
        //Mobile Top-up / Load (Offers 6 network to choose from)
        void MobileLoad(Node *acc, string network, string mobileNo, double amount)
        {
            if (amount <= 0)
            {
                cout << "\nInvalid amount. Please enter a positive value.\n";
                return;
            }
            if (amount > acc->data.balance)
            {
                cout << "\nInsufficient balance!\n";
                return;
            }
            acc->data.balance -= amount;
            cout << fixed << setprecision(2);
            cout << "\n" << LINE;
            cout << "\nMobile Top-up / Load successful!";
            cout << "\n" << LINE;
            cout << "\nNetwork     : " << network;
            cout << "\nMobile No.  : " << mobileNo;
            cout << "\nLoad Amount : PHP " << amount;
            cout << "\nNew Balance : PHP " << acc->data.balance;
            cout << "\n" << LINE << "\n";
        }
};

int MainMenu();
void RegistrationModule(LinkList &atm);
void TransactionModule(LinkList &atm);
Node* Login(LinkList &atm);

int MainMenu()
{
    cout << "\n==================================";
    cout << "\n        THE ATM SIMULATION";
    cout << "\n==================================";
    cout << "\n[1] REGISTER NEW ACCOUNT";
    cout << "\n[2] TRANSACTION";
    cout << "\n[3] EXIT";
    cout << "\n----------------------------------";

    return InputValidInt("\nSelect an option: ");
}

void PrintRegistrationHeader()
{
    cout << "\n----------------------------------";
    cout << "\n        ACCOUNT REGISTRATION";
    cout << "\n----------------------------------";
}

int InputBirthdayField(const string &fieldPrompt, int minVal, int maxVal, const string &name)
{
    int value;
    while (true)
    {
        cout << fieldPrompt;
        if (!(cin >> value))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            system("cls");
            PrintRegistrationHeader();
            cout << "\nEnter your full name: " << name;
            cout << "\n\nEnter your Birthday:";
            cout << "\nInvalid input. Numbers only, please try again.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (value < minVal || value > maxVal)
        {
            system("cls");
            PrintRegistrationHeader();
            cout << "\nEnter your full name: " << name;
            cout << "\n\nEnter your Birthday:";
            cout << "\nInvalid input. Please enter a number between "
                 << minVal << " and " << maxVal << ".\n";
            continue;
        }

        return value;
    }
}

void RegistrationModule(LinkList &atm)
{
    string name, birthday, contact;
    int bMonth, bDay, bYear;
    double initialDeposit;

    system("cls");
    PrintRegistrationHeader();

    cout << "\nEnter your full name: ";
    getline(cin, name);

    cout << "\nEnter your Birthday:";
    bMonth = InputBirthdayField("\n  Month (1-12): ", 1, 12, name);
    bDay   = InputBirthdayField("  Day   (1-31): ", 1, 31, name);
    bYear  = InputBirthdayField("  Year  (e.g. 2000): ", 1950, 2021, name);

    ostringstream bdayStream;
    bdayStream << setw(2) << setfill('0') << bMonth << "/"
               << setw(2) << setfill('0') << bDay << "/"
               << bYear;
    birthday = bdayStream.str();

    contact = InputContactNumber("\nEnter your Contact Number (11 digits): ");

    initialDeposit = InputValidDouble("\nEnter initial deposit amount (Min. PHP 5000): PHP ", 0);

    if (initialDeposit < 5000)
    {
        cout << "\nInitial deposit must be at least PHP 5000. Registration cancelled.\n";
        system("pause");
        system("cls");
        return;
    }
    string pin1, pin2;
    while (true)
    {
        pin1 = InputPin("\nAssign your PIN code (4-6 digits): ");
        pin2 = InputPin("Re-enter your PIN code to confirm: ");

        if (pin1 == pin2)
            break;

        cout << "\nPIN codes do not match. Please try again.\n";
    }

    int accNo = atm.Insert(name, birthday, contact, pin1, initialDeposit);

    //Uses Flash Drive as ATM Card - flash drive is auto-detected
    cout << "\nPlease insert a flash drive to save as your ATM card.";
    bool cardSaved = false;
    while (!cardSaved)
    {
        string drivePath = WaitForFlashDriveInsertion(false);
        if (drivePath.empty())
        {
            cout << "\nRegistration cancelled: no flash drive was inserted.\n";
            system("pause");
            system("cls");
            return;
        }

        cardSaved = SaveCardToFlashDrive(drivePath, accNo, EncryptPin(pin1));
        if (!cardSaved)
        {
            cout << "\nCould not write to that drive. Please try another one.\n";
        }
        else
        {
            cout << "\nRegistration successful!";
            cout << "\nPlease remember your ATM Card details:";
            cout << "\n  Full Name   : " << name;
            cout << "\n  Account No. : " << accNo;
            cout << "\n  PIN Code    : (encrypted and saved, not shown)";
            cout << "\n  ATM Card    : saved to " << drivePath;
            cout << "\n----------------------------------";
            cout << "\nRemember your Full Name, Account Number, and PIN Code -";
            cout << "\nyou will use these three to log in under TRANSACTION.\n";
        }
    }
    system("pause");
    system("cls");
}

// Login asks the user their name, acc no., and pin without the flashdrive being inserted
Node* Login(LinkList &atm)
{
    if (atm.IsEmpty())
    {
        system("cls");
        cout << "\nNo registered accounts yet. Please register first.\n";
        return NULL;
    }

    system("cls");
    cout << "\n----------------------------------";
    cout << "\n           ACCOUNT LOGIN";
    cout << "\n----------------------------------";
    cout << "\nPlease enter your account details below to log in.\n";

    string name;
    cout << "\nFull Name      : ";
    getline(cin, name);

    int accNo = InputValidInt("Account Number : ");

    string pin = InputPin("PIN Code       : ");

    Node *acc = atm.SearchByNameAccNoPin(name, accNo, pin);

    system("cls");
    if (acc == NULL)
        cout << "\nInvalid Name, Account Number, or PIN. Please try again.\n";
    else
        cout << "\nWelcome, " << acc->data.name << "!\n";

    return acc;
}

//Transaction Module - Balance Inquiry, Withdraw, Deposit, Fund Transfer, Change PIN
//Every prompt inside this module can be aborted with ESC; doing so cancels that
//transaction only and returns the user to the TRANSACTION MENU (not all the way out).
void TransactionModule(LinkList &atm)
{
    Node *acc = Login(atm);
    if (acc == NULL)
        return;
    int op;
    double amount;
    int targetAccNo;
    bool exitTransaction = false;

    while (!exitTransaction)
    {
        cout << "\n----------------------------------";
        cout << "\n        TRANSACTION MENU";
        cout << "\n----------------------------------";
        cout << "\n[1] BALANCE INQUIRY";
        cout << "\n[2] WITHDRAW";
        cout << "\n[3] DEPOSIT";
        cout << "\n[4] FUND TRANSFER";
        cout << "\n[5] CHANGE PIN CODE";
        cout << "\n[6] MOBILE TOP-UP / LOAD";
        cout << "\n[7] BACK TO MAIN MENU";
        cout << "\n----------------------------------";
        cout << "\n!! Press ESC at any point to cancel a transaction !!";
        cout << "\n----------------------------------";
        op = InputValidInt("\nSelect your transaction: ");
        system("cls");

        switch (op)
        {
            case 1:
                atm.BalanceInquiry(acc);
                system("pause");
                system("cls");
                break;
            case 2:
                if (!InputDoubleCancelable("\nEnter amount to withdraw (ESC to cancel): PHP ",
                                           0, numeric_limits<double>::max(), amount))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                atm.Withdraw(acc, amount);
                system("pause");
                system("cls");
                break;
            case 3:
                if (!InputDoubleCancelable("\nEnter amount to deposit (ESC to cancel): PHP ",
                                           0, numeric_limits<double>::max(), amount))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                atm.Deposit(acc, amount);
                system("pause");
                system("cls");
                break;
            case 4:
                if (!InputIntCancelable("\nEnter RECEIVER's Account No. (ESC to cancel): ",
                                        numeric_limits<int>::min(), numeric_limits<int>::max(),
                                        targetAccNo))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                if (!InputDoubleCancelable("Enter amount to transfer (ESC to cancel): PHP ",
                                           0, numeric_limits<double>::max(), amount))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                atm.FundTransfer(acc, targetAccNo, amount);
                system("pause");
                system("cls");
                break;
            case 5:
                atm.ChangePin(acc); // ChangePin() has its own ESC handling built in
                system("pause");
                system("cls");
                break;
            case 6:
            {
                string network;
                cout << "\n----------------------------------";
                cout << "\n     MOBILE TOP-UP / LOAD";
                cout << "\n----------------------------------";
                cout << "\n[1] SMART";
                cout << "\n[2] GLOBE";
                cout << "\n[3] DITO";
                cout << "\n[4] GOMO";
                cout << "\n[5] TNT";
                cout << "\n[6] TM";
                cout << "\n----------------------------------";
                int netOp;
                if (!InputIntCancelable("\nSelect Network (ESC to cancel): ", 1, 6, netOp))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                if (netOp == 1)
                    network = "SMART";
                else if (netOp == 2)
                    network = "GLOBE";
                else if (netOp == 3)
                    network = "DITO";
                else if (netOp == 4)
                    network = "GOMO";
                else if (netOp == 5)
                    network = "TNT";
                else if (netOp == 6)
                    network = "TM";

                string mobileNo;
                if (!InputMobileNumberCancelable("Enter Mobile Number, max 11 digits (ESC to cancel): ",
                                                  mobileNo))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                if (!InputDoubleCancelable("Enter amount to load (ESC to cancel): PHP ",
                                           0, numeric_limits<double>::max(), amount))
                {
                    cout << "\nTransaction cancelled.\n";
                    system("pause");
                    system("cls");
                    break;
                }
                atm.MobileLoad(acc, network, mobileNo, amount);
                system("pause");
                system("cls");
                break;
            }
            case 7:
                exitTransaction = true;
                break;
            default:
                cout << "\nInvalid choice. Please try again!\n";
        }
    }
}

int main()
{
    LinkList atm;
    bool running = true;

    while (running)
    {
        switch (MainMenu())
        {
            case 1:
                RegistrationModule(atm);
                break;
            case 2:
                TransactionModule(atm);
                break;
            case 3:
                cout << "\nThank you for using THE ATM SIMULATION. Goodbye!\n";
                running = false;
                break;
            default:
                cout << "\nInvalid choice. Please try again!\n";
        }
    }
    return 0;
}