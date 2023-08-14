#include <iostream>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cctype>
#include <sstream>
#include <iomanip>

constexpr int BUFFER_SIZE = 1024;
constexpr int SLEEP_TIME=1;
constexpr char QUIT_KEY = 'Q';

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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>"<< std::endl;
        return EXIT_FAILURE;
    }

    const char *server_ip = argv[1];
    int server_port = std::stoi(argv[2]);
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Failed to create socket"<< std::endl;
        return EXIT_FAILURE;
    }

    // Set up the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address"<< std::endl;
        return EXIT_FAILURE;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to connect to server"<< std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Connected to server, press " << QUIT_KEY << " to quit"<< std::endl;

    // Periodically send the current time and date and print server's response
    while (!quitKeyPressed()) {
        // Get the current time
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        // Convert to a human-readable string
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
        std::string message = "Current time: " + ss.str();

        ssize_t bytes_sent = send(client_socket, message.c_str(), message.length(), 0);
        if (bytes_sent < 0) {
            std::cerr << "Failed to send message"<< std::endl;
            break;
        }

        ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            std::cerr << "Server disconnected or failed to receive message"<< std::endl;
            break;
        }

        buffer[bytes_read] = '\0'; // Null-terminate the received string
        std::cout << "Received from server: " << buffer<< std::endl;

        // Wait for one second before sending the next message
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));
    }

    std::cout << "\nQuitting client..." << std::endl;
    close(client_socket);

    return 0;
}
