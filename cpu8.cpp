#include "cpu8.h"

cpu8::cpu8(QObject *parent) :
    QObject(parent)
{

}

cpu8::cpu8(QByteArray *rom, QObject *parent) : QObject(parent) {
    this->reset_memory();
    for (int i = 0; i < int(rom->size()); i++)
        memory[i + 0x200] = (unsigned char)rom->at(i);

    stack.resize(16);
    delay_timer = new QTimer();
    cpu_rate_timer = new QTimer();
    connect(delay_timer, SIGNAL(timeout()), this, SLOT(on_delay_timer_ticked()));
    connect(cpu_rate_timer, SIGNAL(timeout()), this, SLOT(on_cpu_timer_tick()));
    delay_value = 0;
    delay_timer->start(066);
    pressed_key = 0x48;
    load_fonts();
}

void cpu8::reset_memory() {
    for (int i = 0; i < 0xFFF; i++)
        memory[i] = 0;
    for (int i = 0; i < 0xF; i++)
        V[i] = 0;
    Ireg = 0x200;
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
        cpu_rate_timer->start(20);
        cpu_state = 1;
    }
    else {
        cpu_rate_timer->stop();
        cpu_state = 0;
    }
}

void cpu8::execute_opcode(unsigned short opcode) {
    switch (get_opcode(opcode)) {
    case 0x0:
        switch (opcode) {
        //clear the screen
        case 0x00E0:
            this->clear_videomem();
            qDebug() << tr("Clearing the screen");
            break;
        //return from subroutin
        case 0x00EE:
        PC = stack.pop();
        SP--;
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
        stack.push(PC);
        SP++;
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
        if ((V[get_x_opcode(opcode)] + get_nn_opcode(opcode)) > 0xFF)
            V[0xF] = 0x1;
        else
            V[0xF] = 0x0;
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
            qDebug() << QString::number(V[get_x_opcode(opcode)], 16) + tr(" and ") + QString::number(V[get_y_opcode(opcode)], 16) + tr(" = ") + QString::number(V[get_x_opcode(opcode)] & V[get_y_opcode(opcode)], 16);
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
            qDebug() << tr("Adding: ") + QString::number(V[get_x_opcode(opcode)], 16) + tr(" + ") + QString::number(V[get_y_opcode(opcode)], 16) + tr(" = ") + QString::number((V[get_x_opcode(opcode)] + V[get_y_opcode(opcode)]) & 0xFF, 16) + tr(" with carry: ") + QString::number(V[0xF], 16);
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
            V[0xF] = V[get_x_opcode(opcode)] & 80;
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
        break;
        //jump to NNN + V0
    case 0xB:
        PC = get_nnn_opcode(opcode) + V[0x0] - 0x02;
        break;
        //VX = random number
    case 0xC:
        V[get_x_opcode(opcode)] = get_random_number(0, 255) & get_nn_opcode(opcode);
        qDebug() << tr("Your random is ") + QString::number(V[get_x_opcode(opcode)], 16);
        break;
        //Draws at x = VX, y = VY sprite form I register with height of N
    case 0xD:
        unsigned short vmem_adress, vmem_offset, oreg, sdv, x, y;
        sdv = 0;
        oreg = 0;
        x = V[get_x_opcode(opcode)];
        y = V[get_y_opcode(opcode)];
        /*vmem_adress = convert_coordto_adress(V[get_x_opcode(opcode)], V[get_y_opcode(opcode)], vmem_offset);

        for (int i = 0; i < get_extendet_opcode(opcode); i++) {
            if (vmem_offset == 0)
                memory[0xF00 + ((vmem_adress + i * 0x8) % 0xFF)] ^= (memory[Ireg + i]);
            else {
                memory[0xF00 + ((vmem_adress + i * 0x8) % 0xFF)] ^= (memory[Ireg + i] >> vmem_offset);
                memory[0xF00 + ((vmem_adress + i * 0x8) % 0xFF) + 1] = memory[0xF00 + ((vmem_adress + i * 0x8) % 0xFF) + 1] ^ (unsigned char)(memory[Ireg + i] << (0x8 - vmem_offset)) >> (0x8 - vmem_offset);
                qDebug() << tr("Before ") + QString::number(memory[Ireg + i], 2) + tr(", after trimming the adress ") + QString::number(Ireg + i, 16) + tr(" with value ") + QString::number((unsigned char)((memory[Ireg + i] << (0x8 - vmem_offset)) >> (0x8 - vmem_offset)), 2);
            }
            qDebug() << tr("Iteration ") + QString::number(i, 10) + tr(" with mem adress ") + QString::number((0xF00 + vmem_adress + i * 0x8) & 0xFFFF, 16) + tr(" has value: ") + QString::number(memory[0xF00 + vmem_adress + i * 0x8], 16) + tr(". Nearest: ") + QString::number(memory[0xF00 + vmem_adress + i * 0x8 + 1], 16);
        }
        //for (int i =0; i <= 0xFF; i++)
         //   memory[0xF00 + i] = 0xAA;*/
       // if (get_extendet_opcode(opcode) == 0) - ДОЛЖНО БЫТЬ 1!!!!

        for (int i = 0; i < get_extendet_opcode(opcode); i++) {
            for (int n = 0; n < 8; n++) {
                video_mem[(y + i) % 32][(x + n) % 64] ^= ((memory[Ireg + i] >> (0x7 - n)) & 0x1) << (0x7 - n);
                if (((memory[Ireg + i] >> (0x7 - n)) & 0x1) << (0x7 - n) == 1 && ((video_mem[(y + i) % 32][(x + n) % 64] >> (0x7 - n)) & 0x1) << (0x7 - n) == 0)
                    V[0xF] = 1;
            }
        }
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
                memory[Ireg + 0x1] = (V[get_x_opcode(opcode)] % 100) / 10;
                memory[Ireg + 0x2] = V[get_x_opcode(opcode)] % 10;
                break;
            case 0x55:
                for (int i = 0; i <= get_x_opcode(opcode); i++)
                    memory[Ireg + i] = V[i];
                break;
            case 0x65:
                for (int i = 0; i <= get_x_opcode(opcode); i++)
                    V[i] = memory[Ireg + i];
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
    qDebug() << QString::number(this->PC, 10) + tr(" opcode: ") + QString::number(this->cop, 16);
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
    for (int i = 0; i < 32; i++) {
        for (int n = 0; n < 64; n++) {
            video_mem[i][n] = 0;
        }
    }
}

void cpu8::load_fonts() {
    memory[0] = 0xF0;
    memory[1] = 0x90;
    memory[2] = 0x90;
    memory[3] = 0x90;
    memory[4] = 0xF0;
    //memory[5] = 0x

}
