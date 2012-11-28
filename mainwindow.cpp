#include "mainwindow.h"

mainwindow::mainwindow(QWidget *parent) :
    QWidget(parent)
{
    main_layout = new QGridLayout();
    reglayout = new QGridLayout;
    run_button = new QPushButton(tr("Run Emulation"));
    load_button = new QPushButton(tr("Load ROM"));
    step_button = new QPushButton(tr("Step"));
//    intepreter_out = new QTextEdit();
//    memdumper = new QTextEdit();
    display = new chip8_display();
//    PClabel = new QLabel("123");
//    Ireglabel = new QLabel("Ireg = 0");
//    coplabel = new QLabel("Curop = 0000");
//    int rowl = 0;
    main_layout->addLayout(reglayout, 1, 2);
  //  for (int i = 0; i < 0xF; i++) {
  //      alotoflables[i] = new QLabel(tr("V") + QString::number(i, 16).toUpper() + tr(" = 0"));
  //      reglayout->addWidget(alotoflables[i], rowl, (i % 4), Qt::AlignCenter);
  //      if (i % 4 == 3) {
  //          rowl++;
  //      }
  //  }
//    main_layout->addWidget(memdumper, 7, 1, 3, 2);
//    memdumper->setReadOnly(true);
    display->setMinimumSize(512, 256);
//    main_layout->addWidget(Ireglabel, 6, 1);
//    main_layout->addWidget(PClabel, 6, 3);
//    main_layout->addWidget(coplabel, 6, 2);
//    main_layout->addWidget(alotoflables[0xF], 6, 4);
    main_layout->addWidget(display, 1, 1);
    main_layout->addWidget(load_button, 4, 1);
    main_layout->addWidget(run_button, 4, 2);
    main_layout->addWidget(step_button, 4, 3);
    this->setLayout(main_layout);
    connect(run_button, SIGNAL(clicked()), this, SLOT(run_emulation()));
    connect(load_button, SIGNAL(clicked()), this, SLOT(load_rom()));
    connect(step_button, SIGNAL(clicked()), this, SLOT(do_step()));
}

void mainwindow::load_rom() {
    qDebug("Loading ROM...");
    QString fname = QFileDialog::getOpenFileName(this, tr("Select a ROM file"), QDir::currentPath());
    if (!fname.isEmpty()) {
        qDebug() << fname;
        QFile romfile(fname);
        QByteArray rom_data;
        romfile.open(QIODevice::ReadOnly);
        rom_data = romfile.readAll();
        main_cpu = new cpu8(&rom_data);
        display->pVideoMem = &main_cpu->video_mem;
        display->mem_is_avaliable = true;
        //qDebug() << QString::number(0xB - 0xF, 2);
        display->update();
        connect(this->main_cpu, SIGNAL(video_mem_updated()), this, SLOT(video_update_request()));
    }
    else
        qDebug("File isnt selected");
}

void mainwindow::run_emulation() {
    main_cpu->run_cpu();
    display->update();
    for (int i = 0; i <= 0xFF; i++) {
        //memdumper->append(QString::number(main_cpu->memory[i + 0xF00], 2));
        //memdumper->setText(QString::number(main_cpu->memory[i + 0xF00], 2) + memdumper->toPlainText());
        //if (i % 8 == 0)
        //   memdumper->append("\n");
    }
    //qDebug("Running CPU...");
}

void mainwindow::video_update_request() {
    display->update();
}

void mainwindow::do_step() {
    main_cpu->execute_single_opcode();
}

void mainwindow::keyPressEvent(QKeyEvent *kevent) {
    if (kevent->key() >= 0x30 && kevent->key() <=0x39)
        main_cpu->pressed_key = kevent->key() - 0x30;
    else if(kevent->key() >= 0x41 && kevent->key() <= 0x46)
        main_cpu->pressed_key = kevent->key() - 0x37;
    //qDebug() << tr("Key Pressed");
}

void mainwindow::keyReleaseEvent(QKeyEvent *kevent) {
    //qDebug() << tr("Key Released");
    main_cpu->pressed_key = 0x48;
}

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    mainwindow *window = new mainwindow();
    window->setMinimumSize(600, 400);
    window->show();
    return app.exec();
}
