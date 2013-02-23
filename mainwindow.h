#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QtGui/QtGui>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
/*
  //CHIP-8 Emulator
  //v. 0.0.0.0.0.01 alpha
  //Main programmer: Alex K.
  //Main patcher: Roma T.
  //Main designer: Alex K.
*/
#include "cpu8.h"
#include "chip8_display.h"

class mainwindow : public QWidget
{
    Q_OBJECT
public:
    explicit mainwindow(QWidget *parent = 0);
    QPushButton *run_button;
    QPushButton *load_button;
    QPushButton *step_button;
    QGridLayout *main_layout;
    QGridLayout *reglayout;
    chip8_display *display;
    cpu8 *main_cpu;
    void keyPressEvent(QKeyEvent *kevent);
    void keyReleaseEvent(QKeyEvent * kevent);

signals:
    
public slots:
    void load_rom();
    void run_emulation();
    void video_update_request();
    void do_step();
};

#endif // MAINWINDOW_H
