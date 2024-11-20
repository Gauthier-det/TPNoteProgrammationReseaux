#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000
#define MAX_ETUDIANTS 50
#define NB_MATIERES 5

typedef struct {
    char nom[50];
    char prenom[50];
    double moyennes[NB_MATIERES];
} Etudiant;

Etudiant etudiants[MAX_ETUDIANTS];
int nb_etudiants = 0;

// Initialisation des données
void init_etudiants() {
    strcpy(etudiants[0].nom, "Dupont");
    strcpy(etudiants[0].prenom, "Jean");
    etudiants[0].moyennes[0] = 14.5;
    etudiants[0].moyennes[1] = 12.0;
    etudiants[0].moyennes[2] = 13.5;
    etudiants[0].moyennes[3] = 10.0;
    etudiants[0].moyennes[4] = 15.0;
    nb_etudiants = 1;
}

// Recherche de l'étudiant
int trouver_etudiant(char *nom, char *prenom) {
    for (int i = 0; i < nb_etudiants; i++) {
        if (strcmp(etudiants[i].nom, nom) == 0 && strcmp(etudiants[i].prenom, prenom) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    // Initialisation des données
    init_etudiants();

    // Création de la socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erreur de création de la socket");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Liaison au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erreur de liaison (bind)");
        close(server_fd);
        exit(1);
    }

    // En écoute
    if (listen(server_fd, 3) < 0) {
        perror("Erreur d'écoute (listen)");
        close(server_fd);
        exit(1);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    while (1) {
        // Acceptation d'une connexion
        if ((new_socket = accept(server_fd, NULL, NULL)) < 0) {
            perror("Erreur d'acceptation");
            continue;
        }

        // Réception de la requête
        read(new_socket, buffer, sizeof(buffer));
        printf("Requête reçue : %s\n", buffer);

        char nom[50], prenom[50];
        int matiere;
        sscanf(buffer, "%s %s %d", nom, prenom, &matiere);

        char response[1024];
        int index = trouver_etudiant(nom, prenom);
        if (index == -1 || matiere < 0 || matiere >= NB_MATIERES) {
            strcpy(response, "Erreur : Étudiant ou matière introuvable.");
        } else {
            sprintf(response, "Moyenne : %.2f", etudiants[index].moyennes[matiere]);
        }

        // Envoi de la réponse
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}