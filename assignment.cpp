#include <iostream>
#include <iomanip>
#include <string>
#include <regex>
#include <vector>
#include <cctype>
#include <ctime>
#include <sstream>
#include <cstdlib>
using namespace std;


// System data
struct Customer {
    string id;
    string name;
    string phone;
    string password;
    bool isMember;
    string membershipExpiry;
    double totalSpent;
    int loyaltyPoints;
};

struct Feedback {
    string orderID;
    string customerID;
    int rating;
    string comment;
};

struct Staff {
    string id;
    string name;
    string phone;
    char gender;
    string dateJoined;
    bool status;
    string password;
};
const string DEFAULT_STAFF_PASSWORD = "staff123";

struct Admin {
    string id;
    string name;
    string password;
};

struct OrderItem {
    string serviceType;
    int quantity;
    double price;
};

struct Order {
    string orderID;
    string bookingID;
    string customerID;
    vector<OrderItem> items;
    double price;
    string status;
    string date;
    bool pointsAwarded;
};

struct MembershipPayment {
    string paymentID;
    string customerID;
    double membershipAmount;
    double amountPaid;
    double change;
    string method;
    string date;
};

struct Booking {
    string bookingID;
    string customerID;
    vector<string> services;
    string date;
    string timeSlot;
    string status;
};

struct Price {
    double price[3][2];
};

struct PriceHistory {
    string date;
    string service;
    double oldPrice;
    double newPrice;
    string customerType;
};

struct Payment {
    string paymentID;
    string orderID;
    string customerID;
    double originalAmount;
    double discount;
    double finalAmount;
    double amountPaid;
    double change;
    string method;
    string date;
};

struct Notification {
    string customerID;
    string message;
    string date;
    bool read;
};


// Stored records
vector<Order> orders;
vector<Staff> staffs;
vector<Customer> customers;
vector<MembershipPayment> membershipPayments;
vector<Admin> admins;
vector<Feedback> feedbacks;
vector<PriceHistory> priceHistories;
vector<Booking> bookings;
vector<Payment> payments;
vector<Notification> notifications;
Price price;

// Fixed values
const double MEMBERSHIP_FEE = 20.00;
const int POINTS_PER_RM = 1;
const int POINTS_TO_REDEEM = 100;
const double REWARD_DISCOUNT = 5.00;


// Input functions
int getMenuChoice(int minOption, int maxOption)
{
    string input;
    int choice;

    while (true)
    {
        cout << "\nEnter your choice: ";
        getline(cin >> ws, input);

        stringstream ss(input);
        char extra;

        if (!(ss >> choice) || (ss >> extra) ||
            choice < minOption || choice > maxOption)
        {
            cout << "Invalid choice! Please enter a number between "
                << minOption << " and " << maxOption << ".\n";
        }
        else
        {
            return choice;
        }
    }
}

// get positive whole number only
int getPositiveInt(string prompt)
{
    string input;
    int value;

    while (true)
    {
        cout << prompt;
        getline(cin >> ws, input);

        stringstream ss(input);
        char extra;

        if (!(ss >> value) || (ss >> extra) || value <= 0)
        {
            cout << "Invalid input! Please enter a positive whole number.\n";
        }
        else
        {
            return value;
        }
    }
}

// get number within the given range
int getIntInput(string prompt, int minValue, int maxValue)
{
    string input;
    int value;

    while (true)
    {
        cout << prompt;
        getline(cin >> ws, input);

        stringstream ss(input);
        char extra;

        if (!(ss >> value) || (ss >> extra) ||
            value < minValue || value > maxValue)
        {
            cout << "Invalid input! Please enter a number between "
                << minValue << " and " << maxValue << ".\n";
        }
        else
        {
            return value;
        }
    }
}

// make sure payment amount is enough
double getMinimumDouble(string prompt, double minValue)
{
    string input;
    double value;

    while (true)
    {
        cout << prompt;
        getline(cin >> ws, input);

        stringstream ss(input);
        char extra;

        if (!(ss >> value) || (ss >> extra) || value < minValue)
        {
            cout << "Invalid amount! Please enter RM "
                << fixed << setprecision(2)
                << minValue << " or more.\n";
        }
        else
        {
            return value;
        }
    }
}

// get positive number only
double getPositiveDouble(string prompt)
{
    string input;
    double value;

    while (true)
    {
        cout << prompt;
        getline(cin >> ws, input);

        stringstream ss(input);
        char extra;

        if (!(ss >> value) || (ss >> extra) || value <= 0)
        {
            cout << "Invalid input! Please enter a positive number.\n";
        }
        else
        {
            return value;
        }
    }
}

// get one valid character choice
char getCharChoice(string prompt, string validChoices)
{
    string input;

    while (true)
    {
        cout << prompt;
        getline(cin >> ws, input);

        if (input.length() == 1)
        {
            char choice = toupper(
                static_cast<unsigned char>(input[0]));

            if (validChoices.find(choice) != string::npos)
            {
                return choice;
            }
        }

        cout << "Invalid choice! Please enter ";

        for (size_t i = 0; i < validChoices.length(); i++)
        {
            if (i > 0)
                cout << " or ";

            cout << validChoices[i];
        }

        cout << ".\n";
    }
}

// get input that cannot be empty
string getNonEmptyInput(string prompt, string errorMessage)
{
    string input;

    while (true)
    {
        cout << prompt;
        getline(cin, input);

        bool hasContent = false;

        for (char c : input)
        {
            if (!isspace(static_cast<unsigned char>(c)))
            {
                hasContent = true;
                break;
            }
        }

        if (hasContent)
        {
            return input;
        }

        cout << errorMessage << endl;
    }
}

// check if input contain numbers only
bool isValidNumber(const string& str)
{
    if (str.empty())
        return false;

    for (char c : str)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    }

    return true;
}

// check valid phone number
bool isValidPhoneNumber(const string& phone)
{
    if (!isValidNumber(phone))
        return false;

    if (phone.length() < 10 || phone.length() > 11)
        return false;

    if (phone[0] != '0')
        return false;

    return true;
}

// check the time slots used for submit booking
bool isTimeSlotValid(string date, int timeChoice)
{
    time_t now = time(0);

    tm currentDate{};
    localtime_s(&currentDate, &now);

    int currentDay = currentDate.tm_mday;
    int currentMonth = currentDate.tm_mon + 1;
    int currentYear = currentDate.tm_year + 1900;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    // Time slot starting hour
    int slotHour;

    if (timeChoice == 1)
        slotHour = 10;
    else if (timeChoice == 2)
        slotHour = 11;
    else if (timeChoice == 3)
        slotHour = 14;
    else
        slotHour = 15;

    // Only check time if selected date is today
    if (day == currentDay &&
        month == currentMonth &&
        year == currentYear)
    {
        int currentHour = currentDate.tm_hour;

        if (slotHour <= currentHour)
        {
            return false;
        }
    }

    return true;
}

// Date functions
bool isValidDate(string date)
{
    regex pattern("\\d{2}/\\d{2}/\\d{4}");

    if (!regex_match(date, pattern))
        return false;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (month < 1 || month > 12)
        return false;

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
    {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    time_t now = time(0);
    tm today{};
    localtime_s(&today, &now);

    int currentYear = today.tm_year + 1900;
    int currentMonth = today.tm_mon + 1;
    int currentDay = today.tm_mday;

    if (year < currentYear)
        return false;

    if (year == currentYear && month < currentMonth)
        return false;

    if (year == currentYear && month == currentMonth && day < currentDay)
    {
        return false;
    }

    return true;
}
// check date used for report
bool isValidReportDate(string date) // used for admin report
{
    regex pattern("\\d{2}/\\d{2}/\\d{4}");

    if (!regex_match(date, pattern))
        return false;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (month < 1 || month > 12)
        return false;

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
    {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    time_t now = time(0);
    tm today{};
    localtime_s(&today, &now);

    int currentYear = today.tm_year + 1900;
    int currentMonth = today.tm_mon + 1;
    int currentDay = today.tm_mday;

    if (year > currentYear)
        return false;

    if (year == currentYear && month > currentMonth)
        return false;

    if (year == currentYear && month == currentMonth && day > currentDay)
        return false;

    return true;
}

// check month used for report
bool isValidReportMonth(string month)
{
    regex pattern("\\d{2}/\\d{4}");

    if (!regex_match(month, pattern))
        return false;

    int monthValue = stoi(month.substr(0, 2));

    int yearValue = stoi(month.substr(3, 4));

    if (monthValue < 1 || monthValue > 12)
    {
        return false;
    }

    if (yearValue < 1)
        return false;

    // Prevent future month
    time_t now = time(0);
    tm today{};
    localtime_s(&today, &now);

    int currentYear = today.tm_year + 1900;
    int currentMonth = today.tm_mon + 1;

    if (yearValue > currentYear)
        return false;

    if (yearValue == currentYear && monthValue > currentMonth)
        return false;

    return true;
}

// check year used for report
bool isValidReportYear(string year)
{
    regex pattern("\\d{4}");

    if (!regex_match(year, pattern))
        return false;

    int yearValue = stoi(year);

    if (yearValue < 1)
        return false;

    // Prevent future year
    time_t now = time(0);
    tm today{};
    localtime_s(&today, &now);

    int currentYear = today.tm_year + 1900;

    if (yearValue > currentYear)
        return false;

    return true;

}

// get membership expiry date
string getMembershipExpiryDate()
{
    time_t now = time(0);
    tm currentDate{};

    localtime_s(&currentDate, &now);

    int day = currentDate.tm_mday;
    int month = currentDate.tm_mon + 1;
    int year = currentDate.tm_year + 1900;

    year++;

    // Handle 29 February
    if (month == 2 && day == 29)
    {
        day = 28;
    }

    string expiryDate = (day < 10 ? "0" : "") + to_string(day) + "/" + (month < 10 ? "0" : "") + to_string(month) + "/" + to_string(year);

    return expiryDate;
}

// check if membership already expired
bool isMembershipExpired(string expiryDate)
{
    if (expiryDate.empty())
    {
        return true;
    }

    int day;
    int month;
    int year;

    char slash1;
    char slash2;

    stringstream ss(expiryDate);

    ss >> day >> slash1 >> month >> slash2 >> year;

    if (ss.fail() || slash1 != '/' || slash2 != '/')
    {
        return true;
    }

    time_t now = time(0);
    tm currentDate;

    localtime_s(&currentDate, &now);

    int currentYear = currentDate.tm_year + 1900;
    int currentMonth = currentDate.tm_mon + 1;
    int currentDay = currentDate.tm_mday;

    if (year < currentYear)
    {
        return true;
    }

    if (year == currentYear && month < currentMonth)
    {
        return true;
    }

    if (year == currentYear && month == currentMonth && day < currentDay)
    {
        return true;
    }

    return false;
}

string getMembershipStatus(const Customer& c)
{
    if (!c.isMember)
    {
        return "Not a Member";
    }

    if (isMembershipExpired(c.membershipExpiry))
    {
        return "Expired";
    }

    return "Active";
}

// get today's date
string getCurrentDate()
{
    time_t now = time(0);
    tm localTime{};
    localtime_s(&localTime, &now);

    string day = (localTime.tm_mday < 10 ? "0" : "") + to_string(localTime.tm_mday);

    string month = (localTime.tm_mon + 1 < 10 ? "0" : "") + to_string(localTime.tm_mon + 1);

    return day + "/" + month + "/" + to_string(localTime.tm_year + 1900);
}

// show available services and prices
void displayServices()
{
    string serviceNames[3] = {
        "Wash",
        "Dry Clean",
        "Iron"
    };

    cout << "\n+==============================================================+\n";
    cout << "|                    AVAILABLE SERVICES                        |\n";
    cout << "+==============================================================+\n";

    cout << left << setw(5) << "No." << setw(18) << "Service" << setw(18) << "Non-member" << setw(18) << "Member" << endl;

    cout << "--------------------------------------------------------------\n";

    for (int i = 0; i < 3; i++)
    {
        cout << left << setw(5) << i + 1 << setw(18) << serviceNames[i] << "RM " << setw(14) << fixed << setprecision(2) << price.price[i][0] << "RM " << fixed << setprecision(2) << price.price[i][1] << "/kg" << endl;
    }

    cout << "--------------------------------------------------------------\n";
}

// get service price based on member type
double getServicePrice(int serviceIndex, bool isMember)
{
    int memberIndex = isMember ? 1 : 0;

    return price.price[serviceIndex][memberIndex];
}

// calculate cost for selected service
double calculateServiceCost(int serviceIndex, int quantity, bool isMember)
{
    return getServicePrice(serviceIndex, isMember) * quantity;
}


// Notification functions
void addNotification(string customerID, string message)
{
    Notification n;

    n.customerID = customerID;
    n.message = message;
    n.date = getCurrentDate();
    n.read = false;

    notifications.push_back(n);

    int count = 0;

    for (int i = (int)notifications.size() - 1; i >= 0; i--)
    {
        if (notifications[i].customerID == customerID)
        {
            count++;

            if (count > 10)
                notifications.erase(notifications.begin() + i);
        }
    }
}

// show customer notifications
void viewNotifications(string customerID)
{
    cout << "\n+==================================================+\n";
    cout << "|                 NOTIFICATIONS                    |\n";
    cout << "+==================================================+\n";

    bool found = false;

    for (Notification& n : notifications)
    {
        if (n.customerID == customerID)
        {
            cout << "\nDate    : " << n.date << endl;
            cout << "Message : " << n.message << endl;
            cout << "Status  : " << (n.read ? "Read" : "New") << endl;

            cout << "------------------------------------------\n";

            n.read = true;
            found = true;
        }
    }

    if (!found)
        cout << "No notifications found.\n";
}


// Loyalty functions
void updateLoyaltyPoints(string customerID, double orderAmount)
{
    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            int earned = static_cast<int>(orderAmount * POINTS_PER_RM);

            c.loyaltyPoints += earned;

            cout << "Loyalty points earned: " << earned << endl;

            cout << "Current loyalty points: " << c.loyaltyPoints << endl;

            return;
        }
    }
}

