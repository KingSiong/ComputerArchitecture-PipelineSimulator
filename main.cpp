#include <bits/stdc++.h>
#define INF (1e9 + 7)

using namespace std;

//===========================================

struct INS{
    string instruction;
    int inst = 0;
    bool read_rs, read_rt;
    int opt, rs, rt, rd, imm, shift, funct;
    int cnt, pc;
    void reset(string _instruction) { 
        read_rs = read_rt = false;
        instruction = _instruction; 
        parse();
    }
    void parse() {
        int ins = 0;
        for (int i = 0; i < 32; ++i) ins = (ins << 1) + (instruction[i] - '0');
        inst = ins;
        opt = (ins >> 26) & ((1 << 6) - 1);
        rs = (ins >> 21) & ((1 << 5) - 1);
        rt = (ins >> 16) & ((1 << 5) - 1);
        rd = (ins >> 11) & ((1 << 5) - 1);
        imm = ins & ((1 << 16) - 1);
        if ((imm >> 15) & 1) imm |= 0xffff0000;
        shift = ins & ((1 << 26) - 1);
        funct = ins & ((1 << 6) - 1);
    }
};

//===========================================

const int MAX_INS = 1 << 10;
const int MAX_MEM = 1 << 6;
const int MAX_REG = 1 << 5;
INS ins[MAX_INS];
string origin_ins[MAX_INS];
int mem[MAX_INS];
int reg[MAX_REG];
vector<string> time_space[MAX_INS];
int tot;
int pc = 0;
int cnt_wb = 0;
int cnt_used = 0;
int cycle = 0;
int stall = INF;
int stall_cycle = -1;
int cnt = 0;
bool zf = false;
bool forwarding = false;
map<string, string> ts2color;

//=========================================== read api

#include "read.h"

//=========================================== alu

#include "alu.h"

//=========================================== modules for pipeline

#include "module.h"

//=========================================== useful api

void show_help() {
    cout << "======================================= help =======================================\n";
    cout << "\033[43m EXTERNAL COMMAND:\033[0m\n"; // external
    // ./run.sh
    cout << "\033[32m\033[1m./run.sh [assembly.s]\033[0m :";
    cout << "to run this simulator with assembly.s or sample.s by default.\n";
    cout << "\033[43m INTERNAL COMMAND:\033[0m\n"; // internal
    // exit
    cout << "\033[32m\033[1mexit, \033[0m";
    cout << "\033[32m\033[1mq, \033[0m";
    cout << "\033[32m\033[1mQ\033[0m :";
    cout << "to exit from this process.\n";
    // continue
    cout << "\033[32m\033[1mCONTINUE, \033[0m";
    cout << "\033[32m\033[1mcontinue, \033[0m";
    cout << "\033[32m\033[1mC, \033[0m";
    cout << "\033[32m\033[1mc\033[0m :";
    cout << "to continue next step while running step by step.\n";
    // memory
    cout << "\033[32m\033[1mmem [num]\033[0m :";
    cout << "to show the certain memory[num] or all memories' status by default.\n";
    // register
    cout << "\033[32m\033[1mreg [num]\033[0m :";
    cout << "to show the certain register[num] or all registers' status by default.\n";
    // IF, ID, EX, MEM, WB
    cout << "\033[32m\033[1mIF, ID, EX, MEM, WB\033[0m :";
    cout << "to show the status of the specific pipeline module.\n";
    // time space
    cout << "\033[32m\033[1mtime-space [cycle]\033[0m :";
    cout << "to show the time space graph on particular cycle or now by default.\n";
    // through put
    cout << "\033[32m\033[1mtp, TP\033[0m :";
    cout << "to show the through put after all instructions were done.\n";
    // speedup
    cout << "\033[32m\033[1ms, S\033[0m :";
    cout << "to show the speedup after all instrutions were done.\n";
    // efficiency
    cout << "\033[32m\033[1me, E\033[0m :";
    cout << "to show the efficiency after all instructions were done.\n";
    // help
    cout << "\033[32m\033[1mh, help\033[0m :";
    cout << "to show the help documentation as you can see now.\n";
    cout << "====================================================================================\n";
}

void show_mem(int num = -1) {
    if (~num) {
        if (num >= MAX_MEM) {
            cout << "segmentation fault. the size of memory if not so big.\n";
        } else {
            cout << "memory" << "[" << num << "]:" << bitset<32>(mem[num]) << "\n";
        }
    } else {
        for (int i = 0; i < MAX_MEM; ++i) 
            cout << "memory" << "[" << i << "]:" << bitset<32>(mem[i]) << "\n";
    }
}

void show_reg(int num = -1) {
    if (~num) {
        if (num >= MAX_REG) {
            cout << "segmentation fault. the size of register file if not so big.\n";
        } else {
            cout << "register" << "[" << num << "]:" << bitset<32>(reg[num]) << "\n";
        }
    } else {
        for (int i = 0; i < MAX_REG; ++i) {
            cout << "register" << "[" << i << "]:" << bitset<32>(reg[i]) << "\n";
        }
    }
}

