ATM Simulation

A C++ console application that simulates an ATM, built as a Data Structures project. Account records are stored in a singly linked list (ADT List), and a USB flash drive acts as the ATM card by storing a pin.code file.

Features: 
Account Registration: Full name, birthday, 11-digit contact number, and a minimum initial deposit of PHP 5,000. A 5-digit account number is auto-generated.
Flash Drive as ATM Card: Removable drives are auto-detected via the Windows API, and the account number and encrypted PIN are saved to pin.code.
Secure Login: Authenticate with full name (case-insensitive), account number, and a masked 4-6 digit PIN.
Transactions: Balance inquiry, withdraw, deposit, fund transfer (enrolled accounts only), change PIN (also updates the card), and mobile top-up (Smart, Globe, DITO, GOMO, TNT, TM).
PIN Encryption: PINs are stored encrypted with a digit-shift cipher.
Input Validation: Robust handling of invalid input, and ESC cancels any transaction and returns to the menu.

Programmers: 
Ontoria, Sophia Chloe T. and Ramos, Rhon Jacob B.
