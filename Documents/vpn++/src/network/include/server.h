#pragma once
#include <iostream> 
// #include "core.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <atomic>
#include <boost/asio.hpp>
#include <thread>
#include "./protocol/include/packet.hpp"
#include "session.h"

using boost::asio::ip::tcp;

class Server {
    public:
        Server(boost::asio::io_context& io_context, short port );

        void do_accept();


    private:
        tcp::acceptor acceptor_;
        std::atomic<int> connection_id_;
};
