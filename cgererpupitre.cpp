#include "cgererpupitre.h"

CGererPupitre::CGererPupitre(QObject *parent) : QObject(parent)
{
    _etatPupitre = ETAT_PUP_HORS_MENU;
    _ind = 0;
    _touches[_ind]=0;
}

CGererPupitre::~CGererPupitre()
{

}

void CGererPupitre::on_toucheRecue(int touche)
{
    // filtre touches autorisées
    switch (touche) {
    case Qt::Key_Slash:
    case Qt::Key_0 ... Qt::Key_9:
    case Qt::Key_Return ... Qt::Key_Enter:
    case Qt::Key_Period:
        _touches[_ind++]=touche; // sauve la touche
        traiterSaisie(touche);  // machine à états
        break;
    default:  // autre touche non conforme
        return;
    } // sw
}

void CGererPupitre::traiterSaisie(int touche)
{
    // machine à états
    switch (_etatPupitre) {
    case ETAT_PUP_HORS_MENU:
        if (touche == Qt::Key_Slash) {
            _etatPupitre = ETAT_PUP_CHOIX_MENU;
//            emit sig_afficherMenu();
//            emit sig_stop(); // vers CJeu pour stopper le jeu (Acquisitions)
            // Pb ! Au départ, annuler les éventuelles détection de toucher de cibles
            // produits pendant l'arrêt du jeu
            emit sig_info("CGererPupitre::traiterSaisie Affichage du menu (1-5).");
        } // if slash
        else {
            emit sig_info("CGererPupitre::traiterSaisie Touche non traitée.");
        }
        break;
    case ETAT_PUP_CHOIX_MENU:
        // seuls les touches 1-5 seront prises en compte
        if ( (touche>=Qt::Key_1) && (touche<=Qt::Key_5) ) {
            switch (touche) {
            case Qt::Key_1:
//                emit sig_afficherMenuSelected(2, "Aff. scores");
//                emit sig_reqAffScores();
                emit sig_info("CGererPupitre::traiterSaisie Affichage des scores. ENTREE pour retour au menu");
                _etatPupitre = ETAT_PUP_MENU_AFFSCORES;
                break;
            case Qt::Key_2:
//                emit sig_afficherMenuSelected(2, "Pénalité !");
//                emit sig_reqAffQuelJoueur();
                emit sig_info("CGererPupitre::traiterSaisie Donner une pénalité. Entrer NO joueur 1-4 :");
                _etatPupitre = ETAT_PUP_MENU_PENALITE;
                break;
            case Qt::Key_3:
//                emit sig_afficherMenuSelected(2, "A qui ?");
//                emit sig_reqAffQuelJoueur();
                emit sig_info("CGererPupitre::traiterSaisie Fixer à qui le tour. Entrer NO joueur 1-4 :");
                _etatPupitre = ETAT_PUP_MENU_AQUI;
                break;
            case Qt::Key_4:
//                emit sig_afficherMenuSelected(2, "Corriger scores");
//                emit sig_reqAffQuelJoueur();
                emit sig_info("CGererPupitre::traiterSaisie Corriger un score.  Entrer NO joueur 1-4 :");
                _etatPupitre = ETAT_PUP_MENU_CORRIGER;
                break;
            case Qt::Key_5:
  //              emit sig_afficherMenuSelected(1, "Retour");
                emit sig_info("CGererPupitre::traiterSaisie Sortie du menu.");
 //               emit sig_afficherSortieMenu(2);  // durée
//                emit sig_start();
                _etatPupitre = ETAT_PUP_HORS_MENU;
                break;
            } // sw
            emit sig_info("CGererPupitre::traiterSaisie Menu sélectionné : "+QString::number(touche-0x30));
        } // if touche 0-5
        else {
            emit sig_info("CGererPupitre::traiterSaisie Touche non traitée.");
        }
        break;

    case ETAT_PUP_MENU_AFFSCORES:
        if ( (touche==Qt::Key_Return) || (touche==Qt::Key_Enter) ) {
            _etatPupitre = ETAT_PUP_CHOIX_MENU;
//            emit sig_afficherMenu();
            emit sig_info("CGererPupitre::traiterSaisie Retour au menu. Choix 1-5 :");
        } // if entree
        break;

    case ETAT_PUP_MENU_PENALITE:
        switch (touche) {
        case Qt::Key_1 ... Qt::Key_4:
//            emit sig_reqCalcEtAffNomScores(touche-0x30);
            emit sig_info("CGererPupitre::traiterSaisie Pénalité sur joueur : "+QString::number(touche-0x30));
            break;
        case Qt::Key_Return ... Qt::Key_Enter:
            // appliquer la pénalité par un emit... vers CJeu
//            emit sig_afficherMenu();
            emit sig_info("CGererPupitre::traiterSaisie Retour au menu. Choix 1-5 : ");
            _etatPupitre = ETAT_PUP_CHOIX_MENU;
            break;
        } // sw
        break;

    case ETAT_PUP_MENU_AQUI:
        switch (touche) {
        case Qt::Key_1 ... Qt::Key_4:
//            emit sig_reqChangeJoueur(touche-0x30);
            emit sig_info("CGererPupitre::traiterSaisie Tour au joueur : "+QString::number(touche-0x30));
            break;
        case Qt::Key_Return ... Qt::Key_Enter:
            // appliquer le changement de tour par emit... vers CJeu
            _etatPupitre = ETAT_PUP_CHOIX_MENU;
//            emit sig_afficherMenu();
            emit sig_info("CGererPupitre::traiterSaisie Retour au menu. Choix 1-5 : ");
            break;
        } // sw
        break;

    case ETAT_PUP_MENU_CORRIGER:
        switch (touche) {
        case Qt::Key_1 ... Qt::Key_4:
            emit sig_info("CGererPupitre::traiterSaisie Choix joueur : "+QString::number(touche-0x30));
            break;
        case Qt::Key_Return ... Qt::Key_Enter:
            _etatPupitre = ETAT_PUP_CORRIGER_QUI;
            // A FAIRE sauver le no du joueur dans donnée membre
            emit sig_info("CGererPupitre::traiterSaisie Correction joueur. Entrez le score (0-9)");
            break;
        } // sw
        break;
    case ETAT_PUP_CORRIGER_QUI:
        switch (touche) {
        case Qt::Key_0 ... Qt::Key_9:
            // A FAIRE indiquer score
            emit sig_info("CGererPupitre::traiterSaisie valeur du score (0-9)");
            break;
        case Qt::Key_Return ... Qt::Key_Enter:
            _etatPupitre = ETAT_PUP_CHOIX_MENU;
            // A FAIRE emit... vers CJeu avec no Joueur et valeur de score
            emit sig_info("CGererPupitre::traiterSaisie FIN Correction joueur. Retoru au menu");
            break;
        } // sw
        break;
    } // sw
    _ind = 0;
    _touches[_ind] = 0;
} // méthode
