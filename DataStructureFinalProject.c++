#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <chrono>
#include <thread>

using namespace std;

class BorrowRecord {
    public:
        string BookTitle;
        string BorrowDate;
        BorrowRecord* Next;
        BorrowRecord* Prev;

        BorrowRecord(string title, string date) {
            BookTitle = title;
            BorrowDate = date;
            Next = NULL;
            Prev = NULL;
        }
    };

class ReturnRecord {
    public:
        int memberID;
        string BookName;
        string ReturnDate;
        ReturnRecord* Next;

        ReturnRecord(int id, string title, string date) {
            memberID = id;
            BookName = title;
            ReturnDate = date;
            Next = NULL;
        }
    };

class Reservation {
    public:
        int memberId;
        string memberName;
        string bookTitle;
        Reservation* Next;

        Reservation(int id, const string& name, const string& book) {
            memberId = id;
            memberName = name;
            bookTitle = book;
            Next = NULL;
        }
    };

class BookNode {
    public:
        int BookID;
        string Title;
        string Author;
        string Genre;
        int CopiesAvailable;
        BookNode* Left;
        BookNode* Right;

        BookNode(int id, const string& title, const string& author,
             const string& genre, int copies) {
            BookID = id;
            Title = title;
            Author = author;
            Genre = genre;
            CopiesAvailable = copies;
            Left = Right = NULL;
        }
    };

class MemberNode {
    public:
        int Id;
        string Name;
        string Email;
        string Phone;
        string Address;
        MemberNode* Next;
        BorrowRecord* History;  

        MemberNode(int id, const string& name, const string& email, const string& phone, const string& address) {
            Id = id;
            Name = name;
            Email = email;
            Phone = phone;
            Address = address;
            Next = NULL;
            History = NULL;
        }
    };

    void welcomeScreen() {
    system("cls"); 
    cout << "\033[1;34m";  
    cout << "=========================================\n";
    cout << "         📚 Welcome to MyLibrary!         \n";
    cout << "=========================================\n";
    cout << "\033[0m";  
    this_thread::sleep_for(chrono::seconds(2));
}

    void loadingEffect(string msg = "Loading") {
    cout << msg;
    for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    cout << "\n";
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    }

    void showMenu() {
        clearScreen();
        cout << "\033[1;36m"; 
        cout << "\n========================================\n";
        cout << "          Library Management Menu        \n";
        cout << "========================================\n";
        cout << "\033[0m";
        cout << "1.  Add Book\n";
        cout << "2.  Remove Book\n";
        cout << "3.  Search Book by ID\n";
        cout << "4.  Display All Books\n";
        cout << "----------------------------------------\n";
        cout << "5.  Add Member\n";
        cout << "6.  Remove Member\n";
        cout << "7.  Search Member by ID\n";
        cout << "8.  Display All Members\n";
        cout << "----------------------------------------\n";
        cout << "9.  Borrow Book\n";
        cout << "10. View Member Borrow History\n";
        cout << "----------------------------------------\n";

        cout << "11. Return Book\n";
        cout << "12. Process Next Return\n";
        cout << "13. View Returned Books Stack\n";

        cout << "----------------------------------------\n";
        cout << "14. Reserve Book\n";
        cout << "15. Process Next Reservation\n";
        cout << "16. View All Book Reservations\n";

        cout << "0.  Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";
    }
class Library {
    private:
        MemberNode* HEAD;
        ReturnRecord* returnTOP;
        Reservation* ResFront;
        Reservation* ResRear;
        BookNode* BookRoot;

        // ------ BST Functions ------
        BookNode* addBook(BookNode* root, int id, const string& title, const string& author, const string& genre, int copies) {
            if (root == NULL) {
                cout << "Book added: " << title << " (ID: " << id << ")" << endl;
                return new BookNode(id, title, author, genre, copies);
            }
            if (id < root->BookID)
                root->Left = addBook(root->Left, id, title, author, genre, copies);
            else if (id > root->BookID)
                root->Right = addBook(root->Right, id, title, author, genre, copies);
            else
                cout << "Book with ID " << id << " already exists." << endl;
            return root;
        }