// show current loyalty points
void showLoyaltyPoints(string customerID)
{
    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            cout << "\n+===============================================+\n";
            cout << "|              LOYALTY POINTS                   |\n";
            cout << "+===============================================+\n";

            cout << "Current Points : " << c.loyaltyPoints << endl;

            cout << "100 Points     : RM 5.00 discount\n";
            cout << "Points earned  : 1 point for every RM 1 spent\n";

            return;
        }
    }

    cout << "Customer not found.\n";
}



// Receipt function
void printReceipt(const Payment& payment, string description)
{
    cout << "\n+==================================================+\n";
    cout << "|                  PAYMENT RECEIPT                 |\n";
    cout << "+==================================================+\n";

    cout << "Payment ID     : " << payment.paymentID << endl;

    cout << "Order ID       : " << payment.orderID << endl;

    cout << "Customer ID    : " << payment.customerID << endl;

    cout << "Description    : " << description << endl;

    cout << "Original Amount: RM " << fixed << setprecision(2) << payment.originalAmount << endl;

    cout << "Discount       : RM " << fixed << setprecision(2) << payment.discount << endl;

    cout << "Final Amount   : RM " << fixed << setprecision(2) << payment.finalAmount << endl;

    cout << "Amount Paid    : RM " << fixed << setprecision(2) << payment.amountPaid << endl;

    cout << "Change         : RM " << fixed << setprecision(2) << payment.change << endl;

    cout << "Payment Method : " << payment.method << endl;

    cout << "Date           : " << payment.date << endl;

    cout << "+==================================================+\n";
}


// Payment functions
bool makePayment(double amount, string customerID, string orderID, string description, Payment& payment, double& finalAmount)
{
    double discount = 0.0;
    int rewardCount = 0;

    cout << "\n+===============================================+\n";
    cout << "|                    PAYMENT                    |\n";
    cout << "+===============================================+\n";

    cout << "Original Amount: RM " << fixed << setprecision(2) << amount << endl;

    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            cout << "Current Loyalty Points: " << c.loyaltyPoints << endl;

            char choice;

            choice = getCharChoice("Use loyalty points for discount? (Y/N): ", "YN");

            if (choice == 'Y')
            {
                if (c.loyaltyPoints < POINTS_TO_REDEEM)
                {
                    cout << "Not enough loyalty points to redeem. " << POINTS_TO_REDEEM << " points = RM " << fixed << setprecision(2) << REWARD_DISCOUNT << " discount.\n";
                }
                else
                {
                    int availableRewards = c.loyaltyPoints / POINTS_TO_REDEEM;

                    int maxUsefulRewards = static_cast<int>(amount / REWARD_DISCOUNT);

                    if (amount > maxUsefulRewards * REWARD_DISCOUNT)
                    {
                        maxUsefulRewards++;
                    }

                    int maxRewards = availableRewards;

                    if (maxRewards > maxUsefulRewards)
                        maxRewards = maxUsefulRewards;

                    cout << "You can use up to " << maxRewards << " reward(s).\n";

                    rewardCount = getIntInput(
                        "How many RM 5.00 rewards to use? ", 1, maxRewards);

                    discount = rewardCount * REWARD_DISCOUNT;

                    if (discount > amount)
                        discount = amount;

                    c.loyaltyPoints -= rewardCount * POINTS_TO_REDEEM;
                }
            }

            break;
        }
    }

    finalAmount = amount - discount;

    cout << "Loyalty Discount: RM " << fixed << setprecision(2) << discount << endl;

    cout << "Amount to Pay   : RM " << fixed << setprecision(2) << finalAmount << endl;

    cout << "\nPayment Method:\n";
    cout << "1. Cash\n";
    cout << "2. Card\n";
    cout << "3. Online Banking\n";

    int paymentChoice = getMenuChoice(1, 3);

    string methods[3] = {
        "Cash",
        "Card",
        "Online Banking"
    };

    payment.method = methods[paymentChoice - 1];

    double amountPaid;

    if (paymentChoice == 1)
    {
        amountPaid = getMinimumDouble("Enter amount received: RM ", finalAmount);
    }
    else
    {
        amountPaid = finalAmount;
        cout << "Payment amount: RM " << fixed << setprecision(2) << amountPaid << endl;
    }

    payment.paymentID = "P" + to_string(payments.size() + 3001);

    payment.orderID = orderID;
    payment.customerID = customerID;
    payment.originalAmount = amount;
    payment.discount = discount;
    payment.finalAmount = finalAmount;
    payment.amountPaid = amountPaid;
    payment.change = amountPaid - finalAmount;
    payment.date = getCurrentDate();

    payments.push_back(payment);

    cout << "\nPayment successful!\n";

    cout << "Payment Method: " << payment.method << endl;

    cout << "Change        : RM " << fixed << setprecision(2) << payment.change << endl;

    printReceipt(payment, description);

    return true;
}

// print membership receipt
void printMembershipReceipt(const MembershipPayment& payment, string type)
{
    cout << "\n+==================================================+\n";
    cout << "|               MEMBERSHIP RECEIPT                 |\n";
    cout << "+==================================================+\n";

    cout << "Payment ID       : " << payment.paymentID << endl;

    cout << "Customer ID      : " << payment.customerID << endl;

    cout << "Transaction      : " << type << endl;

    cout << "Membership Fee   : RM " << fixed << setprecision(2) << payment.membershipAmount << endl;

    cout << "Amount Paid      : RM " << fixed << setprecision(2) << payment.amountPaid << endl;

    cout << "Change           : RM " << fixed << setprecision(2) << payment.change << endl;

    cout << "Payment Method   : " << payment.method << endl;

    cout << "Date             : " << payment.date << endl;

    cout << "+==================================================+\n";
    cout << "|           Thank you for your payment!            |\n";
    cout << "+==================================================+\n";
}

// handle membership payment
bool makeMembershipPayment(string customerID, double membershipFee, string transactionType)
{
    MembershipPayment payment;

    cout << "\n+===============================================+\n";
    cout << "|              MEMBERSHIP PAYMENT               |\n";
    cout << "+===============================================+\n";

    cout << "Membership Fee : RM " << fixed << setprecision(2) << membershipFee << endl;

    cout << "\nPayment Method:\n";
    cout << "1. Cash\n";
    cout << "2. Card\n";
    cout << "3. Online Banking\n";

    int paymentChoice = getMenuChoice(1, 3);

    string methods[3] = {
        "Cash",
        "Card",
        "Online Banking"
    };

    payment.method = methods[paymentChoice - 1];

    double amountPaid;

    if (paymentChoice == 1)
    {
        amountPaid = getMinimumDouble("Enter amount received: RM ", membershipFee);
    }
    else
    {
        amountPaid = membershipFee;

        cout << "Payment amount: RM " << fixed << setprecision(2) << amountPaid << endl;
    }

    // Generate membership payment ID
    payment.paymentID = "MP" + to_string(membershipPayments.size() + 1001);

    payment.customerID = customerID;

    payment.membershipAmount = membershipFee;

    payment.amountPaid = amountPaid;

    payment.change = amountPaid - membershipFee;

    payment.date = getCurrentDate();

    // Store payment
    membershipPayments.push_back(payment);

    cout << "\nMembership payment successful!\n";

    cout << "Payment ID : " << payment.paymentID << endl;

    cout << "Change     : RM " << fixed << setprecision(2) << payment.change << endl;

    // Print receipt
    printMembershipReceipt(payment, transactionType);

    return true;
}

