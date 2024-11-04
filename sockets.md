# Les Protocoles Réseau et les Sockets

## 1. Les protocoles UDP et TCP

### 1.1 Introduction aux protocoles

Les protocoles UDP (User Datagram Protocol) et TCP (Transmission Control Protocol) sont deux protocoles de transport fondamentaux dans les réseaux informatiques. Ils se situent au niveau 4 du modèle OSI.

#### Principales différences :

| Caractéristique | TCP | UDP |
|----------------|-----|-----|
| Connexion | Orienté connexion | Sans connexion |
| Fiabilité | Garantie de livraison | Pas de garantie |
| Ordre des paquets | Garanti | Non garanti |
| Vitesse | Plus lent | Plus rapide |
| Contrôle de flux | Oui | Non |
| Use cases | Web, email, FTP | Streaming, jeux en temps réel |

### 1.2 UDP (User Datagram Protocol)

#### Caractéristiques
- Sans connexion : pas de handshake
- Pas de garantie de livraison
- Pas de contrôle de congestion
- Messages indépendants (datagrammes)
- Header simple (8 bytes)

#### Cas d'utilisation
- Streaming vidéo/audio
- Jeux en temps réel
- DNS
- Broadcast/Multicast

### 1.3 TCP (Transmission Control Protocol)

#### Caractéristiques
- Orienté connexion (handshake)
- Garantie de livraison
- Contrôle de congestion
- Ordre garanti
- Header plus complexe (20 bytes)

#### Cas d'utilisation
- Web (HTTP)
- Email (SMTP)
- Transfert de fichiers (FTP)
- Tout ce qui nécessite une fiabilité

### 1.4 Exemple d'implémentation en C++

#### Exemple UDP : Serveur de broadcast

```cpp
class UDPBroadcaster {
private:
    sf::UdpSocket socket;
    const uint16_t port;
    
public:
    UDPBroadcaster(uint16_t port) : port(port) {
        socket.bind(port);
        socket.setBlocking(false);
    }
    
    bool broadcast(const std::string& message) {
        sf::Packet packet;
        packet << message;
        
        // Envoie à l'adresse de broadcast
        return socket.send(packet, sf::IpAddress::Broadcast, port) == sf::Socket::Done;
    }
    
    bool receive(std::string& message) {
        sf::Packet packet;
        sf::IpAddress sender;
        uint16_t senderPort;
        
        if (socket.receive(packet, sender, senderPort) == sf::Socket::Done) {
            packet >> message;
            return true;
        }
        return false;
    }
};
```

#### Exemple TCP : Serveur et Client

```cpp
class TCPServer {
private:
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> clients;
    
public:
    bool start(uint16_t port) {
        if (listener.listen(port) != sf::Socket::Done)
            return false;
            
        listener.setBlocking(false);
        return true;
    }
    
    void acceptNewClients() {
        sf::TcpSocket* client = new sf::TcpSocket;
        if (listener.accept(*client) == sf::Socket::Done) {
            clients.push_back(client);
            std::cout << "New client connected: " << client->getRemoteAddress() << std::endl;
        }
        else {
            delete client;
        }
    }
};

class TCPClient {
private:
    sf::TcpSocket socket;
    
public:
    bool connect(const sf::IpAddress& address, uint16_t port) {
        return socket.connect(address, port) == sf::Socket::Done;
    }
    
    bool send(const std::string& message) {
        sf::Packet packet;
        packet << message;
        return socket.send(packet) == sf::Socket::Done;
    }
};
```

## 2. Les Sockets

### 2.1 Introduction aux sockets

Un socket est une interface de programmation qui permet la communication entre processus, que ce soit sur la même machine ou à travers un réseau. C'est un point de terminaison d'une communication bidirectionnelle.

#### Types de sockets
1. **Stream Sockets (SOCK_STREAM)**
   - Utilisé avec TCP
   - Communication bidirectionnelle
   - Orienté connexion

