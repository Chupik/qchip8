#include "mainwindow.h"

mainwindow::mainwindow(QWidget *parent) :
    QWidget(parent)
{
    main_layout = new QGridLayout();
    reglayout = new QGridLayout;
    run_button = new QPushButton(tr("Run Emulation"));
    load_button = new QPushButton(tr("Load ROM"));
    step_button = new QPushButton(tr("Step"));
    display = new chip8_display();
    main_layout->addLayout(reglayout, 1, 2);
    display->setMinimumSize(512, 256);
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
        display->pVideoMem = main_cpu->video_mem;
        display->mem_is_avaliable = true;
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
    }
}

void mainwindow::video_update_request() {
    display->update();
}

void mainwindow::do_step() {
    //main_cpu->execute_single_opcode();
    main_cpu->dump_video_mem();
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
    Q_UNUSED(kevent);
    main_cpu->pressed_key = 0x48;
}

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    mainwindow *window = new mainwindow();
    window->setMinimumSize(600, 400);
    window->show();
    return app.exec();
}