// Customer functions
void registerCustomer()
{
    Customer c;

    cout << "\n+==================================================+\n";
    cout << "|               CUSTOMER REGISTRATION              |\n";
    cout << "+==================================================+\n";

    c.name = getNonEmptyInput("Enter Name: ", "Name cannot be empty!");

    do
    {
        cout << "Enter Phone Number: ";
        getline(cin, c.phone);

        if (!isValidPhoneNumber(c.phone))
        {
            cout << "Invalid phone number! Please enter numbers only.\n";
        }

    } while (!isValidPhoneNumber(c.phone));

    c.password = getNonEmptyInput("Enter Password: ", "Password cannot be empty!");

    int nextNumber = static_cast<int>(customers.size()) + 1001;

    while (true)
    {
        c.id = "C" + to_string(nextNumber);

        bool duplicate = false;

        for (Customer& existing : customers)
        {
            if (existing.id == c.id)
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
            break;

        nextNumber++;
    }

    c.totalSpent = 0.0;
    c.loyaltyPoints = 0;
    c.isMember = false;

    char membershipChoice;

    membershipChoice = getCharChoice("\nWould you like to become a member? (Y/N): ", "YN");

    if (membershipChoice == 'Y')
    {
        Payment payment;
        double finalAmount;

        cout << "\nMembership Fee: RM " << fixed << setprecision(2) << MEMBERSHIP_FEE << endl;

        if (makePayment(MEMBERSHIP_FEE, c.id, "MEMBERSHIP", "Membership Fee", payment, finalAmount))
        {
            c.isMember = true;
            c.membershipExpiry = getMembershipExpiryDate();

            cout << "\nMembership activated successfully!\n";
            cout << "Membership Status : Active\n";
            cout << "Expiry Date : " << c.membershipExpiry << endl;
        }
    }

    customers.push_back(c);

    cout << "\n+==================================================+\n";
    cout << "|           REGISTRATION SUCCESSFUL                |\n";
    cout << "+==================================================+\n";
    cout << "Customer ID : " << c.id << endl;
    cout << "Name        : " << c.name << endl;
    cout << "Phone       : " << c.phone << endl;
    cout << "Membership  : " << (c.isMember ? "Member" : "Non-member") << endl;
    cout << "\nPlease remember your Customer ID and password.\n";
}

// ask if customer want another service
bool askAddAnotherService()
{
    char addMore = getCharChoice("\nAdd another service? (Y/N): ", "YN");

    return addMore == 'Y';
}

// customer submit booking request
int submitBooking(string customerID)
{
    Booking b;

    string serviceNames[3] = {
        "Wash",
        "Dry Clean",
        "Iron"
    };

    bool isMember = false;

    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            isMember = c.isMember && !isMembershipExpired(c.membershipExpiry);;
            break;
        }
    }

    cout << "\n+==================================================+\n";
    cout << "|              SUBMIT BOOKING REQUEST              |\n";
    cout << "+==================================================+\n";

    cout << "Customer Type: " << (isMember ? "Member" : "Non-member") << endl;

    displayServices();

    cout << "Actual weight will be measured by staff at the shop.\n";

    while (true)
    {
        int choice;

        cout << "\nChoose Service (1-3) (enter 0 to go back): ";
        choice = getMenuChoice(0, 3);

        if (choice == 0)
        {
            cout << "Returning to customer menu...\n";
            return 0;
        }

        bool alreadySelected = false;

        for (string& service : b.services)
        {
            if (service == serviceNames[choice - 1])
            {
                alreadySelected = true;
                break;
            }
        }

        if (alreadySelected)
        {
            cout << "This service has already been selected.\n";
        }
        else
        {
            b.services.push_back(serviceNames[choice - 1]);

            cout << serviceNames[choice - 1]
                << " added successfully.\n";
        }

        if (!askAddAnotherService())
            break;
    }

    cout << "\nSelected Services:\n";

    for (size_t i = 0; i < b.services.size(); i++)
    {
        cout << i + 1 << ". " << b.services[i] << endl;
    }

    while (true)
    {
        do
        {
            cout << "\nEnter Drop-off Date (DD/MM/YYYY): ";
            getline(cin, b.date);

            if (!isValidDate(b.date))
            {
                cout << "Invalid date! Please enter a valid date that is not in the past.\n";
            }

        } while (!isValidDate(b.date));

        cout << "\nAvailable Time Slots:\n";
        cout << "1. 10:00 AM - 11:00 AM\n";
        cout << "2. 11:00 AM - 12:00 PM\n";
        cout << "3. 2:00 PM - 3:00 PM\n";
        cout << "4. 3:00 PM - 4:00 PM\n";

        int timeChoice = getMenuChoice(1, 4);

        if (!isTimeSlotValid(b.date, timeChoice))
        {
            cout << "This time slot has already passed.\n";
            cout << "Please choose another drop-off date.\n";

            // Go back to Drop-off Date
            continue;
        }

        string timeSlots[4] = {
            "10:00 AM - 11:00 AM",
            "11:00 AM - 12:00 PM",
            "2:00 PM - 3:00 PM",
            "3:00 PM - 4:00 PM"
        };

        b.timeSlot = timeSlots[timeChoice - 1];

        break;
    }

    b.bookingID = "B" + to_string(bookings.size() + 1001);

    b.customerID = customerID;

    b.status = "Pending";

    bookings.push_back(b);

    addNotification(customerID, "Your booking " + b.bookingID + " has been submitted and is waiting for confirmation.");

    cout << "\n+===============================================+\n";
    cout << "|            BOOKING SUBMITTED                  |\n";
    cout << "+===============================================+\n";

    cout << "Booking ID : " << b.bookingID << endl;

    cout << "Services   : ";

    for (size_t i = 0; i < b.services.size(); i++)
    {
        cout << b.services[i];

        if (i < b.services.size() - 1)
        {
            cout << ", ";
        }
    }

    cout << endl;

    cout << "Date       : " << b.date << endl;
    cout << "Time       : " << b.timeSlot << endl;
    cout << "Status     : " << b.status << endl;
    return 1;
}

// show current booking and order status
void viewCurrentOrders(string customerID)
{
    bool found = false;

    cout << "\n+================================================================+\n";
    cout << "|                     CURRENT ORDERS                             |\n";
    cout << "+================================================================+\n";

    for (Booking& b : bookings)
    {
        if (b.customerID == customerID && b.status != "Completed" && b.status != "Cancelled")
        {
            cout << "\nBooking ID : " << b.bookingID << endl;
            cout << "Date       : " << b.date << endl;
            cout << "Time       : " << b.timeSlot << endl;
            cout << "Status     : " << b.status << endl;

            found = true;
        }
    }

    for (Order& o : orders)
    {
        if (o.customerID == customerID && o.status != "Collected" && o.status != "Cancelled")
        {
            cout << "\nOrder ID   : " << o.orderID << endl;
            cout << "Services   :\n";

            for (size_t i = 0; i < o.items.size(); i++)
            {
                cout << "  " << i + 1 << ". " << o.items[i].serviceType << " - " << o.items[i].quantity << " kg - RM " << fixed << setprecision(2) << o.items[i].price << endl;
            }

            cout << "Total Price: RM " << fixed << setprecision(2) << o.price << endl;
            cout << "Status     : " << o.status << endl;

            found = true;
        }
    }

    if (!found)
        cout << "No current orders found.\n";
}

// show customer booking history
void bookingHistory(string customerID)
{
    cout << "\n+==================================================+\n";
    cout << "|                BOOKING HISTORY                   |\n";
    cout << "+==================================================+\n";

    bool found = false;

    for (Booking& b : bookings)
    {
        if (b.customerID == customerID)
        {
            cout << "\nBooking ID : " << b.bookingID << endl;

            cout << "Services   : ";

            for (size_t i = 0; i < b.services.size(); i++)
            {
                cout << b.services[i];

                if (i < b.services.size() - 1)
                {
                    cout << ", ";
                }
            }

            cout << endl;

            cout << "Date       : " << b.date << endl;

            cout << "Time       : " << b.timeSlot << endl;

            cout << "Status     : " << b.status << endl;

            cout << "------------------------------------------\n";

            found = true;
        }
    }

    if (!found)
        cout << "No booking history found.\n";
}

// show selected order details
void viewOrderDetails(string customerID)
{
    bool hasOrder = false;

    // Check whether customer has any order
    for (Order& o : orders)
    {
        if (o.customerID == customerID)
        {
            hasOrder = true;
            break;
        }
    }

    // If customer has no order
    if (!hasOrder)
    {
        cout << "\nYou have no orders yet.\n";
        return;
    }

    string orderID;

    cout << "\nEnter Order ID: ";
    getline(cin, orderID);

    if (!orderID.empty())
    {
        orderID[0] = toupper(static_cast<unsigned char>(orderID[0]));
    }

    for (Order& o : orders)
    {
        if (o.orderID == orderID && o.customerID == customerID)
        {
            cout << "\n+==========================================+\n";
            cout << "|              ORDER DETAILS               |\n";
            cout << "+==========================================+\n";

            cout << "Order ID     : " << o.orderID << endl;
            cout << "Booking ID   : " << o.bookingID << endl;
            cout << "Customer ID  : " << o.customerID << endl;
            cout << "Services     :\n";

            for (size_t i = 0; i < o.items.size(); i++)
            {
                cout << "  " << i + 1 << ". " << o.items[i].serviceType << endl;
                cout << "     Quantity : " << o.items[i].quantity << " kg" << endl;
                cout << "     Price    : RM " << fixed << setprecision(2) << o.items[i].price << endl;
            }

            cout << "Total Price  : RM " << fixed << setprecision(2) << o.price << endl;
            cout << "Date         : " << o.date << endl;
            cout << "Status       : " << o.status << endl;

            return;
        }
    }

    cout << "Order not found.\n";
}

// calculate average rating
double calculateAverageRating(string customerID)
{
    double total = 0.0;
    int count = 0;

    for (Feedback& f : feedbacks)
    {
        if (f.customerID == customerID)
        {
            total += f.rating;
            count++;
        }
    }

    if (count == 0)
        return 0.0;

    return total / count;
}

// customer submit rating and feedback
void submitFeedback(string customerID)
{

    //Check whether customer has any order 
    bool hasOrder = false;

    for (const Order& o : orders) {

        if (o.customerID == customerID) {
            hasOrder = true;
            break;
        }
    }

    // If customer has no order 
    if (!hasOrder) {
        cout << "\nYou have no orders yet.\n";
        return;
    }

    Feedback f;

    f.customerID = customerID;

    // Ask customer for Order ID 
    cout << "\nEnter Order ID to submit feedback: ";
    getline(cin, f.orderID);

    if (!f.orderID.empty()) {
        f.orderID[0] = toupper(static_cast<unsigned char>(f.orderID[0]));
    }

    // Check whether the order belongs to the customer and has already been collected 
    bool validOrder = false;

    for (const Order& o : orders) {

        if (o.orderID == f.orderID && o.customerID == customerID) {

            if (o.status == "Collected") {
                validOrder = true;
            }
            else {
                cout << "\nYou cannot submit feedback yet.\n";

                cout << "Order " << o.orderID << " is currently: " << o.status << endl;
                cout << "Feedback can only be submitted after the order is collected.\n";

            }
            break;
        }
    }

    if (!validOrder) {
        cout << "Invalid Order ID or the order does not belong to you.\n"; return;
    }

    //Check whether feedback has already been submitted 
    for (const Feedback& existingFeedback : feedbacks) {

        if (existingFeedback.orderID == f.orderID && existingFeedback.customerID == customerID) {
            cout << "\nFeedback has already been submitted for this order.\n";
            return;
        }
    }


    //Enter rating
    cout << "\nPlease select your rating (1 - 5).\n";
    f.rating = getMenuChoice(1, 5);

    //Enter comment 
    cout << "Comment: "; getline(cin, f.comment);

    //Save feedback 
    feedbacks.push_back(f);
    cout << "\nFeedback submitted successfully!\n";
    cout << "Average rating for your submitted feedback: " << fixed << setprecision(2) << calculateAverageRating(customerID) << "/5\n";
}

// Membership 
void viewMembership(string customerID)
{
    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            cout << "\n+==========================================+\n";
            cout << "|           MEMBERSHIP STATUS              |\n";
            cout << "+==========================================+\n";

            cout << "Customer ID       : " << c.id << endl;

            if (!c.isMember)
            {
                cout << "Member Status     : Not a Member\n";
                cout << "Membership Expiry : N/A\n";
            }
            else
            {
                if (isMembershipExpired(c.membershipExpiry))
                {
                    cout << "Member Status     : Expired\n";
                }
                else
                {
                    cout << "Member Status     : Active\n";
                }

                cout << "Membership Expiry : " << c.membershipExpiry << endl;
            }

            cout << "+==========================================+\n";

            return;
        }
    }

    cout << "Customer not found!\n";
}

// let customer join membership
void becomeMember(string customerID)
{
    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            // Customer is already a member
            if (c.isMember)
            {
                // Membership expired
                if (isMembershipExpired(c.membershipExpiry))
                {
                    cout << "\n+==========================================+\n";
                    cout << "|        MEMBERSHIP HAS EXPIRED            |\n";
                    cout << "+==========================================+\n";

                    cout << "Membership Expiry : " << c.membershipExpiry << endl;

                    cout << "\nPlease go to 'Renew Membership'.\n";

                    return;
                }

                // Membership is still active
                cout << "\n+==========================================+\n";
                cout << "|           ALREADY A MEMBER               |\n";
                cout << "+==========================================+\n";

                cout << "Member Status     : Active\n";
                cout << "Membership Expiry : " << c.membershipExpiry << endl;

                cout << "\nYou do not need to become a member again.\n";

                return;
            }

            // Customer is NOT a member
            cout << "\n+==========================================+\n";
            cout << "|              BECOME MEMBER               |\n";
            cout << "+==========================================+\n";

            cout << "Membership Fee    : RM " << fixed << setprecision(2) << MEMBERSHIP_FEE << endl;

            cout << "Membership Period : 1 Year\n";

            char confirm;

            confirm = getCharChoice("\nProceed to membership payment? (Y/N): ", "YN");

            if (confirm != 'Y')
            {
                cout << "Membership registration cancelled.\n";
                return;
            }

            // Membership payment
            if (makeMembershipPayment(customerID, MEMBERSHIP_FEE, "BECOME MEMBER"))
            {
                c.isMember = true;

                c.membershipExpiry = getMembershipExpiryDate();

                cout << "\nMembership activated successfully!\n";

                cout << "Membership Status : Active\n";

                cout << "Expiry Date       : " << c.membershipExpiry << endl;
            }

            return;
        }
    }

    cout << "Customer not found.\n";
}

