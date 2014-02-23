#include "cpu8.h"

cpu8::cpu8(QObject *parent) :
    QObject(parent)
{

}

cpu8::cpu8(QByteArray *rom, QObject *parent) : QObject(parent) {
    video_mem = memory + VIDEO_RAM_OFFSET;
    V = memory + REGISTERS_OFFSET;
    stack = memory + STACK_OFFSET;
    this->reset_memory();
    for (int i = 0; i < int(rom->size()); i++)
        memory[i + 0x200] = (unsigned char)rom->at(i);

    delay_timer = new QTimer();
    cpu_rate_timer = new QTimer();
    connect(delay_timer, SIGNAL(timeout()), this, SLOT(on_delay_timer_ticked()));
    connect(cpu_rate_timer, SIGNAL(timeout()), this, SLOT(on_cpu_timer_tick()));
    delay_value = 0;
    delay_timer->start(0x8);
    pressed_key = 0x48;
    load_fonts();
}

void cpu8::reset_memory() {
    for (int i = 0; i < 0xFFF; i++)
        memory[i] = 0;
    for (int i = 0; i < 0xF; i++)
        V[i] = 0;
    Ireg = 0x000;
    PC = 0x200;
    SP = 0;
    cpu_state = 0;
    clear_videomem();
}

void cpu8::run_cpu() {
   /* cop = generate_opcode(PC);
        execute_opcode(cop);
        */
    if (!cpu_state) {
        cpu_rate_timer->start(2);
        cpu_state = 1;
    }
    else {
        cpu_rate_timer->stop();
        cpu_state = 0;
    }
}

void cpu8::execute_single_opcode() {
    cop = generate_opcode(PC);
    execute_opcode(cop);
    qDebug() << QString::number(this->PC, 10) + tr(" opcode: ") + QString::number(this->cop, 16) + " in sprite adr: " + QString::number(Ireg, 16);
}

void cpu8::execute_opcode(unsigned short opcode) {
    switch (get_opcode(opcode)) {
    case 0x0:
        switch (opcode) {
        //clear the screen
        case 0x00E0:
            this->clear_videomem();
#ifdef USER_DEBUG
            qDebug() << tr("Clearing the screen");
#endif
            break;
        //return from subroutin
        case 0x00EE:
            PC = (stack[SP+1] << 0x8) | stack[SP];
            SP += 3;
#ifdef STACK_DEBUG
            qDebug() << tr("Pop ") << QString::number(PC, 16) << tr(" from stack");
#endif
            break;
        //call a program to an adress
        default:
            PC = get_nnn_opcode(opcode) - 0x2;
            break;
        }
        break;
        //jump to adress
    case 0x1:
        PC = get_nnn_opcode(opcode) - 0x2;
        break;
        //jump to subroutin
    case 0x2:
#ifdef STACK_DEBUG
        qDebug() << tr("Push ") << QString::number(PC, 16) << tr(" to stack");
#endif
        SP -= 3;
        stack[SP] = PC & 0xFF;
        stack[SP+1] = (PC >> 0x8) & 0xF;
        PC = get_nnn_opcode(opcode) - 2;
        break;
        //skip the next instruction if VX equal to NN
    case 0x3:
        if (V[get_x_opcode(opcode)] == get_nn_opcode(opcode))
            PC += 2;
        break;
        //skip the next instruction if VX not equal to NN
    case 0x4:
        if (V[get_x_opcode(opcode)] != get_nn_opcode(opcode))
            PC += 2;
        break;
        //skips the next instruction if VX equals VY
    case 0x5:
        if (V[get_x_opcode(opcode)] == V[get_y_opcode(opcode)])
            PC += 2;
        break;
        //VX = NN
    case 0x6:
        V[get_x_opcode(opcode)] = get_nn_opcode(opcode);
        break;
        //VX = VX + NN
    case 0x7:
        //V[0xF] = (V[get_x_opcode(opcode)] + get_nn_opcode(opcode)) & 0x80;
        V[get_x_opcode(opcode)] = (V[get_x_opcode(opcode)] + get_nn_opcode(opcode)) & 0xFF;
        break;
    case 0x8:
        switch (get_extendet_opcode(opcode)) {
        //VX = VY
        case 0x0:
            V[get_x_opcode(opcode)] = V[get_y_opcode(opcode)];
            break;
        //VX = VX or VY
        case 0x1:
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] | V[get_y_opcode(opcode)];
            break;
        //VX = VX and VY
        case 0x2:
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] & V[get_y_opcode(opcode)];
#ifdef USER_DEBUG
            qDebug() << QString::number(V[get_x_opcode(opcode)], 16) + tr(" and ") + QString::number(V[get_y_opcode(opcode)], 16) + tr(" = ") + QString::number(V[get_x_opcode(opcode)] & V[get_y_opcode(opcode)], 16);
#endif
            break;
        //VX = VX xor VY
        case 0x3:
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] ^ V[get_y_opcode(opcode)];
            break;
        //VX = VX + VY, VF - carry flag
        case 0x4:
            if ((V[get_x_opcode(opcode)] + V[get_y_opcode(opcode)]) > 0xFF)
                V[0xF] = 0x1;
            else
                V[0xF] = 0x0;
