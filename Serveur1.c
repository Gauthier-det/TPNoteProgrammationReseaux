#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>

#define PORT 6000
#define MAX_ETU 50
#define NB_MATIERES 5

//création d'une structure Etudiant
typedef struct {
    char nom[50];
    char prenom[50];
    double moyennes[NB_MATIERES];
} Etudiant;

//Variables globales
Etudiant etudiants[MAX_ETU];
int nb_etudiants = 0;

// Initialisation du premier étudiant
void init_etudiants() {
    strcpy(etudiants[0].nom, "LeBarbier");
    strcpy(etudiants[0].prenom, "Raphael");
    etudiants[0].moyennes[0] = 2.5;
    etudiants[0].moyennes[1] = 2.0;
    etudiants[0].moyennes[2] = 3.5;
    etudiants[0].moyennes[3] = 1.0;
    etudiants[0].moyennes[4] = 5.0;
    nb_etudiants = 1;
}

//Ajout d'un Etudiant dans le tableau
void ajoute_etudiant(char *nom, char *prenom, float notes[5]){
    if(nb_etudiants == 50){
        printf("Trop d'étudiants.");
        return;
    }
    strcpy(etudiants[nb_etudiants].nom, nom);
    strcpy(etudiants[nb_etudiants].prenom, prenom);
    for(int i = 0; i < NB_MATIERES; i++){
        etudiants[nb_etudiants].moyennes[i] = notes[i];
    }
    nb_etudiants += 1;
}

// Recherche d'un etudiant du tableau
int trouver_etudiant(char *nom, char *prenom) {
    for (int i = 0; i < nb_etudiants; i++) {
        if (strcmp(etudiants[i].nom, nom) == 0 && strcmp(etudiants[i].prenom, prenom) == 0) {
            return i;
        }
    }
    return -1;
}

//recuperation ip 
char hostname[NI_MAXHOST];
void recup_ip(){
    struct ifaddrs *addr, *intf;
    int family, s;
    int compteur = 0;
    if (getifaddrs(&intf) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    for (addr = intf; addr != NULL; addr = addr->ifa_next) {
        family = addr->ifa_addr->sa_family;
        if (family == AF_INET) {
            s = getnameinfo(addr->ifa_addr, 
                          sizeof(struct sockaddr_in),
                          hostname, 
                          NI_MAXHOST, 
                          NULL, 
                          0, 
                          NI_NUMERICHOST);
            compteur +=1;
            if(compteur == 2){
                return;
            }
        }
    }
}


int main() {
    int server_fd, socket_s;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    // Initialisation des données
    init_etudiants();
    float notes[] = {20.0,20.0,20.0,20.0,20.0};
    ajoute_etudiant("Detroussel", "Gauthier", notes);
    float notes2[] = {20.0,2.0,20.0,2.0,2.0};
    ajoute_etudiant("Lebon", "Lucas", notes2);
    float notes3[] = {40.0,40.0,60.0,70.0,100.0};
    ajoute_etudiant("Yaffa", "Elie", notes3);
    
    // Création du socket
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

    // Mise en écoute du serveur
    if (listen(server_fd, 3) < 0) {
        perror("Erreur d'écoute (listen)");
        close(server_fd);
        exit(1);
    }

    recup_ip();
    printf("Serveur en écoute sur le port %d, à l'adresse %s...\n" , PORT, hostname);

    //Attente de connexion
    while (1) {
        if ((socket_s = accept(server_fd, NULL, NULL)) < 0) {
            perror("Erreur d'acceptation");
            continue;
        }

        for(int i = 0; i < 1024; i++){
            buffer[i] = 0;
        }
        // Réception de la requête
        read(socket_s, buffer, sizeof(buffer));
        printf("Requête reçue : %s\n", buffer);

        char nom[50], prenom[50];
        int matiere;
        sscanf(buffer, "%s %s %d", nom, prenom, &matiere);

        //creation de la reponse
        char response[1024];
        int index = trouver_etudiant(nom, prenom); //recherche etudiant
        if (index == -1 || matiere < 0 || matiere >= NB_MATIERES) {
            strcpy(response, "Erreur : Étudiant ou matière introuvable.");
        } else {
            sprintf(response, "Moyenne : %.2f", etudiants[index].moyennes[matiere]);
        }

        // Envoi de la réponse
        send(socket_s, response, strlen(response), 0);
        close(socket_s);
    }

    close(server_fd);
    return 0;
}