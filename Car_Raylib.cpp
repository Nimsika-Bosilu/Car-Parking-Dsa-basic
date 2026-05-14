#include "raylib.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

// ===== STRUCTS =====
struct CarPark {
    string name;
    int x, y, capacity;
    vector<string> cars;
    CarPark* next;
};
struct ParkOption { CarPark* park; double distance; };

// ===== CLASS =====
class ParkingSystem {
    CarPark* head;
public:
    ParkingSystem() : head(NULL) {}

    string addCarPark(string name, int x, int y, int cap) {
        if (name.empty()) return "Error: Name cannot be empty!";
        if (cap <= 0)     return "Error: Capacity must be > 0!";
        CarPark* n = new CarPark{name, x, y, cap, {}, NULL};
        if (!head) head = n;
        else { CarPark* t = head; while (t->next) t = t->next; t->next = n; }
        return "Park '" + name + "' added!";
    }

    string removeCarPark(string name) {
        CarPark *t = head, *prev = NULL;
        while (t) {
            if (t->name == name) {
                if (prev) prev->next = t->next; else head = t->next;
                delete t; return "Park '" + name + "' removed!";
            }
            prev = t; t = t->next;
        }
        return "Park not found!";
    }

    vector<ParkOption> searchNearest(int ux, int uy) {
        vector<ParkOption> v;
        for (CarPark* t = head; t; t = t->next)
            if ((int)t->cars.size() < t->capacity)
                v.push_back({t, sqrt(pow(t->x-ux,2)+pow(t->y-uy,2))});
        sort(v.begin(), v.end(), [](ParkOption a, ParkOption b){ return a.distance < b.distance; });
        return v;
    }

    string parkCar(string park, string car) {
        if (park.empty()||car.empty()) return "Error: Both fields required!";
        for (CarPark* t = head; t; t = t->next)
            if (t->name == park) {
                if ((int)t->cars.size() < t->capacity) { t->cars.push_back(car); return "Car "+car+" parked in "+park+"!"; }
                return "Park '"+park+"' is FULL!";
            }
        return "Park '"+park+"' not found!";
    }

    string removeCar(string car) {
        if (car.empty()) return "Error: Car number required!";
        for (CarPark* t = head; t; t = t->next)
            for (int i=0;i<(int)t->cars.size();i++)
                if (t->cars[i]==car) { t->cars.erase(t->cars.begin()+i); return "Car "+car+" removed from "+t->name+"!"; }
        return "Car '"+car+"' not found!";
    }

    string findCar(string car) {
        if (car.empty()) return "Error: Car number required!";
        for (CarPark* t = head; t; t = t->next)
            for (auto& c : t->cars) if (c==car) return "Car "+car+" is in "+t->name;
        return "Car '"+car+"' not found!";
    }

    // Collect all park names into a vector
    vector<string> getParkNames() {
        vector<string> v;
        for (CarPark* t = head; t; t = t->next) v.push_back(t->name);
        return v;
    }

    CarPark* getByName(string name) {
        for (CarPark* t = head; t; t = t->next) if (t->name == name) return t;
        return NULL;
    }

    CarPark* getHead() { return head; }
};

// ===== UI HELPERS =====
bool DrawBtn(Rectangle r, const char* lbl, Color col) {
    Vector2 m = GetMousePosition();
    bool hit = CheckCollisionPointRec(m, r);
    bool clicked = hit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    Color c = hit ? (Color){(unsigned char)min(255,col.r+40),(unsigned char)min(255,col.g+40),(unsigned char)min(255,col.b+40),255} : col;
    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 2, (Color){180,180,220,255});
    int tw = MeasureText(lbl, 18);
    DrawText(lbl, (int)(r.x+(r.width-tw)/2), (int)(r.y+(r.height-18)/2), 18, WHITE);
    return clicked;
}

void DrawInput(Rectangle r, string& txt, bool& active, const char* hint="") {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        active = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRec(r, active?(Color){230,240,255,255}:RAYWHITE);
    DrawRectangleLinesEx(r, 2, active?BLUE:GRAY);
    if (txt.empty()&&!active) DrawText(hint,(int)r.x+6,(int)r.y+10,17,LIGHTGRAY);
    else DrawText(txt.c_str(),(int)r.x+6,(int)r.y+10,17,BLACK);
    if (active) {
        int k = GetCharPressed();
        while (k>0) { if (k>=32&&k<=125&&(int)txt.size()<20) txt+=(char)k; k=GetCharPressed(); }
        if (IsKeyPressed(KEY_BACKSPACE)&&!txt.empty()) txt.pop_back();
    }
}

