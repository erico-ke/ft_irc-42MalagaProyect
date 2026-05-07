# 🗺️ Guía Completa de Implementación — ft_irc
> **Perfil:** Equipo de 2 · Linux · Nivel básico-medio en redes · Un archivo por clase · Desde cero  
> **Cliente de referencia:** irssi  
> **Estándar:** C++ 98

---

## 📋 Índice

1. [Antes de empezar — Conceptos clave](#1-antes-de-empezar--conceptos-clave)
2. [Instalar y usar el cliente irssi](#2-instalar-y-usar-el-cliente-irssi)
3. [El protocolo IRC explicado](#3-el-protocolo-irc-explicado)
4. [Arquitectura de clases](#4-arquitectura-de-clases)
5. [Makefile](#5-makefile)
6. [Paso 1 — main.cpp](#6-paso-1--maincpp)
7. [Paso 2 — Clase Server (inicialización)](#7-paso-2--clase-server-inicialización)
8. [Paso 3 — Loop principal con poll()](#8-paso-3--loop-principal-con-poll)
9. [Paso 4 — Clase Client](#9-paso-4--clase-client)
10. [Paso 5 — Aceptar y gestionar clientes](#10-paso-5--aceptar-y-gestionar-clientes)
11. [Paso 6 — El buffer acumulativo](#11-paso-6--el-buffer-acumulativo)
12. [Paso 7 — CommandHandler y parseo](#12-paso-7--commandhandler-y-parseo)
13. [Paso 8 — Autenticación (PASS, NICK, USER)](#13-paso-8--autenticación-pass-nick-user)
14. [Paso 9 — Clase Channel](#14-paso-9--clase-channel)
15. [Paso 10 — JOIN y PRIVMSG](#15-paso-10--join-y-privmsg)
16. [Paso 11 — QUIT y desconexión limpia](#16-paso-11--quit-y-desconexión-limpia)
17. [Paso 12 — Comandos de operador](#17-paso-12--comandos-de-operador)
18. [Paso 13 — MODE (los 5 modos)](#18-paso-13--mode-los-5-modos)
19. [Paso 14 — Señales y cierre limpio](#19-paso-14--señales-y-cierre-limpio)
20. [Códigos de respuesta IRC necesarios](#20-códigos-de-respuesta-irc-necesarios)
21. [Testing paso a paso](#21-testing-paso-a-paso)
22. [Errores comunes y cómo evitarlos](#22-errores-comunes-y-cómo-evitarlos)
23. [División de trabajo para el equipo](#23-división-de-trabajo-para-el-equipo)
24. [Orden de implementación sugerido](#24-orden-de-implementación-sugerido)

---

## 1. Antes de empezar — Conceptos clave

### ¿Qué es un socket?
Un socket es un **file descriptor** (número entero) que representa un extremo de una conexión de red. En Linux, todo es un fd: archivos, pipes, conexiones de red. Puedes hacer `read()`/`write()` sobre ellos igual que con archivos.

### ¿Qué es non-blocking I/O?
Por defecto, `recv()` **bloquea** el proceso hasta que hay datos. En modo non-blocking, si no hay datos, devuelve `-1` con `errno = EAGAIN` en lugar de esperar. Esto es lo que permite manejar múltiples clientes en un solo hilo.

### ¿Qué hace poll()?
`poll()` recibe una lista de file descriptors y **espera hasta que alguno tenga actividad** (datos para leer, listo para escribir, error). Es como un árbitro que vigila todos los fds a la vez y te avisa cuándo actuar.

### TCP/IP y flujo de datos
TCP garantiza que los datos llegan en orden, pero **no garantiza que lleguen de golpe**. Un mensaje de 100 bytes puede llegar en 3 llamadas a `recv()`: 30 + 50 + 20. Por eso necesitáis un buffer acumulativo.

---

## 2. Instalar y usar el cliente irssi

```bash
# Instalar
sudo apt install irssi

# Ejecutar
irssi

# Dentro de irssi, conectar a vuestro servidor:
/connect 127.0.0.1 6667 tupassword

# Unirse a un canal
/join #general

# Enviar mensaje
/msg #general hola mundo

# Enviar mensaje privado
/msg pepito hola

# Salir
/quit
```

### Comandos útiles de irssi para testear

```
/connect 127.0.0.1 6667 password    → conectar
/nick nuevonick                     → cambiar nick
/join #canal                        → unirse a canal
/join #canal clave                  → unirse a canal con contraseña
/part #canal                        → salir del canal
/kick #canal nick razón             → expulsar (si eres op)
/invite nick #canal                 → invitar
/topic #canal nuevo topic           → cambiar topic
/mode #canal +i                     → activar invite-only
/mode #canal +o nick                → dar op a nick
/mode #canal +k clave               → poner contraseña al canal
/mode #canal +l 10                  → límite de usuarios
```

---

## 3. El protocolo IRC explicado

### Formato de mensaje

```
[:prefix] COMANDO [param1] [param2] [:trailing]\r\n
```

- **prefix**: opcional, indica quién envía (`:nick!user@host`)
- **COMANDO**: siempre en mayúsculas
- **params**: hasta 14 parámetros separados por espacio
- **trailing**: parámetro final, puede contener espacios, va precedido de `:`
- **`\r\n`**: terminador obligatorio (CRLF)

### Ejemplos de mensajes que recibirá el servidor

```
PASS mipassword\r\n
NICK pepito\r\n
USER pepito 0 * :Pepe Garcia\r\n
JOIN #general\r\n
JOIN #privado clave123\r\n
PRIVMSG #general :hola a todos\r\n
PRIVMSG pepito :mensaje privado\r\n
KICK #general fulanito :portarse mal\r\n
INVITE fulanito #privado\r\n
TOPIC #general :nuevo topic aquí\r\n
MODE #general +i\r\n
MODE #general +k secreto\r\n
MODE #general +o pepito\r\n
MODE #general +l 20\r\n
QUIT :hasta luego\r\n
```

### Ejemplos de respuestas que envía el servidor

```
:server 001 pepito :Welcome to the IRC server!\r\n
:server 331 pepito #general :No topic is set\r\n
:server 332 pepito #general :bienvenidos al canal\r\n
:pepito!user@host JOIN #general\r\n
:pepito!user@host PRIVMSG #general :hola\r\n
:server 433 * pepito :Nickname already in use\r\n
```

### Prefijo del cliente (hostmask)

Cuando el servidor reenvía mensajes en nombre de un cliente, usa este formato:

```
:nickname!username@hostname
```

Para simplificar, podéis usar `127.0.0.1` o `localhost` como hostname fijo durante el desarrollo.

---

## 4. Arquitectura de clases

### Estructura de ficheros

```
ft_irc/
├── Makefile
├── main.cpp
├── Server.hpp
├── Server.cpp
├── Client.hpp
├── Client.cpp
├── Channel.hpp
├── Channel.cpp
├── CommandHandler.hpp
└── CommandHandler.cpp
```

### Diagrama de relaciones

```
Server
 ├── std::vector<pollfd>          → fds monitorizados por poll()
 ├── std::map<int, Client*>       → fd → cliente
 ├── std::map<string, Channel*>   → nombre → canal
 └── usa CommandHandler::handle() para procesar comandos

Client
 ├── int fd
 ├── string nickname, username, realname
 ├── string buffer                → datos parciales acumulados
 └── bool authenticated, passGiven, nickGiven, userGiven

Channel
 ├── string name, topic, key
 ├── vector<Client*> members
 ├── vector<Client*> operators
 └── bool inviteOnly, topicRestricted; int userLimit

CommandHandler (clase estática o namespace)
 └── handle() → dispatch a handlePass, handleNick, handleJoin...
```

---

## 5. Makefile

```makefile
NAME        = ircserv
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98

SRCS        = main.cpp \
              Server.cpp \
              Client.cpp \
              Channel.cpp \
              CommandHandler.cpp

OBJS        = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

---

## 6. Paso 1 — main.cpp

```cpp
#include "Server.hpp"
#include <iostream>
#include <cstdlib>  // atoi

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: invalid port" << std::endl;
        return 1;
    }

    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Error: password cannot be empty" << std::endl;
        return 1;
    }

    try {
        Server server(port, password);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

---

## 7. Paso 2 — Clase Server (inicialización)

### Server.hpp

```cpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <vector>
#include <map>
#include <string>
#include <poll.h>
#include <stdexcept>

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();

    void run();

    // Métodos de acceso usados por CommandHandler
    void        sendToClient(int fd, const std::string& msg);
    void        removeClient(int fd);
    Client*     getClientByNick(const std::string& nick);
    Channel*    getChannel(const std::string& name);
    Channel*    getOrCreateChannel(const std::string& name);
    bool        isNickInUse(const std::string& nick) const;
    std::string getPassword() const;
    std::string getServerName() const;

private:
    int                              _port;
    std::string                      _password;
    int                              _serverFd;
    std::vector<struct pollfd>       _pollfds;
    std::map<int, Client*>           _clients;    // fd → Client*
    std::map<std::string, Channel*>  _channels;   // "#nombre" → Channel*

    void _initSocket();
    void _acceptClient();
    void _handleClient(int fd);
};

#endif
```

### Server.cpp — constructor e inicialización

```cpp
#include "Server.hpp"
#include "CommandHandler.hpp"
#include <iostream>
#include <cstring>     // memset
#include <unistd.h>    // close
#include <fcntl.h>     // fcntl
#include <arpa/inet.h> // htons, INADDR_ANY
#include <sys/socket.h>
#include <netinet/in.h>

Server::Server(int port, const std::string& password)
    : _port(port), _password(password), _serverFd(-1) {
    _initSocket();
}

Server::~Server() {
    // Cerrar el socket del servidor
    if (_serverFd >= 0)
        close(_serverFd);

    // Eliminar todos los clientes
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first);
        delete it->second;
    }

    // Eliminar todos los canales
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
}

void Server::_initSocket() {
    // 1. Crear el socket TCP/IPv4
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("socket() failed");

    // 2. Permitir reutilizar el puerto inmediatamente al reiniciar
    int opt = 1;
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Non-blocking
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);

    // 4. Configurar dirección
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;    // escuchar en todas las interfaces
    addr.sin_port        = htons(_port);  // convertir al byte order de red

    // 5. Bind
    if (bind(_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    // 6. Listen (cola de hasta 10 conexiones pendientes)
    if (listen(_serverFd, 10) < 0)
        throw std::runtime_error("listen() failed");

    // 7. Añadir el fd del servidor a la lista de poll
    struct pollfd pfd;
    pfd.fd      = _serverFd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);

    std::cout << "Server initialized on port " << _port << std::endl;
}
```

---

## 8. Paso 3 — Loop principal con poll()

```cpp
// Variable global para el signal handler
extern bool g_running;

void Server::run() {
    std::cout << "Server running. Waiting for connections..." << std::endl;

    while (g_running) {
        // poll() bloquea hasta que algún fd tiene actividad
        // timeout -1 = esperar indefinidamente
        int ret = poll(_pollfds.data(), _pollfds.size(), -1);

        if (ret < 0) {
            if (!g_running) break;  // fue interrumpido por señal (SIGINT)
            throw std::runtime_error("poll() failed");
        }

        // Iterar sobre todos los fds monitorizados
        // IMPORTANTE: iterar con índice, no iterador, porque _pollfds puede crecer
        for (size_t i = 0; i < _pollfds.size(); i++) {

            // ¿Tiene evento disponible para leer?
            if (!(_pollfds[i].revents & POLLIN))
                continue;

            if (_pollfds[i].fd == _serverFd) {
                // El fd del servidor tiene actividad → nueva conexión
                _acceptClient();
            } else {
                // Un cliente tiene datos → procesarlos
                _handleClient(_pollfds[i].fd);
            }
        }
    }
    std::cout << "\nServer shutting down." << std::endl;
}
```

### ¿Por qué iterar con índice y no con iterador?

Porque `_acceptClient()` hace `push_back` en `_pollfds`, lo que puede invalidar los iteradores. Con índice, el bucle `for` re-evalúa `_pollfds.size()` en cada iteración y es seguro.

---

## 9. Paso 4 — Clase Client

### Client.hpp

```cpp
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(int fd);
    ~Client();

    // Getters
    int         getFd()        const;
    std::string getNickname()  const;
    std::string getUsername()  const;
    std::string getRealname()  const;
    bool        isAuthenticated() const;
    bool        passGiven()    const;
    bool        nickGiven()    const;
    bool        userGiven()    const;

    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setAuthenticated(bool val);
    void setPassGiven(bool val);
    void setNickGiven(bool val);
    void setUserGiven(bool val);

    // Buffer de entrada
    void        appendToBuffer(const std::string& data);
    std::string& getBufferRef();

    // Prefijo IRC: ":nick!user@host"
    std::string getPrefix() const;

private:
    int         _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _buffer;
    bool        _authenticated;
    bool        _passGiven;
    bool        _nickGiven;
    bool        _userGiven;
};

#endif
```

### Client.cpp

```cpp
#include "Client.hpp"

Client::Client(int fd)
    : _fd(fd), _authenticated(false),
      _passGiven(false), _nickGiven(false), _userGiven(false) {}

Client::~Client() {}

int         Client::getFd()       const { return _fd; }
std::string Client::getNickname() const { return _nickname; }
std::string Client::getUsername() const { return _username; }
std::string Client::getRealname() const { return _realname; }
bool Client::isAuthenticated()    const { return _authenticated; }
bool Client::passGiven()          const { return _passGiven; }
bool Client::nickGiven()          const { return _nickGiven; }
bool Client::userGiven()          const { return _userGiven; }

void Client::setNickname(const std::string& n)  { _nickname = n; }
void Client::setUsername(const std::string& u)  { _username = u; }
void Client::setRealname(const std::string& r)  { _realname = r; }
void Client::setAuthenticated(bool v)           { _authenticated = v; }
void Client::setPassGiven(bool v)               { _passGiven = v; }
void Client::setNickGiven(bool v)               { _nickGiven = v; }
void Client::setUserGiven(bool v)               { _userGiven = v; }

void Client::appendToBuffer(const std::string& data) {
    _buffer += data;
}

std::string& Client::getBufferRef() {
    return _buffer;
}

std::string Client::getPrefix() const {
    return ":" + _nickname + "!" + _username + "@localhost";
}
```

---

## 10. Paso 5 — Aceptar y gestionar clientes

### Aceptar nueva conexión

```cpp
void Server::_acceptClient() {
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &len);
    if (clientFd < 0)
        return;  // en non-blocking, puede no haber conexión real aún

    // Poner el fd del cliente en non-blocking también
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    // Registrar en poll()
    struct pollfd pfd;
    pfd.fd      = clientFd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);

    // Crear el objeto cliente
    _clients[clientFd] = new Client(clientFd);

    std::cout << "New client connected: fd=" << clientFd << std::endl;
}
```

### Eliminar cliente

```cpp
void Server::removeClient(int fd) {
    // Eliminar de los canales en los que está
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); ++it) {
        it->second->removeMember(_clients[fd]);
    }

    // Eliminar de _pollfds
    for (std::vector<struct pollfd>::iterator it = _pollfds.begin();
         it != _pollfds.end(); ++it) {
        if (it->fd == fd) {
            _pollfds.erase(it);
            break;
        }
    }

    // Cerrar fd y liberar memoria
    close(fd);
    delete _clients[fd];
    _clients.erase(fd);

    std::cout << "Client disconnected: fd=" << fd << std::endl;
}
```

### Enviar mensaje a un cliente

```cpp
void Server::sendToClient(int fd, const std::string& msg) {
    // send() puede no enviar todo de golpe si el buffer está lleno
    // Para simplicidad en este proyecto, una llamada suele ser suficiente
    if (send(fd, msg.c_str(), msg.size(), 0) < 0)
        removeClient(fd);
}
```

### Helpers de búsqueda

```cpp
Client* Server::getClientByNick(const std::string& nick) {
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        if (it->second->getNickname() == nick)
            return it->second;
    }
    return NULL;
}

bool Server::isNickInUse(const std::string& nick) const {
    for (std::map<int, Client*>::const_iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        if (it->second->getNickname() == nick)
            return true;
    }
    return false;
}

Channel* Server::getChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

Channel* Server::getOrCreateChannel(const std::string& name) {
    Channel* chan = getChannel(name);
    if (!chan) {
        chan = new Channel(name);
        _channels[name] = chan;
    }
    return chan;
}

std::string Server::getPassword()   const { return _password; }
std::string Server::getServerName() const { return "ircserv"; }
```

---

## 11. Paso 6 — El buffer acumulativo

Este es uno de los puntos más importantes del proyecto. Los datos **llegan en fragmentos** y hay que acumularlos hasta tener una línea completa.

```cpp
void Server::_handleClient(int fd) {
    char buf[512];
    memset(buf, 0, sizeof(buf));

    int bytes = recv(fd, buf, sizeof(buf) - 1, 0);

    if (bytes <= 0) {
        // bytes == 0: el cliente cerró la conexión limpiamente
        // bytes < 0: error de red
        removeClient(fd);
        return;
    }

    // Acumular en el buffer del cliente
    _clients[fd]->appendToBuffer(std::string(buf, bytes));

    // Procesar todas las líneas completas disponibles
    std::string& buffer = _clients[fd]->getBufferRef();
    size_t pos;
    while ((pos = buffer.find("\r\n")) != std::string::npos) {
        std::string line = buffer.substr(0, pos);  // sin el \r\n
        buffer.erase(0, pos + 2);                   // consumir del buffer

        if (!line.empty())
            CommandHandler::handle(*_clients[fd], line, *this);
    }
}
```

### Prueba del buffer con nc

```bash
# Simula datos fragmentados (como indica el subject)
nc -C 127.0.0.1 6667
# Escribe: PASS (ctrl+D)
# Escribe: mypa (ctrl+D)
# Escribe: ss (enter)
# El servidor debe recibir "PASS mypass\r\n" correctamente ensamblado
```

---

## 12. Paso 7 — CommandHandler y parseo

### CommandHandler.hpp

```cpp
#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include <vector>

// Forward declaration
class Server;

class CommandHandler {
public:
    static void handle(Client& client, const std::string& line, Server& server);

private:
    static void handlePass(Client& client, const std::string& params, Server& server);
    static void handleNick(Client& client, const std::string& params, Server& server);
    static void handleUser(Client& client, const std::string& params, Server& server);
    static void handleJoin(Client& client, const std::string& params, Server& server);
    static void handlePrivmsg(Client& client, const std::string& params, Server& server);
    static void handleKick(Client& client, const std::string& params, Server& server);
    static void handleInvite(Client& client, const std::string& params, Server& server);
    static void handleTopic(Client& client, const std::string& params, Server& server);
    static void handleMode(Client& client, const std::string& params, Server& server);
    static void handleQuit(Client& client, const std::string& params, Server& server);

    static void _tryFinishAuth(Client& client, Server& server);

    // Utilidades de parseo
    static std::string         toUpper(const std::string& s);
    static std::vector<std::string> splitParams(const std::string& params);
    static std::string         getTrailing(const std::string& params);
};

#endif
```

### CommandHandler.cpp — dispatch principal

```cpp
#include "CommandHandler.hpp"
#include "Server.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

void CommandHandler::handle(Client& client, const std::string& line, Server& server) {
    if (line.empty()) return;

    // Separar el comando del resto
    std::istringstream iss(line);
    std::string command;
    iss >> command;
    command = toUpper(command);

    // El resto de la línea son los parámetros
    std::string params;
    std::getline(iss, params);
    if (!params.empty() && params[0] == ' ')
        params = params.substr(1);

    // Dispatch
    if      (command == "PASS")    handlePass(client, params, server);
    else if (command == "NICK")    handleNick(client, params, server);
    else if (command == "USER")    handleUser(client, params, server);
    else if (command == "QUIT")    handleQuit(client, params, server);
    else if (command == "JOIN")    handleJoin(client, params, server);
    else if (command == "PRIVMSG") handlePrivmsg(client, params, server);
    else if (command == "KICK")    handleKick(client, params, server);
    else if (command == "INVITE")  handleInvite(client, params, server);
    else if (command == "TOPIC")   handleTopic(client, params, server);
    else if (command == "MODE")    handleMode(client, params, server);
    // Comandos desconocidos: responder 421
    // else: server.sendToClient(client.getFd(), ":server 421 " + command + " :Unknown command\r\n");
}

// Convierte string a mayúsculas
std::string CommandHandler::toUpper(const std::string& s) {
    std::string result = s;
    for (size_t i = 0; i < result.size(); i++)
        result[i] = toupper(result[i]);
    return result;
}

// Extrae el parámetro trailing (el que va tras ":")
// Ej: en "pepito #general :hola mundo" devuelve "hola mundo"
std::string CommandHandler::getTrailing(const std::string& params) {
    size_t pos = params.find(':');
    if (pos == std::string::npos) return "";
    return params.substr(pos + 1);
}

// Divide parámetros separados por espacio (sin el trailing)
std::vector<std::string> CommandHandler::splitParams(const std::string& params) {
    std::vector<std::string> result;
    std::istringstream iss(params);
    std::string token;
    while (iss >> token) {
        if (token[0] == ':') break;  // empieza el trailing, parar
        result.push_back(token);
    }
    return result;
}
```

---

## 13. Paso 8 — Autenticación (PASS, NICK, USER)

Un cliente no está autenticado hasta que ha enviado los tres comandos correctamente. El orden más común es PASS → NICK → USER, pero el servidor debe aceptar cualquier orden.

```cpp
void CommandHandler::handlePass(Client& client, const std::string& params, Server& server) {
    if (client.isAuthenticated()) {
        server.sendToClient(client.getFd(),
            ":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
        return;
    }
    if (params.empty()) {
        server.sendToClient(client.getFd(), ":ircserv 461 PASS :Not enough parameters\r\n");
        return;
    }
    if (params != server.getPassword()) {
        server.sendToClient(client.getFd(), ":ircserv 464 * :Password incorrect\r\n");
        return;
    }
    client.setPassGiven(true);
}

void CommandHandler::handleNick(Client& client, const std::string& params, Server& server) {
    if (params.empty()) {
        server.sendToClient(client.getFd(), ":ircserv 431 * :No nickname given\r\n");
        return;
    }
    // Comprobar que el nick no está en uso
    Client* existing = server.getClientByNick(params);
    if (existing && existing->getFd() != client.getFd()) {
        server.sendToClient(client.getFd(),
            ":ircserv 433 * " + params + " :Nickname already in use\r\n");
        return;
    }
    std::string oldNick = client.getNickname();
    client.setNickname(params);
    client.setNickGiven(true);

    // Si ya estaba autenticado, notificar el cambio de nick
    if (client.isAuthenticated()) {
        server.sendToClient(client.getFd(),
            ":" + oldNick + "!user@localhost NICK :" + params + "\r\n");
    }
    _tryFinishAuth(client, server);
}

void CommandHandler::handleUser(Client& client, const std::string& params, Server& server) {
    if (client.isAuthenticated()) {
        server.sendToClient(client.getFd(),
            ":ircserv 462 " + client.getNickname() + " :You may not reregister\r\n");
        return;
    }
    // FORMAT: USER <username> <mode> <unused> :<realname>
    std::vector<std::string> args = splitParams(params);
    if (args.size() < 1) {
        server.sendToClient(client.getFd(), ":ircserv 461 USER :Not enough parameters\r\n");
        return;
    }
    client.setUsername(args[0]);
    client.setRealname(getTrailing(params));
    client.setUserGiven(true);
    _tryFinishAuth(client, server);
}

void CommandHandler::_tryFinishAuth(Client& client, Server& server) {
    if (!client.passGiven() || !client.nickGiven() || !client.userGiven())
        return;  // aún falta alguno

    client.setAuthenticated(true);

    // Mensajes de bienvenida obligatorios (irssi los necesita todos)
    std::string nick = client.getNickname();
    server.sendToClient(client.getFd(),
        ":ircserv 001 " + nick + " :Welcome to the IRC Network, " + nick + "!\r\n");
    server.sendToClient(client.getFd(),
        ":ircserv 002 " + nick + " :Your host is ircserv, running version 1.0\r\n");
    server.sendToClient(client.getFd(),
        ":ircserv 003 " + nick + " :This server was created today\r\n");
    server.sendToClient(client.getFd(),
        ":ircserv 004 " + nick + " ircserv 1.0 o itkol\r\n");
}
```

---

## 14. Paso 9 — Clase Channel

### Channel.hpp

```cpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

class Client;

class Channel {
public:
    Channel(const std::string& name);
    ~Channel();

    // Miembros
    void    addMember(Client* client);
    void    removeMember(Client* client);
    bool    isMember(Client* client) const;
    bool    isEmpty() const;

    // Operadores
    void    addOperator(Client* client);
    void    removeOperator(Client* client);
    bool    isOperator(Client* client) const;

    // Invitaciones
    void    addInvite(Client* client);
    bool    isInvited(Client* client) const;

    // Broadcast a todos los miembros (excepto 'except' si no es NULL)
    void    broadcast(const std::string& msg, Client* except = NULL);

    // Getters
    std::string getName()     const;
    std::string getTopic()    const;
    std::string getKey()      const;
    bool        isInviteOnly()       const;
    bool        isTopicRestricted()  const;
    int         getUserLimit()       const;
    size_t      getMemberCount()     const;

    // Setters (modos)
    void setTopic(const std::string& topic);
    void setKey(const std::string& key);
    void setInviteOnly(bool val);
    void setTopicRestricted(bool val);
    void setUserLimit(int limit);

private:
    std::string          _name;
    std::string          _topic;
    std::string          _key;
    std::vector<Client*> _members;
    std::vector<Client*> _operators;
    std::vector<Client*> _invited;
    bool                 _inviteOnly;
    bool                 _topicRestricted;
    int                  _userLimit;       // -1 = sin límite
};

#endif
```

### Channel.cpp

```cpp
#include "Channel.hpp"
#include "Client.hpp"
#include <algorithm>

Channel::Channel(const std::string& name)
    : _name(name), _inviteOnly(false), _topicRestricted(false), _userLimit(-1) {}

Channel::~Channel() {}

void Channel::addMember(Client* client) {
    if (!isMember(client))
        _members.push_back(client);
}

void Channel::removeMember(Client* client) {
    _members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
    _operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
    _invited.erase(std::remove(_invited.begin(), _invited.end(), client), _invited.end());
}

bool Channel::isMember(Client* client) const {
    return std::find(_members.begin(), _members.end(), client) != _members.end();
}

bool Channel::isEmpty() const { return _members.empty(); }

void Channel::addOperator(Client* client) {
    if (!isOperator(client))
        _operators.push_back(client);
}

void Channel::removeOperator(Client* client) {
    _operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
}

bool Channel::isOperator(Client* client) const {
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

void Channel::addInvite(Client* client) {
    if (!isInvited(client))
        _invited.push_back(client);
}

bool Channel::isInvited(Client* client) const {
    return std::find(_invited.begin(), _invited.end(), client) != _invited.end();
}

void Channel::broadcast(const std::string& msg, Client* except) {
    for (size_t i = 0; i < _members.size(); i++) {
        if (_members[i] != except)
            send(_members[i]->getFd(), msg.c_str(), msg.size(), 0);
    }
}

// Getters
std::string Channel::getName()           const { return _name; }
std::string Channel::getTopic()          const { return _topic; }
std::string Channel::getKey()            const { return _key; }
bool        Channel::isInviteOnly()      const { return _inviteOnly; }
bool        Channel::isTopicRestricted() const { return _topicRestricted; }
int         Channel::getUserLimit()      const { return _userLimit; }
size_t      Channel::getMemberCount()    const { return _members.size(); }

// Setters
void Channel::setTopic(const std::string& t)  { _topic = t; }
void Channel::setKey(const std::string& k)    { _key = k; }
void Channel::setInviteOnly(bool v)           { _inviteOnly = v; }
void Channel::setTopicRestricted(bool v)      { _topicRestricted = v; }
void Channel::setUserLimit(int l)             { _userLimit = l; }
```

---

## 15. Paso 10 — JOIN y PRIVMSG

```cpp
void CommandHandler::handleJoin(Client& client, const std::string& params, Server& server) {
    if (!client.isAuthenticated()) return;

    std::vector<std::string> args = splitParams(params);
    if (args.empty()) {
        server.sendToClient(client.getFd(), ":ircserv 461 JOIN :Not enough parameters\r\n");
        return;
    }

    std::string chanName = args[0];
    std::string key      = (args.size() > 1) ? args[1] : "";

    // El nombre del canal debe empezar por #
    if (chanName[0] != '#') {
        server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
        return;
    }

    Channel* chan = server.getOrCreateChannel(chanName);

    // Comprobar modo invite-only
    if (chan->isInviteOnly() && !chan->isInvited(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 473 " + client.getNickname() + " " + chanName + " :Cannot join (invite only)\r\n");
        return;
    }

    // Comprobar contraseña del canal
    if (!chan->getKey().empty() && chan->getKey() != key) {
        server.sendToClient(client.getFd(),
            ":ircserv 475 " + client.getNickname() + " " + chanName + " :Bad channel key\r\n");
        return;
    }

    // Comprobar límite de usuarios
    if (chan->getUserLimit() != -1 &&
        (int)chan->getMemberCount() >= chan->getUserLimit()) {
        server.sendToClient(client.getFd(),
            ":ircserv 471 " + client.getNickname() + " " + chanName + " :Channel is full\r\n");
        return;
    }

    // Si es el primer miembro, hacerlo operador
    bool firstMember = chan->isEmpty();
    chan->addMember(&client);
    if (firstMember)
        chan->addOperator(&client);

    // Notificar a todos del canal (incluido el nuevo)
    std::string joinMsg = client.getPrefix() + " JOIN " + chanName + "\r\n";
    chan->broadcast(joinMsg);

    // Enviar topic (331 = sin topic, 332 = con topic)
    if (chan->getTopic().empty()) {
        server.sendToClient(client.getFd(),
            ":ircserv 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n");
    } else {
        server.sendToClient(client.getFd(),
            ":ircserv 332 " + client.getNickname() + " " + chanName + " :" + chan->getTopic() + "\r\n");
    }

    // Enviar lista de usuarios del canal (353 + 366)
    // (irssi lo necesita para mostrar la lista de usuarios)
    // Implementación simplificada — ampliar según necesidad
}

void CommandHandler::handlePrivmsg(Client& client, const std::string& params, Server& server) {
    if (!client.isAuthenticated()) return;

    // FORMAT: PRIVMSG <target> :<message>
    std::vector<std::string> args = splitParams(params);
    std::string message = getTrailing(params);

    if (args.empty() || message.empty()) {
        server.sendToClient(client.getFd(), ":ircserv 461 PRIVMSG :Not enough parameters\r\n");
        return;
    }

    std::string target = args[0];
    std::string fullMsg = client.getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        // Mensaje a canal
        Channel* chan = server.getChannel(target);
        if (!chan) {
            server.sendToClient(client.getFd(), ":ircserv 403 " + target + " :No such channel\r\n");
            return;
        }
        if (!chan->isMember(&client)) {
            server.sendToClient(client.getFd(),
                ":ircserv 404 " + target + " :Cannot send to channel (not a member)\r\n");
            return;
        }
        chan->broadcast(fullMsg, &client);  // no enviar al propio emisor
    } else {
        // Mensaje privado a usuario
        Client* dest = server.getClientByNick(target);
        if (!dest) {
            server.sendToClient(client.getFd(), ":ircserv 401 " + target + " :No such nick\r\n");
            return;
        }
        server.sendToClient(dest->getFd(), fullMsg);
    }
}
```

---

## 16. Paso 11 — QUIT y desconexión limpia

```cpp
void CommandHandler::handleQuit(Client& client, const std::string& params, Server& server) {
    std::string reason = getTrailing(params);
    if (reason.empty()) reason = "Client quit";

    std::string quitMsg = client.getPrefix() + " QUIT :" + reason + "\r\n";

    // Notificar a todos los canales donde estaba el cliente
    // (necesitáis que Server exponga los canales o un método para esto)

    server.removeClient(client.getFd());
}
```

---

## 17. Paso 12 — Comandos de operador

### KICK

```cpp
void CommandHandler::handleKick(Client& client, const std::string& params, Server& server) {
    if (!client.isAuthenticated()) return;

    // FORMAT: KICK <channel> <nick> [:<reason>]
    std::vector<std::string> args = splitParams(params);
    std::string reason = getTrailing(params);
    if (reason.empty()) reason = "Kicked";

    if (args.size() < 2) {
        server.sendToClient(client.getFd(), ":ircserv 461 KICK :Not enough parameters\r\n");
        return;
    }

    std::string chanName   = args[0];
    std::string targetNick = args[1];

    Channel* chan = server.getChannel(chanName);
    if (!chan) {
        server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
        return;
    }
    if (!chan->isOperator(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 482 " + chanName + " :You're not channel operator\r\n");
        return;
    }

    Client* target = server.getClientByNick(targetNick);
    if (!target || !chan->isMember(target)) {
        server.sendToClient(client.getFd(),
            ":ircserv 441 " + targetNick + " " + chanName + " :They aren't on that channel\r\n");
        return;
    }

    // Notificar a todos y eliminar del canal
    std::string kickMsg = client.getPrefix() + " KICK " + chanName + " " + targetNick + " :" + reason + "\r\n";
    chan->broadcast(kickMsg);
    chan->removeMember(target);
}
```

### INVITE

```cpp
void CommandHandler::handleInvite(Client& client, const std::string& params, Server& server) {
    if (!client.isAuthenticated()) return;

    // FORMAT: INVITE <nick> <channel>
    std::vector<std::string> args = splitParams(params);
    if (args.size() < 2) {
        server.sendToClient(client.getFd(), ":ircserv 461 INVITE :Not enough parameters\r\n");
        return;
    }

    std::string targetNick = args[0];
    std::string chanName   = args[1];

    Channel* chan = server.getChannel(chanName);
    if (!chan) {
        server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
        return;
    }
    if (!chan->isMember(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 442 " + chanName + " :You're not on that channel\r\n");
        return;
    }
    if (chan->isInviteOnly() && !chan->isOperator(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 482 " + chanName + " :You're not channel operator\r\n");
        return;
    }

    Client* target = server.getClientByNick(targetNick);
    if (!target) {
        server.sendToClient(client.getFd(), ":ircserv 401 " + targetNick + " :No such nick\r\n");
        return;
    }

    chan->addInvite(target);

    // Notificar al invitado
    server.sendToClient(target->getFd(),
        client.getPrefix() + " INVITE " + targetNick + " :" + chanName + "\r\n");

    // Confirmar al que invita
    server.sendToClient(client.getFd(),
        ":ircserv 341 " + client.getNickname() + " " + targetNick + " " + chanName + "\r\n");
}
```

### TOPIC

```cpp
void CommandHandler::handleTopic(Client& client, const std::string& params, Server& server) {
    if (!client.isAuthenticated()) return;

    std::vector<std::string> args = splitParams(params);
    if (args.empty()) {
        server.sendToClient(client.getFd(), ":ircserv 461 TOPIC :Not enough parameters\r\n");
        return;
    }

    std::string chanName = args[0];
    Channel* chan = server.getChannel(chanName);
    if (!chan) {
        server.sendToClient(client.getFd(), ":ircserv 403 " + chanName + " :No such channel\r\n");
        return;
    }
    if (!chan->isMember(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 442 " + chanName + " :You're not on that channel\r\n");
        return;
    }

    // Sin trailing → consultar topic
    size_t colonPos = params.find(':');
    if (colonPos == std::string::npos) {
        if (chan->getTopic().empty())
            server.sendToClient(client.getFd(),
                ":ircserv 331 " + client.getNickname() + " " + chanName + " :No topic is set\r\n");
        else
            server.sendToClient(client.getFd(),
                ":ircserv 332 " + client.getNickname() + " " + chanName + " :" + chan->getTopic() + "\r\n");
        return;
    }

    // Con trailing → cambiar topic
    if (chan->isTopicRestricted() && !chan->isOperator(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 482 " + chanName + " :You're not channel operator\r\n");
        return;
    }

    std::string newTopic = getTrailing(params);
    chan->setTopic(newTopic);

    // Notificar a todos del canal
    chan->broadcast(client.getPrefix() + " TOPIC " + chanName + " :" + newTopic + "\r\n");
}
```

---

## 18. Paso 13 — MODE (los 5 modos)

MODE es el comando más complejo. Hay que manejar `+` y `-` para cada modo.

```cpp
void CommandHandler::handleMode(Client& client, const std::string& params, Server& server) {
    if (!client.isAuthenticated()) return;

    std::vector<std::string> args = splitParams(params);
    if (args.empty()) {
        server.sendToClient(client.getFd(), ":ircserv 461 MODE :Not enough parameters\r\n");
        return;
    }

    std::string target  = args[0];
    std::string modeStr = (args.size() > 1) ? args[1] : "";
    std::string arg     = (args.size() > 2) ? args[2] : "";

    // Solo manejamos modos de canal
    if (target[0] != '#') return;

    Channel* chan = server.getChannel(target);
    if (!chan) {
        server.sendToClient(client.getFd(), ":ircserv 403 " + target + " :No such channel\r\n");
        return;
    }

    // Consulta de modos (sin modeStr)
    if (modeStr.empty()) {
        // Responder con los modos actuales
        server.sendToClient(client.getFd(),
            ":ircserv 324 " + client.getNickname() + " " + target + " +\r\n");
        return;
    }

    // Solo operadores pueden cambiar modos
    if (!chan->isOperator(&client)) {
        server.sendToClient(client.getFd(),
            ":ircserv 482 " + target + " :You're not channel operator\r\n");
        return;
    }

    bool adding = true;
    std::string appliedModes = "";
    std::string appliedArgs  = "";

    for (size_t i = 0; i < modeStr.size(); i++) {
        char m = modeStr[i];

        if (m == '+') { adding = true;  continue; }
        if (m == '-') { adding = false; continue; }

        switch (m) {
            case 'i':  // invite-only
                chan->setInviteOnly(adding);
                appliedModes += m;
                break;

            case 't':  // topic restringido a operadores
                chan->setTopicRestricted(adding);
                appliedModes += m;
                break;

            case 'k':  // contraseña del canal
                if (adding && arg.empty()) break;
                chan->setKey(adding ? arg : "");
                appliedModes += m;
                if (adding) appliedArgs += " " + arg;
                break;

            case 'l':  // límite de usuarios
                if (adding) {
                    if (arg.empty()) break;
                    int limit = atoi(arg.c_str());
                    if (limit > 0) {
                        chan->setUserLimit(limit);
                        appliedModes += m;
                        appliedArgs += " " + arg;
                    }
                } else {
                    chan->setUserLimit(-1);
                    appliedModes += m;
                }
                break;

            case 'o':  // dar/quitar op a usuario
                if (arg.empty()) break;
                {
                    Client* targetClient = server.getClientByNick(arg);
                    if (targetClient && chan->isMember(targetClient)) {
                        if (adding) chan->addOperator(targetClient);
                        else        chan->removeOperator(targetClient);
                        appliedModes += m;
                        appliedArgs  += " " + arg;
                    }
                }
                break;
        }
    }

    // Notificar cambio de modos a todos en el canal
    if (!appliedModes.empty()) {
        std::string modeChange = client.getPrefix() + " MODE " + target
            + " " + (adding ? "+" : "-") + appliedModes + appliedArgs + "\r\n";
        chan->broadcast(modeChange);
    }
}
```

---

## 19. Paso 14 — Señales y cierre limpio

### En main.cpp

```cpp
#include "Server.hpp"
#include <csignal>
#include <iostream>

bool g_running = true;

void signalHandler(int sig) {
    (void)sig;
    g_running = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT,  signalHandler);  // Ctrl+C
    signal(SIGQUIT, signalHandler);  // Ctrl+\
    signal(SIGPIPE, SIG_IGN);        // ignorar broken pipe (cliente desconectado)

    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    try {
        Server server(atoi(argv[1]), argv[2]);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### En Server.hpp — exponer g_running

```cpp
extern bool g_running;
```

### En Server::run() — cambiar el bucle

```cpp
while (g_running) {
    int ret = poll(_pollfds.data(), _pollfds.size(), 500); // timeout 500ms para revisar g_running
    if (ret < 0 && g_running)
        throw std::runtime_error("poll() failed");
    // ...resto del loop
}
```

---

## 20. Códigos de respuesta IRC necesarios

| Código | Nombre | Cuándo enviarlo |
|--------|--------|----------------|
| 001 | RPL_WELCOME | Autenticación completada |
| 002 | RPL_YOURHOST | Autenticación completada |
| 003 | RPL_CREATED | Autenticación completada |
| 004 | RPL_MYINFO | Autenticación completada |
| 331 | RPL_NOTOPIC | JOIN o TOPIC sin topic |
| 332 | RPL_TOPIC | JOIN o TOPIC con topic |
| 341 | RPL_INVITING | INVITE exitoso |
| 401 | ERR_NOSUCHNICK | PRIVMSG a nick inexistente |
| 403 | ERR_NOSUCHCHANNEL | Canal inexistente |
| 404 | ERR_CANNOTSENDTOCHAN | No eres miembro del canal |
| 421 | ERR_UNKNOWNCOMMAND | Comando desconocido |
| 431 | ERR_NONICKNAMEGIVEN | NICK sin parámetro |
| 433 | ERR_NICKNAMEINUSE | Nick ya en uso |
| 441 | ERR_USERNOTINCHANNEL | KICK a alguien que no está |
| 442 | ERR_NOTONCHANNEL | Comando en canal donde no estás |
| 461 | ERR_NEEDMOREPARAMS | Faltan parámetros |
| 462 | ERR_ALREADYREGISTERED | Ya autenticado |
| 464 | ERR_PASSWDMISMATCH | Contraseña incorrecta |
| 471 | ERR_CHANNELISFULL | Canal lleno (modo l) |
| 473 | ERR_INVITEONLYCHAN | Canal invite-only (modo i) |
| 475 | ERR_BADCHANNELKEY | Contraseña del canal incorrecta |
| 482 | ERR_CHANOPRIVSNEEDED | No eres operador del canal |

---

## 21. Testing paso a paso

### Fase 1 — Conexión básica con nc

```bash
# Terminal 1: arrancar el servidor
./ircserv 6667 mipass

# Terminal 2: conectar con nc
nc -C 127.0.0.1 6667

# Escribir manualmente:
PASS mipass
NICK testuser
USER testuser 0 * :Test User
# Deberías recibir los mensajes 001, 002, 003, 004
```

### Fase 2 — Test de buffer fragmentado

```bash
nc -C 127.0.0.1 6667
# Escribe: PAS (ctrl+D)
# Escribe: S m (ctrl+D)
# Escribe: ipass (enter)
# El servidor debe procesar PASS mipass correctamente
```

### Fase 3 — Dos clientes con irssi

```bash
# Terminal 1: servidor
./ircserv 6667 mipass

# Terminal 2: cliente A
irssi
/connect 127.0.0.1 6667 mipass
/nick alice
/join #test
/msg #test hola desde alice

# Terminal 3: cliente B
irssi
/connect 127.0.0.1 6667 mipass
/nick bob
/join #test
/msg #test hola desde bob
# alice debe ver el mensaje de bob y viceversa
```

### Fase 4 — Test de operadores

```bash
# alice entra primero (se vuelve op automáticamente)
# bob entra después
/mode #test +o bob       # alice da op a bob
/kick #test bob :adiós   # alice expulsa a bob
/invite bob #test        # alice invita a bob
/mode #test +i           # poner canal invite-only
/mode #test +k secreto   # poner contraseña
/mode #test +l 5         # límite de 5 usuarios
/topic #test :nuevo tema # cambiar topic
```

### Fase 5 — Test de robustez

```bash
# Desconectar de golpe (ctrl+C en irssi)
# Enviar mensajes muy largos
# Conectar muchos clientes simultáneos
# Intentar comandos sin autenticar
# Intentar comandos de operador sin serlo
```

---

## 22. Errores comunes y cómo evitarlos

### ❌ No acumular el buffer
El error más frecuente. Procesar `recv()` directamente sin buffer provocará que los mensajes fragmentados se ignoren o rompan el parseo.

**Solución:** Siempre acumular en `client._buffer` y buscar `\r\n` antes de procesar.

### ❌ Invalidar iteradores en el loop de poll
Hacer `push_back` en `_pollfds` dentro del bucle `for` con iterador lo invalida.

**Solución:** Usar índice entero en el bucle, o procesar las nuevas conexiones después del bucle.

### ❌ No poner los fds en non-blocking
Si `recv()` bloquea, el servidor se congela esperando datos de un cliente y los demás no son atendidos.

**Solución:** `fcntl(fd, F_SETFL, O_NONBLOCK)` tanto para el fd del servidor como para cada cliente aceptado.

### ❌ No ignorar SIGPIPE
Si envías datos a un cliente que se desconectó, el OS envía `SIGPIPE` que termina el proceso.

**Solución:** `signal(SIGPIPE, SIG_IGN)` en `main`.

### ❌ Usar `atoi()` para el modo `+l` sin validar
`atoi("abc")` devuelve 0 sin error. Un límite de 0 bloquearía todas las entradas.

**Solución:** Validar que el resultado de `atoi` sea > 0 antes de aplicarlo.

### ❌ No limpiar los canales vacíos
Con muchos joins y parts, los canales vacíos se acumulan en memoria.

**Solución:** En `removeMember`, si el canal queda vacío, eliminarlo del map y hacer `delete`.

### ❌ No enviar `\r\n` al final de cada respuesta
irssi y otros clientes esperan CRLF. Sin él, el cliente no procesa la respuesta.

**Solución:** Asegurarse de que cada `sendToClient()` termina en `\r\n`.

---

## 23. División de trabajo para el equipo

| Persona A | Persona B |
|-----------|-----------|
| `Server.hpp/cpp` | `Channel.hpp/cpp` |
| `Client.hpp/cpp` | `CommandHandler.hpp/cpp` |
| `main.cpp` + Makefile | Señales + cierre limpio |
| Inicialización de socket | Autenticación (PASS, NICK, USER) |
| Loop de poll() | JOIN + PRIVMSG |
| accept() + recv() | KICK + INVITE + TOPIC |
| removeClient() | MODE (los 5 modos) |
| Testing con nc | Testing con irssi |

### Puntos de sincronización (hacer juntos)

1. Definir las interfaces de las clases antes de separar el trabajo
2. Acordar el formato de los mensajes de error
3. Probar el handshake completo (PASS+NICK+USER) juntos antes de avanzar
4. Testing cruzado: cada uno prueba el código del otro

---

## 24. Orden de implementación sugerido

```
Sprint 1 — Fundación de red
  ✓ main.cpp con validación de argumentos
  ✓ Server: socket → bind → listen → poll básico
  ✓ Aceptar conexiones (accept + pollfd)
  ✓ recv() con buffer acumulativo
  ✓ Verificar con: nc -C 127.0.0.1 6667

Sprint 2 — Autenticación
  ✓ CommandHandler: parseo de líneas
  ✓ PASS, NICK, USER
  ✓ Mensajes de bienvenida 001-004
  ✓ Verificar con irssi: /connect 127.0.0.1 6667 pass

Sprint 3 — Canales básicos
  ✓ Clase Channel completa
  ✓ JOIN con primer miembro = operador
  ✓ PRIVMSG a canal y a usuario
  ✓ QUIT y removeClient limpio
  ✓ Verificar con 2 clientes irssi chateando

Sprint 4 — Comandos de operador
  ✓ KICK
  ✓ INVITE
  ✓ TOPIC (con y sin modo +t)
  ✓ Verificar con irssi

Sprint 5 — MODE
  ✓ +i / -i (invite-only)
  ✓ +t / -t (topic restringido)
  ✓ +k / -k (contraseña)
  ✓ +l / -l (límite)
  ✓ +o / -o (operador)

Sprint 6 — Robustez y cierre
  ✓ Señales (SIGINT, SIGPIPE)
  ✓ Limpiar canales vacíos
  ✓ Testing exhaustivo de casos borde
  ✓ Revisión de fugas de memoria (valgrind)
  ✓ Test de buffer fragmentado con nc
```

---

*Guía generada para ft_irc — 42 School · C++ 98 · Linux · irssi*
