#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>

/* 
headerLength is of 4 bytes and maxMessageBytes can be stored as 512 bytes
headerLength stores the body length that is the current body length
messageData stores the headerLength+bodyLength with maximum size of headerLength+maxMessageBytes

client attempts to send message:- It will encode header and put message into the messageData and send messageData
server gets the message, decodes the header, get the bodylength from the header and hence complete body
then server sends the message to all the clients connected to that room.
*/


class Message {
    public: 
        Message() : bodyLength(0) {}
        
        enum {maxMessageBytes = 512};
        enum {headerLength = 4};

        Message(std::string message){
            bodyLength = getNewBodyLength(message.size());
            encodeHeader();
            std::memcpy(messageData + headerLength, message.c_str(), bodyLength);
        };

        void printMessage(){
            std::string message = getData();
            std::cout<<"Message recieved: "<<message<<std::endl;
        }

        std::string getData(){
            int length = headerLength + bodyLength;
            std::string result(messageData, length);
            return result;
        }

        std::string getBody(){
            std::string dataString = getData();
            std::string result = dataString.substr(headerLength, bodyLength);
            return result;
        }

        size_t getNewBodyLength(size_t newLength){
            if(newLength > maxMessageBytes){
                return maxMessageBytes;
            }
            return newLength;
        }

        void encodeHeader(){
            char newHeader[headerLength+1] = "";
            sprintf(newHeader, "%4d", static_cast<int>(bodyLength));
            memcpy(messageData, newHeader, headerLength);
        }
        
       bool decodeHeader(){
            char newHeader[headerLength+1] = "";
            strncpy(newHeader, messageData, headerLength);
            newHeader[headerLength] = '\0';
            int headerValue = atoi(newHeader);
            if(headerValue > maxMessageBytes){
                bodyLength = 0;
                return false;
            }
            bodyLength = headerValue;
            return true;
        }

        size_t getBodyLength(){
            return bodyLength;
        }

    private: 
        char messageData[headerLength+maxMessageBytes];
        size_t bodyLength;
};

#endif MESSAGE_HPP

