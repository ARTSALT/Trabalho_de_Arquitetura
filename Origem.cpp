#include <iostream>;

#include <fstream>;

#include <string>;

#include <sstream>;

#include <bitset>;



using namespace std;

struct Label
{
    string word;
    int linha = 0;
};

struct instrucao
{
    string word[4];
    string codigo[5];
    string funct;
};


string getReg(string a);
string FuncR(string a);
string FuncI(string a);
string FuncJ(string a);
string FuncHex(string a);
string getHex(string a);


int main()
{
    system("chcp 1252");

    Label labels[50];
    string texto;

    ifstream fin;
    fin.open("teste.asm", ios_base::in);
    ifstream ler;
    ler.open("teste.asm", ios_base::in);
    ofstream binario("binario.txt");
    ofstream hexadecimal("hexadecimal.hex");

    hexadecimal << "v2.0 raw" << endl;


    int index = 0, k = 0;
    while (fin.good())
    {
        getline(fin, texto);

        size_t found = texto.find(':');
        if (found != string::npos)
        {
            labels[k].word.insert(0, texto, 0, found);
            labels[k].linha = index;
            k++;
        }
        index++;
    }
    fin.close();



    int linha = 0;
    while (ler.good())
    {
        instrucao inst;
        int i = 0;
        bool num = false;
        getline(ler, texto);
        linha++;

        size_t found = texto.find(':');
        if (found != string::npos)
        {
            texto.erase(0, found + 2);
        }


        do
        {
            found = texto.find(',');
            if (found != string::npos)
            {
                texto.erase(found, 1);
            }
        } while (found != string::npos);


        do
        {
            found = texto.find('(' || ')');
            if (found != string::npos)
            {
                texto.insert(found, 1, ' ');
                texto.erase(found + 1, 1);
                
            }
        } while (found != string::npos);

        
        stringstream tin;
        tin.str(texto);
        
        tin >> inst.word[i];
        if (inst.word[i] != "")
        {
            //tipo J
            if (inst.word[i] == "sll" || inst.word[i] == "srl" || inst.word[i] == "jr" || inst.word[i] == "mfhi" || inst.word[i] == "mflo"
                || inst.word[i] == "mult" || inst.word[i] == "multu" || inst.word[i] == "div" || inst.word[i] == "divu"
                || inst.word[i] == "add" || inst.word[i] == "addu" || inst.word[i] == "sub" || inst.word[i] == "subu"
                || inst.word[i] == "and" || inst.word[i] == "or" || inst.word[i] == "slt" || inst.word[i] == "sltu"
                || inst.word[i] == "mul")
            {
                //pega o opcode, posição 0
                inst.codigo[i] = FuncR(inst.word[i]);
                i++;


                while (tin.good())
                {
                    tin >> inst.word[i];
                    inst.codigo[i] = getReg(inst.word[i]);
                    i++;
                }

                //printagem
                if (inst.word[0] == "sll" || inst.word[i] == "srl")
                {
                    int t0 = stoi(inst.word[3]);
                    bitset<5> t1 = t0;
                    //opcode + rs +       tr       +      rd        +          shamt            +     funct     ;
                    texto = "00000000000" + inst.codigo[2] + inst.codigo[1] + t1.to_string() + inst.codigo[0];
                }
                else if (inst.word[0] == "jr")
                {
                    texto = "000000" + inst.codigo[1] + "000000000000000" + inst.codigo[0];
                }
                else if (inst.word[0] == "mfhi" || inst.word[i] == "mflo")
                {
                    texto = "0000000000000000" + inst.codigo[1] + "00000" + inst.codigo[0];
                }
                else if (inst.word[0] == "mult" || inst.word[i] == "multu" || inst.word[i] == "div" || inst.word[i] == "divu")
                {
                    texto = "000000" + inst.codigo[1] + inst.codigo[2] + "00000" + "00000" + inst.codigo[0];
                }
                else if (inst.word[0] == "add" || inst.word[i] == "addu" || inst.word[i] == "sub" || inst.word[i] == "subu"
                    || inst.word[i] == "and" || inst.word[i] == "or" || inst.word[i] == "slt" || inst.word[i] == "sltu")
                {
                    texto = "000000" + inst.codigo[2] + inst.codigo[3] + inst.codigo[1] + "00000" + inst.codigo[0];
                }
                else if (inst.word[0] == "mul")
                {
                    texto = "0011100" + inst.codigo[2] + inst.codigo[3] + inst.codigo[1] + "00000" + inst.codigo[0];
                }
            }
            //tipo I
            else if (inst.word[i] == "beq" || inst.word[i] == "bne" || inst.word[i] == "addi" || inst.word[i] == "addiu" || inst.word[i] == "slti"
                || inst.word[i] == "sltiu" || inst.word[i] == "andi" || inst.word[i] == "ori" || inst.word[i] == "lui"
                || inst.word[i] == "lw" || inst.word[i] == "sw")
            {
                //pega o opcode, posição 0
                inst.codigo[i] = FuncI(inst.word[i]);
                i++;



                while (tin.good())
                {
                    tin >> inst.word[i];
                    inst.codigo[i] = getReg(inst.word[i]);
                    if (inst.codigo[i] == "a" && (48 <= inst.word[i][0] && inst.word[i][0] >= 57))
                    {
                        for (int j = 0; j < index; j++)
                        {
                            if (inst.word[i] == labels[j].word)
                            {
                                inst.codigo[i] = to_string(labels[index].linha - linha);
                            }
                        }
                    }
                    i++;
                }


                if (inst.word[0] == "beq" || inst.word[i] == "bne")
                {
                    int t0 = stoi(inst.codigo[3]);
                    bitset<16> t1 = bitset<16>(t0);

                    texto = inst.codigo[0] + inst.codigo[1] + inst.codigo[2] + t1.to_string();
                }
                else if (inst.word[0] == "addi" || inst.word[i] == "addiu" || inst.word[i] == "slti" || inst.word[i] == "sltiu"
                    || inst.word[i] == "andi" || inst.word[i] == "ori")
                {
                    int t0 = stoi(inst.word[3]);
                    bitset<16> t1 = bitset<16>(t0);

                    texto = inst.codigo[0] + inst.codigo[2] + inst.codigo[1] + t1.to_string();
                }
                else if (inst.word[0] == "lui")
                {
                    int t0 = stoi(inst.word[2]);
                    std::bitset<16> t1 = bitset<16>(t0);
                    t1 = t1 << 4;

                    texto = inst.codigo[0] + "00000" + inst.codigo[1] + t1.to_string();
                }
                else if (inst.word[0] == "lw" || inst.word[i] == "sw")
                {
                    int t0 = stoi(inst.word[2]);
                    bitset<16> t1 = bitset<16>(t0);

                    texto = inst.codigo[0] + inst.codigo[3] + inst.codigo[1] + t1.to_string();
                }
            }
            //tipo R
            else if (inst.word[i] == "j" || inst.word[i] == "jal")
            {
                inst.codigo[i] = FuncJ(inst.word[i]);
                i++;

                while (tin.good())
                {
                    tin >> inst.word[i];
                    inst.codigo[i] = getReg(inst.word[i]);
                }

                if (inst.codigo[1] == "a")
                {
                    for (int j = 0; j < index; j++)
                    {
                        if (inst.word[1] == labels[j].word)
                        {
                            inst.codigo[1] = to_string(4194304/4 + labels[j].linha);
                        }
                    }
                }

                long t0 = stoi(inst.codigo[1]);
                bitset<26> t1 = bitset<26>(t0);

                texto = inst.codigo[0] + t1.to_string();
            }

            binario << texto << endl;
            hexadecimal << FuncHex(texto) << " ";
            if (linha%4 == 0)
            {
                cout << endl;
            }
        }
    }
}

