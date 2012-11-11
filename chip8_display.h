#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <QWidget>
#include <QGraphicsView>

class chip8_display : public QWidget
{
    Q_OBJECT
public:
    explicit chip8_display(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    unsigned char (*pVideoMem)[32][64];
    bool mem_is_avaliable;
    
signals:
    
public slots:
    
};

#endif // CHIP8_DISPLAY_H
