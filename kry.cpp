#include <iostream>
#include <string>
#include <bitset>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include <sstream>
#include <regex>

using namespace std;

// expanded_lenght je 0 a wrong_lenght je false pre obycajne sha256,
// ich hodnota sa meni len ak ide o length extension attack
std::string padding(std::string message, int expanded_lenght, bool wrong_lenght){

    //vypocet dlzky spravi v bitovej podobe
    string bit_64 = "";
    long long int message_lenght = message.length()*8 + expanded_lenght*8;
    for (int i = 63; i >= 0; i--) {
        int k = message_lenght >> i;
        if (k & 1)
            bit_64 += "1";
        else
            bit_64 += "0";
    }

    //prepis spravy do bitovej podoby 
    string bin_message = "";
    for (char& _char : message) {
        bin_message += std::bitset<8>(_char).to_string();
    }
    bin_message += '1';

    // vypocet dlzky vyplne
    int bin_message_lenght = 0;
    if (wrong_lenght) bin_message_lenght = bin_message.length() + expanded_lenght*8;  
    else bin_message_lenght = bin_message.length() ; //+ expanded_lenght*8;  
    int block_len = int(message.length() / 512) + 1;

    // vytvorenie vyplne vyplne
    for (int i = 0; ((bin_message_lenght + i) % 512*block_len) != (block_len*512-64); i++) {
        bin_message += "0";
    }
    return bin_message + bit_64;
}

std::string ADD(std::string str1, std::string str2){
    std::string res = "";
    int carry_bit = 0;
    for(int i = 31; i >= 0; i--)
    {
        int single_res = (str1[i] - '0') + (str2[i] - '0') + carry_bit;
        carry_bit = (single_res & 2) >> 1;
        res = to_string(single_res & 1) + res;
    }
    return res;
}
std::string XOR(std::string str1, std::string str2){
    std::string res = "";
    for (std::size_t i = 0; i < 32; ++i) {
        res += (str1[i] == str2[i]) ? '0' : '1';
    }
    return res;
}
std::string AND(std::string str1, std::string str2){
    std::string res = "";
    for (std::size_t i = 0; i < 32; ++i) {
        res += (str1[i] == '1' && str2[i] == '1') ? '1' : '0';
    }

return res;
}
std::string NOT(std::string str1){
    std::string res = "";
    for (std::size_t i = 0; i < 32; ++i) {
        res += (str1[i] == '1') ? '0' : '1';
    }
return res;
}

std::string  Choice(std::string e, std::string f, std::string g){
    return XOR( AND(e,f), (AND (NOT(e), g)));
}

std::string shift_xor(std::string row, int shift1, int shift2, int shift3) {
    std::string row1 = row;
    std::string row2 = row;
    std::string row3 = row.substr(0, row.length()-shift3);

    std::rotate(row1.rbegin(), row1.rbegin() + shift1, row1.rend());
    std::rotate(row2.rbegin(), row2.rbegin() + shift2, row2.rend());
    row3.insert(0, shift3, '0');

    return XOR( XOR(row1, row2), row3);
}

std::string SUM(std::string row, int shift1, int shift2, int shift3) {
    std::string row1 = row;
    std::string row2 = row;
    std::string row3 = row;

    std::rotate(row1.rbegin(), row1.rbegin() + shift1, row1.rend());
    std::rotate(row2.rbegin(), row2.rbegin() + shift2, row2.rend());
    std::rotate(row3.rbegin(), row3.rbegin() + shift3, row3.rend());
    
    return XOR( XOR(row1, row2), row3);
}

