#include "cjeu.h"

CJeu::CJeu(QObject *parent) : QObject(parent)
{
    _zdc = new CZdc();
    _zdc->clear();

    _zdc->setEtatJeu(ETAT_JEU_ATTENTE_CONNEXION);

    _aff = new CCommAffichage();
    _pup = new CGererPupitre();
    connect(_pup, &CGererPupitre::sig_afficherMenu, _aff, &CCommAffichage::on_afficherMenu);
    connect(_pup, &CGererPupitre::sig_afficherSortieMenu, _aff, &CCommAffichage::on_afficherSortieMenu);
    connect(_pup, &CGererPupitre::sig_afficherMenuSelected, _aff, &CCommAffichage::on_afficherMenuSelected);
    connect(_pup, &CGererPupitre::sig_stop, this, &CJeu::on_stop);
    connect(_pup, &CGererPupitre::sig_start, this, &CJeu::on_start);
    connect(_pup, &CGererPupitre::sig_info, this, &CJeu::on_info);
    connect(_pup, &CGererPupitre::sig_reqAffScores, this, &CJeu::on_reqAffScores);
    connect(this, &CJeu::sig_toucheRecue, _pup, &CGererPupitre::on_toucheRecue);
}

CJeu::~CJeu()
{
    delete _aff;
    delete _pup;
    delete _zdc;
}

void CJeu ::play()
{
    T_DATAS_STATIC datas;
    datas.nbJoueurs = 4;
    sprintf (datas.nomJoueurs[0],"J1");
    sprintf (datas.nomJoueurs[1],"J2");
    sprintf (datas.nomJoueurs[2],"J3");
    sprintf (datas.nomJoueurs[3],"J4");
    datas.modeJeu = 'M';
    datas.modeFinJeu = 'S';
    datas.cpt = 120;
    datas.nbPointsFaute = 3;
    datas.nbPanneaux = 1;
    datas.nbCiblesOn = 12;
    datas.joker = true;
    datas.nbPointsJoker = 7;
    datas.nbCouleurs = 8;
    datas.nbPointsParCouleur [0] = 0;
    datas.nbPointsParCouleur [1] = 1;
    datas.nbPointsParCouleur [2] = 2;
    datas.nbPointsParCouleur [3] = 3;
    datas.nbPointsParCouleur [4] = 4;
    datas.nbPointsParCouleur [5] = 5;
    datas.nbPointsParCouleur [6] = 6;
    datas.nbPointsParCouleur [7] = 7;
    _zdc->appliquerNewParams(&datas);

    _zdc->setEtatJeu(ETAT_JEU_EN_COURS);

    _aff->afficherBienvenue(2);
    _aff->afficherTypeJeu(2);
    _aff->on_afficherScores(_zdc->getAQuiLeTour());

    _zdc->setDureePoints(_zdc->getCpt());
    _zdc->joueurSuivant();
    //char modeFinJeu = _zdc->getModeFinJeu();
    genererCouleursDesCibles();
}

