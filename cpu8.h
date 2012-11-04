#ifndef CPU8_H
#define CPU8_H

#include <QObject>
#include <QStack>
#include <QDebug>

class cpu8 : public QObject
{
    Q_OBJECT
public:
    explicit cpu8(QObject *parent = 0);
    explicit cpu8(QByteArray *rom, QObject *parent = 0);
    void reset_memory();
    void run_cpu();
    void execute_opcode(unsigned short opcode);
    unsigned short get_random_number(unsigned short low, unsigned short high);
    unsigned short generate_opcode(unsigned short mem_adress);
    unsigned short get_opcode(unsigned short opcode);
    unsigned short get_x_opcode(unsigned short opcode);
    unsigned short get_y_opcode(unsigned short opcode);
    unsigned short get_nnn_opcode(unsigned short opcode);
    unsigned short get_nn_opcode(unsigned short opcode);
    unsigned short get_n_opcode(unsigned short opcode);
    unsigned short get_extendet_opcode(unsigned short opcode);
    void copy_memory(unsigned short from_adress, unsigned short to_adress);
    unsigned short convert_coordto_adress(unsigned char x, unsigned char y, unsigned short &offset);
    void clear_videomem();

    QStack<unsigned int> stack;
    unsigned char memory[0xFFF];  //ram
    unsigned char V[0xF];         //registers
    unsigned short Ireg;           //pointer to memory
    unsigned short SP;             //A stack pointer
    unsigned int PC;             //Program counter
    short cpu_state;
    unsigned short cop;            //current opcode value
    
signals:
    
public slots:
    
};

#endif // CPU8_H
