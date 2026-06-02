*This project has been created as part of the 42 curriculum by erico-ke, fracurul.*

## Description
ft_irc is a minimalist IRC server written in C++. It implements core IRC behavior such as
client connections, channels, and a subset of IRC commands to allow users to chat in
real time. The goal is to practice networking, protocol handling, and robust server-side
design. It focuses on a clean event loop, message parsing, and managing client and
channel state in a way that mirrors real IRC usage while keeping the scope approachable.

## Instructions
Build and run the server from the project root. The binary is named `ircserv` and expects
the port to listen on and the server password.

### Build
```sh
make
```

### Run
```sh
./ircserv <port> <password>
```

For example, you can start the server on port 6667 with a simple password:
```sh
./ircserv 6667 secret
```

### Example client
We used [HexChat](https://hexchat.github.io/) as the example client for manual testing
and basic usage. Connect to `localhost` on the chosen port and set the server password
in the connection settings. Once connected, join a channel like `#general` to chat.

### Clean
```sh
make clean
make fclean
```

## Commands and expected input
Below is a quick reference of the IRC commands supported by this project and the
expected input format. Parameters in angle brackets are required, and square brackets
are optional.

- PASS `<password>`
- NICK `<nickname>`
- USER `<username> 0 * :<realname>`
- JOIN `<#channel>`
- TOPIC `<#channel> [:<topic>]`
- INVITE `<nickname> <#channel>`
- KICK `<#channel> <nickname> [:<reason>]`
- MODE `<#channel> <modes> [mode-params...]`
- PRIVMSG `<target> :<message>` (target is a nickname or `#channel`)
- WHO `[<mask>]`
- QUIT `[:<message>]`

## Resources
- [RFC 1459: Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459)
- [RFC 2812: Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/rfc/rfc2812)
- [RFC 2813: Internet Relay Chat: Server Protocol](https://www.rfc-editor.org/rfc/rfc2813)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

