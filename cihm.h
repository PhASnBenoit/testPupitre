#ifndef CIHM_H
#define CIHM_H

#include <QMainWindow>
#include <QKeyEvent>

#include "cjeu.h"

namespace Ui {
class CIhm;
}

class CIhm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhm(QWidget *parent = nullptr);
    ~CIhm();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::CIhm *ui;
    CJeu *_jeu;

signals:
    void sig_play();
    void sig_toucheRecue(int touche); // vers CJeu
    void sig_cibleTouched(uint8_t noPan, uint8_t cibles);

private slots:
    void on_erreurJeu(QString mess);
    void on_info(QString mess);
};

#endif // CIHM_H