// renew expired membership
void renewMembership(string customerID)
{
    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            // Customer is not a member
            if (!c.isMember)
            {
                cout << "\nYou are not a member.\n";
                cout << "Please choose 'Become Member' first.\n";
                return;
            }

            // Membership is still active
            if (!isMembershipExpired(c.membershipExpiry))
            {
                cout << "\n+==========================================+\n";
                cout << "|          MEMBERSHIP STILL ACTIVE         |\n";
                cout << "+==========================================+\n";

                cout << "Current Expiry Date : " << c.membershipExpiry << endl;

                cout << "\nYou cannot renew your membership yet.\n";
                cout << "Your membership is still active.\n";

                return;
            }

            // Membership has expired
            cout << "\n+==========================================+\n";
            cout << "|            RENEW MEMBERSHIP              |\n";
            cout << "+==========================================+\n";

            cout << "Membership Status : Expired\n";

            cout << "Expired Date      : " << c.membershipExpiry << endl;

            cout << "Renewal Fee       : RM " << fixed << setprecision(2) << MEMBERSHIP_FEE << endl;

            cout << "Renewal Period    : 1 Year\n";

            char confirm;

            confirm = getCharChoice("\nProceed to renewal payment? (Y/N): ", "YN");

            if (confirm != 'Y')
            {
                cout << "Membership renewal cancelled.\n";
                return;
            }

            // Membership payment
            if (makeMembershipPayment(customerID, MEMBERSHIP_FEE, "RENEW MEMBERSHIP"))
            {
                c.isMember = true;

                // New expiry = today + 1 year
                c.membershipExpiry = getMembershipExpiryDate();

                cout << "\nMembership renewed successfully!\n";

                cout << "Membership Status : Active\n";

                cout << "New Expiry Date   : " << c.membershipExpiry << endl;
            }

            return;
        }
    }

    cout << "Customer not found.\n";
}

// membership options
void membershipMenu(string customerID)
{
    int choice;

    do {
        cout << "\n+==========================================+\n";
        cout << "|              MEMBERSHIP                  |\n";
        cout << "+==========================================+\n";
        cout << "|1. View Membership Status                 |\n";
        cout << "|2. Become Member                          |\n";
        cout << "|3. Renew Membership                       |\n";
        cout << "|0. Back                                   |\n";
        cout << "+==========================================+\n";

        choice = getMenuChoice(0, 3);

        switch (choice)
        {
        case 1:
            viewMembership(customerID);
            break;

        case 2:
            becomeMember(customerID);
            break;

        case 3:
            renewMembership(customerID);
            break;

        case 0:
            cout << "Returning...\n";
            break;
        }

    } while (choice != 0);
}

// update basic profile details
void updateBasicProfile(string& name, string& password)
{
    string newName;
    string newPassword;

    newName = getNonEmptyInput("Enter new name: ", "Name cannot be empty.");

    newPassword = getNonEmptyInput("Enter new password: ", "Password cannot be empty.");

    name = newName;
    password = newPassword;
}

// show customer profile
void viewProfile(string customerID)
{
    for (Customer& c : customers)
    {
        if (c.id == customerID)
        {
            cout << "\n+==========================================+\n";
            cout << "|               MY PROFILE                 |\n";
            cout << "+==========================================+\n";
            cout << "Customer ID       : " << c.id << endl;
            cout << "Name              : " << c.name << endl;
            cout << "Phone             : " << c.phone << endl;

            string membershipStatus = getMembershipStatus(c);

            cout << "Member Status     : " << membershipStatus << endl;

            if (c.isMember)
            {
                cout << "Membership Expiry : " << c.membershipExpiry << endl;
            }
            else
            {
                cout << "Membership Expiry : N/A" << endl;
            }

            cout << "Total Spent       : RM " << fixed << setprecision(2) << c.totalSpent << endl;

            cout << "+==========================================+\n";

            return;
        }
    }

    cout << "Customer not found!\n";
}

// update profile based on user role
void updateProfile(const string& userID)
{
    if (userID.empty())
    {
        cout << "Invalid user ID.\n";
        return;
    }

    char role = toupper(static_cast<unsigned char>(userID[0]));

    int choice;

    do
    {
        cout << "\n+==========================================+\n";
        cout << "|            UPDATE PROFILE               |\n";
        cout << "+==========================================+\n";
        cout << "|1. Update Name                            |\n";

        // Phone only available for Customer
        if (role == 'C')
        {
            cout << "|2. Update Phone                           |\n";
        }
        else
        {
            cout << "|2. Update Password                        |\n";
        }

        if (role == 'C')
        {
            cout << "|3. Update Password                        |\n";
        }

        cout << "|0. Back                                   |\n";
        cout << "+==========================================+\n";

        if (role == 'C')
            choice = getMenuChoice(0, 3);
        else
            choice = getMenuChoice(0, 2);

        // ================= UPDATE NAME =================
        if (choice == 1)
        {
            string newName;

            newName = getNonEmptyInput("Enter new name: ", "Name cannot be empty.");

            if (role == 'C')
            {
                for (Customer& c : customers)
                {
                    if (c.id == userID)
                    {
                        c.name = newName;
                        cout << "Name updated successfully!\n";
                        break;
                    }
                }
            }
            else if (role == 'S')
            {
                for (Staff& s : staffs)
                {
                    if (s.id == userID)
                    {
                        s.name = newName;
                        cout << "Name updated successfully!\n";
                        break;
                    }
                }
            }
            else if (role == 'A')
            {
                for (Admin& a : admins)
                {
                    if (a.id == userID)
                    {
                        a.name = newName;
                        cout << "Name updated successfully!\n";
                        break;
                    }
                }
            }
        }

        // ================= CUSTOMER PHONE =================
        else if (role == 'C' && choice == 2)
        {
            string newPhone;

            do
            {
                cout << "Enter new phone number: ";
                getline(cin, newPhone);

                if (!isValidPhoneNumber(newPhone))
                {
                    cout << "Invalid phone number! Please enter 10-11 digits starting with 0.\n";
                }

            } while (!isValidPhoneNumber(newPhone));

            for (Customer& c : customers)
            {
                if (c.id == userID)
                {
                    c.phone = newPhone;
                    cout << "Phone number updated successfully!\n";
                    break;
                }
            }
        }

        // ================= PASSWORD =================
        else if ((role == 'C' && choice == 3) || (role != 'C' && choice == 2))
        {
            string newPassword;

            newPassword = getNonEmptyInput("Enter new password: ", "Password cannot be empty.");


            if (role == 'C')
            {
                for (Customer& c : customers)
                {
                    if (c.id == userID)
                    {
                        c.password = newPassword;
                        cout << "Password updated successfully!\n";
                        break;
                    }
                }
            }
            else if (role == 'S')
            {
                for (Staff& s : staffs)
                {
                    if (s.id == userID)
                    {
                        s.password = newPassword;
                        cout << "Password updated successfully!\n";
                        break;
                    }
                }
            }
            else if (role == 'A')
            {
                for (Admin& a : admins)
                {
                    if (a.id == userID)
                    {
                        a.password = newPassword;
                        cout << "Password updated successfully!\n";
                        break;
                    }
                }
            }
        }

        // ================= BACK =================
        else if (choice == 0)
        {
            cout << "Returning...\n";
        }

    } while (choice != 0);
}

// profile options
void profileMenu(string customerID)
{
    int choice;

    do
    {
        cout << "\n+==========================================+\n";
        cout << "|               MY PROFILE                 |\n";
        cout << "+==========================================+\n";
        cout << "|1. View Profile                           |\n";
        cout << "|2. Update Profile                         |\n";
        cout << "|0. Back                                   |\n";
        cout << "+==========================================+\n";

        choice = getMenuChoice(0, 2);

        switch (choice)
        {
        case 1:
            viewProfile(customerID);
            break;

        case 2:
            updateProfile(customerID);
            break;

        case 0:
            cout << "Returning...\n";
            break;
        }

    } while (choice != 0);
}

// main menu for customer
void customerMenu(string customerID)
{
    int choice;

    do
    {
        cout << "\n+==================================================+\n";
        cout << "|                 CUSTOMER MENU                    |\n";
        cout << "+==================================================+\n";
        cout << "|1. Submit Booking Request                         |\n";
        cout << "|2. View Current Order Status                      |\n";
        cout << "|3. View Booking History                           |\n";
        cout << "|4. View Order Details                             |\n";
        cout << "|5. Submit Rating & Feedback                       |\n";
        cout << "|6. View Loyalty Points                            |\n";
        cout << "|7. Membership                                     |\n";
        cout << "|8. Update Profile                                 |\n";
        cout << "|0. Exit                                           |\n";
        cout << "+==================================================+\n";

        choice = getMenuChoice(0, 8);

        switch (choice)
        {
        case 1:
            submitBooking(customerID);
            break;

        case 2:
            viewCurrentOrders(customerID);
            break;

        case 3:
            bookingHistory(customerID);
            break;

        case 4:
            viewOrderDetails(customerID);
            break;

        case 5:
            submitFeedback(customerID);
            break;

        case 6:
            showLoyaltyPoints(customerID);
            break;

        case 7:
            membershipMenu(customerID);
            break;

        case 8:
            profileMenu(customerID);
            break;
        case 0:
            cout << "\nBack to the login page.\n";
            break;
        }

    } while (choice != 0);
}


// Staff account functions
bool isValidStaffID(string id)
{
    if (id.length() != 5)
        return false;

    id[0] = toupper(static_cast<unsigned char>(id[0]));

    if (id[0] != 'S')
        return false;

    for (int i = 1; i < static_cast<int>(id.length()); i++)
    {
        if (!isdigit(static_cast<unsigned char>(id[i])))
        {
            return false;
        }
    }

    return true;
}

// show all staff records
void viewAllStaff()
{
    cout << "\n+=================================================================================+\n";
    cout << "|                                  ALL STAFF                                      |\n";
    cout << "+=================================================================================+\n";

    if (staffs.empty())
    {
        cout << "No staff records found.\n";
        return;
    }

    cout << left << setw(10) << "ID" << setw(20) << "Name" << setw(15) << "Phone" << setw(10) << "Gender" << setw(15) << "Date Joined" << setw(20) << "Status" << endl;

    cout << "-----------------------------------------------------------------------------------\n";

    for (const Staff& s : staffs)
    {
        cout << left << setw(10) << s.id << setw(20) << s.name << setw(15) << s.phone << setw(10) << (s.gender == 'M' ? "Male" : "Female") << setw(15) << s.dateJoined << setw(20) << (s.status ? "Still Working" : "No Longer Working") << endl;
    }
}

// add new staff
void addStaff()
{
    Staff s;

    cout << "\n========== ADD STAFF ==========\n";

    // Automatically generate Staff ID
    int maxNumber = 1000;

    for (const Staff& existing : staffs)
    {
        if (existing.id.length() > 1)
        {
            int number = stoi(existing.id.substr(1));

            if (number > maxNumber)
            {
                maxNumber = number;
            }
        }
    }

    s.id = "S" + to_string(maxNumber + 1);

    // Staff Name
    // Staff Name
    s.name = getNonEmptyInput("Enter Staff Name: ", "Name cannot be empty!");

    // Phone Number
    do
    {
        cout << "Enter Phone Number: ";
        getline(cin, s.phone);

        if (!isValidPhoneNumber(s.phone))
        {
            cout << "Invalid phone number! Please enter 10-11 digits starting with 0.\n";
        }

    } while (!isValidPhoneNumber(s.phone));

    // Gender
    s.gender = getCharChoice("Enter Gender (M/F): ", "MF");

    // Automatically set Date Joined
    s.dateJoined = getCurrentDate();

    // Automatically set Status
    s.status = true;

    // Automatically set Default Password
    s.password = DEFAULT_STAFF_PASSWORD;

    // Store staff
    staffs.push_back(s);

    cout << "\nStaff added successfully!\n";
    cout << "Staff ID        : " << s.id << endl;
    cout << "Default Password: " << DEFAULT_STAFF_PASSWORD << endl;
}

