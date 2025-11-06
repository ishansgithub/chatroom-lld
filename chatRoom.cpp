#include "chatRoom.hpp"

/**
 * Room class implementation
 * 
 * Manages participants in the chat room and broadcasts messages
 * to all connected clients except the sender.
 */

// Add a new participant to the room
void Room::join(ParticipantPointer participant){
    this->participants.insert(participant);
}

// Remove a participant from the room
void Room::leave(ParticipantPointer participant){
    this->participants.erase(participant);
}

// Broadcast a message to all participants except the sender
void Room::deliver(ParticipantPointer senderParticipant, Message &message){
    messageQueue.push_back(message);
    while (!messageQueue.empty()) {
        Message currentMessage = messageQueue.front();
        messageQueue.pop_front(); 
        
        // Send message to all participants except the sender
        for (ParticipantPointer currentParticipant : participants) {
            if (senderParticipant != currentParticipant) {
                currentParticipant->write(currentMessage);
            }
        }
    }
}

/**
 * Session class implementation
 * 
 * Handles all communication with a single client connection.
 * Reads messages from the client and broadcasts them to the room,
 * and also sends messages from other participants to this client.
 */

// Constructor: takes ownership of socket and reference to room
Session::Session(tcp::socket socket, Room& room): clientSocket(std::move(socket)), room(room){};

// Start the session: join room and begin reading messages
void Session::start(){
    room.join(shared_from_this());
    async_read();
}

// Asynchronously read a message from the client
void Session::async_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(clientSocket, buffer, "\n",
        [this, self](boost::system::error_code errorCode, std::size_t bytesTransferred) {
            if (!errorCode) {
                // Extract the received data from the buffer
                std::string messageData(boost::asio::buffers_begin(buffer.data()), 
                                 boost::asio::buffers_begin(buffer.data()) + bytesTransferred);
                buffer.consume(bytesTransferred);
                std::cout << "Received: " << messageData << std::endl;
                
                // Create message and deliver it to the room
                Message message(messageData);
                deliver(message);
                // Continue reading for next message
                async_read(); 
            } else {
                // Error occurred, remove this session from the room
                room.leave(shared_from_this());
                if (errorCode == boost::asio::error::eof) {
                    std::cout << "Connection closed by peer" << std::endl;
                } else {
                    std::cout << "Read error: " << errorCode.message() << std::endl;
                }
            }
        }
    );
}

// Asynchronously write a message to the client
void Session::async_write(std::string messageBody, size_t messageLength){
    auto writeHandler = [&](boost::system::error_code errorCode, std::size_t bytesTransferred){
        if(!errorCode){
            std::cout<<"Data is written to the socket: "<<std::endl;
        }else{
            std::cerr << "Write error: " << errorCode.message() << std::endl;
        }
    };
    boost::asio::async_write(clientSocket, boost::asio::buffer(messageBody, messageLength), writeHandler);
}

// Write a message to this client (called by room to send messages)
void Session::write(Message &message){
    messageQueue.push_back(message);
    while(messageQueue.size() != 0){
        Message currentMessage = messageQueue.front();
        messageQueue.pop_front();
        // Decode header to get body length
        bool headerDecoded = currentMessage.decodeHeader();
        if(headerDecoded){
            std::string body = currentMessage.getBody(); 
            async_write(body, currentMessage.getBodyLength());
        }else{
            std::cout<<"Message length exceeds the max length"<<std::endl;
        }
    }
}

// Deliver a message to the room (called when we receive from client)
void Session::deliver(Message& incomingMessage){
    room.deliver(shared_from_this(), incomingMessage);
}

/**
 * Server implementation
 * 
 * Handles accepting new client connections and managing the chat room.
 */

using boost::asio::ip::address_v4;

// Accept new client connections asynchronously
void accept_connection(boost::asio::io_context &ioContext, char *port, tcp::acceptor &acceptor, Room &room, const tcp::endpoint& endpoint) {
    tcp::socket socket(ioContext);
    acceptor.async_accept([&](boost::system::error_code errorCode, tcp::socket socket) {
        if(!errorCode) {
            // Create a new session for this client
            std::shared_ptr<Session> session = std::make_shared<Session>(std::move(socket), room);
            session->start();
        }
        // Continue accepting more connections
        accept_connection(ioContext, port, acceptor, room, endpoint);
    });
}

// Main server entry point
int main(int argc, char *argv[]) {
    try {
        if(argc < 2) {
            std::cerr << "Usage: server <port> [<port> ...]\n";
            return 1;
        }
        // Create room and set up networking
        Room room;
        boost::asio::io_context ioContext;
        tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
        tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), std::atoi(argv[1])));
        
        // Start accepting connections
        accept_connection(ioContext, argv[1], acceptor, room, endpoint);
        
        // Run the event loop
        ioContext.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}

