#define DONOTHING "do nothing.\n"
int reg_vis[MAX_REG];
queue<INS> ID_ins, EX_ins, MEM_ins, WB_ins;
int id_cycle = -1;
string IF2ID;
pair<int, int> ID2EX;
int EX2MEM, MEM2WB;
int lst_EX2MEM, lst_MEM2WB;
int MEM_reg[MAX_INS], EX_reg[MAX_INS];
string IF_info, ID_info, EX_info, MEM_info, WB_info;
int flow[MAX_INS];

inline void time_space_modify(int cnt, int cycle, string stat) {
    time_space[cnt][cycle] = stat;
}

void IF() {
    IF_info = DONOTHING;
    bool flag = false;
    if (pc >= tot) {
        cout << "IF: there is no instructions needing to be fetched.\n";
        return;
    }
    if (!ID_ins.empty() || (ID_ins.empty() && cycle == id_cycle)) {
        cout << "IF: nothing to be done.\n";
        return;
    }
    ++cnt_used;
    time_space_modify(cnt, cycle, "IF");
    cout << "IF: working.\n";
    if (cnt > stall && cycle != stall_cycle) {
        time_space_modify(cnt, cycle, "STALL");
        IF_info = IF_info + "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        cout << "IF: \033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        return;
    }
    ofstream OUT("./tmp.txt");
    OUT << "IMAR<-PC(=main+0x" << hex << pc * 4 << ")\n";
    OUT << "IR<-INS_MEM[IMAR](=0x" << hex << ins[pc].inst << ")\n";
    OUT << "PC<-PC+4(=0x" << hex << (pc + 1) * 4 << ")\n";
    OUT.close();
    ifstream IN("./tmp.txt");
    IF_info = "";
    string tmp;
    while (IN >> tmp) IF_info = IF_info + tmp + "\n";
    IN.close();
    if (flag) return;
    flow[cnt] = pc;
    INS instruction = ins[pc];
    instruction.cnt = cnt++;
    instruction.pc = pc++;
    ID_ins.push(instruction);
}

