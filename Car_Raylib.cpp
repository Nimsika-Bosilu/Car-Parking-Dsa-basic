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
        if (name.empty()) return "Error: Park name cannot be empty!";
        if (capacity <= 0) return "Error: Capacity must be greater than 0!";
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
        if (parkName.empty() || carNumber.empty()) return "Error: Park name and car number required!";
        CarPark* temp = head;
        while (temp) {
            if (temp->name == parkName) {
                if ((int)temp->cars.size() < temp->capacity) {
                    temp->cars.push_back(carNumber);
                    return "Car " + carNumber + " parked in " + parkName + "!";
                } else {
                    return "Park '" + parkName + "' is FULL!";
                }
            }
            temp = temp->next;
        }
        return "Car park '" + parkName + "' not found!";
    }

    string removeCar(string carNumber) {
        if (carNumber.empty()) return "Error: Car number required!";
        CarPark* temp = head;
        while (temp) {
            for (int i = 0; i < (int)temp->cars.size(); i++) {
                if (temp->cars[i] == carNumber) {
                    temp->cars.erase(temp->cars.begin() + i);
                    return "Car " + carNumber + " removed from " + temp->name + "!";
                }
            }
            temp = temp->next;
        }
        return "Car '" + carNumber + "' not found!";
    }

    string findCar(string carNumber) {
        if (carNumber.empty()) return "Error: Car number required!";
        CarPark* temp = head;
        while (temp) {
            for (string car : temp->cars) {
                if (car == carNumber) {
                    return "Car " + carNumber + " is located in " + temp->name;
                }
            }
            temp = temp->next;
        }
        return "Car '" + carNumber + "' not found!";
    }

    CarPark* getHead() { return head; }
};


// ===== UI HELPERS =====
bool DrawButton(Rectangle rect, const char* label, Color col = DARKGRAY) {
    Vector2 mouse = GetMousePosition();
    bool clicked = false;
    Color draw = col;
    if (CheckCollisionPointRec(mouse, rect)) {
        draw = (Color){(unsigned char)min(255, col.r + 40), (unsigned char)min(255, col.g + 40), (unsigned char)min(255, col.b + 40), 255};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clicked = true;
    }
    DrawRectangleRec(rect, draw);
    DrawRectangleLinesEx(rect, 2, LIGHTGRAY);
    int tw = MeasureText(label, 18);
    DrawText(label, (int)(rect.x + (rect.width - tw) / 2), (int)(rect.y + (rect.height - 18) / 2), 18, WHITE);
    return clicked;
}

void DrawTextInput(Rectangle rect, string &text, bool &isActive, const char* hint = "") {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        isActive = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRec(rect, isActive ? (Color){240,240,255,255} : RAYWHITE);
    DrawRectangleLinesEx(rect, 2, isActive ? BLUE : GRAY);

    if (text.empty() && !isActive) {
        DrawText(hint, (int)rect.x + 6, (int)rect.y + 10, 18, LIGHTGRAY);
    } else {
        DrawText(text.c_str(), (int)rect.x + 6, (int)rect.y + 10, 18, BLACK);
    }

    if (isActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && (int)text.length() < 20)
                text += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty())
            text.pop_back();
    }
}

// Draw a labeled input row
void DrawInputRow(const char* label, Rectangle rect, string &val, bool &active, const char* hint = "") {
    DrawText(label, (int)rect.x - 130, (int)rect.y + 10, 18, WHITE);
    DrawTextInput(rect, val, active, hint);
}