string getReg(string a) {
    string b;

    if (a == "$zero" || a == "$0") return b = "00000";

    else if (a == "$at" || a == "$1") return b = "00001";

    else if (a == "$v0" || a == "$2") return  b = "00010";

    else if (a == "$v1" || a == "$3") return b = "00011";

    else if (a == "$a0" || a == "$4") return b = "00100";

    else if (a == "$a1" || a == "$5") return b = "00101";

    else if (a == "$a2" || a == "$6") return b = "00110";

    else if (a == "$a3" || a == "$7") return b = "00111";

    else if (a == "$t0" || a == "$8") return b = "01000";

    else if (a == "$t1" || a == "$9") return b = "01001";

    else if (a == "$t2" || a == "$10") return b = "01010";

    else if (a == "$t3" || a == "$11") return b = "01011";

    else if (a == "$t4" || a == "$12") return b = "01100";

    else if (a == "$t5" || a == "$13") return b = "01101";

    else if (a == "$t6" || a == "$14") return b = "01110";

    else if (a == "$t7" || a == "$15") return b = "01111";

    else if (a == "$s0" || a == "$16") return b = "10000";

    else if (a == "$s1" || a == "$17") return b = "10001";

    else if (a == "$s2" || a == "$18") return b = "10010";

    else if (a == "$s3" || a == "$19") return b = "10011";

    else if (a == "$s4" || a == "$20") return b = "10100";

    else if (a == "$s5" || a == "$21") return b = "10101";

    else if (a == "$s6" || a == "$22") return b = "10110";

    else if (a == "$s7" || a == "$23") return b = "10111";

    else if (a == "$t8" || a == "$24") return b = "11000";

    else if (a == "$t9" || a == "$25") return b = "11001";

    else if (a == "$k0" || a == "$26") return b = "11010";

    else if (a == "$k1" || a == "$27") return b = "11011";

    else if (a == "$gp" || a == "$28") return b = "11100";

    else if (a == "$sp" || a == "$29") return b = "11101";

    else if (a == "$fp" || a == "$30") return b = "11110";

    else if (a == "$ra" || a == "$31") return b = "11111";

    else { return b = "a"; }
}

