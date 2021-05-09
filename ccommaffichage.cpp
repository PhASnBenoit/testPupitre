#include "ccommaffichage.h"
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>

#define DEBUG_PANNEAU

#define PORT            "/dev/ttyUSB0"

#define LG_MAX_TRAME    500
#define LG_MAX          150
#define LG_REPONSE      4 // au maximum 4 caractères pour NACK

// cf. man ascii
#define NUL             0x00 // caractère NUL (c'est aussi le code du fin de chaîne)
#define ACK             0x06 // accusé réception positif
#define NACK            0x15 // accusé réception négatif

#define DELAI           1000000 // en micro secondes

CCommAffichage::CCommAffichage(char modeFinJeu, QObject *parent) : QObject(parent)
{
    _modeFinJeu = modeFinJeu;
    _sp.setPortName("/dev/ttyUB0");
    if (_sp.open(QSerialPort::WriteOnly))
        qDebug()<<"Port ouvert !";
    else
        qDebug()<<"Erreur d'ouverture de port !";
}

CCommAffichage::~CCommAffichage(){
}

void CCommAffichage::afficher(char *protocole, char *message)
{
    char trame[LG_MAX_TRAME*2] = { NUL };
    unsigned char checksum;

    // 0. on ajoute le message dans l'en-tête du protocole
    strcat(protocole, message);
    // 1. on calcule le checksum de la trame
    checksum = calculerChecksum(protocole);
    // 2. on fabrique la trame
    sprintf(trame, "<ID01>%s%02X<E>", protocole, checksum);
    // 3 on envoie la trame
    _sp.write(trame,strlen(trame));
}

void CCommAffichage::afficherBienvenue(uint duree)
{
    //Exemple de trame : "<ID01><L1><PA><FE><MA><WC><FE>message1F<E>"
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole, "<L1><PA><FR><MQ><WD><FG><CB><AC><CA>");
    sprintf(message, "RushBall 1.0 2021");
    afficher(protocole, message);

    sleep(duree);
}

void CCommAffichage::afficherTypeJeu(uint duree)
{
    //Exemple de trame : "<ID01><L1><PA><FE><MA><WC><FE>message1F<E>"
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    // A FAIRE MODIF SELON TYPE DE JEU
    sprintf(protocole, "<L1><PA><FR><MQ><WD><FG><CB><AC><CA>");
    sprintf(message, "Type de jeu");
    afficher(protocole, message);

    sleep(duree);
}

void CCommAffichage::on_afficherMenu()
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole,"<L1><PA><FE><MQ><WD><FG><CB><AC>Scores: 1 <CD>Penalité: 2 <CH>A qui le tour: 3 <CS>");
    sprintf(message, "Correction: 4 ");
    afficher(protocole, message);
}

void CCommAffichage::afficherPlusMoins()
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole, "<L1><PA><FE><MA><WB><FG><CB><AC><CA>");
    sprintf(message, " - ou + pour naviguer dans les scores"); // attention : longueur max du message égale à LG_MAX-1
    afficher(protocole, message);
}

void CCommAffichage::on_afficherMenuSelected(uint duree, QString texte)
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole, "<L1><PA><FE><MA><WB><FG><CB><AC><CA>");
    sprintf(message, "%s", texte.toStdString().c_str());
    afficher(protocole, message);

   if (duree >0)
       sleep(duree);
}

void CCommAffichage::afficherQuelJoueur()
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole, "<L1><PA><FE><MQ><WD><FG><CB><AC>Touche 1=P1 /<CD> 2=P2 /<CH> 3=P3 /<CS>");
    sprintf(message," 4=P4");
    afficher(protocole, message);
}

void CCommAffichage::afficherTextePermanent(QString texte)
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole,"<L1><PA><FE><MQ><WD><FG><CB><AC><CS>");
    sprintf(message,"%s",texte.toStdString().c_str());
    afficher(protocole, message);
}

void CCommAffichage::on_afficherSortieMenu(uint duree)
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    sprintf(protocole,"<L1><PA><FE><MQ><WD><FG><CB><AC><CS>");
    sprintf(message,"Sortie menu");
    afficher(protocole, message);
    usleep(duree);
}

void CCommAffichage::on_afficherScores(uint8_t aQuiLeTour)
{
    char protocole[LG_MAX_TRAME];
    char message[LG_MAX];

    // PREVOIR DE METTRE EN EVIDENCE A QUI LE TOUR
    sprintf(protocole,"<L1><PA><FE><MQ><WD><FG><CB><AC>P1=0<CD>P2=8<CH>P3=2<CS>");
    sprintf(message,"P4=5");
    afficher(protocole, message);
}


    unsigned char CCommAffichage::calculerChecksum(char *trame)
    {
       unsigned char checksum = 0;
       uint i = 0;

       #ifdef DEBUG_PANNEAU
       printf("data packet :\t");
       for(i=0 ; i<strlen(trame) ; i++)
          printf("0x%02X ", trame[i]);
       printf("\n");
       #endif

       for(i=0 ; i<strlen(trame) ; i++)
          checksum ^= trame[i];

       #ifdef DEBUG_PANNEAU
       printf("checksum :\t0x%02X\n", checksum);
       #endif

       return checksum;
    }