std::string sha256(std::string blocks, std::array<std::string, 8> h0_h7, bool is_empty){
    string a,b,c,d,e,f,g,h,a2,b2,c2,d2,e2,f2,g2,h2;

    if ( is_empty ) // zakladne hodnoty pre sha-256
    {
        a2 ="01101010000010011110011001100111";
        b2 ="10111011011001111010111010000101";
        c2 ="00111100011011101111001101110010";
        d2 ="10100101010011111111010100111010";
        e2 ="01010001000011100101001001111111";
        f2 ="10011011000001010110100010001100";
        g2 ="00011111100000111101100110101011";
        h2 ="01011011111000001100110100011001";

        a ="01101010000010011110011001100111";
        b ="10111011011001111010111010000101";
        c ="00111100011011101111001101110010";
        d ="10100101010011111111010100111010";
        e ="01010001000011100101001001111111";
        f ="10011011000001010110100010001100";
        g ="00011111100000111101100110101011";
        h ="01011011111000001100110100011001";
    }
    else // pokial ide o length extension attack
    {
        a2 = h0_h7[0];
        b2 = h0_h7[1];
        c2 = h0_h7[2];
        d2 = h0_h7[3];
        e2 = h0_h7[4];
        f2 = h0_h7[5];
        g2 = h0_h7[6];
        h2 = h0_h7[7];

        a = h0_h7[0];
        b = h0_h7[1];
        c = h0_h7[2];
        d = h0_h7[3];
        e = h0_h7[4];
        f = h0_h7[5];
        g = h0_h7[6];
        h = h0_h7[7];
    }
    string temp1 = "";
    string temp2 = "";

    // inicializacia k konštant
    std::array<std::string, 64> k = {
        "01000010100010100010111110011000",
        "01110001001101110100010010010001",
        "10110101110000001111101111001111",
        "11101001101101011101101110100101",
        "00111001010101101100001001011011",
        "01011001111100010001000111110001",
        "10010010001111111000001010100100",
        "10101011000111000101111011010101",
        "11011000000001111010101010011000",
        "00010010100000110101101100000001",
        "00100100001100011000010110111110",
        "01010101000011000111110111000011",
        "01110010101111100101110101110100",
        "10000000110111101011000111111110",
        "10011011110111000000011010100111",
        "11000001100110111111000101110100",
        "11100100100110110110100111000001",
        "11101111101111100100011110000110",
        "00001111110000011001110111000110",
        "00100100000011001010000111001100",
        "00101101111010010010110001101111",
        "01001010011101001000010010101010",
        "01011100101100001010100111011100",
        "01110110111110011000100011011010",
        "10011000001111100101000101010010",
        "10101000001100011100011001101101",
        "10110000000000110010011111001000",
        "10111111010110010111111111000111",
        "11000110111000000000101111110011",
        "11010101101001111001000101000111",
        "00000110110010100110001101010001",
        "00010100001010010010100101100111",
        "00100111101101110000101010000101",
        "00101110000110110010000100111000",
        "01001101001011000110110111111100",
        "01010011001110000000110100010011",
        "01100101000010100111001101010100",
        "01110110011010100000101010111011",
        "10000001110000101100100100101110",
        "10010010011100100010110010000101",
        "10100010101111111110100010100001",
        "10101000000110100110011001001011",
        "11000010010010111000101101110000",
        "11000111011011000101000110100011",
        "11010001100100101110100000011001",
        "11010110100110010000011000100100",
        "11110100000011100011010110000101",
        "00010000011010101010000001110000",
        "00011001101001001100000100010110",
        "00011110001101110110110000001000",
        "00100111010010000111011101001100",
        "00110100101100001011110010110101",
        "00111001000111000000110010110011",
        "01001110110110001010101001001010",
        "01011011100111001100101001001111",
        "01101000001011100110111111110011",
        "01110100100011111000001011101110",
        "01111000101001010110001101101111",
        "10000100110010000111100000010100",
        "10001100110001110000001000001000",
        "10010000101111101111111111111010",
        "10100100010100000110110011101011",
        "10111110111110011010001111110111",
        "11000110011100010111100011110010"
    };
    string block = ""; 
    int startIndex = 0;

    // určenie o aku velku spravu s paddingom ide (512, 1024, 1536,...)
    int num_of_blocks = (blocks.size() / 512);  

    for (int i = 0; i < num_of_blocks; ++i) 
    {
        // vytiahnutie 512 bit bloku zo spravy
        block = blocks.substr(startIndex, 512);
        startIndex += 512;

        std::array<std::string, 64> chunks;
        // rozdelenie bloku na riadky (16 riadkov)
        for (size_t i = 0; i < block.length(); i += 32) 
        {
            chunks[i/32] = block.substr(i, 32);
        }

        // inicializacia zvysnych casti pola pre vypocet hashu (48 riadkov)
        for (size_t i = 16; i <= 63; ++i) 
        {
            chunks[i] = std::string(32, '0');
        }

        // vypocet nulovych w16-w63 
        for (size_t i = 0; i<=47; i++)
        {
            chunks[i+16] = ADD( ADD(chunks[i], shift_xor(chunks[i+1], 7, 18, 3)), ADD(chunks[i+9], shift_xor(chunks[i+14], 17, 19, 10)));
        }

        // uprava klucovych premennych a-h
        for(size_t i = 0; i<= 63; i++)
        {
            //Temp1 = h + Σ1 + Choice + k7 + w7
            temp1 = ADD( ADD(h, SUM(e, 6, 11, 25)), ADD (ADD(Choice(e, f, g), k[i]), chunks[i]));

            //Temp2 = Σ0 + Majority
            temp2 = ADD( SUM(a, 2, 13, 22), XOR( XOR( AND(a,b), AND(a,c)), AND(b,c)));

            // aktualizacia premennych
            h = g;
            g = f;
            f = e;
            e = ADD(d,temp1);
            d = c;
            c = b;
            b = a;
            a = ADD(temp1, temp2);
        }
        
        // urpava startovnych premennych pokial je sprava dlhsia ako 512 znakov
        a = ADD(a,a2);
        b = ADD(b,b2);
        c = ADD(c,c2);
        d = ADD(d,d2);
        e = ADD(e,e2);
        f = ADD(f,f2);
        g = ADD(g,g2);
        h = ADD(h,h2);
        a2 = a;
        b2 = b;
        c2 = c;
        d2 = d;
        e2 = e;
        f2 = f;
        g2 = g;
        h2 = h;
    }
    std::array<std::string, 8> a_h = {a,b,c,d,e,f,g,h};

    // vypocet hashu
    std::string hash = "";
    for (size_t i = 0; i < 8; i += 1)
        for (size_t j = 0; j < 32; j += 4)
        {
            unsigned long long num = std::bitset<4>(a_h[i].substr(j, 4)).to_ullong();
            char hex_digit = (num < 10) ? ('0' + num) : ('a' + num - 10);
            hash += hex_digit;
        }
    return hash;
}



