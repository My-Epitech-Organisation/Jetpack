#ifndef JETPACK_CROSS_PLATFORM_HPP
#define JETPACK_CROSS_PLATFORM_HPP

// Détection du système d'exploitation
#ifdef _WIN32
    // Inclusions Windows
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    
    // Remplacements pour les fonctions POSIX
    #define close closesocket
    typedef int socklen_t;
#else
    // Inclusions UNIX/Linux
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>
#endif

// Fonctions d'initialisation/nettoyage pour Windows
#ifdef _WIN32
inline bool initializeWinsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

inline void cleanupWinsock() {
    WSACleanup();
}
#else
inline bool initializeWinsock() { return true; }
inline void cleanupWinsock() {}
#endif

#endif // JETPACK_CROSS_PLATFORM_HPP