#include <iostream> 
// #include "core.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <atomic>
#include <boost/asio.hpp>
#include <core/io_context_pool.h>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, int id) : socket_(std::move(socket)) , id_(id) {}

        void start() {
            std::cout << "[Server] Handling session #" << id_
          << " on thread " << std::this_thread::get_id() << std::endl;
            do_read();
        }
    private:
        void do_read(){
            auto self = shared_from_this();
            socket_.async_read_some(boost::asio::buffer(buffer_),
                [this,self](boost::system::error_code ec, std::size_t length){
                    if (!ec){
                        std::cout << "Session id: " << id_ << std::endl;
                        std::cout << "recieved: " << std::string(buffer_.data(), length) << std::endl;
                        do_write(length);
                    }
                
                
                });
        }

        void do_write(std::size_t length){
            auto self = shared_from_this();
            boost::asio::async_write(socket_, boost::asio::buffer(buffer_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/){
                if (!ec){
                    do_read();
                }
            });

        }
        tcp::socket socket_;
        std::array<char, 1024> buffer_;
        int id_;
};


class Server {
    public:
        Server(boost::asio::io_context& io_context, short port )
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), connection_id_(0) {
            do_accept();
        }

        void do_accept() {
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


    private:
        tcp::acceptor acceptor_;
        std::atomic<int> connection_id_;
};











int main( ) {
   try{

        IoContextPool pool(4); // create a pool of 4 io_contexts
    
        auto& main_io_context = pool.get_io_context(); // get the main io_context
        
        Server server(main_io_context, 12345); // create a server instance on port 12345
        
        
        pool.run(); // run the io_context pool to start the server

        std::cout << "Server is running on port 12345" << std::endl;
        main_io_context.run(); // run the io_context to start accepting connections
    //use ;; because the loop is infinite and we want to keep the server running while accepting connections
      
   } catch (std::exception& e){
        std::cerr << "Exception : " << e.what() << std::endl;
   }
   return 0;

}