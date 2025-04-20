#include "session.h"
#include <iostream>
#include <stdio.h>
#include <string>

Session::Session(tcp::socket socket, int id): socket_(std::move(socket)), id_(id) {
    std::cout << "Session created with id: " << id_ << std::endl;
}

void Session::start() {
    std::cout << "Starting session with id: " << id_ << std::endl;
    do_read();
}

void Session::do_read(){
    auto self = shared_from_this();
    boost::asio::async_read(socket_, boost::asio::buffer(length_buffer_),
        [this, self](boost::system::error_code ec, std::size_t /*length*/){
            if (!ec) {
                uint32_t len_net;
                std::memcpy(&len_net, length_buffer_.data(), 4);
                uint32_t payload_size = ntohl(len_net);
                std::cout << "Received length: " << payload_size << " in session " << id_ << std::endl;
                
                payload_buffer_.resize(payload_size);

                //now that we read the header of length 4 bytes, we can read the payload
                boost::asio::async_read(socket_, boost::asio::buffer(payload_buffer_),
                    [this, self](boost::system::error_code ec2, std::size_t /*read_len*/){
                        if (!ec2) {
                            // Process the received data
                            Packet packet_ = Packet::from_bytes(payload_buffer_);
                            std::cout << "Packet type: " << static_cast<int>(packet_.type()) << std::endl;
                            std::cout << "Packet payload: " << std::string(packet_.payload().begin(), packet_.payload().end()) << std::endl;
                            // Echo the data back to the client
                            do_write(payload_buffer_); // Send the data back to the client
                        }
                });
            }
    });
    
}

void Session::do_write(const std::vector<uint8_t>& response_data) {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(response_data),
        [this, self](boost::system::error_code ec, std::size_t /*length*/){
            if (!ec) {
                // Send the data back to the client

                std::cout << "Sent data back to client in session " << id_ << std::endl;
                do_read(); // Continue reading from the socket
            }
    });
}