// delete staff
void deleteStaff()
{
    if (staffs.empty())
    {
        cout << "No staff records found.\n";
        return;
    }

    viewAllStaff();

    string id;

    cout << "\nEnter Staff ID to delete: ";
    getline(cin, id);

    if (!id.empty())
    {
        id[0] = toupper(static_cast<unsigned char>(id[0]));
    }

    for (size_t i = 0; i < staffs.size(); i++)
    {
        if (staffs[i].id == id)
        {
            staffs.erase(staffs.begin() + i);

            cout << "Staff deleted successfully!\n";
            return;
        }
    }

    cout << "Staff not found!\n";
}

// update staff
void updateStaff()
{
    string id;

    cout << "Enter Staff ID to update: ";
    getline(cin, id);

    // Convert first letter to uppercase
    if (!id.empty())
    {
        id[0] = toupper(static_cast<unsigned char>(id[0]));
    }

    for (Staff& s : staffs)
    {
        if (s.id == id)
        {
            int choice;

            do
            {
                cout << "\n+==========================================+\n";
                cout << "|              UPDATE STAFF                |\n";
                cout << "+==========================================+\n";
                cout << "| 1. Update Name                           |\n";
                cout << "| 2. Update Phone Number                   |\n";
                cout << "| 3. Update Gender                         |\n";
                cout << "| 4. Update Status                         |\n";
                cout << "| 0. Back                                  |\n";
                cout << "+==========================================+\n";

                choice = getMenuChoice(0, 4);

                switch (choice)
                {
                case 1:
                    s.name = getNonEmptyInput(
                        "Enter New Staff Name: ", "Name cannot be empty."
                    );

                    cout << "Staff name updated successfully.\n";
                    break;

                case 2:
                    do
                    {
                        cout << "Enter New Phone Number: ";
                        getline(cin, s.phone);

                        if (!isValidPhoneNumber(s.phone))
                        {
                            cout << "Invalid phone number! Please enter 10-11 digits starting with 0.\n";
                        }

                    } while (!isValidPhoneNumber(s.phone));

                    cout << "Phone number updated successfully.\n";
                    break;

                case 3:
                    s.gender = getCharChoice("Enter New Gender (M/F): ", "MF");

                    cout << "Gender updated successfully.\n";
                    break;

                case 4:
                {
                    int statusChoice;

                    cout << "\nStaff Status:\n";
                    cout << "1. Still Working\n";
                    cout << "2. No Longer Working\n";

                    statusChoice = getMenuChoice(1, 2);

                    s.status = (statusChoice == 1);

                    cout << "Staff status updated successfully.\n";
                    break;
                }

                case 0:
                    cout << "Returning to Staff Management...\n";
                    break;
                }

            } while (choice != 0);

            return;
        }
    }

    cout << "Staff ID not found.\n";
}


// Staff order and appointment functions
void confirmCustomerBooking()
{
    bool found = false;

    cout << "\n+==================================================+\n";
    cout << "|             CONFIRM CUSTOMER BOOKING             |\n";
    cout << "+==================================================+\n";

    for (Booking& b : bookings)
    {
        if (b.status == "Pending")
        {
            cout << "\nBooking ID : " << b.bookingID << endl;

            cout << "Customer ID: " << b.customerID << endl;

            cout << "Date       : " << b.date << endl;

            cout << "Time       : " << b.timeSlot << endl;

            cout << "Services   : ";

            for (size_t i = 0; i < b.services.size(); i++)
            {
                cout << b.services[i];

                if (i < b.services.size() - 1)
                {
                    cout << ", ";
                }
            }

            cout << endl;

            found = true;
        }
    }

    if (!found)
    {
        cout << "No pending bookings found.\n";
        return;
    }

    string bookingID;

    cout << "\nEnter Booking ID to confirm: ";
    getline(cin, bookingID);

    if (!bookingID.empty())
    {
        bookingID[0] = toupper(static_cast<unsigned char>(bookingID[0]));
    }

    for (Booking& b : bookings)
    {
        if (b.bookingID == bookingID && b.status == "Pending")
        {
            b.status = "Confirmed";

            addNotification(b.customerID, "Your booking " + b.bookingID + " has been confirmed.");

            cout << "Booking confirmed successfully!\n";
            return;
        }
    }

    cout << "Booking not found or has already been confirmed.\n";
}


// Staff enters the real weight when the customer arrives
void processConfirmedBooking()
{
    bool found = false;

    cout << "\n+==================================================+\n";
    cout << "|                 PROCESS BOOKING                  |\n";
    cout << "+==================================================+\n";

    for (Booking& b : bookings)
    {
        if (b.status == "Confirmed")
        {
            cout << "\nBooking ID : " << b.bookingID << endl;
            cout << "Customer ID: " << b.customerID << endl;
            cout << "Date       : " << b.date << endl;
            cout << "Time       : " << b.timeSlot << endl;

            found = true;
        }
    }

    if (!found)
    {
        cout << "No confirmed bookings found.\n";
        return;
    }

    string bookingID;

    cout << "\nEnter Booking ID to process: ";
    getline(cin, bookingID);

    if (!bookingID.empty())
    {
        bookingID[0] = toupper(static_cast<unsigned char>(bookingID[0]));
    }

    for (Booking& b : bookings)
    {
        if (b.bookingID == bookingID && b.status == "Confirmed")
        {
            int customerIndex = -1;

            for (size_t i = 0; i < customers.size(); i++)
            {
                if (customers[i].id == b.customerID)
                {
                    customerIndex = static_cast<int>(i);
                    break;
                }
            }

            if (customerIndex == -1)
            {
                cout << "Customer not found.\n";
                return;
            }

            cout << "\nSelected Services:\n";

            for (size_t i = 0; i < b.services.size(); i++)
            {
                cout << i + 1 << ". " << b.services[i] << endl;
            }

            if (b.services.size() < 3 && askAddAnotherService())
            {
                string serviceNames[3] = { "Wash", "Dry Clean", "Iron" };

                displayServices();

                while (b.services.size() < 3)
                {
                    int serviceChoice;

                    cout << "Choose additional service:\n";
                    serviceChoice = getMenuChoice(1, 3);

                    bool alreadySelected = false;

                    for (string& service : b.services)
                    {
                        if (service == serviceNames[serviceChoice - 1])
                        {
                            alreadySelected = true;
                            break;
                        }
                    }

                    if (alreadySelected)
                    {
                        cout << "This service has already been selected.\n";
                    }
                    else
                    {
                        b.services.push_back(serviceNames[serviceChoice - 1]);
                        cout << serviceNames[serviceChoice - 1] << " added successfully.\n";
                    }

                    if (b.services.size() == 3)
                    {
                        cout << "All available services have been selected.\n";
                        break;
                    }

                    if (!askAddAnotherService())
                        break;
                }
            }

            cout << "\nServices to Process:\n";

            for (size_t i = 0; i < b.services.size(); i++)
            {
                cout << i + 1 << ". " << b.services[i] << endl;
            }

            vector<int> quantities;
            vector<double> serviceCosts;
            double subtotal = 0.0;
            bool validMember = customers[customerIndex].isMember && !isMembershipExpired(customers[customerIndex].membershipExpiry);

            for (size_t i = 0; i < b.services.size(); i++)
            {
                int quantity;

                quantity = getPositiveInt("Enter weight for " + b.services[i] + " (kg): ");

                int serviceIndex = 0;

                if (b.services[i] == "Dry Clean")
                    serviceIndex = 1;
                else if (b.services[i] == "Iron")
                    serviceIndex = 2;

                double cost = calculateServiceCost(serviceIndex, quantity, validMember);

                quantities.push_back(quantity);
                serviceCosts.push_back(cost);
                subtotal += cost;
            }

            cout << "\nTotal Before Discount: RM " << fixed << setprecision(2) << subtotal << endl;

            string orderID = "O" + to_string(orders.size() + 1001);
            Payment payment;
            double finalAmount;

            if (!makePayment(subtotal, b.customerID, orderID, "Laundry Booking", payment, finalAmount))
            {
                cout << "Payment failed.\n";
                return;
            }

            double discount = subtotal - finalAmount;

            Order o;
            o.orderID = orderID;
            o.bookingID = b.bookingID;
            o.customerID = b.customerID;
            o.price = finalAmount;
            o.status = "Pending";
            o.date = getCurrentDate();
            o.pointsAwarded = false;

            double allocatedTotal = 0.0;

            for (size_t i = 0; i < b.services.size(); i++)
            {
                OrderItem item;
                item.serviceType = b.services[i];
                item.quantity = quantities[i];

                if (i == b.services.size() - 1)
                {
                    item.price = finalAmount - allocatedTotal;
                }
                else
                {
                    double itemDiscount = 0.0;

                    if (subtotal > 0)
                        itemDiscount = discount * (serviceCosts[i] / subtotal);

                    item.price = serviceCosts[i] - itemDiscount;
                    allocatedTotal += item.price;
                }

                o.items.push_back(item);
            }

            orders.push_back(o);
            customers[customerIndex].totalSpent += finalAmount;
            b.status = "In Progress";

            addNotification(b.customerID, "Payment received for booking " + b.bookingID + ". Order " + o.orderID + " has been created and laundry processing has started.");

            cout << "\nBooking processed successfully!\n";
            cout << "Order ID    : " << o.orderID << endl;
            cout << "Final Amount: RM " << fixed << setprecision(2) << finalAmount << endl;

            return;
        }
    }

    cout << "Booking not found or is not ready to process.\n";
}