2. **Datagram Sockets (SOCK_DGRAM)**
   - Utilisé avec UDP
   - Messages indépendants
   - Sans connexion

#### États d'un socket
1. **Création**
2. **Binding** (association à une adresse)
3. **Listening** (pour les serveurs TCP)
4. **Connexion** (pour TCP)
5. **Communication**
6. **Fermeture**

### 2.2 Les sockets avec SFML

SFML (Simple and Fast Multimedia Library) fournit une abstraction de haut niveau pour la manipulation des sockets.

#### Classes principales
1. `sf::Socket`
   - Classe de base abstraite
   - Définit les opérations communes

2. `sf::TcpSocket`
   - Pour les connexions TCP
   - Méthodes connect(), send(), receive()

3. `sf::UdpSocket`
   - Pour les communications UDP
   - Méthodes bind(), send(), receive()

4. `sf::TcpListener`
   - Pour l'écoute des connexions TCP entrantes
   - Méthodes listen(), accept()

5. `sf::SocketSelector`
   - Pour la gestion non-bloquante de multiples sockets
   - Méthodes add(), remove(), wait()

#### Gestion des erreurs
SFML utilise une énumération `sf::Socket::Status` :
- `Done` : Opération réussie
- `NotReady` : Opération non-bloquante en attente
- `Partial` : Opération partiellement réussie
- `Disconnected` : Socket déconnecté
- `Error` : Erreur lors de l'opération

### 2.3 Exemple d'implémentation complète

```cpp
class NetworkManager {
private:
    sf::TcpListener listener;
    sf::SocketSelector selector;
    std::vector<sf::TcpSocket*> clients;
    bool isServer;
    
public:
    NetworkManager() : isServer(false) {
        // Configuration non-bloquante
        listener.setBlocking(false);
    }
    
    ~NetworkManager() {
        // Nettoyage
        for(auto client : clients) {
            delete client;
        }
    }
    
    bool startServer(uint16_t port) {
        if (listener.listen(port) != sf::Socket::Done) {
            std::cerr << "Erreur lors de l'écoute sur le port " << port << std::endl;
            return false;
        }
        
        selector.add(listener);
        isServer = true;
        std::cout << "Serveur démarré sur le port " << port << std::endl;
        return true;
    }
    
    void update() {
        if (selector.wait(sf::milliseconds(10))) {
            // Nouvelles connexions
            if (isServer && selector.isReady(listener)) {
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done) {
                    std::cout << "Nouveau client connecté" << std::endl;
                    selector.add(*client);
                    clients.push_back(client);
                }
                else {
                    delete client;
                }
            }
            
            // Réception des données des clients
            for (auto it = clients.begin(); it != clients.end();) {
                sf::TcpSocket* client = *it;
                
                if (selector.isReady(*client)) {
                    sf::Packet packet;
                    if (client->receive(packet) == sf::Socket::Done) {
                        // Traitement du paquet
                        handlePacket(packet, client);
                    }
                    else {
                        // Déconnexion
                        selector.remove(*client);
                        delete client;
                        it = clients.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
    }
    
private:
    void handlePacket(sf::Packet& packet, sf::TcpSocket* client) {
        // Exemple de traitement de paquet
        std::string message;
        if (packet >> message) {
            std::cout << "Message reçu: " << message << std::endl;
            
            // Echo à tous les clients
            sf::Packet response;
            response << message;
            for (auto& c : clients) {
                if (c != client) {
                    c->send(response);
                }
            }
        }
    }
};
```

Cet exemple montre une implémentation complète d'un gestionnaire réseau qui peut :
1. Démarrer un serveur TCP
2. Gérer plusieurs clients simultanément
3. Traiter les connexions/déconnexions
4. Échanger des paquets de données

Les bonnes pratiques illustrées incluent :
- Gestion non-bloquante avec SocketSelector
- Nettoyage approprié des ressources
- Gestion des erreurs
- Structure modulaire et encapsulée