        BookNode* searchBookNode(BookNode* root, int id) {
            if (root == NULL || root->BookID == id)
                return root;
            if (id < root->BookID)
                return searchBookNode(root->Left, id);
            return searchBookNode(root->Right, id);
        }

        BookNode* findMin(BookNode* root) {
            while (root != NULL && root->Left != NULL)
                root = root->Left;
            return root;
        }

        BookNode* deleteBook(BookNode* root, int id) {
            if (root == NULL) 
                return NULL;

            if (id < root->BookID)
                root->Left = deleteBook(root->Left, id);
            else if (id > root->BookID)
                root->Right = deleteBook(root->Right, id);
            else {
                if (root->Left == NULL) {
                    BookNode* temp = root->Right;
                    delete root;
                    cout << "Book with ID " << id << " removed.\n";
                    return temp;
                } else if (root->Right == NULL) {
                    BookNode* temp = root->Left;
                    delete root;
                    cout << "Book with ID " << id << " removed.\n";
                    return temp;
                }
                BookNode* successor = findMin(root->Right);
                root->BookID = successor->BookID;
                root->Title = successor->Title;
                root->Author = successor->Author;
                root->Genre = successor->Genre;
                root->CopiesAvailable = successor->CopiesAvailable;
                root->Right = deleteBook(root->Right, successor->BookID);
            }
            return root;
        }

        void deleteAllBooks(BookNode* root) {
            if (root == NULL) return;
            deleteAllBooks(root->Left);
            deleteAllBooks(root->Right);
            delete root;
        }

        void inorderPrintBooks(BookNode* root) {
            if (root == NULL) return;
            inorderPrintBooks(root->Left);
            cout << left
                 << setw(8) << root->BookID
                 << setw(25) << root->Title
                 << setw(20) << root->Author
                 << setw(15) << root->Genre
                 << setw(8) << root->CopiesAvailable << "\n";
            inorderPrintBooks(root->Right);
        }

        // ------ Member search ------
        MemberNode* searchMemberNode(int id) {
            MemberNode* temp = HEAD;
            while (temp != NULL) {
                if (temp->Id == id)
                    return temp;
                temp = temp->Next;
            }
            return NULL;
        }

    public:
        Library() {
            HEAD = NULL;
            returnTOP = NULL;
            ResFront = NULL;
            ResRear = NULL;
            BookRoot = NULL;
        }

        // ------ Book Functions ------
        BookNode* searchBook(int id) {
            return searchBookNode(BookRoot, id);
        }

        void displayBookByID(int id) {
            BookNode* result = searchBookNode(BookRoot, id);
            if (result == NULL)
                cout << "Book not found.\n";
            else {
                cout << "\nBook Found:\n"
                     << "ID: " << result->BookID << "\n"
                     << "Title: " << result->Title << "\n"
                     << "Author: " << result->Author << "\n"
                     << "Genre: " << result->Genre << "\n"
                     << "Copies Available: " << result->CopiesAvailable << "\n";
            }
        }

        void AddBook(int id, const string& title, const string& author, const string& genre, int copies) {
            BookRoot = addBook(BookRoot, id, title, author, genre, copies);
        }

        void RemoveBook(int id) {
            BookRoot = deleteBook(BookRoot, id);
        }

        void DisplayBooksInOrder() {
            if (BookRoot == NULL) {
                cout << "No books in the library.\n";
                return;
            }
            cout << "\nAll Books:\n";
            cout << left
                 << setw(8) << "ID"
                 << setw(25) << "Title"
                 << setw(20) << "Author"
                 << setw(15) << "Genre"
                 << setw(8) << "Copies" << "\n";
            cout << string(76, '-') << "\n";
            inorderPrintBooks(BookRoot);
        }

