#include <bits/stdc++.h>
#define FATAL "FATAL ERROR: "

using namespace std;

const int MAX_N = 1e3 + 7;
char ins[MAX_N];

vector<string> convert(char *s) {
    int n = strlen(s);
    vector<string> res;
    string tmp = "";
    for (int i = 0; i < n; ++i) {
        // A -> a, B -> b, ..., Z -> z
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] += 'a' - 'A';
    }
    for (int i = 0; i < n; ++i) {
        while (s[i] != ' ' && i < n) tmp = tmp + s[i], ++i;
        if (tmp != "") res.emplace_back(tmp);
        tmp = "";
    }
    return res;
}

int eval(string s) {
    int res = 0, n = s.size();
    int start = (s[0] == '-') ? 1 : 0, f = 1;
    if (start) f = -1;
    for (int i = start; i < n; ++i) res = res * 10 + s[i] - '0';
    return res * f;
}

void get_r(const vector<string> &instruction, int &rs, int &rt, int &rd) {
    string RD = instruction[1], RS = instruction[2], RT = instruction[3];
    string tmp;
    tmp = "";
    for (int i = 1; i < RS.size(); ++i) tmp = tmp + RS[i];
    rs = eval(tmp) & ((1 << 5) - 1);
    tmp = "";
    for (int i = 1; i < RT.size(); ++i) tmp = tmp + RT[i];
    rt = eval(tmp) & ((1 << 5) - 1);
    tmp = "";
    for (int i = 1; i < RD.size(); ++i) tmp = tmp + RD[i];
    rd = eval(tmp) & ((1 << 5) - 1);
}

void get_lw_sw(const vector<string> &instruction, int &rs, int &rt, int &imm) {
    string tmp;
    int i;
    tmp = "";
    for (i = 1; i < instruction[1].size(); ++i) tmp = tmp + instruction[1][i];
    rt = eval(tmp) & ((1 << 5) - 1);
    tmp = "";
    for (i = 0; instruction[2][i] >= '0' && instruction[2][i] <= '9'; ++i)
        tmp = tmp + instruction[2][i];
    imm = eval(tmp) & ((1 << 16) - 1);
    tmp = "";
    for (; i < instruction[2].size(); ++i) {
        if (instruction[2][i] >= '0' && instruction[2][i] <= '9') tmp = tmp + instruction[2][i];
    }
    rs = eval(tmp) & ((1 << 5) - 1);
}

void get_i(const vector<string> &instruction, int &rs, int &rt, int &imm) {
    string tmp;
    tmp = "";
    for (int i = 1; i < instruction[1].size(); ++i) tmp = tmp + instruction[1][i];
    rt = eval(tmp) & ((1 << 5) - 1);
    tmp = "";
    for (int i = 1; i < instruction[2].size(); ++i) tmp = tmp + instruction[2][i];
    rs = eval(tmp) & ((1 << 5) - 1);
    tmp = "";
    for (int i = 0; i < instruction[3].size(); ++i) tmp = tmp + instruction[3][i];
    imm = eval(tmp) & ((1 << 16) - 1);
}

void get_beqz(const vector<string> &instruction, int &rs, int &imm) {
    string tmp;
    tmp = "";
    for (int i = 1; i < instruction[1].size(); ++i) tmp = tmp + instruction[1][i];
    rs = eval(tmp) & ((1 << 5) - 1);
    tmp = "";
    for (int i = 0; i < instruction[2].size(); ++i) tmp = tmp + instruction[2][i];
    imm = eval(tmp) & ((1 << 16) - 1);
}

int parse(char *s) {
    vector<string> instruction = convert(s);
    if (!instruction.size()) return -1;
    int res = 0;
    if (instruction[0] == "add") {
        int opt = 0 & ((1 << 6) - 1), funct = 0x20 & ((1 << 6) - 1);
        if (instruction.size() != 4) {
            cerr << FATAL << s << "\n";
            return -1;
        }
        int rs, rt, rd;
        get_r(instruction,rs, rt, rd);
        res = (opt << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (funct);
    } else if (instruction[0] == "sub") {
        int opt = 0 & ((1 << 6) - 1), funct = 0x22 & ((1 << 6) - 1);
        if (instruction.size() != 4) {
            cerr << FATAL << s << "\n";
            return -1;
        }        
        int rs, rt, rd;
        get_r(instruction, rs, rt, rd);
        res = (opt << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (funct);
    } else if (instruction[0] == "lw") {
        int opt = 0x23 & ((1 << 6) - 1);
        if (instruction.size() != 3) {
            cerr << FATAL << s << "\n";
            return -1;
        }
        int rs, rt, imm;
        get_lw_sw(instruction, rs, rt, imm);
        res = (opt << 26) | (rs << 21) | (rt << 16) | (imm);
    } else if (instruction[0] == "sw") {
        int opt = 0x2b & ((1 << 6) - 1);
        if (instruction.size() != 3) {
            cerr << FATAL << s << "\n";
            return -1;
        }
        int rs, rt, imm;
        get_lw_sw(instruction, rs, rt, imm);
        res = (opt << 26) | (rs << 21) | (rt << 16) | (imm);
    } else if (instruction[0] == "beqz") {
        int opt = 0x06 & ((1 << 6) - 1);
        if (instruction.size() != 3) {
            cerr << FATAL << s << "\n";
            return -1;
        }
        int rs, imm;
        get_beqz(instruction, rs, imm);
        res = (opt << 26) | (rs << 21) | (imm);
    } else if (instruction[0] == "addi") {
        int opt = 0x0c & ((1 << 6) - 1);
        if (instruction.size() != 4) {
            cerr << FATAL << s << "\n";
            return -1;
        }
        int rs, rt, imm;
        get_i(instruction, rs, rt, imm);
        res = (opt << 26) | (rs << 21) | (rt << 16) | (imm);
    }
    return res;
}

void read_write(string s) {
    ifstream IN(s);
    ofstream OUT("./pipeline.txt");
    while (IN.getline(ins, MAX_N)) {
        int out = parse(ins);
        if (out == -1) break;
        OUT << bitset<32>(out) << "\n";
    }
    IN.close();
    OUT.close();
}

int main(int argc, char *argv[]) {
    read_write(argv[1]);
    return 0;
}