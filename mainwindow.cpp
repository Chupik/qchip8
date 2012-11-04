#include "mainwindow.h"

mainwindow::mainwindow(QWidget *parent) :
    QWidget(parent)
{
    main_layout = new QGridLayout();
    reglayout = new QGridLayout;
    run_button = new QPushButton(tr("Run Emulation"));
    load_button = new QPushButton(tr("Load ROM"));
    intepreter_out = new QTextEdit();
    display = new chip8_display();
    PClabel = new QLabel("123");
    Ireglabel = new QLabel("Ireg = 0");
    coplabel = new QLabel("Curop = 0000");
    int rowl = 0;
    main_layout->addLayout(reglayout, 1, 2);
    for (int i = 0; i <= 0xF; i++) {
        alotoflables[i] = new QLabel(tr("V") + QString::number(i, 16).toUpper() + tr(" = 0"));
        reglayout->addWidget(alotoflables[i], rowl, (i % 4), Qt::AlignCenter);
        if (i % 4 == 3) {
            rowl++;
        }
    }
    display->setMinimumSize(512, 256);
    main_layout->addWidget(Ireglabel, 6, 1);
    main_layout->addWidget(PClabel, 6, 3);
    main_layout->addWidget(coplabel, 6, 2);
    main_layout->addWidget(display, 1, 1);
    main_layout->addWidget(load_button, 4, 1);
    main_layout->addWidget(run_button, 4, 2);
    this->setLayout(main_layout);
    connect(run_button, SIGNAL(clicked()), this, SLOT(run_emulation()));
    connect(load_button, SIGNAL(clicked()), this, SLOT(load_rom()));
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
        display->pVideoMem = main_cpu->memory + 0xF00;
        display->mem_is_avaliable = true;
        //qDebug() << QString::number(0xB - 0xF, 2);
        for (int i = 0; i < rom_data.size(); i++)
            qDebug() << QString::number(main_cpu->memory[i + 0x200], 16);
        display->update();
        qDebug() << QString::number(main_cpu->memory[0x200], 16);
        qDebug("Loaded");
    }
    else
        qDebug("File isnt selected");
}

void mainwindow::run_emulation() {
    main_cpu->run_cpu();
    for (int i = 0; i <= 0xF; i++) {
        alotoflables[i]->setText(tr("NV") + QString::number(i, 16) + tr(" = ") + QString::number(main_cpu->V[i], 16));
    }
    PClabel->setText(tr("NPC = ") + QString::number(main_cpu->PC, 16));
    Ireglabel->setText(tr("NIreg = ") + QString::number(main_cpu->Ireg, 16));
    coplabel->setText(tr("Current opcode = ") + QString::number(main_cpu->Ireg, 16));
    display->update();
    //qDebug("Running CPU...");
    qDebug() << QString::number(main_cpu->PC, 10) + tr(" opcode: ") + QString::number(main_cpu->cop, 16);
}

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    mainwindow *window = new mainwindow();
    window->setMinimumSize(600, 400);
    window->show();
    return app.exec();
}
