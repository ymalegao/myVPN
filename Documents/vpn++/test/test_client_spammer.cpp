#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <string>


using boost::asio::ip::tcp;

void run_client(const std::string& host, int id, short port){
    try{
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);

        socket.connect(tcp::endpoint(boost::asio::ip::make_address(host), port));
        std::string message = "Hello from client " + std::to_string(id);
        boost::asio::write(socket, boost::asio::buffer(message));

        std::vector<char> buffer(1024);
        size_t len = socket.read_some(boost::asio::buffer(buffer));

        std::string reply(buffer.begin(),  buffer.begin() + len);
        std::cout << "Client " << id << " received: " << reply << std::endl;
    } catch (std::exception& e){
        std::cerr << "Client error: " << e.what() << std::endl;

    }
}

int main(){
    const int num_clients = 100;
    const std::string host = "127.0.0.1";
    const short port = 12345;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_clients; ++i){
        threads.emplace_back(run_client, host, i, port);
    }
    for (auto& thread : threads){
        if (thread.joinable()){
            thread.join();
        }
    }

    std::cout << "All clients finished." << std::endl;
    return 0;
}