int main(int argc, char* argv[]) {
if (argc == 1) 
    {
        std::cout << "-c – Vypočíta a vypíše SHA-256 checksum vstupnej správy na STDOUT (zakončenej práve jedným \\n).\n";
        std::cout << "-s – Vypočíta MAC, použitím implementovanej SHA-256, pre vstupnú správu a výpis výsledku na STDOUT (zakončeného práve jedným \\n). Musí byť použité v kombinacií s -k parametrom.\n";
        std::cout << "-v – Oveří MAC pre daný kľúč a vstupnú správu. Vracia 0 v případě validného MAC, inak 1. Musí být použité v kombinácií s -k a -m parametrami.\n";
        std::cout << "-e – Vykoná length extension útok na MAC a vstupnú správu. Přepočítaný MAC a predĺžená správa sú vypiísané na STDOUT v tomto poradí (každá položka je zakončená práve jedným \\n). Padding pridaný k vstupnej správe bude vo formáte postupnosti escapových znakov \\xXX, kde XX je ASCII hodnota znaku v hexadecimálnej sústave. Musí byť použité v kombinácií s -m, -n a -a parametrami.\n";
        std::cout << "-k KEY – Špecifikuje tajný kľúč pro výpočet MAC. Formát kľúča sa očekávate podla regulárního výrazu ^[A-Fa-f0-9]*$.\n";
        std::cout << "-m CHS – Špecifikuje MAC vstupnej správy pre jeho verifikáciu alebo vykonanie útoku.\n";
        std::cout << "-n NUM – Špecifikuje dĺžku tajného kľúča (nutnú k vykonaniu útoku).\n";
        std::cout << "-a MSG – Špecifikuje predĺženie vstupnej správy pre vykonanie útoku. Formát rozšířenia správy sa očekáva podla regulárneho výrazu^[a-zA-Z0-9!#$%&’\"()*+,\\-.\\/:;<>=?@[\\]\\^_{}|~]*$.\n";
        return 1;
    }

    int ch;
    std::string message;
    while ((ch = getc(stdin)) != EOF && ch != '\n') 
    {
        message += static_cast<char>(ch);
    }

if  ((argc == 2) && (argv[1][1] == 'c')) {
    string blocks = padding(message, 0, false);
    std::cout << sha256(blocks, {}, true) << std::endl;

} else if ((argc == 4) && (argv[1][1] == 's') && (argv[2][1] == 'k')) {
    string key = argv[3];
    std::regex key_pattern("^[A-Za-z0-9]*$");
    if (!std::regex_match(key, key_pattern)) 
    {
        std::cerr << "Error: chybná hodnota klúča" << std::endl;
        return 1;
    } 

    string blocks = padding(key + message, 0, false);
    std::cout << sha256(blocks, {}, true) << std::endl;

} else if ((argc == 6) && (argv[1][1] == 'v')) {
    string key = "";
    string input_hash = "";
    if ((argv[2][1] == 'k') && (argv[4][1] == 'm'))
    {
        key = argv[3];
        input_hash = argv[5];
    }
    else if ((argv[2][1] == 'm') && (argv[4][1] == 'k'))
    {
        key = argv[5];
        input_hash = argv[3];
    }
    else 
    {
    std::cerr << "Error: neznami/chybajuci argument/hodnota" << std::endl;
    return 1;
    }

    std::regex key_pattern("^[A-Za-z0-9]*$");
    if (!std::regex_match(key, key_pattern)) 
    {
        std::cerr << "Error: chybná hodnota klúča" << std::endl;
        return 1;
    } 


    string blocks = padding(key + message, 0, false);
    string hash = sha256(blocks, {}, true); 

    if (hash == input_hash) return 0;
    else return 1;


} else if ((argc == 8) && (argv[1][1] == 'e')) {
    int key_len = 0; 
    string extent = ""; 
    string complete_hash = "";
    //spracovanie argumentov
    for(int i = 2; i < argc-1; i+=2)
    {
        if (argv[i][1] == 'n')
        {
            key_len = stoi(argv[i+1]);
        }
        else if (argv[i][1] == 'a')
        {   
            extent = argv[i+1];
        }
        else if (argv[i][1] == 'm')
        {
            complete_hash = argv[i+1];
        }
        else 
        {
        std::cerr << "Error: neznami/chybajuci argument/hodnota" << std::endl;
        return 1;
        }
    }

    std::regex extent_pattern("^[a-zA-Z0-9!#$%&’\"()*+,-.\\/:;<>=?@\\[\\]^_{}|~]*$");
    if (!std::regex_match(extent, extent_pattern)) 
    {
        std::cerr << "Error: chybná hodnota rozširujúcej správy" << std::endl;
        return 1;
    } 


    string blocks = padding(message,key_len, true);

    // vypocet sprava\x80\x00...
    string extent_message = "";
    for (size_t i = 0; i < blocks.length(); i += 8)
        {
            if(i < message.length()*8)  // text spravy
            {
                string sub = blocks.substr(i, 8); 
                bitset<8> bits(sub);
                extent_message += static_cast<char>(bits.to_ulong());
            }
            else    // pridana vypln spravy
            {
                extent_message += "\\x";
                unsigned long long num1 = std::bitset<4>(blocks.substr(i, 4)).to_ullong();
                extent_message += (num1 < 10) ? ('0' + num1) : ('a' + num1 - 10);

                unsigned long long num2 = std::bitset<4>(blocks.substr(i+4, 4)).to_ullong();
                extent_message += (num2 < 10) ? ('0' + num2) : ('a' + num2 - 10);
            }
            key_len++;
        }
    extent_message += extent;

    // vypocet startovnych hodnot h0_h7 zo ziskanej spravy
    int startIndex = 0;
    std::array<std::string, 8> h0_h7 = {};
    for (int i = 0; i < 8; ++i) 
    {   
        unsigned long int dec_value;
        std::stringstream stream(complete_hash.substr(startIndex, 8));
        stream >> std::hex >> dec_value;
        startIndex += 8;

        std::bitset<32> bits(dec_value);
        h0_h7[i] = bits.to_string();
    }

    string extent_blocks = padding(extent, key_len, false);
    string new_hash = sha256(extent_blocks, h0_h7, false);

    std::cout << new_hash + extent_message << std::endl;

} else {
    std::cerr << "Error: neznami/chybajuci argument/hodnota" << std::endl;
    return 1;
}
    return 0;
}