const int MAX_WIDTH = 10;
void show_time_space(int _cycle = -1) {
    if (_cycle == -1) _cycle = cycle;
    cout << "\033[33m time-space graph:\033[0m\n";
    cout << left << setw(24) << "";
    int start = max(0, _cycle - MAX_WIDTH + 1);
    for (int j = start; j <= _cycle; ++j) cout << left << setw(9) << j;
    cout << "\n";
    for (int i = 0; i < cnt; ++i) {
        cout << left << setw(24) << origin_ins[flow[i]];
        for (int j = start; j <= _cycle; ++j) {
            cout << ts2color[time_space[i][j]] << left << setw(9) << time_space[i][j] << "\033[0m";
        }
        cout << left << setw(9) << " ";
        cout << "\n";
    }
}

void show_IF_info() { cout << IF_info; }

void show_ID_info() { cout << ID_info; }

void show_EX_info() { cout << EX_info; }

void show_MEM_info() { cout << MEM_info; }

void show_WB_info() { cout << WB_info; }

bool chk_end() {
    for (int i = 0; i < cnt; ++i) {
        if (time_space[i][cycle] != "") return false;
    }
    return true;
}

void show_through_put() {
    if (!chk_end()) {
        cout << "instructions are still running.\n";
        return;
    }
    cout << "through put is: " << setiosflags(ios::fixed) << setprecision(6) << 
        1.0 * cnt_wb / cycle << " (IPC)\n";
}

void show_speedup() {
    if (!chk_end()) {
        cout << "instructions are still running.\n";
        return;
    }
    cout << "speedup is: " << setiosflags(ios::fixed) << setprecision(6) << 
        5.0 * cnt_wb / cycle << "\n";
}

void show_efficiency() {
    if (!chk_end()) {
        cout << "instructions are still running.\n";
        return;
    }
    cout << "efficiency is: " << setiosflags(ios::fixed) << setprecision(6) << 
        5.0 * cnt_wb / (cycle * cnt) << "\n";
    cout << "efficiency(take stall into account) is: " << setiosflags(ios::fixed) << setprecision(6) << 
        1.0 * cnt_used / (cycle * cnt) << "\n";
}

vector<string> convert(char *s) {
    int n = strlen(s);
    vector<string> res;
    string tmp = "";
    for (int i = 0; i < n; ++i) {
        while (s[i] != ' ' && i < n) tmp = tmp + s[i], ++i;
        if (tmp != "") res.emplace_back(tmp);
        tmp = "";
    }
    return res;
}

bool chk_int(string s) {
    int n = s.size();
    for (int i = 0; i < n; ++i) if (s[i] > '9' || s[i] < '0') return false;
    return true;
}

int eval(string s) {
    int res = 0, n = s.size();
    for (int i = 0; i < n; ++i) res = res * 10 + s[i] - '0';
    return res;
}

bool exit(string cmd) {
    if (cmd == "q" || cmd == "Q" || cmd == "exit") {
        return true;
    }
    return false;
}

bool help(string cmd) {
    if (cmd == "help" || cmd == "h") {
        show_help();
        return true;
    }
    return false;
}

bool cmd_parse(bool &flag, vector<string> &cmd) {
    if (cmd[0] == "q" || cmd[0] == "Q" || cmd[0] == "exit") {
        flag = true;
        return true;
    }
    if (cmd[0] == "c" || cmd[0] == "C" || cmd[0] == "CONTINUE" || cmd[0] == "continue") {
        return true;
    }
    if (cmd[0] == "mem") {
        int num = -1;
        if (cmd.size() > 1) {
            if (chk_int(cmd[1])) {
                num = eval(cmd[1]);
            } else {
                cout << "command mem: integer required.\n";
                return false;
            }
        }
        show_mem(num);
        return false;
    }
    if (cmd[0] == "reg") {
        int num = -1;
        if (cmd.size() > 1) {
            if (chk_int(cmd[1])) {
                num = eval(cmd[1]);
            } else {
                cout << "command reg: integer required.\n";
                return false;
            }
        }
        show_reg(num);
        return false;
    }
    if (cmd[0] == "IF") {
        show_IF_info();
        return false;
    }
    if (cmd[0] == "ID") {
        show_ID_info();
        return false;
    }
    if (cmd[0] == "EX") {
        show_EX_info();
        return false;
    }
    if (cmd[0] == "MEM") {
        show_MEM_info();
        return false;
    }
    if (cmd[0] == "WB") {
        show_WB_info();
        return false;
    }
    if (cmd[0] == "time-space") {
        int _cycle = -1;
        if (cmd.size() > 1) {
            if (chk_int(cmd[1])) {
                _cycle = eval(cmd[1]);
            } else {
                cout << "command time-space: integer required.\n";
                return false;
            } 
        }
        show_time_space(_cycle);
        return false;
    }
    if (cmd[0] == "tp" || cmd[0] == "TP") {
        show_through_put();
        return false;
    }
    if (cmd[0] == "s" || cmd[0] == "S") {
        show_speedup();
        return false;
    }
    if (cmd[0] == "e" || cmd[0] == "E") {
        show_efficiency();
        return false;
    }
    if (cmd[0] == "h" || cmd[0] == "help") {
        show_help();
        return false;
    }
    cout << "invalid command. please try again.\n";
    return false;
}