        // ------ Member Functions ------
        MemberNode* searchMember(int id) {
            return searchMemberNode(id);
        }

        void displayMemberByID(int memberID) {
            MemberNode* temp = searchMemberNode(memberID);
            if (temp == NULL) {
                cout << "Member not found.\n";
            } else {
                cout << "\nMember Found:\n"
                     << "ID: " << temp->Id << "\n"
                     << "Name: " << temp->Name << "\n"
                     << "Email: " << temp->Email << "\n"
                     << "Phone: " << temp->Phone << "\n"
                     << "Address: " << temp->Address << "\n";
            }
        }

        void AddMember(int id, const string& name, const string& email, const string& phone, const string& address) {
            MemberNode* NewMember = new MemberNode(id, name, email, phone, address);
            NewMember->Next = HEAD;
            HEAD = NewMember;
            cout << "Member added: " << name << " (ID: " << id << ")\n";
        }

        void RemoveMember(int id) {
            MemberNode* current = HEAD;
            MemberNode* prev = NULL;
            while (current != NULL) {
                if (current->Id == id) {
                    if (prev == NULL) // FIrst node
                        HEAD = current->Next;
                    else
                        prev->Next = current->Next;

                    BorrowRecord* record = current->History;
                    while (record != NULL) {
                        BorrowRecord* temp = record;
                        record = record->Next;
                        delete temp;
                    }

                    cout << "Removing member: " << current->Name << " (ID: " << id << ")\n";
                    delete current;
                    return;
                }
                prev = current;
                current = current->Next;
            }
            cout << "Member with ID " << id << " not found.\n";
        }

        void DisplayMembers() const {
            if (HEAD == NULL) {
                cout << "No members in the library.\n";
                return;
            }
            cout << "\nAll Library Members:\n";
            cout << left
                 << setw(8) << "ID"
                 << setw(20) << "Name"
                 << setw(25) << "Email"
                 << setw(15) << "Phone"
                 << setw(25) << "Address" << "\n";
            cout << string(93, '-') << "\n";
            MemberNode* temp = HEAD;
            while (temp != NULL) {
                cout << left
                     << setw(8) << temp->Id
                     << setw(20) << temp->Name
                     << setw(25) << temp->Email
                     << setw(15) << temp->Phone
                     << setw(25) << temp->Address << "\n";
                temp = temp->Next;
            }
        }

        // ------ Borrow History as Doubly Linked List ------
        void AddBorrowRecord(int memberid, const string& booktitle, const string& borrowdate) {
            MemberNode* temp = HEAD;
            while (temp != NULL) {
                if (temp->Id == memberid) {
                    BorrowRecord* newRecord = new BorrowRecord(booktitle, borrowdate);
                    if (temp->History == NULL) {
                        temp->History = newRecord;
                    } else {
                        BorrowRecord* tail = temp->History;
                        while (tail->Next != NULL)
                            tail = tail->Next;
                        tail->Next = newRecord;
                        newRecord->Prev = tail;
                    }
                    cout << "Borrow record added for " << temp->Name << "\n";
                    return;
                }
                temp = temp->Next;
            }
            cout << "Member ID not found.\n";
        }

        void ViewHistory(int memberid) {
            MemberNode* temp = HEAD;
            while (temp != NULL) {
                if (temp->Id == memberid) {
                    // Print forward
                    cout << "Borrow History (Oldest → Newest) for " << temp->Name << ":\n";
                    BorrowRecord* curr = temp->History;
                    if (!curr) {
                        cout << "No borrowing history.\n";
                        return;
                    }
                    while (curr != NULL) {
                        cout << " - " << curr->BookTitle << " (" << curr->BorrowDate << ")\n";
                        if (curr->Next == NULL) break;
                        curr = curr->Next;
                    }
                    // curr is now tail
                    cout << "Borrow History (Newest → Oldest) for " << temp->Name << ":\n";
                    while (curr != NULL) {
                        cout << " - " << curr->BookTitle << " (" << curr->BorrowDate << ")\n";
                        curr = curr->Prev;
                    }
                    return;
                }
                temp = temp->Next;
            }
            cout << "Member ID not found.\n";
        }

