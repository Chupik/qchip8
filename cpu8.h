/*
  //CHIP-8 Emulator
  //v. 0.0.0.0.0.01 alpha
  //Main programmer: Alex K.
  //Main patcher: Roma T.
  //Main designer: Alex K.
*/
#ifndef CPU8_H
#define CPU8_H

//#define USER_DEBUG
//#define LEGACY_VIDEO
//#define VIDEO_DEBUG
#define VIDEO_RAM_OFFSET 0xF00
#define REGISTERS_OFFSET 0x6F0
#define STACK_OFFSET     0x6CF

#include <QObject>
#include <QStack>
#include <QDebug>
#include <QTimer>

const size_t video_mem_row_size = 64;
const size_t video_mem_offset = 0xF00;
typedef unsigned char video_mem_row[video_mem_row_size];

class cpu8 : public QObject
{
    Q_OBJECT
public:
    explicit cpu8(QObject *parent = 0);
    explicit cpu8(QByteArray *rom, QObject *parent = 0);
    void reset_memory();
    void run_cpu();
    void execute_opcode(unsigned short opcode);
    void execute_single_opcode();
    //opcode operations
    unsigned short get_random_number(unsigned short low, unsigned short high);
    unsigned short generate_opcode(unsigned short mem_adress);
    unsigned short get_opcode(unsigned short opcode);
    unsigned short get_x_opcode(unsigned short opcode);
    unsigned short get_y_opcode(unsigned short opcode);
    unsigned short get_nnn_opcode(unsigned short opcode);
    unsigned short get_nn_opcode(unsigned short opcode);
    unsigned short get_n_opcode(unsigned short opcode);
    unsigned short get_extendet_opcode(unsigned short opcode);
    //video memory mapping to RAM
    bool get_video_bit_from_ram(unsigned short x, unsigned short y);
    void write_bit_to_video(unsigned short x, unsigned short y, bool bit);
    void copy_memory(unsigned short from_adress, unsigned short to_adress);
    //dump
    void dump_video_mem();
    //deprecated
    unsigned short convert_coordto_adress(unsigned char x, unsigned char y, unsigned short &offset);
    void clear_videomem();
    void load_fonts();

    unsigned char *stack;
    unsigned char memory[0x1000];   //ram
    unsigned char *video_mem;
    unsigned char *V;               //registers
    unsigned short Ireg;            //pointer to memory
    unsigned short SP;              //stack pointer
    unsigned int PC;                //program counter
    short cpu_state;
    unsigned short cop;             //current opcode value
    QTimer *delay_timer;
    QTimer *cpu_rate_timer;
    unsigned short delay_value;
    int pressed_key;
    
signals:
    void video_mem_updated();
    
public slots:
    void on_delay_timer_ticked();
    void on_cpu_timer_tick();
    
};

#endif // CPU8_H
