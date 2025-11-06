#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>

/**
 * Message class for handling chat room messages
 * 
 * Messages are structured with a 4-byte header that stores the body length,
 * followed by the actual message body (max 512 bytes). This allows the
 * receiver to know how much data to read before processing the message.
 */
class Message {
    public: 
        // Default constructor - creates empty message
        Message() : bodyLength(0) {}
        
        // Maximum message body size in bytes
        static constexpr size_t maxMessageBytes = 512;
        // Header size in bytes (stores body length as 4-digit number)
        static constexpr size_t headerLength = 4;

        /**
         * Constructor that creates a message from a string
         * Automatically encodes the header with the body length
         */
        Message(std::string message){
            bodyLength = getNewBodyLength(message.size());
            encodeHeader();
            // Copy message body after the header
            std::memcpy(messageData + headerLength, message.c_str(), bodyLength);
        };

        /**
         * Prints the received message to console
         * Useful for debugging and server-side logging
         */
        void printMessage(){
            std::string message = getData();
            std::cout<<"Message recieved: "<<message<<std::endl;
        }

        /**
         * Returns the complete message data (header + body) as a string
         * This is what gets sent over the network
         */
        std::string getData(){
            int length = headerLength + bodyLength;
            std::string result(messageData, length);
            return result;
        }

        /**
         * Extracts and returns only the message body (without header)
         * This is what the user actually typed
         */
        std::string getBody(){
            std::string dataString = getData();
            // Skip the header and extract just the body
            std::string result = dataString.substr(headerLength, bodyLength);
            return result;
        }

        /**
         * Validates and returns the body length
         * If message exceeds max size, truncate to maxMessageBytes
         */
        size_t getNewBodyLength(size_t newLength){
            if(newLength > maxMessageBytes){
                return maxMessageBytes;
            }
            return newLength;
        }

        /**
         * Encodes the body length into the 4-byte header
         * Header format: 4-digit number (e.g., "0050" for 50 bytes)
         */
        void encodeHeader(){
            char newHeader[headerLength+1] = "";
            // Format as 4-digit number with leading zeros
            sprintf(newHeader, "%4d", static_cast<int>(bodyLength));
            memcpy(messageData, newHeader, headerLength);
        }
        
        /**
         * Decodes the header to extract body length
         * Returns false if header value is invalid (exceeds max size)
         */
       bool decodeHeader(){
            char newHeader[headerLength+1] = "";
            strncpy(newHeader, messageData, headerLength);
            newHeader[headerLength] = '\0'; // Null terminate for atoi
            int headerValue = atoi(newHeader);
            
            // Safety check: reject messages that claim to be too large or negative
            if(headerValue < 0 || static_cast<size_t>(headerValue) > maxMessageBytes){
                bodyLength = 0;
                return false;
            }
            bodyLength = static_cast<size_t>(headerValue);
            return true;
        }

        /**
         * Returns the current body length
         */
        size_t getBodyLength(){
            return bodyLength;
        }

    private: 
        // Buffer to store header + message body
        // Size: 4 bytes (header) + 512 bytes (max body) = 516 bytes total
        char messageData[headerLength+maxMessageBytes];
        // Current length of the message body (not including header)
        size_t bodyLength;
};

#endif // MESSAGE_HPP