        // ------ Return Stack ------
        void PushReturn(int memberid, const string& booktitle, const string& returndate) {
            ReturnRecord* newreturn = new ReturnRecord(memberid, booktitle, returndate);
            newreturn->Next = returnTOP;
            returnTOP = newreturn;

            MemberNode* temp = HEAD;
            while (temp != NULL) {
                if (temp->Id == memberid) {
                    cout << "Book returned by " << temp->Name << ": " << booktitle << "\n";
                    return;
                }
                temp = temp->Next;
            }
            cout << "Book returned by Unknown Member (ID: " << memberid << ")\n";
        }

        // Pop
        void ProcessReturn() { 
            if (returnTOP == NULL) {
                cout << "No returned books to process.\n";
                return;
            }
            ReturnRecord* temprocess = returnTOP;
            MemberNode* temp = HEAD;
            string memberName = "Unknown";
            while (temp != NULL) {
                if (temp->Id == temprocess->memberID) {
                    memberName = temp->Name;
                    break;
                }
                temp = temp->Next;
            }
            cout << "Processing return from " << memberName << ": "
                 << temprocess->BookName << " (" << temprocess->ReturnDate << ")\n";
            returnTOP = returnTOP->Next;
            delete temprocess;
        }

        void DisplayReturns() {
    if (returnTOP == NULL) {
        cout << "No returned books in the stack.\n";
        return;
    }

    cout << "\n📚 Returned Books Stack (Newest → Oldest):\n";
    ReturnRecord* temp = returnTOP;
    while (temp != NULL) {
        cout << " - Member ID: " << temp->memberID
             << ", Book: " << temp->BookName
             << ", Date: " << temp->ReturnDate << "\n";
        temp = temp->Next;
    }
}


        // ------ Reservation Queue ------
        void enqueueReservation(int memberId, const string& memberName, const string& bookTitle) {
            Reservation* newres = new Reservation(memberId, memberName, bookTitle);
            if (ResFront == NULL)
                ResFront = ResRear = newres;
            else {
                ResRear->Next = newres;
                ResRear = newres;
            }
            cout << "Reservation added for " << memberName << " on book \"" << bookTitle << "\"\n";
        }

        void dequeueReservation() {
            if (ResFront == NULL) {
                cout << "No reservations to assign.\n";
                return;
            }
            Reservation* temp = ResFront;
            cout << "Assigning book \"" << temp->bookTitle << "\" to "
                 << temp->memberName << " (ID: " << temp->memberId << ")\n";
            ResFront = ResFront->Next;
            if (ResFront == NULL)
                ResRear = NULL;
            delete temp;
        }

        void DisplayReservations() {
    if (ResFront == NULL) {
        cout << "No current reservations.\n";
        return;
    }

    cout << " Current Book Reservations:\n";
    Reservation* temp = ResFront;
    int count = 1;

    while (temp != NULL) {
        cout << count++ << ". Member: " << temp->memberName
             << " (ID: " << temp->memberId << ") - Book: " << temp->bookTitle << "\n";
        temp = temp->Next;
    }
}


        // ------ Combined Search & Display ------
        void SearchBookByID(int bookID) {
            displayBookByID(bookID);
        }

        void SearchMemberByID(int memberID) {
            displayMemberByID(memberID);
        }

    
        ~Library() {
            MemberNode* curr = HEAD;
            while (curr) {
                MemberNode* temp = curr;
                BorrowRecord* rec = curr->History;
                while (rec) {
                    BorrowRecord* toDelete = rec;
                    rec = rec->Next;
                    delete toDelete;
                }
                curr = curr->Next;
                delete temp;
            }

            while (returnTOP) {
                ReturnRecord* temp = returnTOP;
                returnTOP = returnTOP->Next;
                delete temp;
            }

            while (ResFront) {
                Reservation* temp = ResFront;
                ResFront = ResFront->Next;
                delete temp;
            }

            deleteAllBooks(BookRoot);
        }
    };

