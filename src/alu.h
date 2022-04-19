enum CTRL {add, sub};
map<CTRL, string> op;
int alu(int a, int b, CTRL ctrl) {
    int res = 0;
    switch (ctrl) {
        case add: res = a + b; break;
        case sub: res = a - b; break;
    }
    return res;
}

CTRL get_ctrl(int opt, int funct = -1) {
    CTRL res = add;
    if (opt == 0) {
        // R
        switch (funct) {
            case 0x20: res = add; break;
            case 0x22: res = sub; break;
        }
    } else {
        switch (opt) {
            case 0x23: // lw
            case 0x2b: // sw
            case 0x0c: // addi
            case 0x06: res = add; break; // beqz
        }
    }
    return res;
}

void build_ctrl() {
    op[add] = "+";
    op[sub] = "-";
}