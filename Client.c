#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <IP> <Port> <Nom> <Prenom> <Matiere>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    char *nom = argv[3];
    char *prenom = argv[4];
    int matiere = atoi(argv[5]);

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};

    // Création de la socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur de création de la socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide ou non supportée");
        close(sockfd);
        exit(1);
    }

    // Connexion au serveur
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de connexion");
        close(sockfd);
        exit(1);
    }

    // Envoi de la requête
    snprintf(buffer, sizeof(buffer), "%s %s %d", nom, prenom, matiere);
    send(sockfd, buffer, strlen(buffer), 0);

    // Réception de la réponse
    int valread = read(sockfd, buffer, sizeof(buffer) - 1);
    if (valread > 0) {
        buffer[valread] = '\0';
        printf("Réponse du serveur : %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
