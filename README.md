# 🚗 Smart Car Park System (DSA Project)

A Data Structures and Algorithms (DSA) based C++ application to manage and visualize a Smart Car Park System. 

This project uses **Linked Lists** to dynamically manage multiple parking lots across a city, keeping track of their locations, capacities, and currently parked vehicles. It features both a traditional Console CLI and a beautiful **Graphical User Interface (GUI)** powered by Raylib.

## ✨ Features
* **Dynamic Memory Management:** Uses Linked Lists to store unlimited parking lots without fixed array constraints.
* **Nearest Neighbor Search:** Calculates the Euclidean distance to find the closest available parking lot to a user's `(X, Y)` coordinates.
* **Real-time Status Tracking:** Easily check how many spots are left in any parking lot.
* **Graphical 2D Map Visualization:** View the city grid and parking lots interactively.
* **Car Tracking:** Park cars with license plates, remove them, or search for a specific car's location.

## 🛠️ Data Structures Used
* **Linked List:** The core `ParkingSystem` uses a custom Singly-Linked List of `CarPark` structs.
* **Vectors & Sorting:** Uses `std::vector` to collect nearby parks and sorts them by shortest distance.

## 🚀 Getting Started

### Prerequisites
* Windows OS
* MinGW (GCC Compiler) installed and added to PATH.

### Running the Graphical UI (Recommended)
We have included an automated script that downloads the required UI libraries and compiles the application for you!
1. Double-click the `build_raylib.bat` file in Windows Explorer.
2. The script will automatically fetch Raylib, compile the `Car_Raylib.cpp` file, and launch the Smart Car Park Dashboard.

### Running the Standard Console Version
If you prefer the original terminal text-based version:
```bash
g++ Car.c++ -o Car
./Car
```

## 🎮 Example Usage Flow (Using the GUI)

1. **Viewing the Map:** When you open the application, you'll see a city grid with all car parks. Green circles indicate available parks, and red circles indicate full parks.
2. **Finding a Park:** Click on the **"Search Parks"** tab. Enter your current `X` and `Y` coordinates (e.g., `0` and `0`) and click **"Search Nearest"**. The system will calculate and show you the closest available parking lot!
3. **Parking a Car:** Click on the **"Manage Cars"** tab. Enter the name of the closest park (e.g., `ParkE`) and your license plate (e.g., `CAR-1234`). Click **"Park Car"**. 
4. **Verifying:** Go back to the **"View Map"** tab. You'll see the capacity of that parking lot update in real-time!
5. **Removing a Car:** When you leave, go back to **"Manage Cars"**, enter your license plate, and click **"Remove Car"** to free up the space.

## 📁 Project Structure
* `Car.c++`: Original console-based implementation of the Smart Car Park System.
* `Car_Raylib.cpp`: The modern Graphical UI implementation using Raylib.
* `Car_FTXUI.cpp`: An advanced Terminal UI implementation.
* `build_raylib.bat`: Automated build script for MinGW users.

## 👨‍💻 Developed By
Created as a Data Structures & Algorithms project.
