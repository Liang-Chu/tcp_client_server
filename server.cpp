#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cctype>
#include <sys/select.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;
constexpr char QUIT_KEY = 'Q';

// Function to handle each client connection
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Echo back client messages
    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the received string
        std::cout << "Received from client: " << buffer << '\n';
        send(client_socket, buffer, bytes_read, 0);
    }

    std::cout << "Client disconnected"<< std::endl;
    close(client_socket);
}

// Function to detect if the quit key is pressed
bool quitKeyPressed() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch == QUIT_KEY || ch == tolower(QUIT_KEY);
}

int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return EXIT_FAILURE;
    }

    // Set the socket to non-blocking
    int flags = fcntl(server_socket, F_GETFL, 0);
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

    // Bind the socket to a specific IP and port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) < 0) {
        std::cerr << "Failed to listen" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Server is listening on port " << PORT << ", press " << QUIT_KEY << " to quit"<< std::endl;

    std::vector<std::thread> threads;
    fd_set read_fds;
    struct timeval timeout;

    // Accept and handle multiple clients in a loop. quit when quitkey is detected
    while (!quitKeyPressed()) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);

        // Set a timeout for the select call
        timeout.tv_sec = 0.1; // 0.1 second
        timeout.tv_usec = 0;

        int activity = select(server_socket + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0) {
            std::cerr << "Select error" << std::endl;
            continue;
        }

        if (FD_ISSET(server_socket, &read_fds)) {
            int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
            if (client_socket < 0) {
                continue; // No clients are connecting; continue to the next iteration
            }

            std::cout << "Client connected" << std::endl;

            // Create a new thread to handle the client
            threads.push_back(std::thread(handle_client, client_socket));
        }
    }

    // Close the socket
    std::cout << "\nQuitting server..."<< std::endl;
    close(server_socket);

    // Terminate any ongoing client threads
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.detach(); 
        }
    }

    return EXIT_SUCCESS; // Gracefully terminate the program
}