// ===== DROPDOWN WIDGET =====
// Returns true if selection changed. isOpen is the open/close flag.
bool DrawDropdown(Rectangle r, vector<string>& items, int& sel, bool& isOpen) {
    bool changed = false;
    string lbl = (sel>=0&&sel<(int)items.size()) ? items[sel] : "-- Select Park --";
    // Main box
    bool boxHit = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRec(r, (Color){230,240,255,255});
    DrawRectangleLinesEx(r, 2, isOpen?BLUE:GRAY);
    DrawText(lbl.c_str(), (int)r.x+8, (int)r.y+10, 17, BLACK);
    // Arrow
    DrawText(isOpen ? "^" : "v", (int)(r.x+r.width-22), (int)r.y+10, 17, DARKGRAY);
    if (boxHit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) isOpen = !isOpen;

    // Dropdown list (drawn on top, so call AFTER other widgets)
    if (isOpen) {
        float itemH = 34;
        for (int i=0;i<(int)items.size();i++) {
            Rectangle ir = {r.x, r.y+r.height+i*itemH, r.width, itemH};
            bool hover = CheckCollisionPointRec(GetMousePosition(), ir);
            DrawRectangleRec(ir, hover?(Color){180,210,255,255}:(Color){240,245,255,255});
            DrawRectangleLinesEx(ir, 1, (Color){180,200,240,255});
            DrawText(items[i].c_str(), (int)ir.x+8, (int)ir.y+8, 17, BLACK);
            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                sel = i; isOpen = false; changed = true;
            }
        }
    }
    return changed;
}