QByteArray CJeu::genererCouleursDesCibles()
{
    // Appelé à l'initialisation du jeu seulement

    int nb; // indice
    int cpt;  // pour recherche couleur
    uint8_t nbAlCoul;
    uint8_t nbPans = _zdc->getNbPanneaux();
    uint8_t nbCouls = _zdc->getNbCouleurs();
    uint8_t nbCiblesAff = nbPans*NB_CIBLES_PAN/2;
    char mode = _zdc->getModeJeu();

    // suivant la règle choisie
    uint8_t refCibles[MAX_PANS*NB_CIBLES_PAN]; // indices des cibles éclairées
    T_COULEURS tabCibles[MAX_PANS][NB_CIBLES_PAN]; // qui peut le plus peut le moins
    QByteArray couleurs = _zdc->getCouleurs();

    switch(mode) {

    case 'M':  // jusqu'à moitié, elle se rallume
        // génération aléatoires des cibles affichées
        for (int i = 0; i < nbCiblesAff; i++) {
            refCibles[i] = qrand()%(nbPans*3); // entre 1 et NbPC*3
            for (int j = 0; j < i; j++) {      // vérification unicité du nombre
                if (refCibles[i] == refCibles[j]) {
                    i--;
                    break;
                } // if =
            } // j
        } // for i

        // génération du tableau des couleurs pour les cibles sélectionnée
        for (int i = 1; i <= 3; i++) { // forcement 3 lignes
            nb = i;
            for (int j = 1; j <= nbPans; j++) { // selon le nombre de panneau
                bool trouve = false;
                for (int k = 0; k < nbCiblesAff; k++) { // recherche si cellule à éclairer
                    if (nb == refCibles[k]) {
                        trouve = true;
                        break;
                    } // if
                } // for k
                if (!trouve) {
                    tabCibles[i-1][j-1] = ETEINT;
                } else { // si cible à éclairée
                    nbAlCoul = 1+qrand()%nbCouls;// generation aléatoire des couleurs
                    // cherche la couleur
                    cpt=0;
                    for (int k=0 ; k<MAX_NB_COULEURS ; k++) {
                        if (couleurs.at(k) > 0)
                            cpt++;
                        if (cpt == nbAlCoul) { // si trouvé couleur
                            tabCibles[i-1][j-1] = static_cast<T_COULEURS>(couleurs.at(k));
                            break;
                        } // if trouvé couleur
                    } // for k
                } // else cible à éclairer
                nb += 3;
            } // for j
        } // for i
        // maj les couleurs
        _zdc->setCouleurs(&tabCibles[0][0]);
        break;

    case 'P': // toutes les cibles allumées, il faut les éteindre
        // génération du tableau des couleurs pour les cibles sélectionnée
        for (int i = 1; i <= 3; i++) { // forcement 3 lignes
            for (int j = 1; j <= nbPans; j++) { // selon le nombre de panneau
                nbAlCoul = 1+qrand()%nbCouls;// generation aléatoire des couleurs
                // cherche la couleur
                int cpt=0;
                for (int k=0 ; k<MAX_NB_COULEURS ; k++) {
                    if (couleurs.at(k) > 0)
                        cpt++;
                    if (cpt == nbAlCoul) { // si trouvé couleur
                        tabCibles[i-1][j-1] = static_cast<T_COULEURS>(couleurs.at(k));
                        break;
                    } // if trouvé couleur
                } // for k
            } // for j
        } // for i
        // maj les couleurs
        _zdc->setCouleurs(&tabCibles[0][0]);
        break;

    default:
        emit sig_erreur("CJeu::on_cibleTouchee : Erreur de mode de jeu");
        break;
    } // sw

    return _zdc->getCouleurs();
}

bool CJeu::isFinDePartie()
{
    QList<uint16_t> scores;
    int nbj;  // nombre de joueur
    char mfj = _zdc->getModeFinJeu();
    uint16_t cpt = _zdc->getCpt();

    switch(mfj) {
    case 'S': // atteindre un nombre de points
        scores = _zdc->getScores();
        nbj = _zdc->getNbJoueurs();
        for (int i=0 ; i<nbj ; i++) {
            if (scores.at(i) >= cpt)
                return true;
        } // for i
        break;
    case 'T': // temps décroissant
        if (cpt == 0)
            return true;
        break;
    default:
        emit sig_erreur("CJeu::isFinDePartie : Mode de fin de jeu non cohérent !");
        break;
    } // sw
    return false;
}

void CJeu::on_newConnection()
{
    emit sig_info("CJeu::on_newConnection un client vient de se connecter");
}

void CJeu::on_disconnected()
{
    emit sig_info("CJeu::on_disconnected un client vient de se déconnecter");

}

