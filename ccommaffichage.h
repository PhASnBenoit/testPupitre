#ifndef CCOMMAFFICHAGE_H
#define CCOMMAFFICHAGE_H

#include <QObject>
#include <QtSerialPort>

#define ID "00"  // tous les afficheurs

class CCommAffichage : public QObject
{
    Q_OBJECT
public:
    explicit CCommAffichage(char modeFinJeu = 'S', QObject *parent = nullptr);
    ~CCommAffichage();
    void afficherBienvenue(uint duree);
    void afficherTypeJeu(uint duree);
    void afficherQuelJoueur();
    void afficherTextePermanent(QString texte);
    void afficherPlusMoins();

private:
    char _modeFinJeu; // pour savoir si score ou temps
    QSerialPort _sp;  // communication avec l'afficheur
    QByteArray trame;
    int initPortSerie(); // init du port série afficheur
    uint8_t calculerCS(); // Cheksum
   // CRs232 *_
   unsigned char calculerChecksum(char *trame);
   void afficher(char *prot, char *mess);

public slots:
    void on_afficherScores(uint8_t aQuiLeTour);
    void on_afficherMenu();
    void on_afficherSortieMenu(uint duree);
    void on_afficherMenuSelected(uint duree, QString texte);
};

#endif // CCOMMAFFICHAGE_H
