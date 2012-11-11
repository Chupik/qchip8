#include "cpu8.h"

cpu8::cpu8(QObject *parent) :
    QObject(parent)
{
    stack.resize(16);
}

cpu8::cpu8(QByteArray *rom, QObject *parent) : QObject(parent) {
    this->reset_memory();
    for (int i = 0; i < int(rom->size()); i++)
        memory[i + 0x200] = (unsigned char)rom->at(i);
}

void cpu8::reset_memory() {
    for (int i = 0; i < 0xFFF; i++)
        memory[i] = 0;
    for (int i = 0; i < 0xF; i++)
        V[i] = 0;
    Ireg = 0x200;
    PC = 0x200;
    SP = 0;
    cpu_state = 1;
}

void cpu8::run_cpu() {
   // while (cpu_state) {
    cop = generate_opcode(PC);
        execute_opcode(cop);
   // }
}

void cpu8::execute_opcode(unsigned short opcode) {
    switch (get_opcode(opcode)) {
    case 0x0:
        switch (opcode)
        case 0x00E0:
            //clear_videomem();
            break;
        case 0x00EE:
        PC = stack.pop();
        SP--;
        break;
        default:
            PC = get_nnn_opcode(opcode);
        break;
    case 0x1:
        PC = get_nnn_opcode(opcode);
        break;
    case 0x2:
        stack.push(PC);
        SP++;
        PC = get_nnn_opcode(opcode) - 2;
        break;
    case 0x3:
        if (V[get_x_opcode(opcode)] == get_nn_opcode(opcode))
            PC += 2;
        break;
    case 0x4:
        if (V[get_x_opcode(opcode)] != get_nn_opcode(opcode))
            PC += 2;
        break;
    case 0x5:
        if (V[get_x_opcode(opcode)] == V[get_y_opcode(opcode)])
            PC += 2;
        break;
    case 0x6:
        V[get_x_opcode(opcode)] = get_nn_opcode(opcode);
        break;
    case 0x7:
        V[get_x_opcode(opcode)] += get_nn_opcode(opcode);
        break;
    case 0x8:

        switch (get_extendet_opcode(opcode)) {
        case 0x0:
            V[get_x_opcode(opcode)] = V[get_y_opcode(opcode)];
            break;
        case 0x1:
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] | V[get_y_opcode(opcode)];
            break;
        case 0x2:
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] & V[get_y_opcode(opcode)];
            break;
        case 0x3:
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] ^ V[get_y_opcode(opcode)];
            break;
        case 0x4:
            V[get_x_opcode(opcode)] += V[get_y_opcode(opcode)];
            V[0xF] = (V[get_x_opcode(opcode)] & 0xF00) >> 8;
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode) & 0x0FF];
            break;
        case 0x5:
            if (V[get_x_opcode(opcode)] >= V[get_y_opcode(opcode)])
                V[0xF] = 0x01;
            else
                V[0xF] = 0x00;
            V[get_x_opcode(opcode)] -= V[get_y_opcode(opcode)];
            break;
        case 0x6:
            V[0xF] = V[get_x_opcode(opcode)] & 0x1;
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] >> 1;
            break;
        case 0x7:
            if (V[get_y_opcode(opcode)] >= V[get_x_opcode(opcode)])
                V[0xF] = 0x01;
            else
                V[0xF] = 0x00;
            V[get_x_opcode(opcode)] = V[get_y_opcode(opcode)] - V[get_x_opcode(opcode)];
            break;
        case 0xE:
            V[0xF] = V[get_x_opcode(opcode)] & 80;
            V[get_x_opcode(opcode)] = V[get_x_opcode(opcode)] << 1;
            break;
        }

        break;
    case 0x9:
        if (V[get_x_opcode(opcode)] != V[get_y_opcode(opcode)])
            PC +=2;
        break;
    case 0xA:
        Ireg = get_nnn_opcode(opcode);
        break;
    case 0xB:
        PC = get_nnn_opcode(opcode) + V[0x0];
        break;
    case 0xC:
        V[get_x_opcode(opcode)] = get_random_number(0, 255) & get_nn_opcode(opcode);
        break;
    case 0xD:
        unsigned short vmem_adress, vmem_offset, oreg, sdv, x, y;
        sdv = 0;
        oreg = 0;
        x = V[get_x_opcode(opcode)];
        y = V[get_y_opcode(opcode)];
        vmem_adress = convert_coordto_adress(V[get_x_opcode(opcode)], V[get_y_opcode(opcode)], vmem_offset);

        for (int i = 0; i <= get_extendet_opcode(opcode); i++) {
            if (vmem_offset == 0)
                memory[0xF00 + vmem_adress + i * 0x8] ^= (memory[Ireg + i]);
            else {
                memory[0xF00 + vmem_adress + i * 0x8] ^= (memory[Ireg + i] >> vmem_offset);
                memory[0xF00 + vmem_adress + i * 0x8 + 1] = memory[0xF00 + vmem_adress + i * 0x8 + 1] ^ (unsigned char)(memory[Ireg + i] << (0x8 - vmem_offset)) >> (0x8 - vmem_offset);
                qDebug() << tr("Before ") + QString::number(memory[Ireg + i], 2) + tr(", after trimming the adress ") + QString::number(Ireg + i, 16) + tr(" with value ") + QString::number((unsigned char)((memory[Ireg + i] << (0x8 - vmem_offset)) >> (0x8 - vmem_offset)), 2);
            }
            qDebug() << tr("Iteration ") + QString::number(i, 10) + tr(" with mem adress ") + QString::number((0xF00 + vmem_adress + i * 0x8) & 0xFFFF, 16) + tr(" has value: ") + QString::number(memory[0xF00 + vmem_adress + i * 0x8], 16) + tr(". Nearest: ") + QString::number(memory[0xF00 + vmem_adress + i * 0x8 + 1], 16);
        }
        //for (int i =0; i <= 0xFF; i++)
         //   memory[0xF00 + i] = 0xAA;
        break;
    case 0xE:
        //clava
        break;
    case 0xF:
        //timer, klava and others
        break;
    }
PC+=2;
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

void cpu8::copy_memory(unsigned short from_adress, unsigned short to_adress) {
    memory[to_adress] = memory[from_adress];
}

unsigned short cpu8::convert_coordto_adress(unsigned char x, unsigned char y, unsigned short &offset) {
    offset = x % 0x8;
    return y * 0x8 + x / 0x8;
}

void cpu8::clear_videomem() {
    for (int i =0; i <= 0xFF; i++)
        memory[i + 0xF00] = 0;
}
