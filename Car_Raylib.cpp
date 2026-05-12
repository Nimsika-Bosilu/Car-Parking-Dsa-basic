#include "raylib.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

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

// ===== CLASS (From Original Logic, modified to return strings for UI) =====
class ParkingSystem {
private:
    CarPark* head;

public:
    ParkingSystem() {
        head = NULL;
    }

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
            if ((int)temp->cars.size() < temp->capacity) {
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
                if ((int)temp->cars.size() < temp->capacity) {
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
            for (int i = 0; i < (int)temp->cars.size(); i++) {
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


// ===== UI HELPER FUNCTIONS =====
bool DrawButton(Rectangle rect, const char* text) {
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();
    
    Color btnColor = DARKGRAY;
    if (CheckCollisionPointRec(mousePoint, rect)) {
        btnColor = GRAY;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clicked = true;
    }
    
    DrawRectangleRec(rect, btnColor);
    DrawRectangleLinesEx(rect, 2, LIGHTGRAY);
    
    int textWidth = MeasureText(text, 20);
    DrawText(text, rect.x + (rect.width - textWidth) / 2, rect.y + (rect.height - 20) / 2, 20, WHITE);
    
    return clicked;
}

void DrawTextInput(Rectangle rect, string &text, bool &isActive) {
    Vector2 mousePoint = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isActive = CheckCollisionPointRec(mousePoint, rect);
    }
    
    DrawRectangleRec(rect, isActive ? LIGHTGRAY : RAYWHITE);
    DrawRectangleLinesEx(rect, 2, isActive ? BLUE : GRAY);
    
    DrawText(text.c_str(), rect.x + 5, rect.y + 10, 20, BLACK);
    
    if (isActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (text.length() < 15)) {
                text += (char)key;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && text.length() > 0) {
            text.pop_back();
        }
    }
}


// ===== MAIN LOOP =====
int main() {
    // Initialization
    const int screenWidth = 1000;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Smart Car Park System - GUI");
    SetTargetFPS(60);

    ParkingSystem ps;

    // Load initial data (same as original code)
    string names[10] = {"ParkA","ParkB","ParkC","ParkD","ParkE", "ParkF","ParkG","ParkH","ParkI","ParkJ"};
    int coords[10][2] = {{2,3},{10,5},{-4,7},{6,-2},{0,0}, {8,1},{-6,-3},{3,9},{-2,4},{7,-5}};
    int capacity[10] = {3,5,4,2,6,3,4,5,2,3};
    for (int i = 0; i < 10; i++) {
        ps.addCarPark(names[i], coords[i][0], coords[i][1], capacity[i]);
    }

    // UI State
    int currentTab = 0; // 0=Map, 1=Park/Remove, 2=Search
    string inputParkName = "";
    string inputCarNum = "";
    string inputX = "";
    string inputY = "";
    bool activeParkName = false, activeCarNum = false, activeX = false, activeY = false;
    string statusMessage = "Welcome to Smart Car Park System!";

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818FF)); // Dark background

        // Title
        DrawText("SMART CAR PARK SYSTEM", 20, 20, 30, SKYBLUE);
        DrawLine(20, 60, screenWidth - 20, 60, LIGHTGRAY);

        // Tabs
        if (DrawButton({ 20, 80, 150, 40 }, "View Map")) currentTab = 0;
        if (DrawButton({ 180, 80, 180, 40 }, "Manage Cars")) currentTab = 1;
        if (DrawButton({ 370, 80, 150, 40 }, "Search Parks")) currentTab = 2;

        // Status Bar
        DrawRectangle(0, screenHeight - 40, screenWidth, 40, DARKBLUE);
        DrawText(statusMessage.c_str(), 20, screenHeight - 30, 20, WHITE);

        if (currentTab == 0) {
            // Draw Map
            int centerX = screenWidth / 2;
            int centerY = screenHeight / 2 + 50;
            int scale = 25; // Pixels per coordinate unit
            
            // Draw axes
            DrawLine(centerX, 150, centerX, screenHeight - 60, GRAY);
            DrawLine(20, centerY, screenWidth - 20, centerY, GRAY);

            CarPark* temp = ps.getHead();
            while (temp) {
                int drawX = centerX + temp->x * scale;
                int drawY = centerY - temp->y * scale; // Invert Y for cartesian
                
                Color parkColor = ((int)temp->cars.size() < temp->capacity) ? GREEN : RED;
                DrawCircle(drawX, drawY, 15, parkColor);
                DrawText(temp->name.c_str(), drawX - 20, drawY - 30, 15, WHITE);
                
                string capText = to_string(temp->cars.size()) + "/" + to_string(temp->capacity);
                DrawText(capText.c_str(), drawX - 10, drawY + 20, 15, YELLOW);
                
                temp = temp->next;
            }
        } 
        else if (currentTab == 1) {
            // Manage Cars Menu
            DrawText("Park Name:", 50, 150, 20, WHITE);
            DrawTextInput({ 200, 140, 200, 40 }, inputParkName, activeParkName);

            DrawText("Car Number:", 50, 210, 20, WHITE);
            DrawTextInput({ 200, 200, 200, 40 }, inputCarNum, activeCarNum);

            if (DrawButton({ 50, 280, 120, 40 }, "Park Car")) {
                statusMessage = ps.parkCar(inputParkName, inputCarNum);
            }
            if (DrawButton({ 190, 280, 140, 40 }, "Remove Car")) {
                statusMessage = ps.removeCar(inputCarNum);
            }
            if (DrawButton({ 350, 280, 120, 40 }, "Find Car")) {
                statusMessage = ps.findCar(inputCarNum);
            }
        }
        else if (currentTab == 2) {
            // Search Nearest Menu
            DrawText("Your X:", 50, 150, 20, WHITE);
            DrawTextInput({ 150, 140, 100, 40 }, inputX, activeX);

            DrawText("Your Y:", 50, 210, 20, WHITE);
            DrawTextInput({ 150, 200, 100, 40 }, inputY, activeY);

            if (DrawButton({ 50, 280, 200, 40 }, "Search Nearest")) {
                try {
                    int x = stoi(inputX);
                    int y = stoi(inputY);
                    vector<ParkOption> options = ps.searchNearestParks(x, y);
                    if (options.empty()) statusMessage = "No available parking!";
                    else statusMessage = "Nearest: " + options[0].park->name + " (Dist: " + to_string((int)options[0].distance) + ")";
                } catch(...) {
                    statusMessage = "Error: Invalid X or Y coordinates!";
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