void ID() {
    ID_info = DONOTHING;
    bool flag = false;
    if (ID_ins.empty()) {
        cout << "ID: nothing to be done.\n";
        return;
    }
    ++cnt_used;
    cout << "ID: working.\n";
    INS instruction = ID_ins.front();
    time_space_modify(instruction.cnt, cycle, "ID");
    int opt = instruction.opt, _cnt = instruction.cnt;
    int rs = instruction.rs, 
        rt = instruction.rt, 
        rd = instruction.rd, 
        imm = instruction.imm;
    if (!reg_vis[rs]) instruction.read_rs = true;
    if (!reg_vis[rt]) instruction.read_rt = true;
    if (_cnt > stall && cycle != stall_cycle) {
        time_space_modify(instruction.cnt, cycle, "STALL");
        ID_ins.pop();
        id_cycle = cycle;
        --pc;
        ID_info = ID_info + "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        cout << "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        return;
    }
    ID2EX = make_pair(reg[rs], reg[rt]);
    ID_info = "";
    if (opt == 0) {
        // R
        ofstream OUT("./tmp.txt");
        OUT << "A<-R" << rs << "\n";
        OUT << "B<-R" << rt << "\n";
        if (forwarding) {
            ++reg_vis[rd];
            cout << rd << endl;
        } else {
            if (reg_vis[rs] || reg_vis[rt]) { // stall
                time_space_modify(instruction.cnt, cycle, "STALL");
                OUT << "\033[31mstall_happened_cause_RAW_occurred.\033[0m\n";
                cout << "\033[31mstall happened.\033[0m\n";
                flag = true;
            } else {
                ++reg_vis[rd]; // rd is gonna be written
            }
        }
        OUT.close();
        ifstream IN("./tmp.txt");
        string tmp;
        while (IN >> tmp) ID_info = ID_info + tmp + "\n";
        IN.close();
        if (flag) return;
    } else {
        if (opt == 0x23) {
            // lw
            ofstream OUT("./tmp.txt");
            OUT << "A<-R" << rs << "\n";
            if (forwarding) {
                ++reg_vis[rt];
            } else {
                if (reg_vis[rs]) { // stall
                    time_space_modify(instruction.cnt, cycle, "STALL");
                    OUT << "\033[31mstall_happened_cause_RAW_occurred.\033[0m\n";
                    cout << "\033[31mstall happened.\033[0m\n";
                    flag = true;
                } else {
                    ++reg_vis[rt];
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) ID_info = ID_info + tmp + "\n";
            IN.close();
            if (flag) return;
        } else if (opt == 0x2b) {
            // sw
            ofstream OUT("./tmp.txt");
            OUT << "A<-R" << rs << "\n";
            if (forwarding) {

            } else {
                if (reg_vis[rs]) { // stall
                    time_space_modify(instruction.cnt, cycle, "STALL");
                    OUT << "\033[31mstall_happened_cause_RAW_occurred.\033[0m\n";
                    cout << "\033[31mstall happened.\033[0m\n";
                    flag = true;
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) ID_info = ID_info + tmp + "\n";
            IN.close();
            if (flag) return;
        } else if (opt == 0x06) {
            // beqz
            ofstream OUT("./tmp.txt");
            OUT << "A<-R" << rs << "\n";
            stall = instruction.cnt;
            stall_cycle = cycle;
            if (forwarding) {

            } else {
                if (reg_vis[rs]) { // stall
                    time_space_modify(instruction.cnt, cycle, "STALL");
                    OUT << "\033[31mstall_happened_cause_RAW_occurred.\033[0m\n";
                    cout << "\033[31mstall happened.\033[0m\n";
                    flag = true;
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) ID_info = ID_info + tmp + "\n";
            IN.close();
            if (flag) return;
        } else if (opt == 0x0c) {
            // addi
            ofstream OUT("./tmp.txt");
            OUT << "A<-R" << rs << "\n";
            if (forwarding) {
                ++reg_vis[rt];
            } else {
                if (reg_vis[rs]) { // stall
                    time_space_modify(instruction.cnt, cycle, "STALL");
                    OUT << "\033[31mstall_happened_cause_RAW_occurred.\033[0m\n";
                    cout << "\033[31mstall happened.\033[0m\n";
                    flag = true;
                } else {
                    ++reg_vis[rt];
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) ID_info = ID_info + tmp + "\n";
            IN.close();
            if (flag) return;
        }
    }
    EX_ins.push(instruction);
    ID_ins.pop();
}

void EX() {
    lst_EX2MEM = EX2MEM;
    EX_info = DONOTHING;
    bool flag = false;
    if (EX_ins.empty()) {
        cout << "EX: nothing to be done.\n";
        return;
    }
    ++cnt_used;
    cout << "EX: working.\n";
    INS instruction = EX_ins.front();
    time_space_modify(instruction.cnt, cycle, "EX");
    int opt = instruction.opt, _cnt = instruction.cnt;
    int rs = instruction.rs, 
        rt = instruction.rt,
        imm = instruction.imm, 
        funct = instruction.funct;
    if (_cnt > stall && cycle != stall_cycle) {
        time_space_modify(instruction.cnt, cycle, "STALL");
        EX_info = EX_info + "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        cout << "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        return;
    }
    EX_info = "";
    int a = ID2EX.first, b = ID2EX.second, res;
    if (opt == 0) {
        // R
        CTRL ctrl = get_ctrl(opt, funct);
        ofstream OUT("./tmp.txt");
        OUT << "ALU<-A" << op[ctrl] << "B\n"; 
        if (forwarding) {
            a = instruction.read_rs ? ID2EX.first : reg[rs];
            b = instruction.read_rs ? ID2EX.second : reg[rt];
            if ((reg_vis[rs] && (!instruction.read_rs)) || (reg_vis[rt] && (!instruction.read_rt))) {
                // forwarding
                OUT << "\033[32mforwarding_applicated.\033[0m\n";
                int conflict = ((reg_vis[rs] && (!instruction.read_rs)) + 
                    (reg_vis[rt] && (!instruction.read_rt)));
                if ((reg_vis[rs] && (!instruction.read_rs))) {
                    if (EX_reg[cycle - 1] == rs) {
                        --conflict;
                        a = lst_EX2MEM;
                        OUT << "A<-pipeline_register:EX2MEM\n";
                    } else if (MEM_reg[cycle - 1] == rs) {
                        --conflict;
                        OUT << "A<-pipeline_register:MEM2WB\n";
                        a = lst_MEM2WB;
                    }
                }
                if ((reg_vis[rt] && (!instruction.read_rt))) {
                    if (EX_reg[cycle - 1] == rt) {
                        --conflict;
                        OUT << "B<-pipeline_register:EX2MEM\n";
                        b = lst_EX2MEM;
                    } else if (MEM_reg[cycle - 1] == rt) {
                        --conflict;
                        OUT << "B<-pipeline_register:MEM2WB\n";
                        b = lst_MEM2WB;
                    }
                }
                if (conflict > 0) {
                    // stall 
                    time_space_modify(instruction.cnt, cycle, "STALL");
                    OUT << "but_stall_still_happened_cause_RAW_occurred.\n";
                    cout << "\033[31mstall happened.\033[0m\n";
                    flag = true;
                }
            }
        }
        OUT.close();
        ifstream IN("./tmp.txt");
        string tmp;
        while (IN >> tmp) EX_info = EX_info + tmp + "\n";
        IN.close();
        if (flag) return;
        res = alu(a, b, ctrl);
        EX_reg[cycle] = instruction.rd;
    } else {
        if (opt == 0x23 || opt == 0x2b) {
            // lw and sw
            b = imm;
            CTRL ctrl = get_ctrl(opt);
            ofstream OUT("./tmp.txt");
            OUT << "ALU<-A" << op[ctrl] << "(" << hex << b << ")\n";
            if (forwarding) {
                a = instruction.read_rs ? ID2EX.first : reg[rs];
                if ((reg_vis[rs] && (!instruction.read_rs))) {
                    // forwarding
                    OUT << "\033[32mforwarding_applicated.\033[0m\n";
                    int conflict = (reg_vis[rs] && (!instruction.read_rs));
                    if (EX_reg[cycle - 1] == rs) {
                        --conflict;
                        a = lst_EX2MEM;
                        OUT << "A<-pipeline_register:EX2MEM\n";
                    } else if (MEM_reg[cycle - 1] == rs) {
                        --conflict;
                        OUT << "A<-pipeline_register:MEM2WB\n";
                        a = lst_MEM2WB;
                    }
                    if (conflict > 0) {
                        // stall 
                        time_space_modify(instruction.cnt, cycle, "STALL");
                        OUT << "but_stall_still_happened_cause_RAW_occurred.\n";
                        cout << "\033[31mstall happened.\033[0m\n";
                        flag = true;
                    }
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) EX_info = EX_info + tmp + "\n";
            IN.close();
            if (flag) return;
            res = alu(a, b, ctrl);
        } else if (opt == 0x06) {
            // beqz
            int flag2 = ID2EX.first;
            ofstream OUT("./tmp.txt");
            CTRL ctrl = get_ctrl(opt);
            OUT << "ALU<-PC" << op[ctrl] << "(" << imm << ")\n";
            OUT << "ALU<-A" << rs << "-(0)\n";
            if (forwarding) {
                a = instruction.read_rs ? ID2EX.first : reg[rs];
                if ((reg_vis[rs] && (!instruction.read_rs))) {
                    // forwarding
                    OUT << "\033[32mforwarding_applicated.\033[0m\n";
                    int conflict = (reg_vis[rs] && (!instruction.read_rs));
                    if (EX_reg[cycle - 1] == rs) {
                        --conflict;
                        flag2 = lst_EX2MEM;
                        OUT << "A<-pipeline_register:EX2MEM\n";
                    } else if (MEM_reg[cycle - 1] == rs) {
                        --conflict;
                        OUT << "A<-pipeline_register:MEM2WB\n";
                        flag2 = lst_MEM2WB; 
                    }
                    if (conflict > 0) {
                        // stall 
                        time_space_modify(instruction.cnt, cycle, "STALL");
                        OUT << "but_stall_still_happened_cause_RAW_occurred.\n";
                        cout << "\033[31mstall happened.\033[0m\n";
                        flag = true;
                    }
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) EX_info = EX_info + tmp + "\n";
            IN.close();
            if (flag) return;
            a = instruction.pc + 1;
            b = imm;
            res = alu(a, b, ctrl);
            zf = flag2 ? false : true;
        } else if (opt == 0x0c) {
            // addi
            CTRL ctrl = get_ctrl(opt);
            ofstream OUT("./tmp.txt");
            OUT << "ALU<-A" << op[ctrl] << "(" << imm << ")\n";
            if (forwarding) {
                a = instruction.read_rs ? ID2EX.first : reg[rs];
                if ((reg_vis[rs] && (!instruction.read_rs))) {
                    // forwarding
                    OUT << "\033[32mforwarding_applicated.\033[0m\n";
                    int conflict = (reg_vis[rs] && (!instruction.read_rs));
                    if (EX_reg[cycle - 1] == rs) {
                        --conflict;
                        a = lst_EX2MEM;
                        OUT << "A<-pipeline_register:EX2MEM\n";
                    } else if (MEM_reg[cycle - 1] == rs) {
                        --conflict;
                        a = lst_MEM2WB; 
                        OUT << "A<-pipeline_register:MEM2WB\n";
                    }
                    if (conflict > 0) {
                        // stall 
                        time_space_modify(instruction.cnt, cycle, "STALL");
                        OUT << "but_stall_still_happened_cause_RAW_occurred.\n";
                        cout << "\033[31mstall happened.\033[0m\n";
                        flag = true;
                    }
                }
            }
            OUT.close();
            ifstream IN("./tmp.txt");
            string tmp;
            while (IN >> tmp) EX_info = EX_info + tmp + "\n";
            IN.close();
            if (flag) return;
            b = imm;
            res = alu(a, b, ctrl);
            EX_reg[cycle] = instruction.rt;
        }
    }
    EX2MEM = res;
    MEM_ins.push(instruction);
    EX_ins.pop();
}

void MEM() {
    lst_MEM2WB = MEM2WB;
    MEM_info = DONOTHING;
    if (MEM_ins.empty()) {
        cout << "MEM: nothing to be done.\n";
        return;
    }
    ++cnt_used;
    cout << "MEM: working.\n";
    INS instruction = MEM_ins.front();
    time_space_modify(instruction.cnt, cycle, "MEM");
    int opt = instruction.opt, _cnt = instruction.cnt;
    int rt = instruction.rt;
    if (_cnt > stall && cycle != stall_cycle) {
        time_space_modify(instruction.cnt, cycle, "STALL");
        MEM_info = MEM_info + "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        cout << "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        return;
    }
    if (opt == 0) {
        MEM2WB = EX2MEM;
        MEM_reg[cycle] = instruction.rd;
    } else {
        if (opt == 0x23) {
            // lw
            MEM2WB = mem[EX2MEM];
            MEM_reg[cycle] = instruction.rt;
            MEM_info = "LDR<-DATA_MEM[ALU]\n";
        } else if (opt == 0x2b) {
            // sw
            mem[EX2MEM] = reg[rt];
            ofstream OUT("./tmp.txt");
            OUT << "DATA_MEM[ALU]<-R" << rt << "(=0x" << reg[rt] << ")\n";
            OUT.close();
            ifstream IN("./tmp.txt");
            IN >> MEM_info;
            MEM_info = MEM_info + "\n";
            IN.close();
        } else if (opt == 0x06) {
            // beqz
            if (zf) {
                pc = EX2MEM;
                MEM_info = "PC<-ALU\n";
            }
        } else if (opt == 0x0c) {
            // addi
            MEM2WB = EX2MEM;
            MEM_reg[cycle] = instruction.rt;
        }
    }
    WB_ins.push(instruction);
    MEM_ins.pop();
}

void WB() {
    WB_info = DONOTHING;
    if (WB_ins.empty()) {
        cout << "WB: nothing to be done.\n";
        return;
    }
    ++cnt_used;
    cout << "WB: working.\n";
    INS instruction = WB_ins.front();
    time_space_modify(instruction.cnt, cycle, "WB");
    int opt = instruction.opt, _cnt = instruction.cnt;
    int rd = instruction.rd,
        rt = instruction.rt;
    if (_cnt > stall && cycle != stall_cycle) {
        time_space_modify(instruction.cnt, cycle, "STALL");
        WB_info = WB_info + "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        cout << "\033[31mstall occurred cause the branch instruction had been decoded.\033[0m\n";
        return;
    }
    if (opt == 0) {
        // R
        reg[rd] = MEM2WB;
        --reg_vis[rd]; // writing is done
        ofstream OUT("./tmp.txt");
        OUT << "R" << rd << "<-ALU\n";
        OUT.close();
        ifstream IN("./tmp.txt");
        IN >> WB_info;
        WB_info = WB_info + "\n";
    } else {
        if (opt == 0x23) { 
            // lw
            reg[rt] = MEM2WB;
            --reg_vis[rt];
            ofstream OUT("./tmp.txt");
            OUT << "R" << rt << "<-LDR\n";
            OUT.close();
            ifstream IN("./tmp.txt");
            IN >> WB_info; 
            WB_info = WB_info + "\n";
            IN.close();
        } else if (opt == 0x0c) {
            // addi
            reg[rt] = MEM2WB;
            --reg_vis[rt];
            ofstream OUT("./tmp.txt");
            OUT << "R" << rt << "<-ALU\n";
            OUT.close();
            ifstream IN("./tmp.txt");
            IN >> WB_info;
            WB_info = WB_info + "\n";
            IN.close();
        } else if (opt == 0x06) {
            // beqz
            stall = INF;
        }
    }
    ++cnt_wb;
    WB_ins.pop();
}