#include "chip8_display.h"

chip8_display::chip8_display(QWidget *parent) :
    QWidget(parent)
{
    mem_is_avaliable = false;
}

void chip8_display::paintEvent(QPaintEvent *) {
    QPainter disp_painter(this);
    disp_painter.setPen(QPen(Qt::black, 4, Qt::SolidLine));
   // disp_painter.drawPoint(20, 3);
   // disp_painter.drawPoint(10, 3);
    disp_painter.drawLine(5, 5, 320, 5);
    disp_painter.drawLine(5, 5, 5, 165);
    disp_painter.drawLine(5, 165, 320, 165);
    disp_painter.drawLine(320, 5, 320, 165);
    disp_painter.setPen(QPen(Qt::green, 4, Qt::SolidLine));
    unsigned char x, y;
    if (mem_is_avaliable) {
    for (int i = 0; i <= 0xFF; i++) {
        for (int offset = 0x7; offset >= 0; offset--) {
            y = (i / 0x8);
            x = ( ((i % 0x8) * 0x8) + 0x7 - offset );
            if ( (pVideoMem[i] >> offset) & 0x1 == 1 )
                disp_painter.drawPoint( x * 5 + 5, y * 5 + 5 );
        }
    }
    }
    else {
        for (int i = 0; i <= 0xFF; i++)
           // disp_painter.drawPoint(i * 7, i * 7);
            disp_painter.drawPoint(5,5);
    }
}
