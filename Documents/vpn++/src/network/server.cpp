#include "server.h"
#include <iostream>

Server::Server(boost::asio::io_context& io_context, short port ): acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), connection_id_(0) 
    { 
        do_accept();

    }

void Server::do_accept() {
            acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket_){
                    if (!ec) {
                        int id = connection_id_++;
                        std::cout << "Accepted connection with id: " << id << std::endl;
                        std::make_shared<Session>(std::move(socket_), id)->start();
                    }
                    do_accept(); // accept the next connection
                }
            );

        }
