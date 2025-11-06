# chatroom-lld

A high-performance, asynchronous chat room application built with C++ and Boost.Asio. This project demonstrates modern C++ networking patterns, allowing multiple clients to connect to a server and exchange messages in real-time.

## 🚀 Features

- **Asynchronous I/O**: Uses Boost.Asio for non-blocking network operations, enabling the server to handle multiple clients concurrently without blocking
- **Real-time Messaging**: Instant message delivery to all connected clients
- **Scalable Architecture**: Supports up to 100 concurrent participants per room
- **Thread-safe Design**: Proper use of shared pointers and async patterns for safe concurrent access
- **Message Protocol**: Custom message encoding with header-based length detection
- **Clean Code**: Well-documented codebase with camelCase naming conventions

## 📋 Prerequisites

- **C++20** compatible compiler (GCC 10+ or Clang 12+)
- **Boost Libraries** (version 1.70 or higher)
  - `libboost-system-dev`
  - `libboost-thread-dev`
- **Make** build system
- **Linux/Unix** environment (tested on Ubuntu)

### Installing Boost Libraries

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libboost-system-dev libboost-thread-dev
```

**macOS (using Homebrew):**
```bash
brew install boost
```

**Fedora/RHEL:**
```bash
sudo dnf install boost-devel
```

## 🏗️ Architecture

The application follows a client-server architecture with the following key components:

### Core Components

#### 1. **Message Class** (`message.hpp`)
- Handles message encoding and decoding
- 4-byte header stores message body length
- Maximum message size: 512 bytes
- Provides methods for header encoding/decoding and body extraction

#### 2. **Participant Interface** (`chatRoom.hpp`)
- Abstract base class for all chat room participants
- Defines interface for message delivery and writing

#### 3. **Room Class** (`chatRoom.hpp`, `chatRoom.cpp`)
- Manages all connected participants
- Handles joining/leaving participants
- Broadcasts messages to all participants except the sender
- Supports up to 100 concurrent participants

#### 4. **Session Class** (`chatRoom.hpp`, `chatRoom.cpp`)
- Represents a single client connection
- Handles asynchronous read/write operations
- Manages message queue for each client
- Implements the Participant interface

#### 5. **Server** (`chatRoom.cpp`)
- Accepts incoming client connections
- Creates Session instances for each client
- Manages the chat room lifecycle

#### 6. **Client** (`client.cpp`)
- Connects to the server
- Handles user input in a separate thread
- Asynchronously receives and displays messages from the server

### Message Flow

1. **Client sends message:**
   - User types message → Client thread captures input
   - Message posted to I/O context → Written to socket

2. **Server receives message:**
   - Session reads from socket → Creates Message object
   - Message delivered to Room → Room broadcasts to all participants

3. **Server sends to clients:**
   - Room calls `write()` on each Session (except sender)
   - Session queues message → Asynchronously writes to client socket

4. **Client receives message:**
   - Client reads from socket → Displays message to user

## 📦 Building the Project

### Clone the Repository
```bash
git clone <repository-url>
cd chatRoomCpp-main
```

### Build
```bash
make
```

This will create two executables:
- `chatApp` - The chat server
- `clientApp` - The chat client

### Clean Build Artifacts
```bash
make clean
```

## 🎮 Usage

### Starting the Server

```bash
./chatApp <port>
```

**Example:**
```bash
./chatApp 9099
```

The server will start listening on the specified port and wait for client connections.

### Connecting a Client

In a separate terminal, run:

```bash
./clientApp <port>
```

**Example:**
```bash
./clientApp 9099
```

### Using the Chat Room

1. Start the server on a port (e.g., `9099`)
2. Connect multiple clients to the same port
3. Type messages in any client terminal
4. Messages will be broadcast to all other connected clients
5. Press `Ctrl+C` to disconnect

**Example Session:**
```bash
# Terminal 1 - Server
$ ./chatApp 9099
Received: Hello everyone!

# Terminal 2 - Client 1
$ ./clientApp 9099
Enter message: Hello everyone!
Server: Hi there!

# Terminal 3 - Client 2
$ ./clientApp 9099
Server: Hello everyone!
Enter message: Hi there!
```

## 📁 Project Structure

```
chatRoomCpp-main/
├── chatRoom.hpp          # Header file with Room, Session, and Participant classes
├── chatRoom.cpp          # Server implementation and Room/Session logic
├── client.cpp            # Client application implementation
├── message.hpp           # Message class with encoding/decoding
├── Makefile              # Build configuration
├── README.md             # This file
├── .github/
│   └── workflows/
│       └── c-cpp.yml     # GitHub Actions CI/CD workflow
└── old/                  # Original files (for reference)
    ├── chatRoom.hpp
    ├── chatRoom.cpp
    ├── client.cpp
    ├── message.hpp
    └── Makefile
```

## 🔧 Technical Details

### Message Protocol

Messages are structured as follows:
- **Header (4 bytes)**: ASCII-encoded decimal number representing body length (e.g., "0050" for 50 bytes)
- **Body (up to 512 bytes)**: The actual message content

**Example:**
```
Header: "0025" (25 bytes)
Body:   "Hello, this is a message!"
Total:  29 bytes
```

### Threading Model

**Server:**
- Single I/O thread running `io_context.run()`
- All async operations handled by the I/O context
- No explicit threading needed (Boost.Asio handles concurrency)

**Client:**
- Main thread: Runs I/O context for async operations
- Input thread: Captures user input and posts writes to I/O context
- Thread-safe communication via `boost::asio::post()`

### Error Handling

- Connection errors are caught and logged
- Invalid messages are rejected (header validation)
- Client disconnections are handled gracefully
- Server continues running after client disconnects

## 🧪 Testing

### Manual Testing

1. Start the server:
   ```bash
   ./chatApp 9099
   ```

2. Connect multiple clients in separate terminals:
   ```bash
   ./clientApp 9099
   ```

3. Send messages from different clients and verify they appear in all other clients

### CI/CD

The project includes a GitHub Actions workflow (`.github/workflows/c-cpp.yml`) that:
- Builds the project on every push and pull request
- Installs required dependencies
- Verifies successful compilation

## 🛠️ Development

### Code Style

- **Naming Convention**: camelCase for variables and functions
- **Comments**: Comprehensive documentation for classes and methods
- **Standards**: C++20 with `-Wall -Wextra` warnings enabled

### Key Design Patterns

- **RAII**: Automatic resource management with smart pointers
- **Async/Await Pattern**: Boost.Asio async operations with completion handlers
- **Observer Pattern**: Room notifies all participants of new messages
- **Factory Pattern**: Server creates Session instances for new connections

## 📝 Limitations

- Maximum message size: 512 bytes
- Maximum participants per room: 100
- No authentication or user identification
- No message history persistence
- No private messaging (all messages are broadcast)

## 🚧 Future Enhancements

- [ ] User authentication and identification
- [ ] Private messaging between users
- [ ] Message history and persistence
- [ ] Multiple chat rooms
- [ ] File transfer support
- [ ] Encrypted communication
- [ ] WebSocket support for web clients

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is open source and available under the MIT License.

## 🙏 Acknowledgments

- Built with [Boost.Asio](https://www.boost.org/doc/libs/1_82_0/doc/html/boost_asio.html) for asynchronous networking
- Inspired by modern C++ networking patterns

## 📧 Contact

For questions or suggestions, please open an issue on GitHub.

---

**Happy Chatting! 💬**