// create walk in order
void createWalkInOrder()
{
    string customerID;

    cout << "\n+==================================================+\n";
    cout << "|               CREATE WALK-IN ORDER               |\n";
    cout << "+==================================================+\n";

    cout << "Enter Customer ID: ";
    getline(cin, customerID);

    if (!customerID.empty())
    {
        customerID[0] = toupper(static_cast<unsigned char>(customerID[0]));
    }

    int customerIndex = -1;

    for (size_t i = 0; i < customers.size(); i++)
    {
        if (customers[i].id == customerID)
        {
            customerIndex = static_cast<int>(i);
            break;
        }
    }

    if (customerIndex == -1)
    {
        cout << "Customer not found! Please register the customer first.\n";
        return;
    }

    cout << "Customer Name : " << customers[customerIndex].name << endl;

    bool validMember = customers[customerIndex].isMember && !isMembershipExpired(customers[customerIndex].membershipExpiry);

    cout << "Customer Type : " << (validMember ? "Member" : "Non-member") << endl;

    displayServices();

    string serviceNames[3] = {
        "Wash",
        "Dry Clean",
        "Iron"
    };

    vector<int> serviceIndexes;
    vector<int> quantities;
    vector<double> serviceCosts;
    double subtotal = 0.0;

    while (true)
    {
        int serviceChoice;

        if (serviceIndexes.empty())
        {
            serviceChoice = getIntInput(
                "Choose Service (1-3) (enter 0 to go back): ",
                0,
                3
            );

            if (serviceChoice == 0)
            {
                cout << "Returning to menu...\n";
                return;
            }
        }
        else
        {
            serviceChoice = getIntInput(
                "Choose Service (1-3) (enter 4 to finish): ",
                1,
                4
            );

            if (serviceChoice == 4)
            {
                break;
            }
        }
        int serviceIndex = serviceChoice - 1;
        bool alreadySelected = false;

        for (int index : serviceIndexes)
        {
            if (index == serviceIndex)
            {
                alreadySelected = true;
                break;
            }
        }

        if (alreadySelected)
        {
            cout << "This service has already been selected.\n";
            continue;
        }

        int quantity;

        quantity = getPositiveInt(
            "Enter weight for " + serviceNames[serviceIndex] + " (kg): ");

        double cost = calculateServiceCost(serviceIndex, quantity, validMember);

        serviceIndexes.push_back(serviceIndex);
        quantities.push_back(quantity);
        serviceCosts.push_back(cost);
        subtotal += cost;

        cout << serviceNames[serviceIndex] << " added successfully.\n";

        if (serviceIndexes.size() == 3)
        {
            cout << "All available services have been selected.\n";
            break;
        }

        if (!askAddAnotherService())
            break;
    }

    cout << "\n+==================================================+\n";
    cout << "|                SELECTED SERVICES                 |\n";
    cout << "+==================================================+\n";

    for (size_t i = 0; i < serviceIndexes.size(); i++)
    {
        cout << i + 1 << ". " << serviceNames[serviceIndexes[i]] << " - " << quantities[i] << " kg - RM " << fixed << setprecision(2) << serviceCosts[i] << endl;
    }

    cout << "+==================================================+\n";
    cout << "Total Before Discount: RM " << fixed << setprecision(2) << subtotal << endl;

    string orderID = "O" + to_string(orders.size() + 1001);
    Payment payment;
    double finalAmount;

    if (!makePayment(subtotal, customerID, orderID, "Walk-in Laundry Order", payment, finalAmount))
    {
        cout << "Payment failed. Order cancelled.\n";
        return;
    }

    double discount = subtotal - finalAmount;

    Order o;
    o.orderID = orderID;
    o.bookingID = "WALK-IN";
    o.customerID = customerID;
    o.price = finalAmount;
    o.status = "Pending";
    o.date = getCurrentDate();
    o.pointsAwarded = false;

    double allocatedTotal = 0.0;

    for (size_t i = 0; i < serviceIndexes.size(); i++)
    {
        OrderItem item;
        item.serviceType = serviceNames[serviceIndexes[i]];
        item.quantity = quantities[i];

        if (i == serviceIndexes.size() - 1)
        {
            item.price = finalAmount - allocatedTotal;
        }
        else
        {
            double itemDiscount = 0.0;

            if (subtotal > 0)
                itemDiscount = discount * (serviceCosts[i] / subtotal);

            item.price = serviceCosts[i] - itemDiscount;
            allocatedTotal += item.price;
        }

        o.items.push_back(item);
    }

    orders.push_back(o);
    customers[customerIndex].totalSpent += finalAmount;

    addNotification(customerID, "Walk-in order " + o.orderID + " has been created. Status: Pending.");

    cout << "\n+==================================================+\n";
    cout << "|              WALK-IN ORDER CREATED               |\n";
    cout << "+==================================================+\n";

    cout << "Order ID   : " << o.orderID << endl;
    cout << "Customer   : " << customers[customerIndex].name << " (" << customers[customerIndex].id << ")" << endl;
    cout << "Services   :\n";

    for (size_t i = 0; i < o.items.size(); i++)
    {
        cout << "  " << i + 1 << ". " << o.items[i].serviceType << " - " << o.items[i].quantity << " kg - RM " << fixed << setprecision(2) << o.items[i].price << endl;
    }

    cout << "Total Price: RM " << fixed << setprecision(2) << o.price << endl;
    cout << "Status     : " << o.status << endl;
}

// view unfinished orders
void viewUnfinishedOrders()
{
    bool found = false;

    cout << "\n+======================================================================================+\n";
    cout << "|                                  UNFINISHED ORDERS                                   |\n";
    cout << "+======================================================================================+\n";

    cout << left << setw(10) << "Order ID" << setw(12) << "Customer" << setw(34) << "Services" << setw(20) << "Status" << "Price" << endl;
    cout << "--------------------------------------------------------------------------------------\n";

    for (Order& o : orders)
    {
        if (o.status != "Collected" && o.status != "Cancelled")
        {
            string serviceList;

            for (size_t i = 0; i < o.items.size(); i++)
            {
                serviceList += o.items[i].serviceType;

                if (i < o.items.size() - 1)
                    serviceList += ", ";
            }

            cout << left << setw(10) << o.orderID << setw(12) << o.customerID << setw(34) << serviceList << setw(20) << o.status << "RM " << fixed << setprecision(2) << o.price << endl;
            found = true;
        }
    }

    if (!found)
        cout << "No unfinished orders found.\n";
}


// Check whether every order under one booking is collected
void updateBookingCompletion(string bookingID)
{
    if (bookingID == "WALK-IN")
        return;

    bool foundOrder = false;
    bool allCollected = true;

    for (Order& o : orders)
    {
        if (o.bookingID == bookingID)
        {
            foundOrder = true;

            if (o.status != "Collected")
            {
                allCollected = false;
                break;
            }
        }
    }

    if (!foundOrder || !allCollected)
    {
        return;
    }

    for (Booking& b : bookings)
    {
        if (b.bookingID == bookingID)
        {
            if (b.status != "Completed")
            {
                b.status = "Completed";

                addNotification(b.customerID, "Booking " + b.bookingID + " has been completed.");
            }

            return;
        }
    }
}

// update laundry status
void updateLaundryStatus()
{
    if (orders.empty())
    {
        cout << "No orders found.\n";
        return;
    }

    viewUnfinishedOrders();

    string orderID;

    cout << "\nEnter Order ID to update: ";
    getline(cin, orderID);

    if (!orderID.empty())
    {
        orderID[0] = toupper(static_cast<unsigned char>(orderID[0]));
    }

    for (Order& o : orders)
    {
        if (o.orderID == orderID)
        {
            if (o.status == "Cancelled")
            {
                cout << "Cancelled order cannot be updated.\n";
                return;
            }

            cout << "\nCurrent Status: " << o.status << endl;

            cout << "1. Pending\n";
            cout << "2. Washing\n";
            cout << "3. Drying\n";
            cout << "4. Ironing\n";
            cout << "5. Ready for Pickup\n";
            cout << "6. Collected\n";

            int choice = getMenuChoice(1, 6);

            string newStatus;

            switch (choice)
            {
            case 1:
                newStatus = "Pending";
                break;

            case 2:
                newStatus = "Washing";
                break;

            case 3:
                newStatus = "Drying";
                break;

            case 4:
                newStatus = "Ironing";
                break;

            case 5:
                newStatus = "Ready for Pickup";
                break;

            case 6:
                newStatus = "Collected";
                break;
            }

            o.status = newStatus;

            if (newStatus == "Washing")
            {
                addNotification(o.customerID, "Order " + o.orderID + " is now being washed.");
            }
            else if (newStatus == "Drying")
            {
                addNotification(o.customerID, "Order " + o.orderID + " is now drying.");
            }
            else if (newStatus == "Ironing")
            {
                addNotification(o.customerID, "Order " + o.orderID + " is now being ironed.");
            }
            else if (newStatus == "Ready for Pickup")
            {
                addNotification(o.customerID, "Order " + o.orderID + " is ready for pickup.");
            }
            else if (newStatus == "Collected")
            {
                addNotification(o.customerID, "Order " + o.orderID + " has been collected.");

                if (!o.pointsAwarded)
                {
                    updateLoyaltyPoints(o.customerID, o.price);

                    o.pointsAwarded = true;
                }

                updateBookingCompletion(o.bookingID);
            }
            else
            {
                addNotification(o.customerID, "Order " + o.orderID + " status is now Pending.");
            }

            cout << "Order status updated successfully!\n";
            return;
        }
    }

    cout << "Order not found.\n";
}

// view todays orders
void viewTodaysOrders()
{
    string today = getCurrentDate();
    bool found = false;

    cout << "\n+===============================================================+\n";
    cout << "|                    TODAY'S SCHEDULED ORDERS                   |\n";
    cout << "+===============================================================+\n";

    for (Booking& b : bookings)
    {
        if (b.date == today)
        {
            cout << "\nBooking ID : " << b.bookingID << endl;
            cout << "Customer ID: " << b.customerID << endl;
            cout << "Time       : " << b.timeSlot << endl;
            cout << "Status     : " << b.status << endl;

            found = true;
        }
    }

    for (Order& o : orders)
    {
        if (o.date == today)
        {
            cout << "\nOrder ID   : " << o.orderID << endl;
            cout << "Customer ID: " << o.customerID << endl;
            cout << "Services   :\n";

            for (size_t i = 0; i < o.items.size(); i++)
            {
                cout << "  " << i + 1 << ". " << o.items[i].serviceType << " - " << o.items[i].quantity << " kg" << endl;
            }

            cout << "Status     : " << o.status << endl;
            found = true;
        }
    }

    if (!found)
        cout << "No orders scheduled for today.\n";
}

// search order by id
void searchOrderByID()
{
    string orderID;

    cout << "\nEnter Order ID: ";
    getline(cin, orderID);
    if (!orderID.empty())
    {
        orderID[0] = toupper(static_cast<unsigned char>(orderID[0]));
    }


    for (const Order& o : orders)
    {
        if (o.orderID == orderID)
        {
            cout << "\nOrder ID   : " << o.orderID << endl;
            cout << "Booking ID : " << o.bookingID << endl;
            cout << "Customer ID: " << o.customerID << endl;
            cout << "Services   :\n";

            for (size_t i = 0; i < o.items.size(); i++)
            {
                cout << "  " << i + 1 << ". " << o.items[i].serviceType << " - " << o.items[i].quantity << " kg - RM " << fixed << setprecision(2) << o.items[i].price << endl;
            }

            cout << "Total Price: RM " << fixed << setprecision(2) << o.price << endl;
            cout << "Date       : " << o.date << endl;
            cout << "Status     : " << o.status << endl;

            return;
        }
    }

    cout << "Order not found.\n";

}

// search orders by customer id
void searchOrdersByCustomerID()
{
    string customerID;

    cout << "\nEnter Customer ID: ";
    getline(cin, customerID);

    if (!customerID.empty())
    {
        customerID[0] = toupper(static_cast<unsigned char>(customerID[0]));
    }

    bool found = false;

    for (const Order& o : orders)
    {
        if (o.customerID == customerID)
        {
            cout << "\nOrder ID   : " << o.orderID << endl;
            cout << "Booking ID : " << o.bookingID << endl;
            cout << "Customer ID: " << o.customerID << endl;
            cout << "Services   :\n";

            for (size_t i = 0; i < o.items.size(); i++)
            {
                cout << "  " << i + 1 << ". " << o.items[i].serviceType << " - " << o.items[i].quantity << " kg - RM " << fixed << setprecision(2) << o.items[i].price << endl;
            }

            cout << "Total Price: RM " << fixed << setprecision(2) << o.price << endl;
            cout << "Date       : " << o.date << endl;
            cout << "Status     : " << o.status << endl;

            found = true;
        }
    }

    if (!found)
    {
        cout << "No orders found for this customer.\n";
    }
}
// staff menu
void staffMenu(const string& staffID)
{
    int choice;

    do
    {
        cout << "\n+==================================================+\n";
        cout << "|                    STAFF MENU                    |\n";
        cout << "+==================================================+\n";
        cout << "|1. Confirm Customer Booking                       |\n";
        cout << "|2. Order Management                               |\n";
        cout << "|3. Order Search                                   |\n";
        cout << "|4. Update Profile                                 |\n";
        cout << "|0. Exit                                           |\n";
        cout << "+==================================================+\n";

        choice = getMenuChoice(0, 4);

        switch (choice)
        {
        case 1:
            confirmCustomerBooking();
            break;

        case 2:
        {
            int orderChoice;

            do
            {
                cout << "\n+==================================================+\n";
                cout << "|                 ORDER MANAGEMENT                 |\n";
                cout << "+==================================================+\n";
                cout << "|1. Process Confirmed Booking                      |\n";
                cout << "|2. Create Walk-in Order                           |\n";
                cout << "|3. Update Laundry Status                          |\n";
                cout << "|4. View Today's Orders                            |\n";
                cout << "|5. View Unfinished Orders                         |\n";
                cout << "|0. Back                                           |\n";
                cout << "+==================================================+\n";

                orderChoice = getMenuChoice(0, 5);

                switch (orderChoice)
                {
                case 1:
                    processConfirmedBooking();
                    break;

                case 2:
                    createWalkInOrder();
                    break;

                case 3:
                    updateLaundryStatus();
                    break;

                case 4:
                    viewTodaysOrders();
                    break;

                case 5:
                    viewUnfinishedOrders();
                    break;

                }

            } while (orderChoice != 0);

            break;
        }

        case 3:
        {
            int searchChoice;

            do
            {
                cout << "\n+==================================================+\n";
                cout << "|                   ORDER SEARCH                   |\n";
                cout << "+==================================================+\n";
                cout << "|1. Search by Order ID                             |\n";
                cout << "|2. Search by Customer ID                          |\n";
                cout << "|0. Back                                           |\n";
                cout << "+==================================================+\n";

                searchChoice = getMenuChoice(0, 2);

                switch (searchChoice)
                {
                case 1:
                    searchOrderByID();
                    break;

                case 2:
                    searchOrdersByCustomerID();
                    break;
                }

            } while (searchChoice != 0);

            break;
        }

        case 4:
            updateProfile(staffID);
            break;

        case 0:
            cout << "Logging out...\n";
            break;
        }

    } while (choice != 0);
}



