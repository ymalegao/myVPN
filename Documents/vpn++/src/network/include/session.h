#pragma once
#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <thread>
#include "./protocol/include/packet.hpp"
#include <string>
#include <vector>


using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {

    public:
        Session(tcp::socket socket, int id);

        void start();
    private:
        void do_read();

        void do_write(const std::vector<uint8_t>& response_data);
        tcp::socket socket_;
        std::array<uint8_t, 4> length_buffer_;
        std::vector<uint8_t> payload_buffer_;
        int id_;
};