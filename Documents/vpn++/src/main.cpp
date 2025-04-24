#include <iostream> 
// #include "core.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <atomic>
#include <boost/asio.hpp>
#include <core/io_context_pool.h>
#include "network/server.h"
#include "network/session.h"

using boost::asio::ip::tcp;

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