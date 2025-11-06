#include "message.hpp"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/**
 * Client implementation for chat room
 * 
 * Connects to the server and handles bidirectional communication:
 * - Reads messages from server asynchronously
 * - Sends user input to server in a separate thread
 */

// Asynchronously read messages from the server
void async_read(tcp::socket &socket) {
    auto buffer = std::make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(socket, *buffer, "\n",
        [&socket, buffer](boost::system::error_code errorCode, std::size_t /* bytesTransferred */) {
            if (!errorCode) {
                // Extract the received message from the buffer
                std::istream inputStream(buffer.get());
                std::string receivedMessage;
                std::getline(inputStream, receivedMessage);
                std::cout << "Server: " << receivedMessage << std::endl;
                // Continue reading for next message
                async_read(socket); 
            }
        }
    );
}

// Main client entry point
int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Provide port too as second argument" << std::endl;
        return 1;
    }
    
    // Set up networking components
    boost::asio::io_context ioContext;
    tcp::socket socket(ioContext);
    tcp::resolver resolver(ioContext);

    // Connect to the server
    boost::asio::connect(socket, resolver.resolve("127.0.0.1", argv[1]));

    // Start reading messages from server
    async_read(socket);

    // Create a separate thread for user input
    std::thread inputThread([&ioContext, &socket]() {
        while (true) {
            std::string userInput;
            std::cout << "Enter message: ";
            std::getline(std::cin, userInput);
            userInput += "\n";

            // Post the write operation to the io_context thread
            boost::asio::post(ioContext, [&, userInput]() {
                boost::asio::write(socket, boost::asio::buffer(userInput));
            });
        }
    });

    // Run the event loop
    ioContext.run();
    inputThread.join();

    return 0;
}

