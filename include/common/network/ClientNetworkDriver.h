//
// Created by 15201 on 11/24/2025.
//

#ifndef UNDEROCEAN_CLIENTNETWORKDRIVER_H
#define UNDEROCEAN_CLIENTNETWORKDRIVER_H
#include <array>
#include <memory>
#include <queue>
#include <string>

#include "common/Types.h"
#include "common/net(depricate)/enet.h"
using namespace ClientTypes;
class ClientNetworkDriver {
public:
    ClientNetworkDriver() : clientHost_(nullptr), serverPeer_(nullptr) {}
    ~ClientNetworkDriver() {
        if (serverPeer_) {
            enet_peer_disconnect(serverPeer_, 0);
        }
        if (clientHost_) {
            enet_host_destroy(clientHost_);
        }
    }
    bool connect(const std::string& ip, int port) {
        ENetAddress clientAddr {};
        clientAddr.host = ENET_HOST_ANY;
        clientAddr.port = 0; // 0 = 系统随机
        clientHost_ = enet_host_create(&clientAddr, 1, 2, 0, 0);
        // clientHost_ = enet_host_create(nullptr, 1, 2, 0, 0);
        if (clientHost_ == nullptr) {
            return false;
        }
        enet_address_set_host(&address_, ip.c_str());
        address_.port = static_cast<enet_uint16>(port);
        serverPeer_ = enet_host_connect(clientHost_, &address_, 2, 0);
        if (serverPeer_ == nullptr) {
            return false;
        }
        enet_peer_timeout(serverPeer_, PING_TIMES, PING_TIMEOUT_MIN, PING_TIMEOUT_MAX);
        enet_peer_ping_interval(serverPeer_, HEARTBEAT_INTERVAL);
        return true;
    }
    void setOnConnect(const std::function<void()> &onConnect) { onConnect_ = onConnect; }
    void setOnDisconnect(const std::function<void()> &onDisConnect) { onDisConnect_ = onDisConnect; }
    void send(const Packet* packet, int channel, int packetType, bool reliable = false) {
        if (!serverPeer_) return;
        size_t total = packet->size() + 1; //extra byte for PacketTypeID
        ENetPacket* enetPacket = enet_packet_create(nullptr, total,
            !reliable ? ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT : ENET_PACKET_FLAG_RELIABLE);
        if (!enetPacket) return;
        enetPacket->data[0] = static_cast<uint8_t>(packetType);
        std::memcpy(enetPacket->data + 1, packet->data(), packet->size());
        enet_peer_send(serverPeer_, channel, enetPacket);
    }
    bool reconnect() {
        clientHost_ = enet_host_create(nullptr, 1, 2, 0, 0);
        if (clientHost_ == nullptr) {
            return false;
        }
        serverPeer_ = enet_host_connect(clientHost_, &address_, 2, 0);
        if (serverPeer_ == nullptr) {
            return false;
        }
        enet_peer_timeout(serverPeer_, PING_TIMES, PING_TIMEOUT_MIN, PING_TIMEOUT_MAX);
        enet_peer_ping_interval(serverPeer_, HEARTBEAT_INTERVAL);
        return true;
    }
    void pollPacket() { //read buffer, distribute by types and try reconnecting, and maintain heartbeat
        if ((!serverPeer_ || serverPeer_->state == ENET_PEER_STATE_DISCONNECTED) && reconnectTimer_.getElapsedTime().asSeconds() > RECONNECT_THRESHOLD) {
            reconnectTimer_.restart();
            reconnect();
        }
        ENetEvent event;
        while (enet_host_service(clientHost_, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    if (event.packet->dataLength < 1) {  //empty packet
                        enet_packet_destroy(event.packet);
                        break;
                    }
                    uint8_t typeByte = event.packet->data[0];
                    if (typeByte >= PacketType::COUNT) { //type bad
                        enet_packet_destroy(event.packet);
                        break;
                    }
                    auto packet = std::make_unique<Packet>(
                        event.packet->data + 1,  //remove type
                        event.packet->data + event.packet->dataLength);
                    packets_[typeByte].push(std::move(packet));
                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                case ENET_EVENT_TYPE_DISCONNECT:
                    event.peer->data = nullptr;
                    serverPeer_ = nullptr;
                    connected_ = false;
                    onDisConnect_();
                    break;
                case ENET_EVENT_TYPE_CONNECT:
                    connected_ = true;
                    onConnect_();
                    break;
                default:
                    break;
            }
        }
    }
    std::unique_ptr<Packet> popPacket(int packetType) {  //caution: must update peer list
        if (packets_[packetType].empty()) {
            return nullptr;
        }
        std::unique_ptr<Packet> packet = std::move(packets_[packetType].front());
        packets_[packetType].pop();
        return packet;
    }
    [[nodiscard]] bool hasPacket(int packetType) const {
        if (packets_[packetType].empty()) return false;
        return true;
    }
    [[nodiscard]] bool isConnected() const { return connected_; }  //callback func instead
private:
    ENetHost* clientHost_;
    ENetPeer* serverPeer_;
    ENetAddress address_{};
    sf::Clock reconnectTimer_;
    bool connected_ { false };
    std::array<std::queue<std::unique_ptr<Packet>>, PacketType::COUNT> packets_;
    std::function<void()> onConnect_;
    std::function<void()> onDisConnect_;   //call LevelSelectMenu
    static constexpr int RECONNECT_THRESHOLD = 10;  //by second
};
#endif //UNDEROCEAN_CLIENTNETWORKDRIVER_H