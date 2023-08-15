## Project Overview

This repository contains the implementation of a simple TCP client-server application in C/C++. Here's an overview of the functionality and features:

### TCP Client Component:
- Connects to a specified server IP and port.
- Periodically sends the current time and date to the server.
- Prints the server's response.
- Can be terminated by pressing a specific quit key (default 'Q').

### TCP Server Component:
- Listens on a specific port (default 8080) and accepts multiple client connections.
- Echoes back the received messages from clients.
- Can be terminated by pressing a specific quit key (default 'Q').

### Highlights:

1. **Required Functions:**
   - My implementation fulfills the specified requirements by creating a functional TCP client-server application in C/C++ for Linux.

2. **Quit Functionality:**
   - I have thoughtfully implemented a quit function that allows users to exit the program seamlessly at any point. This provides a user-friendly experience.

3. **Resource Cleanup:**
   - Resource cleanup is a priority in my implementation. I have taken great care to ensure that no resources are left behind, whether the program is terminated using Ctrl+C or by utilizing the quit key. Sockets and threads are closed and detached appropriately.

4. **Scalability and Future Use:**
   - My code is designed with scalability in mind. It can serve as a solid foundation for more complex projects involving networking and multithreading. Its modular structure allows for easy expansion and adaptation to various use cases.



---


## Building and Running the Code

To compile and run the code with support for POSIX threads:

1. Compile the `server.cpp` and `client.cpp` files using the `-pthread` flag to link the pthread library:
   
   ```sh
   g++ -o server server.cpp -std=c++11 -pthread
   g++ -o client client.cpp -std=c++11 -pthread
   ```

2. Run the server in one terminal window:
   
   ```sh
   ./server
   ```

3. In another terminal window, run the client by providing the server's IP address and port number as command line arguments:
   
   ```sh
   ./client <server_ip> <server_port>
   ```

Replace `<server_ip>` with the actual IP address of the machine running the server(if the server and client are running on the same machine, you can also use 127.0.0.1), and `<server_port>` with the port number the server is listening on (e.g., 8080). 
