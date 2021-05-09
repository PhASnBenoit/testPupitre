#ifndef CGERERPUPITRE_H
#define CGERERPUPITRE_H

#include <QObject>
#include <QTextStream>

#include "communs.h"

class CGererPupitre : public QObject
{
    Q_OBJECT
public:
    explicit CGererPupitre(QObject *parent = nullptr);
    ~CGererPupitre();
    void on_toucheRecue(int touche);

private:
    int _etatPupitre;
    void traiterSaisie(int touche);
    int _touches[5];  // sauvgarde d'un ensemble de touches
    int _ind; // indice de sauvegarde touche

signals:
    void sig_stop();
    void sig_start();
    void sig_reqAffScores();
    void sig_reqAffQuelJoueur();
    void sig_reqChangeJoueur(int noJoueur);
    void sig_reqCalcEtAffNomScores(int noJoueur);
    void sig_info(QString mess);
    void sig_afficherMenu();
    void sig_afficherSortieMenu(int duree);
    void sig_afficherMenuSelected(int duree, QString texte);

public slots:

};

#endif // CGERERPUPITRE_H
