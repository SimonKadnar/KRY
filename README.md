KRY - KRYPTOGRAFIE (A.R. 2023/2024)

utor: Šimon Kadnár  
Login: xkadna00  
Dátum: 29.4.2024  
Názov: MAC za použitia SHA-256 + Length extension attack

**Riešenie:**
- Program pozostáva zo 4 základných častí.

1. padding() - riesši výplň vstupnej správy
2. pomocné funkcie : ADD, XOR, AND, NOT, Choice, SUM.
3. sha_256() - výpočet sha256
4. main() - spracovanie vstupu a následne volanie príslušných funkcií

**Spúštanie:** 
- Preklad pomocov `make`

1. výpis významu argumentov `./kry` 
2. SHA-256 `echo -ne "správa" | ./kry -c`
3. Generovanie MAC pre vstupnú správu a kľúč `echo -ne "správa" | ./kry -s -k "heslo" ` 
4. MAC ktorý overí vstupnú správu a tajný kľúč `echo -ne "správa" | ./kry -v -k "heslo" -m "hash"` a následne `echo $?`      
    - Ak sa hashe rovnajú návratová hodnota je `0` 
    - Ak sa hashe nerovnajú návratová hodnota je `1` 
5. Length extension attack `echo -ne "správa" | ./kry -e -n 5 -a "pridaný text" -m "hash"` 