void CJeu::on_cibleTouchee(uint8_t noPan, uint8_t cibles)
{
    // appelé dès qu'une cible est touchée
    emit sig_info("CJeu::on_cibleTouchee : Panneau n°:"+QString::number(noPan+1)+" Cible n°:"+QString::number(cibles));

    if (_zdc->etatJeu() == ETAT_JEU_EN_COURS) {
        // Chercher combien de point vaut la cible touchée
        uint16_t nbPoints = _zdc->getNbPoint1Cible(noPan, cibles);
        // mettre à jour le score correspondant dans zdc
        uint8_t qui = _zdc->getAQuiLeTour();
        _zdc->mettreAjourScore1Joueur(qui, nbPoints);
    // A FAIRE sauver dans la base de données pour l'historique partie
        T_SCORES scores;
        histoScores.append(scores);  // mémorisation dans l'historique
        // maj des couleurs des cibles suite au touché
        switch(_zdc->getModeJeu()) {
        case 'M':  // jusqu'à moitié, elle se rallume
            _zdc->eteindre1Cible(noPan, cibles);
            _zdc->allumer1AutreCible(noPan, cibles);  // coordonnées de la cible pour conservation de la couleur
            break;
        case 'P': // toutes les cibles allumées, il faut les éteindre
             _zdc->eteindre1Cible(noPan, cibles);
            break;
        default:
            emit sig_erreur("CJeu::on_cibleTouchee : Erreur de mode de jeu");
            break;
        } // sw

        bool fin = isFinDePartie();

        if (fin)
            emit sig_finDePartie();

        _zdc->joueurSuivant();
        emit sig_majScores(_zdc->getAQuiLeTour()); // envoi le signal vers CCserveurTcp et CCommAffichage
    } // if etat
}

void CJeu::on_play()
{
    emit sig_info("CJeu::on_play : Lancement jeu, params correctes");
    play(); // lancement du jeu
}

void CJeu::on_annulationPartie()
{
    // appelée si demande d'annulation partie

    // A FAIRE effacer toutes les données ZDC
    // A FAIRE effacer les données de la partie dans la BDD
    // A FAIRE déconnecter les clients
    // A FAIRE remettre à l'état initial
}

void CJeu::on_finCycleCommPanneau()
{
    // appelée lorsque CCommPanneau a terminé son cycle de lecture écriture vers les panneaux
   // Je m'en sert que pour l'affichage
   emit sig_info("Cycle I2C terminé");
}

void CJeu::on_stop()
{
    // provoqué par le bouton STOP du pupitre
    _zdc->setEtatJeu(ETAT_JEU_EN_PAUSE);
    emit sig_info("CJeu::on_stop : JETAT_JEU_EN_PAUSE");
}

void CJeu::on_start()
{
    // méthode le jeu reprend après une correction
    _zdc->setEtatJeu(ETAT_JEU_EN_COURS);
    emit sig_info("CJeu::on_start : ETAT_JEU_EN_COURS");
}

void CJeu::on_timeout()
{

}

void CJeu::on_finDePartie()
{
    // A FAIRE fin de partie
    // Arrêt timer
    _zdc->setEtatJeu(ETAT_JEU_ATTENTE_PARAMS);
    // réinitialisation de tout
}

void CJeu::on_trameConnexionValidated()
{
    _zdc->setEtatJeu(ETAT_JEU_ATTENTE_PARAMS);
}

void CJeu::on_toucheRecue(int touche)
{
    //if ( (_zdc->etatJeu()==ETAT_JEU_EN_COURS) || (_zdc->etatJeu() == ETAT_JEU_EN_PAUSE))
        emit sig_toucheRecue(touche); // vers CGererPupitre
    // sinon on ne fait rien.
}

void CJeu::on_reqAffScores()
{
    emit sig_majScores(_zdc->getAQuiLeTour());
}

void CJeu::on_erreur(QString mess)
{
     emit sig_erreur(mess);  // remontée des erreurs à l'IHM-
}

void CJeu::on_info(QString mess)
{
    emit sig_info(mess);  // remontée des erreurs à l'IHM
}
