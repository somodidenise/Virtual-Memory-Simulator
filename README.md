# 🧠 Virtual Memory Simulator

A C++ and Qt-based interactive application that simulates virtual memory management in an operating system. It visualizes complex memory operations, including page replacement policies, address translation, and memory allocation. This project was developed as part of an academic assignment to deepen understanding of memory management systems.


## 🚀 Features

- Simulates **virtual memory management** with **page replacement policies** (FIFO, LRU).
- Interactive visualization of:
  - **Virtual Address Space**
  - **Page Table**
  - **Translation Lookaside Buffer (TLB)**
  - **Physical Memory**
- Step-by-step address translation with detailed logging.
- Read/write memory operations with user-defined addresses and data.
- Dynamic GUI developed using **Qt Framework**.


## 🛠️ Technologies Used

- **C++** for core logic and memory management.
- **Qt Framework** for GUI (widgets, signals/slots).
- **OOP principles** and modular design.


## 📂 Folder Structure
```
/VirtualMemorySimulator 
│── main.cpp 
│── PageTable.cpp / PageTable.h 
│── PhysicalMemory.cpp / PhysicalMemory.h 
│── TLB.cpp / TLB.h 
│── VirtualAddressSpace.cpp / VirtualAddressSpace.h 
│── VirtualMemoryManager.cpp / VirtualMemoryManager.h 
│── VirtualMemorySimulator.cpp / VirtualMemorySimulator.h 
│── README.md 
```

## 🖥️ How It Works

1. **The user selects:**
   - RAM size
   - Page size
   - Replacement policy (FIFO, LRU)

2. **The system generates:**
   - Virtual and physical memory tables
   - TLB and page tables

3. **Users can:**
   - Perform **read/write** operations on virtual addresses
   - Step through address translation and page loading into physical memory
   - Visualize page faults, TLB hits/misses, and page replacements

## ✅ Setup Instructions

1. **Prerequisites**:
   - C++17 or newer
   - Qt5/Qt6 installed and configured (Qt Widgets module)
   - CMake (optional) or your favorite IDE (CLion, Qt Creator)

2. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/VirtualMemorySimulator.git
   cd VirtualMemorySimulator

3. **Open the project in Qt Creator or your preferred C++ IDE.**

4. **Build and Run**:
   - Configure the build kit (compiler + Qt version)
   - Build the project
   - Run the executable

## ✨ Screenshots
![Virtual Memory Table](screenshots/virtual_memory.png)

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