// ===== MAIN =====
int main() {
    const int SW = 1280, SH = 800;
    InitWindow(SW, SH, "Smart Car Park System");
    SetTargetFPS(60);

    ParkingSystem ps;
    string pnames[10]={"ParkA","ParkB","ParkC","ParkD","ParkE","ParkF","ParkG","ParkH","ParkI","ParkJ"};
    int pcoords[10][2]={{2,3},{10,5},{-4,7},{6,-2},{0,0},{8,1},{-6,-3},{3,9},{-2,4},{7,-5}};
    int pcaps[10]={3,5,4,2,6,3,4,5,2,3};
    for (int i=0;i<10;i++) ps.addCarPark(pnames[i],pcoords[i][0],pcoords[i][1],pcaps[i]);

    int tab = 0;
    string statusMsg = "Welcome to Smart Car Park System!";
    bool statusOK = true;

    // Tab 1 – Park/Remove/Find Car
    int   t1SelPark=-1; bool t1DDOpen=false;
    string t1CarNum; bool t1ACar=false;

    // Tab 2 – Add / Remove Park
    string t2Name,t2X,t2Y,t2Cap;
    bool t2AN=false,t2AX=false,t2AY=false,t2AC=false;
    int  t2SelRem=-1; bool t2DDOpen=false;

    // Tab 4 – Search
    string t4X,t4Y; bool t4AX=false,t4AY=false;
    vector<ParkOption> searchRes;

    // Tab labels + widths
    const char* tabLbls[5]={"  View Map  ","  Park Car  ","  Add Park  ","  Show All  ","   Search   "};
    int tabW[5]={150,150,150,150,150};
    int tabStartX=24, tabY=68, tabH=42;

    while (!WindowShouldClose()) {
        // Build park names each frame (may change if added/removed)
        vector<string> parkNames = ps.getParkNames();

        BeginDrawing();
        ClearBackground(GetColor(0x0F0F2AFF));

        // ---- HEADER ----
        DrawRectangle(0,0,SW,60,(Color){18,18,50,255});
        DrawText("SMART CAR PARK SYSTEM", 24, 14, 30, SKYBLUE);
        DrawText("DSA Project", SW-120, 20, 18, (Color){100,140,220,255});
        DrawLine(0,60,SW,60,(Color){60,60,120,255});

        // ---- MENU BAR ----
        DrawRectangle(0,60,SW,54,(Color){22,22,58,255});
        int tx = tabStartX;
        for (int i=0;i<5;i++) {
            Rectangle tr={(float)tx,(float)tabY,(float)tabW[i],(float)tabH};
            Color tc = (tab==i)?(Color){40,100,220,255}:(Color){45,45,80,255};
            if (DrawBtn(tr, tabLbls[i], tc)) tab=i;
            // Active underline
            if (tab==i) DrawRectangle(tx, tabY+tabH-4, tabW[i], 4, SKYBLUE);
            tx += tabW[i]+6;
        }
        DrawLine(0,114,SW,114,(Color){50,50,100,255});

        // ---- STATUS BAR ----
        DrawRectangle(0,SH-46,SW,46,(Color){15,15,40,255});
        DrawLine(0,SH-46,SW,SH-46,(Color){50,50,110,255});
        DrawText("STATUS:", 14, SH-30, 17, (Color){100,120,200,255});
        DrawText(statusMsg.c_str(), 90, SH-30, 17, statusOK?(Color){80,240,130,255}:(Color){255,100,100,255});

        // ========== TAB 0: VIEW MAP ==========
        if (tab==0) {
            int cx=SW/2, cy=(SH+114)/2+10, scale=40;
            // Grid lines
            for (int g=-12;g<=12;g++) {
                DrawLine(cx+g*scale,115,cx+g*scale,SH-46,(Color){35,35,65,255});
                DrawLine(24,cy-g*scale,SW-24,cy-g*scale,(Color){35,35,65,255});
            }
            // Axes
            DrawLine(cx,115,cx,SH-46,(Color){80,80,120,255});
            DrawLine(24,cy,SW-24,cy,(Color){80,80,120,255});
            DrawText("N",cx+6,120,16,(Color){120,120,160,255});
            DrawText("S",cx+6,SH-68,16,(Color){120,120,160,255});
            DrawText("E",SW-38,cy-18,16,(Color){120,120,160,255});
            DrawText("W",28,cy-18,16,(Color){120,120,160,255});

            // Legend
            DrawCircle(SW-130,130,10,(Color){40,200,80,255}); DrawText("Available",SW-115,123,15,LIGHTGRAY);
            DrawCircle(SW-130,155,10,(Color){220,50,50,255}); DrawText("Full",SW-115,148,15,LIGHTGRAY);

            for (CarPark* t=ps.getHead();t;t=t->next) {
                int dx=cx+t->x*scale, dy=cy-t->y*scale;
                bool av=(int)t->cars.size()<t->capacity;
                Color pc=av?(Color){40,200,80,255}:(Color){220,50,50,255};
                DrawCircle(dx,dy,20,pc);
                DrawCircleLines(dx,dy,20,WHITE);
                // Park name above
                int nw=MeasureText(t->name.c_str(),14);
                DrawText(t->name.c_str(),dx-nw/2,dy-38,14,WHITE);
                // Capacity below
                string ct=to_string(t->cars.size())+"/"+to_string(t->capacity);
                int cw=MeasureText(ct.c_str(),13);
                DrawText(ct.c_str(),dx-cw/2,dy+24,13,YELLOW);
            }
        }

        // ========== TAB 1: PARK / REMOVE / FIND CAR ==========
        else if (tab==1) {
            DrawText("Manage Cars", 30, 130, 22, SKYBLUE);
            DrawLine(30,156,SW-30,156,(Color){50,50,100,255});

            // Left panel – inputs
            DrawRectangle(24,164,520,360,(Color){20,20,50,255});
            DrawRectangleLinesEx({24,164,520,360},2,(Color){50,50,100,255});

            DrawText("Select Park:", 44, 190, 18, LIGHTGRAY);
            DrawText("Car Number:", 44, 250, 18, LIGHTGRAY);
            DrawInput({220,244,300,38},t1CarNum,t1ACar,"e.g. CAR-1234");

            // Action buttons
            if (DrawBtn({44,310,150,44},"Park Car",(Color){20,130,50,255})) {
                string pk = (t1SelPark>=0&&t1SelPark<(int)parkNames.size()) ? parkNames[t1SelPark] : "";
                statusMsg=ps.parkCar(pk,t1CarNum);
                statusOK=statusMsg.find("parked")!=string::npos;
            }
            if (DrawBtn({210,310,150,44},"Remove Car",(Color){160,30,30,255})) {
                statusMsg=ps.removeCar(t1CarNum);
                statusOK=statusMsg.find("removed")!=string::npos;
            }
            if (DrawBtn({376,310,144,44},"Find Car",(Color){30,80,160,255})) {
                string pk=(t1SelPark>=0&&t1SelPark<(int)parkNames.size())?parkNames[t1SelPark]:"";
                statusMsg=ps.findCar(t1CarNum);
                statusOK=statusMsg.find("in")!=string::npos;
            }

            // Right panel – selected park info
            string selPkName = (t1SelPark>=0&&t1SelPark<(int)parkNames.size()) ? parkNames[t1SelPark] : "";
            CarPark* sp = selPkName.empty() ? NULL : ps.getByName(selPkName);
            DrawRectangle(560,164,SW-584,360,(Color){20,20,50,255});
            DrawRectangleLinesEx({560,164,(float)(SW-584),360},2,(Color){50,50,100,255});
            DrawText("Park Details", 578, 180, 18, SKYBLUE);
            if (sp) {
                DrawText(("Name     : "+sp->name).c_str(),578,210,17,WHITE);
                DrawText(("Location : ("+to_string(sp->x)+", "+to_string(sp->y)+")").c_str(),578,234,17,LIGHTGRAY);
                DrawText(("Capacity : "+to_string(sp->capacity)).c_str(),578,258,17,WHITE);
                int fr=sp->capacity-(int)sp->cars.size();
                DrawText(("Free     : "+to_string(fr)).c_str(),578,282,17,fr>0?(Color){80,220,100,255}:(Color){220,80,80,255});
                DrawText("Cars parked:", 578, 318, 17, LIGHTGRAY);
                if (sp->cars.empty()) DrawText("  (none)",578,342,16,GRAY);
                else for (int i=0;i<(int)sp->cars.size();i++)
                    DrawText(("  "+sp->cars[i]).c_str(),578,342+i*22,16,YELLOW);
            } else {
                DrawText("Select a park to see details", 578, 220, 16, GRAY);
            }

            // Draw dropdown LAST (on top of everything)
            DrawDropdown({220,184,300,38},parkNames,t1SelPark,t1DDOpen);
        }

        // ========== TAB 2: ADD / REMOVE PARK ==========
        else if (tab==2) {
            DrawText("Add / Remove Car Park", 30, 130, 22, SKYBLUE);
            DrawLine(30,156,SW-30,156,(Color){50,50,100,255});

            // Add section
            DrawRectangle(24,164,560,340,(Color){20,20,50,255});
            DrawRectangleLinesEx({24,164,560,340},2,(Color){50,50,100,255});
            DrawText("Add New Park", 44,180,18,SKYBLUE);
            DrawLine(44,202,550,202,(Color){40,40,90,255});

            DrawText("Park Name:", 44, 218, 17, LIGHTGRAY);
            DrawText("Location X:", 44, 264, 17, LIGHTGRAY);
            DrawText("Location Y:", 44, 310, 17, LIGHTGRAY);
            DrawText("Capacity :", 44, 356, 17, LIGHTGRAY);

            DrawInput({200,212,340,36},t2Name,t2AN,"e.g. ParkZ");
            DrawInput({200,258,120,36},t2X,t2AX,"e.g. 5");
            DrawInput({200,304,120,36},t2Y,t2AY,"e.g. -3");
            DrawInput({200,350,120,36},t2Cap,t2AC,"e.g. 4");

            if (DrawBtn({44,404,180,44},"Add Car Park",(Color){20,130,60,255})) {
                try {
                    int x=t2X.empty()?0:stoi(t2X), y=t2Y.empty()?0:stoi(t2Y), c=t2Cap.empty()?0:stoi(t2Cap);
                    statusMsg=ps.addCarPark(t2Name,x,y,c);
                    statusOK=statusMsg.find("added")!=string::npos;
                    if(statusOK){t2Name="";t2X="";t2Y="";t2Cap="";}
                } catch(...){ statusMsg="Error: X,Y,Capacity must be numbers!"; statusOK=false; }
            }

            // Remove section
            DrawRectangle(608,164,SW-632,340,(Color){20,20,50,255});
            DrawRectangleLinesEx({608,164,(float)(SW-632),340},2,(Color){50,50,100,255});
            DrawText("Remove Park", 628,180,18,SKYBLUE);
            DrawLine(628,202,SW-40,202,(Color){40,40,90,255});
            DrawText("Select Park:", 628, 218, 17, LIGHTGRAY);
            if (DrawBtn({628,390,200,44},"Remove Park",(Color){160,30,30,255})) {
                string rn=(t2SelRem>=0&&t2SelRem<(int)parkNames.size())?parkNames[t2SelRem]:"";
                statusMsg=ps.removeCarPark(rn);
                statusOK=statusMsg.find("removed")!=string::npos;
                if(statusOK) t2SelRem=-1;
            }
            // Dropdown last (on top)
            DrawDropdown({628,238,(float)(SW-668),38},parkNames,t2SelRem,t2DDOpen);
        }

        // ========== TAB 3: SHOW ALL ==========
        else if (tab==3) {
            DrawText("All Car Parks", 30, 130, 22, SKYBLUE);
            DrawLine(30,156,SW-30,156,(Color){50,50,100,255});

            // Header row
            DrawRectangle(24,162,SW-48,32,(Color){30,35,75,255});
            DrawText("Park",     40, 170, 16, (Color){150,180,255,255});
            DrawText("Location", 180,170, 16, (Color){150,180,255,255});
            DrawText("Cap",      310,170, 16, (Color){150,180,255,255});
            DrawText("Free",     380,170, 16, (Color){150,180,255,255});
            DrawText("Status",   460,170, 16, (Color){150,180,255,255});
            DrawText("Cars Parked", 580, 170, 16, (Color){150,180,255,255});
            DrawLine(24,194,SW-24,194,(Color){50,55,110,255});

            CarPark* t=ps.getHead(); int row=0;
            while(t) {
                int ry=198+row*40;
                if(ry+40>SH-50){DrawText("...",(int)40,ry,15,GRAY);break;}
                bool av=(int)t->cars.size()<t->capacity;
                if(row%2==0) DrawRectangle(24,ry,SW-48,40,(Color){22,22,55,255});
                DrawText(t->name.c_str(),40,ry+10,17,WHITE);
                string loc="("+to_string(t->x)+","+to_string(t->y)+")";
                DrawText(loc.c_str(),180,ry+10,17,LIGHTGRAY);
                DrawText(to_string(t->capacity).c_str(),318,ry+10,17,WHITE);
                int fr=t->capacity-(int)t->cars.size();
                DrawText(to_string(fr).c_str(),386,ry+10,17,fr>0?(Color){80,220,100,255}:(Color){220,80,80,255});
                DrawText(av?"OPEN":"FULL",462,ry+10,17,av?(Color){60,210,80,255}:(Color){220,60,60,255});
                string cl; for(int i=0;i<(int)t->cars.size();i++){if(i)cl+=", ";cl+=t->cars[i];}
                if(cl.empty())cl="-";
                DrawText(cl.c_str(),582,ry+10,15,YELLOW);
                t=t->next; row++;
            }
        }

        // ========== TAB 4: SEARCH ==========
        else if (tab==4) {
            DrawText("Search Nearest Available Park", 30, 130, 22, SKYBLUE);
            DrawLine(30,156,SW-30,156,(Color){50,50,100,255});

            DrawRectangle(24,164,400,160,(Color){20,20,50,255});
            DrawRectangleLinesEx({24,164,400,160},2,(Color){50,50,100,255});
            DrawText("Your X:", 44,190,18,LIGHTGRAY);
            DrawText("Your Y:", 44,240,18,LIGHTGRAY);
            DrawInput({160,184,200,36},t4X,t4AX,"e.g. 0");
            DrawInput({160,234,200,36},t4Y,t4AY,"e.g. 0");

            if (DrawBtn({44,300,200,44},"Search Nearest",(Color){30,80,200,255})) {
                try {
                    int x=t4X.empty()?0:stoi(t4X), y=t4Y.empty()?0:stoi(t4Y);
                    searchRes=ps.searchNearest(x,y);
                    statusOK=!searchRes.empty();
                    statusMsg=searchRes.empty()?"No available parking!":"Found "+to_string(searchRes.size())+" park(s).";
                } catch(...){ statusMsg="Error: X and Y must be numbers!"; statusOK=false; searchRes.clear(); }
            }

            if (!searchRes.empty()) {
                DrawRectangle(24,355,SW-48,32,(Color){30,35,75,255});
                DrawText("Rank",40,363,16,(Color){150,180,255,255});
                DrawText("Park Name",120,363,16,(Color){150,180,255,255});
                DrawText("Distance",320,363,16,(Color){150,180,255,255});
                DrawText("Free Slots",490,363,16,(Color){150,180,255,255});
                DrawLine(24,387,SW-24,387,(Color){50,55,110,255});

                for (int i=0;i<(int)searchRes.size()&&i<12;i++) {
                    int ry=391+i*38;
                    if(ry+38>SH-50) break;
                    if(i%2==0) DrawRectangle(24,ry,SW-48,38,(Color){22,22,55,255});
                    Color rc=(i==0)?(Color){255,215,0,255}:WHITE;
                    DrawText(to_string(i+1).c_str(),50,ry+10,17,rc);
                    DrawText(searchRes[i].park->name.c_str(),120,ry+10,17,rc);
                    string ds=to_string(searchRes[i].distance).substr(0,5);
                    DrawText(ds.c_str(),320,ry+10,17,LIGHTGRAY);
                    int fr=searchRes[i].park->capacity-(int)searchRes[i].park->cars.size();
                    DrawText(to_string(fr).c_str(),506,ry+10,17,(Color){80,220,100,255});
                }
            }
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