//===========================================

const int MAX_N = 1e3 + 7;
char cmd_line[MAX_N];
int main(int argc, char *argv[]) {
    read("./pipeline.txt");
    read_origin(argv[1]);
    memset(MEM_reg, -1, sizeof MEM_reg);
    memset(EX_reg, -1, sizeof EX_reg);
    build_ctrl();
    ts2color["IF"] = "\033[33m"; ts2color["ID"] = "\033[34m"; ts2color["EX"] = "\033[31m";
    ts2color["MEM"] = "\033[32m"; ts2color["WB"] = "\033[35m"; ts2color[""] = "\033[30m";
    ts2color["STALL"] = "\033[41m";
    cout << "\nthe MIPS32 instructions set pipeline simulator is now ready.\n";
    cout << "the number of instructions in " << argv[1] << " is " << tot << "\n";
    cout << "\033[43mHINT:\033[0m you can type help or h to get help.\n\n";
    int opt = 0, bp = 0;
    bool flag;
    while (true) {
        cout << "select the execution way you want:\n";
        cout << "for running step by step [type: 0]\n";
        cout << "for jumping to the break point [type: 1]\n";
        cout << "for executing all the program and seeing the result [type: 2]\n";
        cout << "> ";
        cin.getline(cmd_line, MAX_N);
        vector<string> cmd = convert(cmd_line);
        if (!cmd.size()) continue;
        if (exit(cmd[0])) return 0;
        if (help(cmd[0])) continue;
        if (!chk_int(cmd[0])) {
            cout << "integer required.\n";
            continue;
        }
        opt = eval(cmd[0]);
        if (opt > 2) {
            cout << "invalid input. try again.\n";
            continue;
        }
        break;
    }
    if (opt == 1) {
        cout << "input break point(integer: 0~" << tot - 1 << ")\n";
        while (true) {
            cout << "> ";
            cin.getline(cmd_line, MAX_N);
            vector<string> cmd = convert(cmd_line);
            if (!cmd.size()) continue;
            if (exit(cmd[0])) return 0;
            if (help(cmd[0])) continue;
            if (!chk_int(cmd[0])) {
                cout << "integer required.\n";
                continue;
            }
            bp = eval(cmd[0]);
            if (bp < 0 || bp >= tot) {
                cout << "incorrect point.\n";
                continue;
            }
            break;
        }
    } else if (opt == 2) {
        bp = INF;
    }
    cout << "would you like to use forwarding? [yes(y)/no(n)]\n";
    while (true) {
        cout << "> ";
        cin.getline(cmd_line, MAX_N);
        vector<string> cmd = convert(cmd_line);
        if (!cmd.size()) continue;
        if (exit(cmd[0])) return 0;
        if (help(cmd[0])) continue;
        if (cmd[0][0] == 'y' || cmd[0][0] == 'Y') {
            forwarding = true;
        } else if (cmd[0][0] == 'n' || cmd[0][0] == 'N') {
            forwarding = false;
        } else {
            cout << "invalid input. try again.\n";
            continue;
        }
        break;
    }
    for (cycle = 0; ; ++cycle) {
        cout << "cycle #" << cycle << ":\n";
        for (int i = 0; i < MAX_INS; ++i) time_space[i].emplace_back("");
        int lst_pc = pc;
        WB();
        MEM();
        EX();
        ID();
        IF();
        if (chk_end()) break;
        if (lst_pc != bp) continue;
        bp = pc;
        show_time_space();
        flag = false;
        while (true) {
            cout << "> ";
            cin.getline(cmd_line, MAX_N);
            vector<string> cmd = convert(cmd_line);
            if (!cmd.size()) continue;
            if (cmd_parse(flag, cmd)) break;
        }
        if (flag) return 0;
    }
    cout << "\n\033[43mexecution has been done.\033[0m\n";
    show_time_space(cycle - 1);
    while (true) {
        flag = false;
        while (true) {
            cout << "> ";
            cin.getline(cmd_line, MAX_N);
            vector<string> cmd = convert(cmd_line);
            if (!cmd.size()) continue;
            if (cmd_parse(flag, cmd)) break;
        }
        if (flag) return 0;
    }
    return 0;
}