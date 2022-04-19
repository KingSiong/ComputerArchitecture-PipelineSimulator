void read(string s) {
    ifstream IN(s);
    string instruction;
    tot = 0;
    while (IN >> instruction) {
        ins[tot++].reset(instruction);
    }
    IN.close();
}

const int MAX_ORIGIN = 1e3 + 7;
void read_origin(string s) {
    char instruction[MAX_ORIGIN];
    ifstream IN(s);
    int cnt = 0;
    string tmp;
    while (IN.getline(instruction, MAX_ORIGIN)) {
        tmp = "";
        int n = strlen(instruction);
        for (int i = 0; i < n; ++i) tmp = tmp + instruction[i];
        cout << tmp << endl;
        origin_ins[cnt++] = tmp;
    }
    IN.close();
}