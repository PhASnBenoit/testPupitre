#include "cihm.h"
#include "ui_cihm.h"

CIhm::CIhm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CIhm)
{
    ui->setupUi(this);
    _jeu = new CJeu();
    connect(this, &CIhm::sig_toucheRecue, _jeu, &CJeu::on_toucheRecue);
    connect(_jeu, &CJeu::sig_info, this, &CIhm::on_info);
    connect(_jeu, &CJeu::sig_erreur, this, &CIhm::on_erreurJeu);
}

CIhm::~CIhm()
{
    delete  _jeu;
    delete ui;
}

void CIhm::on_erreurJeu(QString mess)
{
    ui->teSuivi->append(mess);
}

void CIhm::on_info(QString mess)
{
    ui->teSuivi->append(mess);
}

void CIhm::keyPressEvent(QKeyEvent *event)
{
    // frontal de réception des touches du pupitre
    // emission d'un signal vers CJeu qui emettra un signal vers CGererPupitre
    // CGererPupitre::on_toucheRecue() traitera la touche et formera la saisie selon le protocole établi.
    // CGererPupitre::on_traiterSaisie() gère la machine à état du pupitre
    setWindowTitle(tr("KeyPressEvent!"));
    int touche = event->key();
    on_info("CIhm::keyPressEvent Une touche est pressée : " + QString::number(touche));
    emit sig_toucheRecue(touche);
    QWidget::keyPressEvent(event); // détournement terminé, compportement par défaut
}
