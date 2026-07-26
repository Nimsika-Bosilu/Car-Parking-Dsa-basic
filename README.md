# 🚗 Smart Car Park System (DSA Project) -v2

Welcome to the **Smart Car Park System**! This is a beginner-friendly C++ project created to demonstrate how Data Structures and Algorithms (DSA) can be used to solve real-world problems.

This application helps users find the closest available parking lot in a city and lets them park or remove their cars. It features a beautiful **Graphical User Interface (GUI)** that shows a visual map of the parking lots!

---

## 🧠 How it Works (For Beginners!)

Behind the scenes, this project uses two main concepts from Computer Science to work efficiently:

### 1. Linked Lists (The "Chain" of Parking Lots)
Instead of using a fixed array (where you have to guess exactly how many parking lots you will ever need), this project uses a **Singly Linked List**. 
* Think of a Linked List like a **treasure hunt**. 
* The system starts at the first parking lot (the `head`). 
* If it needs to find a different parking lot, it follows a "pointer" (a clue) to the next parking lot, and then the next, until it finds the one it's looking for! 
* **Why this is good:** We can easily add new parking lots to the city at any time without running out of memory!

### 2. The Distance Formula (Finding the Closest Park)
When you type in your `X` and `Y` location, the system needs to find the nearest parking lot that isn't full.
* It uses the math formula for a straight line: `Distance = √( (X2 - X1)² + (Y2 - Y1)² )`.
* It calculates the distance to *every* available parking lot, puts those distances into a list (a `vector`), and then **Sorts** them from smallest to largest so you always get the closest one.

---

## ✨ Features
* **Visual City Map:** View all parking lots as green (available) or red (full) circles on a grid.
* **Park & Track Cars:** Enter your license plate to park your car, and easily find out which lot you parked in later.
* **Smart Search:** Automatically calculates the closest available parking space to your location.
* **Dynamic Memory:** Because it uses Linked Lists, it's very lightweight and memory-efficient.

---

## 🎮 How to Use the App

1. **Viewing the Map:** When you open the application, you'll see a city grid.
2. **Finding a Park:** Click on the **"Search Parks"** tab. Enter your current `X` and `Y` coordinates (e.g., `0` and `0`) and click **"Search Nearest"**. The system will calculate and show you the closest available parking lot!
3. **Parking a Car:** Click on the **"Manage Cars"** tab. Enter the name of the closest park (e.g., `ParkE`) and your license plate (e.g., `CAR-1234`). Click **"Park Car"**. 
4. **Verifying:** Go back to the **"View Map"** tab. You'll see the capacity of that parking lot update in real-time!
5. **Removing a Car:** When you leave, go back to **"Manage Cars"**, enter your license plate, and click **"Remove Car"** to free up the space.

---

## 🚀 How to Run the Code

### For Beginners (Running the Graphical UI)
We have made it extremely easy to run the beautiful UI version of this app if you are on Windows using MinGW.
1. Just double-click the `build_raylib.bat` file in your folder!
2. The script will automatically download the graphics library (Raylib), compile the C++ code for you, and open the app.

### For Advanced Users (Console Version)
If you just want to run the raw text-based terminal version without graphics:
```bash
g++ Car.c++ -o Car
./Car
```

---

## 📁 What are these files?
* `Car.c++`: The original, logic-heavy text-based version of the system.
* `Car_Raylib.cpp`: The exact same logic, but upgraded to use a Graphical User Interface!
* `build_raylib.bat`: A helpful script that builds the graphical version automatically.
* `.gitignore`: A file that tells Git to ignore compiled `.exe` files when uploading to GitHub.
