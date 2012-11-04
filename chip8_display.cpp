#include "chip8_display.h"

chip8_display::chip8_display(QWidget *parent) :
    QWidget(parent)
{
    mem_is_avaliable = false;
}

void chip8_display::paintEvent(QPaintEvent *) {
    QPainter disp_painter(this);
    disp_painter.setPen(QPen(Qt::green, 7, Qt::SolidLine));
   // disp_painter.drawPoint(20, 3);
   // disp_painter.drawPoint(10, 3);
    //unsigned char x, y;
    if (mem_is_avaliable) {
    for (int i = 0; i <= 0xFF; i++) {
        for (int n = 0xF; n >= 0; n--) {
            if ( (pVideoMem[i] >> n) & 0x1 == 1 )
                disp_painter.drawPoint( ((i % 16) + n) * 7, (i / 16) * 7);
        }
    }
    }
    else {
        for (int i = 0; i <= 0xFF; i++)
           // disp_painter.drawPoint(i * 7, i * 7);
            disp_painter.drawPoint(14,14);
    }
}