// ===== MAIN =====
int main() {
    const int SW = 1100;
    const int SH = 700;
    InitWindow(SW, SH, "Smart Car Park System");
    SetTargetFPS(60);

    ParkingSystem ps;

    // Pre-load 10 parks (same as original)
    string names[10] = {"ParkA","ParkB","ParkC","ParkD","ParkE","ParkF","ParkG","ParkH","ParkI","ParkJ"};
    int coords[10][2] = {{2,3},{10,5},{-4,7},{6,-2},{0,0},{8,1},{-6,-3},{3,9},{-2,4},{7,-5}};
    int caps[10]      = {3,5,4,2,6,3,4,5,2,3};
    for (int i = 0; i < 10; i++)
        ps.addCarPark(names[i], coords[i][0], coords[i][1], caps[i]);

    // Tab: 0=Map, 1=ParkCar, 2=AddPark, 3=ShowAll, 4=Search
    int tab = 0;
    string statusMsg = "Welcome to Smart Car Park System!";
    bool statusOK = true;

    // --- Tab 1: Manage Car inputs ---
    string t1ParkName, t1CarNum;
    bool t1APark = false, t1ACar = false;

    // --- Tab 2: Add Car Park inputs ---
    string t2Name, t2X, t2Y, t2Cap;
    bool t2AName=false, t2AX=false, t2AY=false, t2ACap=false;

    // --- Tab 4: Search ---
    string t4X, t4Y;
    bool t4AX=false, t4AY=false;
    vector<ParkOption> searchResults;

    // Tab buttons
    Rectangle tabs[5] = {
        {20, 80, 130, 38},
        {160, 80, 130, 38},
        {300, 80, 140, 38},
        {450, 80, 120, 38},
        {580, 80, 160, 38}
    };
    const char* tabLabels[5] = {"View Map","Park Car","Add Park","Show All","Search"};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x141428FF));

        // Header
        DrawText("SMART CAR PARK SYSTEM", 20, 20, 28, SKYBLUE);
        DrawLine(20, 58, SW-20, 58, (Color){80,80,130,255});

        // Draw tabs
        for (int i = 0; i < 5; i++) {
            Color tc = (tab == i) ? (Color){30,80,180,255} : (Color){50,50,80,255};
            DrawButton(tabs[i], tabLabels[i], tc);
            if (CheckCollisionPointRec(GetMousePosition(), tabs[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                tab = i;
        }

        DrawLine(20, 126, SW-20, 126, (Color){60,60,100,255});

        // Status bar
        DrawRectangle(0, SH-44, SW, 44, (Color){20,20,50,255});
        DrawLine(0, SH-44, SW, SH-44, (Color){60,60,120,255});
        DrawText(statusMsg.c_str(), 16, SH-30, 18, statusOK ? (Color){80,255,140,255} : (Color){255,100,100,255});

        // ============ TAB 0: VIEW MAP ============
        if (tab == 0) {
            int cx = SW/2, cy = SH/2 + 40, scale = 30;
            // Axes
            DrawLine(cx, 135, cx, SH-55, (Color){80,80,80,255});
            DrawLine(20, cy, SW-20, cy, (Color){80,80,80,255});
            DrawText("N", cx+4, 140, 14, GRAY);
            DrawText("S", cx+4, SH-68, 14, GRAY);
            DrawText("E", SW-32, cy-16, 14, GRAY);
            DrawText("W", 24, cy-16, 14, GRAY);
            DrawText("City Map - Green=Available  Red=Full", 30, 138, 16, LIGHTGRAY);

            CarPark* tmp = ps.getHead();
            while (tmp) {
                int dx = cx + tmp->x * scale;
                int dy = cy - tmp->y * scale;
                bool avail = (int)tmp->cars.size() < tmp->capacity;
                Color pc = avail ? (Color){40,200,80,255} : (Color){220,50,50,255};
                DrawCircle(dx, dy, 18, pc);
                DrawCircleLines(dx, dy, 18, WHITE);
                DrawText(tmp->name.c_str(), dx - 22, dy - 36, 15, WHITE);
                string ct = to_string(tmp->cars.size()) + "/" + to_string(tmp->capacity);
                DrawText(ct.c_str(), dx - 14, dy + 22, 14, YELLOW);
                tmp = tmp->next;
            }
        }

        // ============ TAB 1: PARK / REMOVE / FIND CAR ============
        else if (tab == 1) {
            DrawText("Manage Cars", 30, 148, 22, SKYBLUE);
            DrawLine(30, 174, 500, 174, (Color){60,60,100,255});

            DrawInputRow("Park Name:", {200, 190, 280, 38}, t1ParkName, t1APark, "e.g. ParkA");
            DrawInputRow("Car Number:", {200, 244, 280, 38}, t1CarNum, t1ACar, "e.g. CAR-1234");

            if (DrawButton({200, 302, 130, 40}, "Park Car",   (Color){20,130,50,255})) {
                statusMsg = ps.parkCar(t1ParkName, t1CarNum);
                statusOK  = statusMsg.find("parked") != string::npos;
            }
            if (DrawButton({344, 302, 140, 40}, "Remove Car", (Color){160,40,40,255})) {
                statusMsg = ps.removeCar(t1CarNum);
                statusOK  = statusMsg.find("removed") != string::npos;
            }
            if (DrawButton({498, 302, 120, 40}, "Find Car",   (Color){40,80,160,255})) {
                statusMsg = ps.findCar(t1CarNum);
                statusOK  = statusMsg.find("located") != string::npos;
            }

            // Show parked cars for the entered park
            if (!t1ParkName.empty()) {
                CarPark* tmp = ps.getHead();
                while (tmp) {
                    if (tmp->name == t1ParkName) {
                        DrawText("Cars currently parked:", 200, 370, 18, LIGHTGRAY);
                        if (tmp->cars.empty()) {
                            DrawText("  (empty)", 200, 394, 16, GRAY);
                        } else {
                            for (int i = 0; i < (int)tmp->cars.size(); i++) {
                                DrawText(("  " + tmp->cars[i]).c_str(), 200, 394 + i*22, 16, YELLOW);
                            }
                        }
                        break;
                    }
                    tmp = tmp->next;
                }
            }
        }

        // ============ TAB 2: ADD / REMOVE PARK ============
        else if (tab == 2) {
            DrawText("Add / Remove Car Park", 30, 148, 22, SKYBLUE);
            DrawLine(30, 174, 600, 174, (Color){60,60,100,255});

            DrawInputRow("Park Name:", {240, 192, 280, 38}, t2Name, t2AName, "e.g. ParkZ");
            DrawInputRow("Location X:", {240, 244, 120, 38}, t2X, t2AX, "e.g. 5");
            DrawInputRow("Location Y:", {240, 298, 120, 38}, t2Y, t2AY, "e.g. -3");
            DrawInputRow("Capacity:", {240, 352, 120, 38}, t2Cap, t2ACap, "e.g. 4");

            if (DrawButton({240, 410, 160, 42}, "Add Car Park", (Color){20,130,60,255})) {
                try {
                    int x = t2X.empty() ? 0 : stoi(t2X);
                    int y = t2Y.empty() ? 0 : stoi(t2Y);
                    int c = t2Cap.empty() ? 0 : stoi(t2Cap);
                    statusMsg = ps.addCarPark(t2Name, x, y, c);
                    statusOK = statusMsg.find("added") != string::npos;
                    if (statusOK) { t2Name=""; t2X=""; t2Y=""; t2Cap=""; }
                } catch (...) {
                    statusMsg = "Error: X, Y, Capacity must be numbers!";
                    statusOK = false;
                }
            }
            if (DrawButton({420, 410, 190, 42}, "Remove Car Park", (Color){140,30,30,255})) {
                statusMsg = ps.removeCarPark(t2Name);
                statusOK = statusMsg.find("removed") != string::npos;
            }
        }

        // ============ TAB 3: SHOW ALL ============
        else if (tab == 3) {
            DrawText("All Car Parks", 30, 148, 22, SKYBLUE);
            DrawLine(30, 174, SW-30, 174, (Color){60,60,100,255});

            // Column headers
            DrawText("Park Name", 30, 185, 16, LIGHTGRAY);
            DrawText("Location", 200, 185, 16, LIGHTGRAY);
            DrawText("Capacity", 320, 185, 16, LIGHTGRAY);
            DrawText("Free", 430, 185, 16, LIGHTGRAY);
            DrawText("Status", 510, 185, 16, LIGHTGRAY);
            DrawText("Cars Parked", 620, 185, 16, LIGHTGRAY);
            DrawLine(30, 205, SW-30, 205, (Color){60,60,100,255});

            CarPark* tmp = ps.getHead();
            int row = 0;
            while (tmp) {
                int ry = 212 + row * 44;
                bool avail = (int)tmp->cars.size() < tmp->capacity;

                // Row background
                if (row % 2 == 0)
                    DrawRectangle(28, ry - 2, SW - 56, 40, (Color){30,30,55,255});

                DrawText(tmp->name.c_str(), 30, ry + 8, 17, WHITE);
                string loc = "(" + to_string(tmp->x) + "," + to_string(tmp->y) + ")";
                DrawText(loc.c_str(), 200, ry + 8, 17, LIGHTGRAY);
                DrawText(to_string(tmp->capacity).c_str(), 340, ry + 8, 17, WHITE);
                int free = tmp->capacity - (int)tmp->cars.size();
                DrawText(to_string(free).c_str(), 440, ry + 8, 17, free > 0 ? (Color){80,220,100,255} : (Color){220,80,80,255});
                DrawText(avail ? "OPEN" : "FULL", 510, ry + 8, 17, avail ? (Color){60,220,80,255} : (Color){220,60,60,255});

                // Cars list
                string carList = "";
                for (int i = 0; i < (int)tmp->cars.size(); i++) {
                    if (i > 0) carList += ", ";
                    carList += tmp->cars[i];
                }
                if (carList.empty()) carList = "-";
                DrawText(carList.c_str(), 620, ry + 8, 15, YELLOW);

                tmp = tmp->next;
                row++;
                if (ry + 44 > SH - 50) { // safety: don't draw off screen
                    DrawText("... scroll not shown ...", 30, ry + 48, 15, GRAY);
                    break;
                }
            }
        }

        // ============ TAB 4: SEARCH NEAREST ============
        else if (tab == 4) {
            DrawText("Search Nearest Available Park", 30, 148, 22, SKYBLUE);
            DrawLine(30, 174, 700, 174, (Color){60,60,100,255});

            DrawInputRow("Your X:", {220, 192, 120, 38}, t4X, t4AX, "e.g. 0");
            DrawInputRow("Your Y:", {220, 246, 120, 38}, t4Y, t4AY, "e.g. 0");

            if (DrawButton({220, 304, 200, 42}, "Search Nearest", (Color){40,80,180,255})) {
                try {
                    int x = t4X.empty() ? 0 : stoi(t4X);
                    int y = t4Y.empty() ? 0 : stoi(t4Y);
                    searchResults = ps.searchNearestParks(x, y);
                    if (searchResults.empty()) { statusMsg = "No available parking!"; statusOK = false; }
                    else { statusMsg = "Found " + to_string(searchResults.size()) + " available park(s)."; statusOK = true; }
                } catch (...) {
                    statusMsg = "Error: X and Y must be numbers!";
                    statusOK = false;
                    searchResults.clear();
                }
            }

            // Results table
            if (!searchResults.empty()) {
                DrawText("Rank", 30, 368, 16, LIGHTGRAY);
                DrawText("Park Name", 110, 368, 16, LIGHTGRAY);
                DrawText("Distance", 300, 368, 16, LIGHTGRAY);
                DrawText("Free Slots", 430, 368, 16, LIGHTGRAY);
                DrawLine(30, 388, 650, 388, (Color){60,60,100,255});

                for (int i = 0; i < (int)searchResults.size() && i < 10; i++) {
                    int ry = 396 + i * 36;
                    if (i % 2 == 0) DrawRectangle(28, ry-2, 640, 32, (Color){30,30,55,255});
                    Color rc = (i == 0) ? (Color){255,215,0,255} : WHITE;
                    DrawText(to_string(i + 1).c_str(), 44, ry + 6, 17, rc);
                    DrawText(searchResults[i].park->name.c_str(), 110, ry + 6, 17, rc);
                    string dist = to_string(searchResults[i].distance).substr(0, 5);
                    DrawText(dist.c_str(), 300, ry + 6, 17, LIGHTGRAY);
                    int fr = searchResults[i].park->capacity - (int)searchResults[i].park->cars.size();
                    DrawText(to_string(fr).c_str(), 448, ry + 6, 17, (Color){80,220,100,255});
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