#ifdef USER_DEBUG
            qDebug() << tr("Adding: ") + QString::number(V[get_x_opcode(opcode)], 16) + tr(" + ") + QString::number(V[get_y_opcode(opcode)], 16) + tr(" = ") + QString::number((V[get_x_opcode(opcode)] + V[get_y_opcode(opcode)]) & 0xFF, 16) + tr(" with carry: ") + QString::number(V[0xF], 16);
#endif
            V[get_x_opcode(opcode)] = (V[get_x_opcode(opcode)] + V[get_y_opcode(opcode)]) & 0xFF;
            break;
        //VX = VX - VY, VF - carry flag
        case 0x5:
            if (V[get_x_opcode(opcode)] >= V[get_y_opcode(opcode)])
                V[0xF] = 0x01;
            else
                V[0xF] = 0x00;
            V[get_x_opcode(opcode)] -= V[get_y_opcode(opcode)];
            break;
        //VX >> 1, VF - carry flag
        case 0x6:
            V[0xF] = V[get_x_opcode(opcode)] & 0x1;
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] >> 1;
            break;
        //VX = VY - VX, VF - carry flag
        case 0x7:
            if (V[get_y_opcode(opcode)] >= V[get_x_opcode(opcode)])
                V[0xF] = 0x01;
            else
                V[0xF] = 0x00;
            V[get_x_opcode(opcode)] = V[get_y_opcode(opcode)] - V[get_x_opcode(opcode)];
            break;
        //VX << 1, VF - carry flag
        case 0xE:
            V[0xF] = (V[get_x_opcode(opcode)] & 0x80) >> 7;
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] << 1;
            break;
        }

        break;
    case 0x9:
        //skips next instruction if VX doesnt equal VY
        if (V[get_x_opcode(opcode)] != V[get_y_opcode(opcode)])
            PC +=2;
        break;
        //Ireg = NNN
    case 0xA:
        Ireg = get_nnn_opcode(opcode);
#ifdef USER_DEBUG
        qDebug() << tr("Ireg has changed to: ") << QString::number(Ireg, 16);
#endif
        break;
        //jump to NNN + V0
    case 0xB:
        PC = get_nnn_opcode(opcode) + V[0x0] - 0x02;
        break;
        //VX = random number
    case 0xC:
        V[get_x_opcode(opcode)] = get_random_number(0, 255) & get_nn_opcode(opcode);
#ifdef USER_DEBUG
        qDebug() << tr("Your random is ") + QString::number(V[get_x_opcode(opcode)], 16);
#endif
        break;
        //Draws at x = VX, y = VY sprite form I register with height of N
    case 0xD:
        unsigned short x, y;
        x = V[get_x_opcode(opcode)];
        y = V[get_y_opcode(opcode)];
        V[0xF] = 0;
        for (int i = 0; i < get_extendet_opcode(opcode); i++) {
            for (int n = 0; n < 8; n++) {
                if (((memory[Ireg + i] >> (0x7 - n)) & 0x1) != 0 && this->get_video_bit_from_ram((x + n) % 64, (y + i) % 32) == 1)
                {
                    V[0xF] = 1;
#ifdef USER_DEBUG
                    qDebug() << tr("VF[0xF] setted to 1 (occlusion)");
#endif
                }
#ifdef USER_DEBUG
                qDebug() << tr("in mem:") + QString::number(((memory[Ireg + i] >> (0x7 - n)) & 0x1), 2) + tr("in v_ram: ") + QString::number(video_mem[(y + i) % 32][(x + n) % 64], 2);
#endif
                this->write_bit_to_video((x + n) % 64, (y + i) % 32, (memory[Ireg + i] >> (0x7 - n)) & 0x1);

            }

        }
