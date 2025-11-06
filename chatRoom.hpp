#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include "message.hpp"
#include <deque>
#include <set>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/**
 * Participant interface for chat room members
 * 
 * All participants (sessions) must implement these methods to
 * send and receive messages in the chat room.
 */
class Participant {
    public: 
        // Deliver a message to this participant (for receiving)
        virtual void deliver(Message& message) = 0;
        // Write a message from this participant (for sending)
        virtual void write(Message &message) = 0;
        virtual ~Participant() = default;
};

// Smart pointer type for managing participant lifetime
typedef std::shared_ptr<Participant> ParticipantPointer;

/**
 * Room class manages all connected participants in the chat room
 * 
 * Handles joining/leaving participants and broadcasting messages
 * to all connected clients except the sender.
 */
class Room{
    public:
        // Add a new participant to the room
        void join(ParticipantPointer participant);
        // Remove a participant from the room
        void leave(ParticipantPointer participant);
        // Broadcast a message to all participants except the sender
        void deliver(ParticipantPointer senderParticipant, Message &message);
    private:
        // Queue for pending messages (though we process immediately)
        std::deque<Message> messageQueue;
        // Maximum number of participants allowed in the room
        enum {maxParticipants = 100};
        // Set of all active participants in the room
        std::set<ParticipantPointer> participants;
};

/**
 * Session class represents a single client connection
 * 
 * Each client that connects gets a Session instance that handles
 * all communication with that specific client. Sessions read messages
 * from the client and broadcast them to the room, and also receive
 * messages from other participants to send back to the client.
 */
class Session: public Participant, public std::enable_shared_from_this<Session>{
    public:
        // Constructor: takes ownership of the socket and reference to room
        Session(tcp::socket socket, Room &room);
        // Start the session: join room and begin reading messages
        void start();
        // Deliver a message to the room (called when we receive from client)
        void deliver(Message& message);
        // Write a message to this client (called by room to send messages)
        void write(Message &message);
        // Asynchronously read a message from the client
        void async_read();
        // Asynchronously write a message to the client
        void async_write(std::string messageBody, size_t messageLength);
    private:
        // TCP socket for this client connection
        tcp::socket clientSocket;
        // Buffer for reading incoming data
        boost::asio::streambuf buffer;
        // Reference to the room this session belongs to
        Room& room;
        // Queue of messages waiting to be sent to this client
        std::deque<Message> messageQueue; 
};

#endif //CHATROOM_HPP

