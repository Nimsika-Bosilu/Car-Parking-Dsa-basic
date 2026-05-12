#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

// ===== COLORS =====
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"

// ===== UI =====
void line() {
    cout << "========================================\n";
}

void header(string title) {
    line();
    cout << CYAN << title << RESET << endl;
    line();
}

// ===== SAFE PAUSE (FIXED) =====
void pauseScreen() {
    cout << "\nPress ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ===== CLEAR INPUT STATE =====
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ===== STRUCTS =====
struct CarPark {
    string name;
    int x, y;
    int capacity;
    vector<string> cars;
    CarPark* next;
};

struct ParkOption {
    CarPark* park;
    double distance;
};

// ===== CLASS =====
class ParkingSystem {
private:
    CarPark* head;

public:
    ParkingSystem() {
        head = NULL;
    }

    // ADD PARK
    void addCarPark(string name, int x, int y, int capacity, bool silent = false) {
        CarPark* newPark = new CarPark{name, x, y, capacity, {}, NULL};

        if (!head) head = newPark;
        else {
            CarPark* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newPark;
        }

        if (!silent)
            cout << GREEN << "Car park added!\n" << RESET;
    }

    // REMOVE PARK
    void removeCarPark(string name) {
        CarPark* temp = head;
        CarPark* prev = NULL;

        while (temp) {
            if (temp->name == name) {
                if (prev) prev->next = temp->next;
                else head = temp->next;

                delete temp;
                cout << RED << "Car park removed!\n" << RESET;
                return;
            }
            prev = temp;
            temp = temp->next;
        }

        cout << RED << "Car park not found!\n" << RESET;
    }

    // SEARCH
    vector<ParkOption> searchNearestParks(int userX, int userY) {
        vector<ParkOption> options;
        CarPark* temp = head;

        while (temp) {
            if (temp->cars.size() < temp->capacity) {
                double dist = sqrt(pow(temp->x - userX, 2) +
                                   pow(temp->y - userY, 2));

                options.push_back({temp, dist});
            }
            temp = temp->next;
        }

        sort(options.begin(), options.end(),
             [](ParkOption a, ParkOption b) {
                 return a.distance < b.distance;
             });

        return options;
    }

    void showNearestParks(int x, int y) {
        vector<ParkOption> options = searchNearestParks(x, y);

        if (options.empty()) {
            cout << RED << "No available parking!\n" << RESET;
            return;
        }

        cout << "\nNearest Parking Options:\n";
        line();

        for (int i = 0; i < options.size(); i++) {
            cout << i + 1 << ". " << options[i].park->name
                 << " | Distance: " << options[i].distance
                 << " | Free: "
                 << (options[i].park->capacity -
                     options[i].park->cars.size())
                 << endl;
        }
    }

    // PARK CAR
    void parkCar(string parkName, string carNumber) {
        CarPark* temp = head;

        while (temp) {
            if (temp->name == parkName) {
                if (temp->cars.size() < temp->capacity) {
                    temp->cars.push_back(carNumber);
                    cout << GREEN << "Car parked in " << parkName << RESET << endl;
                } else {
                    cout << RED << "This park is FULL!\n" << RESET;
                }
                return;
            }
            temp = temp->next;
        }

        cout << RED << "Car park not found!\n" << RESET;
    }

    // REMOVE CAR
    void removeCar(string carNumber) {
        CarPark* temp = head;

        while (temp) {
            for (int i = 0; i < temp->cars.size(); i++) {
                if (temp->cars[i] == carNumber) {
                    temp->cars.erase(temp->cars.begin() + i);
                    cout << GREEN << "Car removed!\n" << RESET;
                    return;
                }
            }
            temp = temp->next;
        }

        cout << RED << "Car not found!\n" << RESET;
    }

    // FIND CAR
    void findCar(string carNumber) {
        CarPark* temp = head;

        while (temp) {
            for (string car : temp->cars) {
                if (car == carNumber) {
                    cout << GREEN << "Car is in " << temp->name << endl;
                    return;
                }
            }
            temp = temp->next;
        }

        cout << RED << "Car not found!\n" << RESET;
    }

    // DISPLAY
    void display() {
        header("CAR PARK STATUS");

        CarPark* temp = head;

        while (temp) {
            cout << YELLOW << temp->name << RESET
                 << " (" << temp->cars.size()
                 << "/" << temp->capacity << ")\n";

            for (string car : temp->cars) {
                cout << "  - " << car << endl;
            }

            line();
            temp = temp->next;
        }
    }
};

// ===== MENU =====
void menu() {
    header("SMART CAR PARK SYSTEM");

    cout << YELLOW;
    cout << "1. Search Nearest Parks\n";
    cout << "2. Park Car\n";
    cout << "3. Remove Car\n";
    cout << "4. Find Car\n";
    cout << "5. Show All\n";
    cout << "6. Add Car Park\n";
    cout << "7. Remove Car Park\n";
    cout << "8. Exit\n";
    cout << RESET;

    line();
    cout << "Enter choice: ";
}

// ===== MAIN =====
int main() {
    ParkingSystem ps;

    string names[10] = {
        "ParkA","ParkB","ParkC","ParkD","ParkE",
        "ParkF","ParkG","ParkH","ParkI","ParkJ"
    };

    int coords[10][2] = {
        {2,3},{10,5},{-4,7},{6,-2},{0,0},
        {8,1},{-6,-3},{3,9},{-2,4},{7,-5}
    };

    int capacity[10] = {3,5,4,2,6,3,4,5,2,3};

    for (int i = 0; i < 10; i++) {
        ps.addCarPark(names[i], coords[i][0], coords[i][1], capacity[i], true);
    }

    int choice;

    while (true) {
        header("SMART CAR PARK SYSTEM");
        menu();

        if (!(cin >> choice)) {
            cout << RED << "Invalid input!\n" << RESET;
            clearInput();
            continue;
        }

        if (choice == 1) {
            int x, y;
            cout << "Enter location (x y): ";
            cin >> x >> y;

            ps.showNearestParks(x, y);
            pauseScreen();
        }

        else if (choice == 2) {
            string park, car;
            cout << "Park name: ";
            cin >> park;
            cout << "Car number: ";
            cin >> car;

            ps.parkCar(park, car);
            pauseScreen();
        }

        else if (choice == 3) {
            string car;
            cout << "Car number: ";
            cin >> car;

            ps.removeCar(car);
            pauseScreen();
        }

        else if (choice == 4) {
            string car;
            cout << "Car number: ";
            cin >> car;

            ps.findCar(car);
            pauseScreen();
        }

        else if (choice == 5) {
            ps.display();
            pauseScreen();
        }

        else if (choice == 6) {
            string name;
            int x, y, cap;

            cout << "Park name: ";
            cin >> name;

            cout << "Location (x y): ";
            cin >> x >> y;

            cout << "Capacity: ";
            cin >> cap;

            ps.addCarPark(name, x, y, cap);
            pauseScreen();
        }

        else if (choice == 7) {
            string name;
            cout << "Park name: ";
            cin >> name;

            ps.removeCarPark(name);
            pauseScreen();
        }

        else if (choice == 8) {
            break;
        }
    }

    return 0;
}