#ifdef USER_DEBUG
        qDebug() << "Drawing at x: " + QString::number(x, 10) + " y: " + QString::number(y, 10) + " opcode: " + QString::number(opcode, 16) + tr(" sprite:") + QString::number(memory[Ireg], 2);
#endif
        emit video_mem_updated();
        break;
    case 0xE:
        //clava
            switch (get_nn_opcode(opcode)) {
            case 0x9E:
                if (V[get_x_opcode(opcode)] == pressed_key)
                    PC += 2;
                break;
            case 0xA1:
                if (V[get_x_opcode(opcode)] != pressed_key)
                    PC += 2;
                break;
            }

        break;
    case 0xF:
        //timer, klava and others
            switch (get_nn_opcode(opcode)) {
            //VX = delay timer value
            case 0x07:
                V[get_x_opcode(opcode)] = delay_value;
                break;
            case 0x0A:
                break;
            //Sets delay timer value to VX
            case 0x15:
                delay_value = V[get_x_opcode(opcode)];
                break;
            case 0x18:
                break;
            //не помню
            case 0x1E:
                if ((Ireg + V[get_x_opcode(opcode)]) > 0xFFF)
                    V[0xF] = 1;
                else
                    V[0xF] = 0;
                Ireg += V[get_x_opcode(opcode)];
                break;
            case 0x29:
                Ireg = V[get_x_opcode(opcode)] * 5;
                break;
            case 0x33:
                memory[Ireg] = V[get_x_opcode(opcode)] / 100;
                memory[Ireg + 1] = (V[get_x_opcode(opcode)] % 100) / 10;
                memory[Ireg + 2] = V[get_x_opcode(opcode)] % 10;
                break;
            case 0x55:
                for (int i = 0; i <= get_x_opcode(opcode); i++)
                    memory[Ireg + i] = V[i];
                Ireg +=  get_x_opcode(opcode) + 1;
                break;
            case 0x65:
                for (int i = 0; i <= get_x_opcode(opcode); i++)
                    V[i] = memory[Ireg + i];
                Ireg += get_x_opcode(opcode) + 1;
                break;


            }

        break;
    }
PC+=2;
}

void cpu8::on_delay_timer_ticked() {
    if (delay_value > 0)
        delay_value--;
}

void cpu8::on_cpu_timer_tick() {
    cop = generate_opcode(PC);
    execute_opcode(cop);
    //qDebug() << QString::number(this->PC, 10) + tr(" opcode: ") + QString::number(this->cop, 16);
}

unsigned short cpu8::generate_opcode(unsigned short mem_adress) {
    return (this->memory[mem_adress] << 8) + (this->memory[mem_adress+1]);
}

unsigned short cpu8::get_opcode(unsigned short opcode) {
    return (opcode & 0xF000) >> 12;
}

unsigned short cpu8::get_nnn_opcode(unsigned short opcode) {
    return opcode & 0x0FFF;
}

unsigned short cpu8::get_nn_opcode(unsigned short opcode) {
    return opcode & 0x00FF;
}

unsigned short cpu8::get_x_opcode(unsigned short opcode) {
    return (opcode & 0x0F00) >> 8;
}

unsigned short cpu8::get_y_opcode(unsigned short opcode) {
    return (opcode & 0x00F0) >> 4;
}

unsigned short cpu8::get_extendet_opcode(unsigned short opcode) {
    return opcode & 0x000F;
}

unsigned short cpu8::get_random_number(unsigned short low, unsigned short high) {
    return qrand() % ((high+1) - low) + low;
}

bool cpu8::get_video_bit_from_ram(unsigned short x, unsigned short y)
{
    //в каждой строке 64/8 бит = 8 байт
    //32 строки = 8*32 байт
#ifdef VIDEO_DEBUG
    qDebug() << tr("Read from video in x: ") << x << tr(" and y: ") << y << tr(" bit: ") << QString::number((bool)(((this->memory[VIDEO_RAM_OFFSET + y*8 + x/8] >> (7 - (x % 8))) & 0x1)), 2);
#endif
    return (bool)(((this->memory[VIDEO_RAM_OFFSET + y*8 + x/8] >> (7 - (x % 8))) & 0x1));
}

