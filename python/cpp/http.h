// This is so damn ugly. Don't even forgive me I'm ashamed.
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

// Super hard-coded "HTTP" server that answers to the challenges.
const int BUFFER_SIZE = 2 * 1024 * 1024;
struct HttpServer {

    HttpServer(int port);

    // Returns whether we have a challenge to solve or not.
    bool wait_for_client();
    void close_conn();

    std::string_view read_chal();
    void send_content(const std::string_view content);

private:
    // Send headers for a HTTP 200 response.
    void send_200_headers(std::string_view::size_type contentLen);

private:
    // Server data
    int fd_;
    struct sockaddr_in address_;
    int addrlen_;

    // Client data (only one client at a time)
    int client_socket_;
    char buffer_[BUFFER_SIZE];
};


HttpServer::HttpServer(int port) {
    addrlen_ = sizeof(address_);

    if ((fd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Error in socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port);

    memset(address_.sin_zero, '\0', sizeof address_.sin_zero);

    if (bind(fd_, (struct sockaddr *)&address_, sizeof(address_))<0) {
        std::cerr << "In bind" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(fd_, 10) < 0) {
        std::cerr << "In listen" << std::endl;
        exit(EXIT_FAILURE);
    }
}

bool HttpServer::wait_for_client() {
    if ((client_socket_ = accept(fd_, (struct sockaddr *)&address_,
                                 (socklen_t*)&addrlen_)) < 0) {
        std::cerr << "In accept" << std::endl;
        exit(EXIT_FAILURE);
    }
    // TODO: doesn't look like this is helping...
    const int sndsize = 8 * 1024 * 1024;
    if (setsockopt(client_socket_, SOL_SOCKET, SO_SNDBUF, (char *)&sndsize,
                   (int)sizeof(sndsize)) < 0) {
        std::cerr << "sndsize" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int rcvsize = 8 * 1024 * 1024;  
    if (setsockopt(client_socket_, SOL_SOCKET, SO_RCVBUF, (char *)&rcvsize,
                   (int)sizeof(rcvsize)) < 0) {
        std::cerr << "rcvsize" << std::endl;
        exit(EXIT_FAILURE);
    }
    read(client_socket_, buffer_, 4);
    if (buffer_[0] == 'G') {  // Is this a GET? Just 200 OK and ignore
        send_content(std::string{});
        return false;  // no challenge
    }
    return true;  // Assume that this is a POST (challenge).
}

void HttpServer::close_conn() {
    close(client_socket_);
}

std::string_view HttpServer::read_chal() {
    long valread = read(client_socket_, buffer_, BUFFER_SIZE);
    buffer_[valread] = '\0'; // useful to debug print
    const char* contentLenStr = "Content-Length: ";
    char* ptr = strstr(buffer_, contentLenStr);
    ptr += strlen(contentLenStr);
    std::string_view::size_type contentLen;
    sscanf(ptr, "%ld", &contentLen);
    ptr = strstr(ptr, "\r\n\r\n"); // search for the end of headers
    ptr += 4; // skip \r\n\r\n
    const char* content = ptr; // this is where the json starts
    long remainingContent = contentLen;
    valread -= (content - buffer_);
    if (valread > 0) {
        remainingContent -= valread;
        ptr += valread;
    }
    while (remainingContent > 0) {
        // note this can totally overflow, but whatever
        if ((valread = read(client_socket_, ptr, remainingContent)) <= 0) {
            std::cerr << "Err? Conn closed? " << valread << std::endl;
            break;
        }
        remainingContent -= valread;
        ptr += valread;
    }
    if (remainingContent > 0) exit(EXIT_FAILURE);
    return std::string_view{content, contentLen};
}

void HttpServer::send_200_headers(std::string_view::size_type contentLen) {
    const char* ok200start = "HTTP/1.1 200 OK\r\nContent-Length: ";
    write(client_socket_, ok200start, strlen(ok200start));
    char outlen_buf[1024];
    sprintf(outlen_buf, "%ld", contentLen);
    write(client_socket_, outlen_buf, strlen(outlen_buf));
    const char* ok200end = "\r\n\r\n";
    write(client_socket_, ok200end, strlen(ok200end));
}

void HttpServer::send_content(const std::string_view content) {
    send_200_headers(content.size());
    write(client_socket_, content.data(), content.size());
}