string FuncR(string a)

{
    string b;

    if (a == "sll")
    {
        return b = "000000";
    }
    else if (a == "srl")
    {
        return b = "000010";
    }
    else if (a == "jr")
    {
        return b = "001000";
    }
    else if (a == "mfhi")
    {
        return b = "010000";
    }
    else if (a == "mhlo")
    {
        return b = "010010";
    }
    else if (a == "mult")
    {
        return b = "011000";
    }
    else if (a == "multu")
    {
        return b = "011001";
    }
    else if (a == "div")
    {
        return b = "011010";
    }
    else if (a == "divu")
    {
        return b = "011011";
    }
    else if (a == "add")
    {
        return b = "100000";
    }
    else if (a == "addu")
    {
        return b = "100001";
    }
    else if (a == "sub")
    {
        return b = "100010";
    }
    else if (a == "subu")
    {
        return b = "100011";
    }
    else if (a == "and")
    {
        return b = "100110";
    }
    else if (a == "or")
    {
        return b = "100111";
    }
    else if (a == "slt")
    {
        return b = "101010";
    }
    else if (a == "sltu")
    {
        return b = "101011";
    }
    else if (a == "mul")
    {
        return b = "000010";
    }
    else
    {
        return b;
    }
}

string FuncI(string a)
{
    string b;
    if (a == "beq") return b = "000100";

    else if (a == "bne") return b = "000101";

    else if (a == "addi") return b = "001000";

    else if (a == "addiu") return b = "001001";

    else if (a == "slti") return b = "001010";

    else if (a == "stliu") return b = "001011";

    else if (a == "andi") return b = "001100";

    else if (a == "ori") return b = "001101";

    else if (a == "lui") return b = "001111";

    else if (a == "lw") return b = "100011";

    else if (a == "sw") return b = "101011";

    else return b;
}

string FuncJ(string a) 
{
    string b;
    if (a == "j") return b = "000010";
    else if (a == "jal") return b = "000011";
    else return b;
}

string FuncHex(string a) 
{
    string hexa = "", addhexa = "";
    int b = 4;
    for (int i = 0; i < 7; i++)
    {
        a.insert(b, " ");
        b += 5;
    }

    stringstream to_hex;
    to_hex.str(a);

    while (to_hex.good())
    {
        to_hex >> addhexa;
        hexa = hexa + getHex(addhexa);
    }

    return hexa;
}

string getHex(string a) {
    string b;
    if (a == "0000") return b = "0";
    else if (a == "0001") return b = "1";
    else if (a == "0010") return b = "2";
    else if (a == "0011") return b = "3";
    else if (a == "0100") return b = "4";
    else if (a == "0101") return b = "5";
    else if (a == "0110") return b = "6";
    else if (a == "0111") return b = "7";
    else if (a == "1000") return b = "8";
    else if (a == "1001") return b = "9";
    else if (a == "1010") return b = "a";
    else if (a == "1011") return b = "b";
    else if (a == "1100") return b = "c";
    else if (a == "1101") return b = "d";
    else if (a == "1110") return b = "e";
    else if (a == "1111") return b = "f";
    else return b;
}