void cpu8::write_bit_to_video(unsigned short x, unsigned short y, bool bit)
{
    //if (this->get_video_bit_from_ram(x, y) != bit && this->get_video_bit_from_ram(x,y) != 0)
#ifdef VIDEO_DEBUG
    qDebug() << tr("Write to video in x: ") << x << tr(" and y: ") << y << tr(" bit: ") << QString::number(this->memory[VIDEO_RAM_OFFSET + y*8 + x/8], 2);
#endif
    this->memory[VIDEO_RAM_OFFSET + y*8 + x/8] ^= (bit << (7 - (x % 8)));
}

void cpu8::clear_videomem() {
    for (int i = 0; i < 0xFF; i++)
        video_mem[i] = 0;
}

void cpu8::dump_video_mem() {
    for (int i = 0; i < 0xFF; i += 0x8)
        qDebug() << QString::number(video_mem[i],2) << QString::number(video_mem[i+1],2) << QString::number(video_mem[i+2],2) << QString::number(video_mem[i+3],2) << QString::number(video_mem[i+4],2) << QString::number(video_mem[i+5],2) << QString::number(video_mem[i+6],2) << QString::number(video_mem[i+7],2);
}

void cpu8::load_fonts() {
    memory[0] = 0xF0;
    memory[1] = 0x90;
    memory[2] = 0x90;
    memory[3] = 0x90;
    memory[4] = 0xF0;   //0

    memory[5] = 0x10;
    memory[6] = 0x30;
    memory[7] = 0x50;
    memory[8] = 0x10;
    memory[9] = 0x10;   //1

    memory[10] = 0xF0;
    memory[11] = 0x10;
    memory[12] = 0xF0;
    memory[13] = 0x80;
    memory[14] = 0xF0;  //2

    memory[15] = 0xF0;
    memory[16] = 0x10;
    memory[17] = 0xF0;
    memory[18] = 0x10;
    memory[19] = 0xF0;  //3

    memory[20] = 0x90;
    memory[21] = 0x90;
    memory[22] = 0xF0;
    memory[23] = 0x10;
    memory[24] = 0x10;  //4

    memory[25] = 0xf0;
    memory[26] = 0x80;
    memory[27] = 0xF0;
    memory[28] = 0x10;
    memory[29] = 0xf0;  //5

    memory[30] = 0xf0;
    memory[31] = 0x80;
    memory[32] = 0xF0;
    memory[33] = 0x90;
    memory[34] = 0xf0;  //6

    memory[35] = 0xf0;
    memory[36] = 0x10;
    memory[37] = 0x20;
    memory[38] = 0x40;
    memory[39] = 0x80;  //7

    memory[40] = 0xF0;
    memory[41] = 0x90;
    memory[42] = 0xf0;
    memory[43] = 0x90;
    memory[44] = 0xF0;  //8

    memory[45] = 0xF0;
    memory[46] = 0x90;
    memory[47] = 0xf0;
    memory[48] = 0x10;
    memory[49] = 0xF0;  //9

    memory[50] = 0x60;
    memory[51] = 0x90;
    memory[52] = 0x90;
    memory[53] = 0xf0;
    memory[54] = 0x90;  //A

    memory[55] = 0xe0;
    memory[56] = 0x90;
    memory[57] = 0xe0;
    memory[58] = 0x90;
    memory[59] = 0xe0;  //B

    memory[60] = 0xf0;
    memory[61] = 0x80;
    memory[62] = 0x80;
    memory[63] = 0x80;
    memory[64] = 0xf0;  //C

    memory[65] = 0xe0;
    memory[66] = 0x90;
    memory[67] = 0x90;
    memory[68] = 0x90;
    memory[69] = 0xe0;  //D

    memory[70] = 0xf0;
    memory[71] = 0x80;
    memory[72] = 0xf0;
    memory[73] = 0x80;
    memory[74] = 0xf0;  //E

    memory[75] = 0xf0;
    memory[76] = 0x80;
    memory[77] = 0xf0;
    memory[78] = 0x80;
    memory[79] = 0x80;  //E


}
