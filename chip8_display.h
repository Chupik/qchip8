/*
  //CHIP-8 Emulator
  //v. 0.0.0.0.0.01 alpha
  //Main programmer: Alex K.
  //Main patcher: Roma T.
  //Main designer: Alex K.
*/
#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <QWidget>
#include <QGraphicsView>
#include "cpu8.h"


class chip8_display : public QWidget
{
    Q_OBJECT
public:
    explicit chip8_display(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    unsigned char *pVideoMem;
    bool mem_is_avaliable;
    
signals:
    
public slots:
    
};

#endif // CHIP8_DISPLAY_H