// Admin functions
void viewCustomers()
{
    cout << "\n+===============================================================================+\n";
    cout << "|                               CUSTOMER RECORDS                                |\n";
    cout << "+===============================================================================+\n";

    if (customers.empty())
    {
        cout << "No customer records found.\n";
        return;
    }

    cout << left << setw(10) << "ID" << setw(20) << "Name" << setw(15) << "Phone" << setw(15) << "Membership" << setw(15) << "Spent" << setw(10) << "Points" << endl;

    cout << "---------------------------------------------------------------------------------\n";

    for (Customer& c : customers)
    {
        cout << left << setw(10) << c.id << setw(20) << c.name << setw(15) << c.phone << setw(15) << getMembershipStatus(c) << "RM " << setw(12) << fixed << setprecision(2) << c.totalSpent << setw(10) << c.loyaltyPoints << endl;
    }
}

// search customer
void searchCustomer()
{
    string id;

    cout << "\nEnter Customer ID: ";
    getline(cin, id);

    for (char& c : id) {
        c = toupper(c);
    }
    for (Customer& c : customers)
    {
        if (c.id == id)
        {
            cout << "\n+==========================================+\n";
            cout << "|             CUSTOMER DETAILS             |\n";
            cout << "+==========================================+\n";
            cout << "Customer ID : " << c.id << endl;
            cout << "Name        : " << c.name << endl;
            cout << "Phone       : " << c.phone << endl;
            cout << "Membership  : " << getMembershipStatus(c) << endl;
            cout << "Total Spent : RM " << fixed << setprecision(2) << c.totalSpent << endl;
            cout << "Loyalty     : " << c.loyaltyPoints << " points\n";

            return;
        }
    }

    cout << "Customer not found!\n";
}

// view customer feedback
void viewCustomerFeedback()
{
    cout << "\n+==================================================+\n";
    cout << "|              CUSTOMER FEEDBACK                   |\n";
    cout << "+==================================================+\n";

    if (feedbacks.empty())
    {
        cout << "No feedback records found.\n";
        return;
    }

    for (Feedback& f : feedbacks)
    {
        cout << "\nOrder ID    : " << f.orderID << endl;

        cout << "Customer ID : " << f.customerID << endl;

        cout << "Rating      : " << f.rating << "/5" << endl;

        cout << "Comment     : " << f.comment << endl;

        cout << "-----------------------------------------------\n";
    }
}

// delete customer
void deleteCustomer()
{
    if (customers.empty())
    {
        cout << "No customer records found.\n";
        return;
    }

    viewCustomers();

    string id;

    cout << "\nEnter Customer ID to delete: ";
    getline(cin, id);

    if (!id.empty())
    {
        id[0] = toupper(static_cast<unsigned char>(id[0]));
    }

    for (size_t i = 0; i < customers.size(); i++)
    {
        if (customers[i].id == id)
        {
            customers.erase(customers.begin() + i);

            cout << "Customer deleted successfully!\n";
            return;
        }
    }

    cout << "Customer not found!\n";
}

// customer management
void customerManagement()
{
    int choice;

    do
    {
        cout << "\n+================================+\n";
        cout << "|     CUSTOMER MANAGEMENT        |\n";
        cout << "+================================+\n";

        cout << "|1. View Customers               |\n";
        cout << "|2. Search Customer              |\n";
        cout << "|3. View Customer Feedback       |\n";
        cout << "|4. Delete Customer              |\n";
        cout << "|0. Back                         |\n";

        cout << "+================================+\n";

        choice = getMenuChoice(0, 4);

        switch (choice)
        {
        case 1:
            viewCustomers();
            break;

        case 2:
            searchCustomer();
            break;

        case 3:
            viewCustomerFeedback();
            break;

        case 4:
            deleteCustomer();
            break;

        case 0:
            cout << "Returning...\n";
            break;
        }

    } while (choice != 0);
}

// view report
void viewReport()
{
    int choice;
    do
    {
        cout << "\n+==========================================+\n";
        cout << "|              INCOME REPORT               |\n";
        cout << "+==========================================+\n";
        cout << "|1. Daily Income                           |\n";
        cout << "|2. Monthly Income                         |\n";
        cout << "|3. Annual Income                          |\n";
        cout << "|0. Back                                   |\n";
        cout << "+==========================================+\n";

        choice = getMenuChoice(0, 3);

        switch (choice)
        {
        case 1:
        {
            string date;

            do
            {
                cout << "\nEnter Date (DD/MM/YYYY): ";
                getline(cin, date);

                if (!isValidReportDate(date))
                    cout << "Invalid date! Please use DD/MM/YYYY.\n";

            } while (!isValidReportDate(date));

            double totalIncome = 0.0;
            int totalOrders = 0;
            int completedOrders = 0;

            for (Order& o : orders)
            {
                if (o.date == date)
                {
                    totalIncome += o.price;
                    totalOrders++;

                    if (o.status == "Collected")
                    {
                        completedOrders++;
                    }
                }
            }

            cout << "\n+==========================================+\n";
            cout << "|             DAILY INCOME                |\n";
            cout << "+==========================================+\n";

            cout << "Date             : " << date << endl;

            cout << "Total Orders     : " << totalOrders << endl;

            cout << "Completed Orders : " << completedOrders << endl;

            cout << "Total Income     : RM " << fixed << setprecision(2) << totalIncome << endl;

            cout << "+==========================================+\n";

            break;
        }

        case 2:
        {
            string month;

            do
            {
                cout << "\nEnter Month (MM/YYYY): ";
                getline(cin, month);

                if (!isValidReportMonth(month))
                {
                    cout << "Invalid month! " << "Please enter in MM/YYYY format.\n";
                }

            } while (!isValidReportMonth(month));

            double totalIncome = 0.0;
            int totalOrders = 0;
            int completedOrders = 0;

            for (Order& o : orders)
            {
                if (o.date.length() == 10)
                {
                    string orderMonth = o.date.substr(3, 7);

                    if (orderMonth == month)
                    {
                        totalIncome += o.price;
                        totalOrders++;

                        if (o.status == "Collected")
                        {
                            completedOrders++;
                        }
                    }
                }
            }

            cout << "\n+==========================================+\n";
            cout << "|             MONTHLY INCOME               |\n";
            cout << "+==========================================+\n";

            cout << "Month            : " << month << endl;

            cout << "Total Orders     : " << totalOrders << endl;

            cout << "Completed Orders : " << completedOrders << endl;

            cout << "Total Income     : RM " << fixed << setprecision(2) << totalIncome << endl;

            cout << "+==========================================+\n";

            break;
        }

        case 3:
        {
            string year;

            do
            {
                cout << "\nEnter Year (YYYY): ";
                getline(cin, year);

                if (!isValidReportYear(year))
                {
                    cout << "Invalid year! " << "Please enter in YYYY format.\n";
                }

            } while (!isValidReportYear(year));

            double totalIncome = 0.0;
            int totalOrders = 0;
            int completedOrders = 0;

            for (Order& o : orders)
            {
                if (o.date.length() == 10)
                {
                    string orderYear = o.date.substr(6, 4);

                    if (orderYear == year)
                    {
                        totalIncome += o.price;
                        totalOrders++;

                        if (o.status == "Collected")
                        {
                            completedOrders++;
                        }
                    }
                }
            }

            cout << "\n+==========================================+\n";
            cout << "|              ANNUAL INCOME               |\n";
            cout << "+==========================================+\n";

            cout << "Year             : " << year << endl;

            cout << "Total Orders     : " << totalOrders << endl;

            cout << "Completed Orders : " << completedOrders << endl;

            cout << "Total Income     : RM " << fixed << setprecision(2) << totalIncome << endl;

            cout << "+==========================================+\n";

            break;
        }

        case 0:
            cout << "Returning...\n";
            break;
        }

    } while (choice != 0);
}

