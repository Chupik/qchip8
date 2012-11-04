#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QtGui/QtGui>
#include "cpu8.h"
#include "chip8_display.h"

class mainwindow : public QWidget
{
    Q_OBJECT
public:
    explicit mainwindow(QWidget *parent = 0);
    QPushButton *run_button;
    QPushButton *load_button;
    QGridLayout *main_layout;
    QGridLayout *reglayout;
    QLabel *alotoflables[0xF];
    QLabel *PClabel;
    QLabel *coplabel;
    QLabel *Ireglabel;
    QTextEdit *intepreter_out;
    chip8_display *display;
    cpu8 *main_cpu;

signals:
    
public slots:
    void load_rom();
    void run_emulation();
};

#endif // MAINWINDOW_H