#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                 // for text, hbox, vbox, window, etc.

using namespace std;
using namespace ftxui;

// ===== STRUCTS (From Original Logic) =====
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

// ===== CLASS (From Original Logic) =====
class ParkingSystem {
private:
    CarPark* head;

public:
    ParkingSystem() { head = NULL; }

    string addCarPark(string name, int x, int y, int capacity) {
        CarPark* newPark = new CarPark{name, x, y, capacity, {}, NULL};
        if (!head) head = newPark;
        else {
            CarPark* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newPark;
        }
        return "Car park '" + name + "' added successfully!";
    }

    string removeCarPark(string name) {
        CarPark* temp = head;
        CarPark* prev = NULL;
        while (temp) {
            if (temp->name == name) {
                if (prev) prev->next = temp->next;
                else head = temp->next;
                delete temp;
                return "Car park '" + name + "' removed!";
            }
            prev = temp;
            temp = temp->next;
        }
        return "Car park not found!";
    }

    vector<ParkOption> searchNearestParks(int userX, int userY) {
        vector<ParkOption> options;
        CarPark* temp = head;
        while (temp) {
            if (temp->cars.size() < temp->capacity) {
                double dist = sqrt(pow(temp->x - userX, 2) + pow(temp->y - userY, 2));
                options.push_back({temp, dist});
            }
            temp = temp->next;
        }
        sort(options.begin(), options.end(), [](ParkOption a, ParkOption b) {
            return a.distance < b.distance;
        });
        return options;
    }

    string parkCar(string parkName, string carNumber) {
        CarPark* temp = head;
        while (temp) {
            if (temp->name == parkName) {
                if (temp->cars.size() < temp->capacity) {
                    temp->cars.push_back(carNumber);
                    return "Car " + carNumber + " parked in " + parkName;
                } else {
                    return "This park is FULL!";
                }
            }
            temp = temp->next;
        }
        return "Car park not found!";
    }

    string removeCar(string carNumber) {
        CarPark* temp = head;
        while (temp) {
            for (int i = 0; i < temp->cars.size(); i++) {
                if (temp->cars[i] == carNumber) {
                    temp->cars.erase(temp->cars.begin() + i);
                    return "Car " + carNumber + " removed successfully!";
                }
            }
            temp = temp->next;
        }
        return "Car not found!";
    }

    string findCar(string carNumber) {
        CarPark* temp = head;
        while (temp) {
            for (string car : temp->cars) {
                if (car == carNumber) {
                    return "Car " + carNumber + " is located in " + temp->name;
                }
            }
            temp = temp->next;
        }
        return "Car not found!";
    }
    
    CarPark* getHead() { return head; }
};

int main(int argc, const char* argv[]) {
    ParkingSystem ps;

    string names[10] = {"ParkA","ParkB","ParkC","ParkD","ParkE", "ParkF","ParkG","ParkH","ParkI","ParkJ"};
    int coords[10][2] = {{2,3},{10,5},{-4,7},{6,-2},{0,0}, {8,1},{-6,-3},{3,9},{-2,4},{7,-5}};
    int capacity[10] = {3,5,4,2,6,3,4,5,2,3};
    for (int i = 0; i < 10; i++) {
        ps.addCarPark(names[i], coords[i][0], coords[i][1], capacity[i]);
    }

    auto screen = ScreenInteractive::TerminalOutput();

    int tab_index = 0;
    vector<string> tab_entries = {
        "View Status", "Park/Remove Car", "Search Nearest", "Exit"
    };
    auto tab_selection = Toggle(&tab_entries, &tab_index);

    // Park/Remove State
    string park_name;
    string car_number;
    string status_message = "Welcome to Smart Car Park System!";
    
    auto input_park = Input(&park_name, "Enter Park Name...");
    auto input_car = Input(&car_number, "Enter Car Number...");

    auto park_button = Button("Park Car", [&] {
        status_message = ps.parkCar(park_name, car_number);
    });
    auto remove_button = Button("Remove Car", [&] {
        status_message = ps.removeCar(car_number);
    });
    
    // Search State
    string user_x = "0";
    string user_y = "0";
    string search_result = "";
    auto input_x = Input(&user_x, "X Coord");
    auto input_y = Input(&user_y, "Y Coord");
    auto search_button = Button("Search", [&] {
        try {
            int x = stoi(user_x);
            int y = stoi(user_y);
            auto options = ps.searchNearestParks(x, y);
            if(options.empty()) search_result = "No parks available!";
            else search_result = "Nearest is " + options[0].park->name + " (Dist: " + to_string((int)options[0].distance) + ")";
        } catch(...) {
            search_result = "Invalid coordinates!";
        }
    });

    auto container = Container::Tab({
        Container::Vertical({}), // Tab 0 doesn't need interactive inputs
        Container::Vertical({
            input_park,
            input_car,
            Container::Horizontal({park_button, remove_button})
        }),
        Container::Vertical({
            input_x,
            input_y,
            search_button
        }),
        Container::Vertical({}) // Exit
    }, &tab_index);

    auto main_container = Container::Vertical({
        tab_selection,
        container,
    });

    auto renderer = Renderer(main_container, [&] {
        if (tab_index == 3) {
            screen.ExitLoopClosure()();
        }

        Element content;
        if (tab_index == 0) {
            Elements park_elements;
            CarPark* temp = ps.getHead();
            while (temp) {
                string status = temp->name + " [" + to_string(temp->cars.size()) + "/" + to_string(temp->capacity) + "]";
                park_elements.push_back(text(status) | (temp->cars.size() < temp->capacity ? color(Color::Green) : color(Color::Red)));
                temp = temp->next;
            }
            content = vbox(park_elements) | border;
        } 
        else if (tab_index == 1) {
            content = vbox({
                hbox(text("Park Name: "), input_park->Render()),
                hbox(text("Car Number: "), input_car->Render()),
                hbox(park_button->Render(), text("   "), remove_button->Render()),
                separator(),
                text(status_message) | color(Color::Yellow)
            }) | border;
        }
        else if (tab_index == 2) {
            content = vbox({
                hbox(text("Your X: "), input_x->Render()),
                hbox(text("Your Y: "), input_y->Render()),
                search_button->Render(),
                separator(),
                text(search_result) | color(Color::Cyan)
            }) | border;
        }

        return vbox({
            text(" SMART CAR PARK SYSTEM ") | bold | center,
            separator(),
            tab_selection->Render(),
            separator(),
            content
        }) | border;
    });

    screen.Loop(renderer);
    return 0;
}
