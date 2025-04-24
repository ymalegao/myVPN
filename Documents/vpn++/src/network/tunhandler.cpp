#include <core/tun_device.h>
#include <iostream>
#include "network/tunhandler.h"
#include <network/session.h>
#include <string>
#include <sys/socket.h>

TunHandler::TunHandler(boost::asio::io_context &io_context, const std::string& device_name)
    : io_context_(io_context), tun_fd_(tunOpen(&tun_name, nullptr)), tun_stream_(io_context, tun_fd_){
        // Open a TUN device
        // turn_stream wraps the TUN device
        std::cout << "Opened TUN device: " << tun_name.name << std::endl;

}

void TunHandler::set_session(std::shared_ptr<Session> session) {
    this->session_ = session;
}

void TunHandler::async_read_from_tun() {
    // Reads raw system packet from TUN.
    tun_stream_.async_read_some(boost::asio::buffer(read_buffer_),
                            [this](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                    std::vector<uint8_t> packet(read_buffer_.begin(), read_buffer_.end());
                                    parse_system_packet(packet);
                                    async_read_from_tun();
                                }else if(ec == boost::asio::error::operation_aborted){
                                    // Ignore operation aborted error
                                }else{
                                    std::cerr << "Error reading from TUN device: " << ec.message() << std::endl;
                                }
                            });
}

void TunHandler::parse_system_packet(const std::vector<uint8_t>& data) {
    // Adds your VPN framing: Packet(Packet::DATA, data).
    //
    //
    //figure out if it's an IP4 or IP6 packet

    //figure out if it's an IP4 or IP6 packet

    if (session_){
        session_->handle_packet_from_tun(Packet(Packet::DATA, data));
    }
    if (data.size() < TUN_OPEN_PACKET_OFFSET){
        return;
    }

    if (!TUN_OPEN_IS_IP4(data.data()) && !TUN_OPEN_IS_IP6(data.data())){
        return;
    }

    if (!TUN_OPEN_IS_IP4(data.data())){
        std::vector<uint8_t> payload(data.begin() + TUN_OPEN_PACKET_OFFSET, data.end());
        Packet packet = Packet(Packet::DATA, payload);
    }else if(TUN_OPEN_IS_IP6(data.data())){
        std::vector<uint8_t> payload(data.begin() + TUN_OPEN_PACKET_OFFSET, data.end());
        Packet packet = Packet(Packet::DATA, payload);
    }else{
        std::vector<uint8_t> payload(data.begin() + TUN_OPEN_PACKET_OFFSET, data.end());
        Packet packet = Packet(Packet::DATA, payload);
    }
    if (on_packet_ready_){
        on_packet_ready_(packet);
    }
}

void TunHandler::handle_vpn_response(const std::vector<uint8_t>& vpn_payload) {
    // Called by Session or similar to deliver data back.
    //


}


void TunHandler::send_to_tun(const std::vector<uint8_t>& system_payload) {
    // Write back to the TUN device
    std::vector<uint8_t> full_packet;

    full_packet.push_back(0x00);
    full_packet.push_back(0x00);
    if (!system_payload.empty() && (system_payload[0] >> 4) == 4 ){
        full_packet.push_back(0x00);
        full_packet.push_back(AF_INET);


    }else{
        full_packet.push_back(0x00);
        full_packet.push_back(AF_INET6);
    }


    full_packet.insert(full_packet.end(), system_payload.begin(), system_payload.end());
    boost::asio::async_write(tun_stream_, boost::asio::buffer(full_packet),
    [](const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (error) {
            // Handle error
            // Log error
            std::cerr << "Failed to write to TUN device" << std::endl;
        }
    });


}
