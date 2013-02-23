#include "chip8_display.h"

chip8_display::chip8_display(QWidget *parent) :
    QWidget(parent)
{
    mem_is_avaliable = false;
}

void chip8_display::paintEvent(QPaintEvent *) {
    QPainter disp_painter(this);
    int pxsize = 8;
    disp_painter.setPen(QPen(Qt::black, pxsize, Qt::SolidLine));
    disp_painter.setBrush(Qt::SolidPattern);
    disp_painter.drawRect(0, 0, pxsize*64, pxsize*32);
    disp_painter.setPen(QPen(Qt::green, pxsize, Qt::SolidLine));
    if (mem_is_avaliable) {
        for (int i = 0; i < 32; i++) {
            for (int n = 0; n < 64; n++) {
                if (pVideoMem[i][n] != 0)
                    disp_painter.drawPoint(n * pxsize + 1, i * pxsize + 1);
            }
        }
    }
    else {
        for (int i = 0; i <= 0xFF; i++)
           // disp_painter.drawPoint(i * 7, i * 7);
            disp_painter.drawPoint(pxsize + 1 ,pxsize + 1);
    }
}