// view order report
void viewOrderReport()
{
    int choice;

    do
    {
        cout << "\n+==========================================+\n";
        cout << "|              ORDER REPORT                |\n";
        cout << "+==========================================+\n";
        cout << "|1. Daily Orders                           |\n";
        cout << "|2. Monthly Orders                         |\n";
        cout << "|3. Annual Orders                          |\n";
        cout << "|0. Back                                   |\n";
        cout << "+==========================================+\n";

        choice = getMenuChoice(0, 3);

        switch (choice)
        {
        case 1:
        {
            string date;

            do
            {
                cout << "\nEnter Date (DD/MM/YYYY): ";
                getline(cin, date);

                if (!isValidReportDate(date))
                    cout << "Invalid date! Please use DD/MM/YYYY.\n";

            } while (!isValidReportDate(date));

            int totalOrders = 0;
            int pendingCount = 0;
            int washingCount = 0;
            int dryingCount = 0;
            int ironingCount = 0;
            int readyCount = 0;
            int collectedCount = 0;
            int cancelledCount = 0;

            for (Order& o : orders)
            {
                if (o.date == date)
                {
                    totalOrders++;

                    if (o.status == "Pending")
                        pendingCount++;
                    else if (o.status == "Washing")
                        washingCount++;
                    else if (o.status == "Drying")
                        dryingCount++;
                    else if (o.status == "Ironing")
                        ironingCount++;
                    else if (o.status == "Ready for Pickup")
                        readyCount++;
                    else if (o.status == "Collected")
                        collectedCount++;
                    else if (o.status == "Cancelled")
                        cancelledCount++;
                }
            }

            cout << "\n+==========================================+\n";
            cout << "|               DAILY ORDERS               |\n";
            cout << "+==========================================+\n";

            cout << "Date              : " << date << endl;
            cout << "Total Orders      : " << totalOrders << endl;
            cout << "Pending           : " << pendingCount << endl;
            cout << "Washing           : " << washingCount << endl;
            cout << "Drying            : " << dryingCount << endl;
            cout << "Ironing           : " << ironingCount << endl;
            cout << "Ready for Pickup  : " << readyCount << endl;
            cout << "Collected         : " << collectedCount << endl;
            cout << "Cancelled         : " << cancelledCount << endl;

            cout << "+==========================================+\n";

            break;
        }

        case 2:
        {
            string month;

            do
            {
                cout << "\nEnter Month (MM/YYYY): ";
                getline(cin, month);

                if (!isValidReportMonth(month))
                {
                    cout << "Invalid month! " << "Please enter in MM/YYYY format.\n";
                }

            } while (!isValidReportMonth(month));

            int totalOrders = 0;
            int pendingCount = 0;
            int washingCount = 0;
            int dryingCount = 0;
            int ironingCount = 0;
            int readyCount = 0;
            int collectedCount = 0;
            int cancelledCount = 0;

            for (Order& o : orders)
            {
                if (o.date.length() == 10)
                {
                    string orderMonth = o.date.substr(3, 7);

                    if (orderMonth == month)
                    {
                        totalOrders++;

                        if (o.status == "Pending")
                            pendingCount++;
                        else if (o.status == "Washing")
                            washingCount++;
                        else if (o.status == "Drying")
                            dryingCount++;
                        else if (o.status == "Ironing")
                            ironingCount++;
                        else if (o.status == "Ready for Pickup")
                            readyCount++;
                        else if (o.status == "Collected")
                            collectedCount++;
                        else if (o.status == "Cancelled")
                            cancelledCount++;
                    }
                }
            }

            cout << "\n+==========================================+\n";
            cout << "|              MONTHLY ORDERS              |\n";
            cout << "+==========================================+\n";

            cout << "Month             : " << month << endl;
            cout << "Total Orders      : " << totalOrders << endl;
            cout << "Pending           : " << pendingCount << endl;
            cout << "Washing           : " << washingCount << endl;
            cout << "Drying            : " << dryingCount << endl;
            cout << "Ironing           : " << ironingCount << endl;
            cout << "Ready for Pickup  : " << readyCount << endl;
            cout << "Collected         : " << collectedCount << endl;
            cout << "Cancelled         : " << cancelledCount << endl;

            cout << "+==========================================+\n";

            break;
        }

        case 3:
        {
            string year;

            do
            {
                cout << "\nEnter Year (YYYY): ";
                getline(cin, year);

                if (!isValidReportYear(year))
                {
                    cout << "Invalid year! " << "Please enter in YYYY format.\n";
                }

            } while (!isValidReportYear(year));

            int totalOrders = 0;
            int pendingCount = 0;
            int washingCount = 0;
            int dryingCount = 0;
            int ironingCount = 0;
            int readyCount = 0;
            int collectedCount = 0;
            int cancelledCount = 0;

            for (Order& o : orders)
            {
                if (o.date.length() == 10)
                {
                    string orderYear = o.date.substr(6, 4);

                    if (orderYear == year)
                    {
                        totalOrders++;

                        if (o.status == "Pending")
                            pendingCount++;
                        else if (o.status == "Washing")
                            washingCount++;
                        else if (o.status == "Drying")
                            dryingCount++;
                        else if (o.status == "Ironing")
                            ironingCount++;
                        else if (o.status == "Ready for Pickup")
                            readyCount++;
                        else if (o.status == "Collected")
                            collectedCount++;
                        else if (o.status == "Cancelled")
                            cancelledCount++;
                    }
                }
            }

            cout << "\n+==========================================+\n";
            cout << "|               ANNUAL ORDERS              |\n";
            cout << "+==========================================+\n";

            cout << "Year              : " << year << endl;
            cout << "Total Orders      : " << totalOrders << endl;
            cout << "Pending           : " << pendingCount << endl;
            cout << "Washing           : " << washingCount << endl;
            cout << "Drying            : " << dryingCount << endl;
            cout << "Ironing           : " << ironingCount << endl;
            cout << "Ready for Pickup  : " << readyCount << endl;
            cout << "Collected         : " << collectedCount << endl;
            cout << "Cancelled         : " << cancelledCount << endl;

            cout << "+==========================================+\n";

            break;
        }

        case 0:
            cout << "Returning...\n";
            break;
        }

    } while (choice != 0);
}

// view price history
void viewPriceHistory()
{
    cout << "\n+========================================================================+\n";
    cout << "|                           PRICE HISTORY                                |\n";
    cout << "+========================================================================+\n";

    if (priceHistories.empty())
    {
        cout << "No price change history found.\n";
        return;
    }

    cout << left << setw(15) << "Date" << setw(20) << "Service" << setw(15) << "Type" << setw(15) << "Old Price" << setw(15) << "New Price" << endl;

    cout << "--------------------------------------------------------------------------\n";

    for (PriceHistory& h : priceHistories)
    {
        cout << left << setw(15) << h.date << setw(20) << h.service << setw(15) << h.customerType << "RM " << setw(12) << fixed << setprecision(2) << h.oldPrice << "RM " << fixed << setprecision(2) << h.newPrice << endl;
    }
}

// price management
void priceManagement()
{
    int choice;

    string serviceNames[3] = {
        "Wash",
        "Dry Clean",
        "Iron"
    };

    do
    {
        cout << "\n+================================+\n";
        cout << "|       SERVICE PRICE            |\n";
        cout << "+================================+\n";

        cout << "|1. View Current Price           |\n";
        cout << "|2. Update Service Price         |\n";
        cout << "|3. View Price History           |\n";
        cout << "|0. Back                         |\n";

        cout << "+================================+\n";

        choice = getMenuChoice(0, 3);

        switch (choice)
        {
        case 1:
            displayServices();
            break;

        case 2:
        {
            displayServices();

            int serviceChoice = getMenuChoice(1, 3);

            cout << "\n1. Non-member Price\n";
            cout << "2. Member Price\n";

            int customerType = getMenuChoice(1, 2);
            int serviceIndex = serviceChoice - 1;
            int priceIndex = customerType - 1;
            double oldPrice = price.price[serviceIndex][priceIndex];
            double newPrice;

            newPrice = getPositiveDouble(
                "\nEnter new " + serviceNames[serviceIndex] + " price: RM ");

            PriceHistory h;
            h.date = getCurrentDate();
            h.service = serviceNames[serviceIndex];
            h.oldPrice = oldPrice;
            h.newPrice = newPrice;
            h.customerType = (priceIndex == 0 ? "Non-member" : "Member");

            price.price[serviceIndex][priceIndex] = newPrice;

            priceHistories.push_back(h);

            cout << "Price updated successfully!\n";

            break;
        }

        case 3:
            viewPriceHistory();
            break;

        case 0:
            cout << "Returning...\n";
            break;
        }

    } while (choice != 0);
}

// admin menu
void adminMenu(const string& adminID)
{
    int choice;

    do
    {
        cout << "\n+==================================================+\n";
        cout << "|                   ADMIN MENU                     |\n";
        cout << "+==================================================+\n";

        cout << "|1. Staff Management                              |\n";
        cout << "|2. Customer Management                           |\n";
        cout << "|3. View Income Report                            |\n";
        cout << "|4. View Order Report                             |\n";
        cout << "|5. Manage Service Price                          |\n";
        cout << "|6. Update Profile                                |\n";
        cout << "|0. Exit                                          |\n";

        cout << "+==================================================+\n";

        choice = getMenuChoice(0, 6);

        switch (choice)
        {
        case 1:
        {
            int staffChoice;

            do
            {
                cout << "\n+================================+\n";
                cout << "|       STAFF MANAGEMENT         |\n";
                cout << "+================================+\n";

                cout << "|1. Add Staff                    |\n";
                cout << "|2. Delete Staff                 |\n";
                cout << "|3. Update Staff                 |\n";
                cout << "|4. View All Staff               |\n";
                cout << "|0. Back                         |\n";

                cout << "+================================+\n";

                staffChoice = getMenuChoice(0, 4);

                switch (staffChoice)
                {
                case 1:
                    addStaff();
                    break;

                case 2:
                    deleteStaff();
                    break;

                case 3:
                    updateStaff();
                    break;

                case 4:
                    viewAllStaff();
                    break;
                }

            } while (staffChoice != 0);

            break;
        }

        case 2:
            customerManagement();
            break;

        case 3:
            viewReport();
            break;

        case 4:
            viewOrderReport();
            break;

        case 5:
            priceManagement();
            break;

        case 6:
            updateProfile(adminID);
            break;

        case 0:
            cout << "Logging out...\n";
            break;
        }

    } while (choice != 0);
}


// Login functions
void seedSampleAccounts()
{
    price.price[0][0] = 5.00;
    price.price[0][1] = 4.50;

    price.price[1][0] = 10.00;
    price.price[1][1] = 9.00;

    price.price[2][0] = 3.00;
    price.price[2][1] = 2.70;

    Customer c;

    c.id = "C1001";
    c.name = "John Tan";
    c.phone = "0123456789";
    c.password = "cust123";
    c.isMember = true;
    c.membershipExpiry = "23/08/2026";
    c.totalSpent = 0.0;
    c.loyaltyPoints = 0;

    customers.push_back(c);

    Staff s;
    s.id = "S1001";
    s.name = "Ali";
    s.phone = "0123456789";
    s.gender = 'M';
    s.dateJoined = "26/08/2026";
    s.status = true;
    s.password = "staff123";
    staffs.push_back(s);

    Admin a;
    a.id = "A1001";
    a.name = "Jia Le";
    a.password = "admin123";
    admins.push_back(a);
}

// authenticate user
bool authenticateUser(const string& id, const string& password, char& role)
{
    if (id.empty())
        return false;

    role = toupper(static_cast<unsigned char>(id[0]));

    if (role == 'C')
    {
        for (Customer& c : customers)
        {
            if (c.id == id && c.password == password)
            {
                return true;
            }
        }
    }
    else if (role == 'S')
    {
        for (Staff& s : staffs)
        {
            if (s.id == id && s.password == password && s.status)
            {
                return true;
            }
        }
    }
    else if (role == 'A')
    {
        for (Admin& a : admins)
        {
            if (a.id == id && a.password == password)
            {
                return true;
            }
        }
    }

    return false;
}

// main menu
void mainMenu(string id, char role)
{
    if (role == 'C')
    {
        customerMenu(id);
    }
    else if (role == 'S')
    {
        staffMenu(id);
    }
    else if (role == 'A')
    {
        adminMenu(id);
    }
}

// login page
void loginPage()
{
    string id;
    string password;

    cout << "\n+==================================================+\n";
    cout << "|     WELCOME TO SPARKLE LAUNDRY SERVICE SYSTEM    |\n";
    cout << "+==================================================+\n";
    cout << "|1. Login                                          |\n";
    cout << "|2. Register Customer                              |\n";
    cout << "|0. Exit                                           |\n";
    cout << "+==================================================+\n";

    int choice = getMenuChoice(0, 2);

    if (choice == 2)
    {
        registerCustomer();
        return;
    }

    if (choice == 0)
    {
        cout << "Thank you for using Sparkle Laundry Service System.\n";
        exit(0);
    }

    cout << "\nEnter your ID: ";
    getline(cin, id);

    cout << "Enter your password: ";
    getline(cin, password);

    if (id.empty())
    {
        cout << "ID cannot be empty!\n";
        return;
    }

    id[0] = toupper(static_cast<unsigned char>(id[0]));

    regex pattern("[ACS]\\d{4}");

    if (!regex_match(id, pattern))
    {
        cout << "Invalid ID format!\n";
        return;
    }

    char role;

    if (!authenticateUser(id, password, role))
    {
        cout << "Invalid ID or Password!\n";
        return;
    }

    if (role == 'C')
    {
        for (Customer& c : customers)
        {
            if (c.id == id)
            {
                cout << "\nLogin successful! Welcome, " << c.name << endl;
                viewNotifications(id);

                break;
            }
        }
    }
    else if (role == 'S')
    {
        for (Staff& s : staffs)
        {
            if (s.id == id)
            {
                cout << "\nLogin successful! Welcome, " << s.name << " (Staff)\n";
                break;
            }
        }
    }
    else if (role == 'A')
    {
        for (Admin& a : admins)
        {
            if (a.id == id)
            {
                cout << "\nLogin successful! Welcome, " << a.name << " (Admin)\n";
                break;
            }
        }
    }

    mainMenu(id, role);
}


// main
int main()
{
    seedSampleAccounts();

    while (true)
    {
        loginPage();
    }

    return 0;
}
