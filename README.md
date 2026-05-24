# Network Traffic Monitor

## Description

A C++ application for real-time network traffic monitoring. Users authenticate through a login system and access a dashboard where they can start/stop live packet capture on the active network interface and inspect captured packets. Packets are asynchronously collected into a thread-safe queue and analyzed layer by layer.

Built with [PcapPlusPlus](https://github.com/seladb/PcapPlusPlus) for packet capture and parsing.

---

## Assignment 1

### Classes

| Class | Description |
|---|---|
| `Panel` | Base panel class — holds display logic and menu state |
| `User` | Stores authenticated user data (username, role, session info) |
| `Network` | Manages the network interface and controls packet capture |
| `NetworkPacket` | Represents a single captured packet with parsed layer data |
| `SafeQueue` | Thread-safe queue using mutex and condition variable |

### Non-trivial public member functions

- `Network::start_capture()` — opens the active interface and begins live packet interception in a background thread
- `Network::stop_capture()` — signals the capture thread to stop and joins it cleanly
- `NetworkPacket::parse()` — walks through protocol layers (Ethernet, IP, TCP/UDP) and extracts relevant fields
- `SafeQueue::push()` / `SafeQueue::pop()` — thread-safe enqueue/dequeue with blocking wait on empty queue

### Usage scenario (`main`)

```
Login → load users from users.json → authenticate →
Dashboard → start traffic monitor → packets pushed to SafeQueue →
check packets → display parsed output → stop monitor → exit
```

---

## Assignment 2

### Inheritance hierarchy

Base class `Panel` with 3 derived classes:

| Class | Derives from | Description |
|---|---|---|
| `Login` | `Panel` | Singleton; handles authentication and user loading |
| `Dashboard` | `Panel` | Main panel; owns the traffic monitor and packet queue |
| `CmdExec` | `Panel` | 4th derived class added in a separate commit; executes commands on captured data |

User hierarchy — base class `User` with derived classes:

| Class | Derives from | Description |
|---|---|---|
| `BasicUser` | `User` | Standard authenticated user |
| `Admin` | `User` | Privileged user with extended access |

### Requirements implemented

- **`.h` / `.cpp` separation** — all classes split across `include/` and `src/`
- **Pure virtual functions** called through base pointers; at least one is theme-specific (not a simple display)
- **Virtual display** via non-virtual interface in `Panel`
- **Virtual constructors (clone)** implemented in the `Panel` hierarchy
- **`Dashboard`** holds a `std::unique_ptr<Panel>` and calls virtual functions through it
- **Copy-and-swap** — correct copy/assignment where needed
- **`dynamic_cast`** — used for meaningful downcast in `dashboard_panel`
- **Exceptions** — custom hierarchy derived from `std::exception` (`include/Exceptions.h`); 3+ distinct error classes; `throw` in constructors/functions, `try`/`catch` in `main`
- **`static`** — `Login` implemented as a Singleton with a static instance
- **STL** — used for user collections, packet queues, interface enumeration
- **`const`** — applied broadly on functions and parameters

---

## Assignment 3

### Design patterns

- **Singleton** — `Login` class; single instance accessible via `Login::getInstance()`
- **Observer / Producer-Consumer** — `Network` produces packets and pushes them into `SafeQueue`; `Dashboard` consumes them on demand

### Template class

`SafeQueue<T>` — a generic thread-safe queue; instantiated with:
- `SafeQueue<NetworkPacket>` — for captured network packets
- `SafeQueue<std::string>` — for log/command output strings

### Requirements implemented

- **2 design patterns** with meaningful use in the application flow
- **Template class** with at least 2 instantiations
- **C++ code** ≥ 80–90% of own code

---

## Class Overview

| Class | Header | Source | Role |
|---|---|---|---|
| `Panel` | `include/panel.h` | `src/panel.cpp` | Abstract base for UI panels |
| `Login` | `include/login_panel.h` | `src/login_panel.cpp` | Auth + Singleton |
| `Dashboard` | `include/dashboard_panel.h` | `src/dashboard_panel.cpp` | Main control panel |
| `CmdExec` | `include/cmd_exec.h` | — | 4th derived panel |
| `User` | `include/user.h` | `src/user.cpp` | Base user class |
| `BasicUser` | `include/basic_user.h` | — | Standard user |
| `Admin` | `include/admin.h` | — | Privileged user |
| `Network` | `include/network.h` | `src/network.cpp` | Packet capture engine |
| `NetworkPacket` | `include/network_packet.h` | `src/network_packet.cpp` | Packet representation |
| `SafeQueue<T>` | `include/safe_queue.h` | `src/safe_queue.cpp` | Thread-safe generic queue |

---

## System Requirements

- **C++23** (GCC, Clang, or MSVC)
- **CMake** ≥ 3.26
- **libpcap** (Linux/macOS) or **Npcap** (Windows)
- **Git** (PcapPlusPlus fetched automatically via FetchContent)

---

## Build Instructions

### Linux / macOS

```bash
# Install libpcap if not already present
sudo apt install libpcap-dev   # Ubuntu/Debian
brew install libpcap            # macOS

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 6
```

### Windows

Install [Npcap](https://npcap.com/) (with SDK), then:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -G Ninja
cmake --build build --parallel 6
```

### With AddressSanitizer (Linux/macOS)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON
cmake --build build --parallel 6
```

---

## Running

```bash
./build/oop
```

On macOS a launcher script is also available:

```bash
./launcher.command
```

For non-interactive runs (e.g. with Valgrind), input is read from `tastatura.txt`:

```bash
./scripts/run_valgrind.sh
# or interactively:
RUN_INTERACTIVE=true ./scripts/run_valgrind.sh
```

---

## Usage

After logging in, the dashboard presents:

```
1. Start Traffic Monitor   — starts live packet capture on the active interface
2. Stop Traffic Monitor    — stops the capture
3. Check Packets           — displays packets collected from the queue
0. Exit
```

User credentials are persisted in `users.json` and loaded at startup.
```admin:admin``` for the admin user.
---

## Project Structure

```
fan_courier/
├── src/                   # .cpp implementations
├── include/               # .h headers
├── assets/                # tastatura.txt and other runtime assets
├── ext/                   # header-only external dependencies
├── cmake/                 # CMake helper modules
├── scripts/               # build/run/valgrind scripts
├── .github/               # CI (GitHub Actions)
├── main.cpp
├── CMakeLists.txt
└── users.json
```

---


## License

Licensed under [AGPLv3](LICENSE).  
Base template: [Unlicense](LICENSE.template).