int main() {
    welcomeScreen();
    loadingEffect("Launching system");
    
    Library myLib;
    int choice = -1;

    while (true) {
        showMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) {
            cout << "\n\033[1;33mExiting the program. Goodbye! 👋\033[0m\n";
            break;
        }

        switch (choice) {
            case 1: {
                int id, copies;
                string title, author, genre;
                cout << "Enter Book ID: "; cin >> id;
                cin.ignore();
                cout << "Enter Title: "; getline(cin, title);
                cout << "Enter Author: "; getline(cin, author);
                cout << "Enter Genre: "; getline(cin, genre);
                cout << "Enter Copies: "; cin >> copies;

                myLib.AddBook(id, title, author, genre, copies);
                cout << "\033[1;32mBook added successfully! ✅\033[0m\n";
                break;
            }
            case 2: {
                int id;
                cout << "Enter Book ID to remove: ";
                cin >> id;
                myLib.RemoveBook(id);
                break;
            }
            case 3: {
                int id;
                cout << "Enter Book ID to search: ";
                cin >> id;
                myLib.displayBookByID(id);
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            }
            case 4:
                myLib.DisplayBooksInOrder();
                break;
            case 5: {
                int id;
                string name, email, phone, address;
                cout << "Enter Member ID: "; cin >> id; cin.ignore();
                cout << "Enter Name: "; getline(cin, name);
                cout << "Enter Email: "; getline(cin, email);
                cout << "Enter Phone: "; getline(cin, phone);
                cout << "Enter Address: "; getline(cin, address);

                myLib.AddMember(id, name, email, phone, address);
                cout << "\033[1;32mMember added successfully! ✅\033[0m\n";
                break;
            }
            case 6: {
                int id;
                cout << "Enter Member ID to remove: ";
                cin >> id;
                myLib.RemoveMember(id);
                break;
            }
            case 7: {
                int id;
                cout << "Enter Member ID to search: ";
                cin >> id;
                myLib.displayMemberByID(id);
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                break;
            }
            case 8:
                myLib.DisplayMembers();
                break;
            case 9: {
                int mid, bid;
                string date;
                cout << "Enter Member ID: "; cin >> mid;
                cout << "Enter Book ID: "; cin >> bid;
                cin.ignore();
                cout << "Enter Borrow Date (YYYY-MM-DD): ";
                getline(cin, date);
                myLib.AddBorrowRecord(mid, to_string(bid), date);
                break;
            }
            case 10: {
                int mid;
                cout << "Enter Member ID: "; cin >> mid;
                myLib.ViewHistory(mid);
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                break;
            }
            case 11: {
                int mid, bid;
                string date;
                cout << "Enter Member ID: "; cin >> mid;
                cout << "Enter Book ID: "; cin >> bid;
                cin.ignore();
                cout << "Enter Return Date (YYYY-MM-DD): ";
                getline(cin, date);

                myLib.PushReturn(mid, to_string(bid), date);
                break;
            }
            case 12:
                myLib.ProcessReturn();
                break;
            case 13:
                myLib.DisplayReturns();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                break;

            case 14: {
                int mid, bid;
                cout << "Enter Member ID: "; cin >> mid;
                cout << "Enter Book ID: "; cin >> bid;
                myLib.enqueueReservation(mid, "", to_string(bid));
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            case 15:{
                myLib.dequeueReservation();
                break;
            }
            case 16:{
                myLib.DisplayReservations();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                break;
            }
            default:
                cout << "\033[1;31mInvalid choice. Try again! ❌\033[0m\n";
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    return 0;
}