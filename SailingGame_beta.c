#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ENTER 13
#define WIDTH 100
#define HEIGHT 33
#define MAX_PLAYERS 5
#define MAX_YSTEPS 4     // Σταθερές που περιορίζουν 
#define MAX_XSTEPS 10    // την κίνηση των παικτών

HANDLE hConsole;

struct player      // Structure που έχει τις πληροφορίες των παικτών
{
    char name[20];
    char symbol;
    int X;
    int Y;
    char input[100];

    bool skipRound;
    bool checkpoint1;
    bool checkpoint2;
    bool finish;
    bool hasCrushed;
};

void move(struct player * pplayer);      // Συνάρτηση που κινεί τους παίκτες
void gotoXY(short column, short line);    // Συνάρτηση που κινεί τον κέρσορα (έτοιμη από τον κώδικα παραδειγμάτων)
void getBackgroundColor(int x, int y, bool red);   // Συνάρτηση που ρυθμίζει το χρώμα την κονσόλας
void drawMap();   // Συνάρτηση που τυπώνει τον χάρτη
void deleteMessage(int line);   // Σηνάρτηση που διαγράφει ό,τι είναι τυπωμένο σε μια σειρά
int windGeneration(int *slope, int *scale);   // Συνάρτηση που δημιουργεί (με κάποια τυχαιότητα) τις παραμέτρους του ανέμου
int windEffect(int slope, int scale, char input[100]);   // Συνάρτηση που δημιουργεί μια επιπλέον παράμετρο για τον άνεμο, η οποία εξαρτάται από το πλήθον των βημάτων που κάνει ο παίκτης
void windPush(int slope, int scale, int effectFactor, struct player * pplayer);   // Συνάρτηση που κινεί τους παίκτες σύμφωνα με τον άνεμο
void getOutOfLand(struct player * pplayer, int slope);   // Συνάρτηση που επαναφέρει τους παίκτες στο νερό όταν ο άνεμος τους πετάει έξω από αυτό
void HideConsoleCursor();   // Συνάρτηση που σβήνει τον κέρσορα (έτοιμη από τον κώδικα παραδειγμάτων)
void moveConstraint(char key, bool *limitReached, int *countX, int *countY, bool* XlimitReached, bool* YlimitReached);   // Συνάρτηση που περιορίζει την κίνηση των παικτών
void windPoint(int slope);   // Συνάρτηση που τυπώνει μια πθξίδα στην οποία φαίνεται η κατεύθυνση και η ένταση του ανέμου σε κάθε γύρο

char map[33][101] = {
    "::::::::::::oooooooooooooooooXXXXXXXXXXXXXXXXXXXXooooXXXXooooooooooooooooooo::::::::::::::::::::::::",
    ":::::::::::ooooooooooooooXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXooooooooooooooooooo::::::::::::::::",
    ":::::::::ooooooooooooooXXXXXX;;;;;;;;;;;;;;;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXoooooooooooooooooooo:::::::",
    ":::::::ooooooooooooooXXXXXX;;               ;;;;XXXXXXXXXXXXXXXXXXXXXXXXXXXoooooooooooooooooooooo:::",
    ":::::ooooooooXXXXXXXXXXXXX;                     ;;;;;;;;;;;;;;;;;;;;;;;XXXXXXXXoooooooooooooooooooo:",
    "::oooooooXXXXXXXXXXXXXXXX;;                                            ;;;XXXXXXXXXooooooooooooooooo",
    "::ooooooXXXXXXXXXXX;;;;;;                                                 ;;;XXXXXXXoooooooooooooooo",
    ":oooooXXXXXXXXX;;;;    1                                                     ;;;;;XXXXXXXXXooooooooo",
    "oooooXXXXXXX;;;        1          ;;;;;;;;;;;;                                    ;;;;;XXXXXoooooooo",
    "oooooXXXXXX;           1   ;;;;;;;XXXXXXXXXXXX;;;                                      ;XXXXXXoooooo",
    "ooooXXXXXXX;           ;;;;XXXXXXXXXXXXXXXXXXXXXX;;                                     ;XXXXXXooooo",
    "ooooXXXXXX;            ;XXXXXXXXXoooooXXXXXXXXXXXXX;                                     ;XXXXXooooo",
    "ooooXXXXXX;           ;XXXXXXXXXoooooooooooXXXXXXXXX;;                                   ;XXXXXooooo",
    "oooXXXXXX;             ;XXXXooooooooooooooooooXXXXXXXX;                                  ;XXXXXooooo",
    "ooXXXXXX;              ;;XXXXXooooooooo:::::ooooXXXXXXX;;;;                              ;XXXXXXoooo",
    "oXXXXXXX;                ;XXXXXXXXooooo::::::oooooXXXXXXXXX;;;                           ;XXXXXXoooo",
    "XXXXXXXX;                 ;XXXXXXXXooo:::::::::oooooXXXXXXXXXX;;;                       ;XXXXXXooooo",
    "oXXXXXX;                  ;XXXXXXXXooo:::::::oooooooXXXXXXXXXXXX;fffffffffffffffffffffff;XXXXXXooooo",
    "XXXXXX;                    ;XXXXXXXooo::::::ooooooXXXXXXXXXXXXX;                       ;XXXXXXoooooo",
    "XXXX;;                      ;XXXXXXooo::::ooooXXXXXXXXXXXXXXX;;                        ;XXXXXooooooo",
    "XXX;                         ;XXXXXooo::ooooXXXXXXXXXXX;;;;;;                          ;XXXXXooooooo",
    "oXXX;                       ;XXXXXXoooooooXXXXXXXXXX;;;                                ;XXXXXooooooo",
    "oXXX;                        ;XXXXXXoooooXXXXXXXXXX;                                    ;XXXXXXooooo",
    "ooXXX;;;                    ;XXXXXXXXXXXXXXXXXXXXX;                                     ;XXXXXXooooo",
    "oooXXXXX;;;;                ;XXXXXXXXXXXXXXXXXXX;;                                      ;XXXXXXooooo",
    "ooooXXXXXXXX;;             ;;;XXXXXXXXXXXXX;;;;;                                        ;XXXXXXooooo",
    "::oooooXXXXXXX;;              ;;;;;;;;;;;;;                                            ;XXXXXXXoooo:",
    "::::ooooooXXXXXX;                    2                                               ;;XXXXXXooooo::",
    ":::::oooooooXXXX;                    2                                      ;;;;;;;;;XXXXXXXooooo:::",
    ":::::oooooooXXXXX;;;;                2                                  ;;;;XXXXXXXXXXXXXXXoooo:::::",
    "::::::oooooooXXXXXXXX;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                ;;;XXXXXXXXXXXXXXooooooo:::::::",
    "::::::::::oooooooXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;;;;;;;;;;;;;;;;XXXXXXXXXXXXXXooooooo::::::::::",
    ":::::::::::::ooooXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXooooooooooo::::::::::::"
};   // Ο χάρτης του παιχνιδιού ως δισδιάστατος πίνακας χαρακτήρων

int main()
{
    system("mode 650");
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned) time(NULL));
    system("cls");

    struct player players[MAX_PLAYERS];   // Λίστες που περιέχουν αντίστοιχα όλους τους παίκτες και τους
    struct player winners[MAX_PLAYERS];   // παίκτες που έχουν τερματίσει (η δεύτερη ανανεώνεται σε κάθε γύρο)
    int player_number;
    int slope, scale, effectFactor;
    char selection = '\0';

    // Εμφανίζεται η αρχική οθόνη, στην οποία υπάρχει η επιλογή να εμφανιστούν οδηγίες για το παιχνίδι

    printf("****************************************************\n");
    printf("*             Welcome to Sailing Game!             *\n");
    printf("*    -Made by Gkekas Aggelos and Kikas Georgios    *\n");
    printf("****************************************************\n");
    printf(">Press Enter to Play\n");
    printf(">Press Space for Instructions\n");
    while(true)
    {   
        selection = getch();
        if(selection == ' ')
        {
            for(int i = 0; i <= 5; i++)
            {
                deleteMessage(i);
            }
            gotoXY(0,0);
            printf("\t\t\t>INSTRUCTIONS<\n\n");
            printf("\tA Sailing Game set in a lake with an island on its center.\n");
            printf("  The goal is to be the first player to make a full lap around the island.\n");
            printf("Each turn, the players one by one enter their moves using the WASD keys\n");
            printf("and the moves are then executed simultanuously for all players. But, the\n");
            printf("wind is an unpredictable factor each turn and it's behaviour is revealed\n");
            printf("after the players have entered their moves. Then the winds affects them,\n");
            printf("and their actual position is revealed.\n");
            printf("  Players could possibly crash into each other, in which case they are\n");
            printf("sent back to the start of the lap. Also, there are two types of waters:\n");
            printf("deep and shallow. Players can move regularly through deep waters(blue\n");
            printf("color), but cannot move to shallow waters(light blue). Only the wind can\n");
            printf("send a player there, in which case they lose a turn. Note that players\n");
            printf("once they get to shallow waters can only move out to deep ones and not\n");
            printf("to adjacent shallow water blocks.\n");
            printf("  Have fun playing!\n");
            printf("  *Players 2 to 5*\n\n");
            printf("When you are ready to play, press ENTER.\n");
            while(true)
            {
                int sel = '\0';
                sel = getch();
                if(sel == ENTER) break;
            }
            for(int i = 0; i <= 19; i++)
            {
                deleteMessage(i);
            }
            break;
        }
        if(selection == ENTER)
        {
            for(int i = 0; i <= 5; i++)
            {
                deleteMessage(i);
            }
            break;
        }
    }

    // Ο χρήστης εισάγει το πλήθος των παικτών και ένα όνομα και ένα σύμβολο για καθέναν από αυτούς
        
    gotoXY(0, 0);
    printf("How many players are there? ");
    scanf("%d", &player_number);

    for (int i = 0; i < player_number; i++)
    {
        system("cls");
        printf("Enter the name of player %d: ", i + 1);
        scanf("%s", players[i].name);
        fflush(stdin);
        printf("Enter the symbol for player %d: ", i + 1);
        players[i].symbol = getchar();
        players[i].X = 70 + 2*i;
        players[i].Y = 18;

        players[i].skipRound = false;
        players[i].checkpoint1 = false;
        players[i].checkpoint2 = false;
        players[i].finish = false;
        players[i].hasCrushed = false;
    }

    system("cls");

    // Τυπώνεται ο χάρτης και οι παίκτες τοποθετούνται στις αρχικές τους θέσεις

    drawMap();
    SetConsoleTextAttribute(hConsole, 16);

    for (int i = 0; i < player_number; i++)
    {
        gotoXY(players[i].X, players[i].Y);
        putch(players[i].symbol);
    }

    while (true)  // Η κύρια λούπα του παιχνιδιού
    {
        HideConsoleCursor();
        SetConsoleTextAttribute(hConsole, 07);

        // Κάθε παίκτης εισάγει την κίνηση που θέλει να κάνει με τα πλήκτρα w, a, s, d

        for (int i = 0; i < player_number; i++)
        {
            gotoXY(0, 34);

            if (players[i].skipRound)  // Εαν στον προηγούμενο γύρο κάποιος έχασε τη σειρά του, δεν εισάγει κίνηση
            {
                printf("%s lost their turn because of shallow waters!\n", players[i].name);
                Sleep(2000);
            }

            else   // Οι υπόλοιποι παίκτες παίζουν κανονικά
            {
                printf("Its %s's move\n", players[i].name);

                int counter = 0;
                char key = ' ';
                int countX = 0;
                int countY = 0;
                bool limitReached = false;
                bool XlimitReached = false;
                bool YlimitReached = false;

             /* Οι παίκτες εισάγουν τις κινήσεις τους, ενώ γίνεται έλεγχος για το αν έχουν φτάσει τον
                μέγιστο αριθμό βημάτων στην οριζόντια και στην κατακόρυφη κατεύθυνση */

                while (key != ENTER)
                {
                    moveConstraint(key, &limitReached, &countX, &countY, &XlimitReached, &YlimitReached);
                    key = getch();
                    players[i].input[counter] = key;
                    if(limitReached) 
                    {
                        key = ENTER;
                        players[i].input[counter] = key;
                    }
                    else if(YlimitReached && ((key == 'w') || (key == 's'))) counter--;
                    else if(XlimitReached && ((key == 'a') || (key == 'd'))) counter--;  
                    counter++;
                }
            }

            deleteMessage(34);
            deleteMessage(35);
            deleteMessage(36);
        }

        // Όσοι παίκτες δεν έχασαν τη σειρά τους μετακινούνται ανάλογα με την κίνησει που εισήγαγαν

        for (int i = 0; i < player_number; i++)
        {
            if (!players[i].skipRound)
            {
                move(&(players[i]));
            }
        }
        
        // Γίνεται έλεγχος για το αν έχει φτάσει κάποιος στον τερματισμό

        int number_of_winners = 0;
        for (int i = 0; i < player_number; i++)
        {
            if (players[i].finish)
            {
                winners[number_of_winners] = players[i];
                number_of_winners++;
                gotoXY(players[i].X, players[i].Y);
                getBackgroundColor(players[i].X, players[i].Y, false);
                putch(players[i].symbol);
            }
        }

     /* Στην περίπτωση που υπάρχουν παίκτες που έχουν τερματίσει, τυπώνεται
        ένα μήνυμα ανάλογα με το πλήθος τους και τελειώνει το παιχνίδι */

        if (number_of_winners == 1)
        {
            gotoXY(0, 34);
            SetConsoleTextAttribute(hConsole, 07);
            printf("%s has won! Congratulations!", winners[0].name);
            Sleep(5000);
            exit(0);
        }

        if (number_of_winners > 1)
        {
            gotoXY(0, 34);
            SetConsoleTextAttribute(hConsole, 07);
            printf("There has been a tie between: ", winners[0].name);
            for (int i = 0; i < number_of_winners - 2; i++)
            {
                printf("%s, ", winners[i].name);
            }
            printf("%s ", winners[number_of_winners - 2].name);
            printf("and %s", winners[number_of_winners - 1].name);
            Sleep(5000);
            exit(0);
        }

        // Αν κανείς δεν έχει τερματίσει, οι παίκτες δέχονατι την επίδραση του ανέμου και τυπώνονται αντίστοιχα μηνύματα

        SetConsoleTextAttribute(hConsole, 07);
        gotoXY(0, 34);
        printf("The players wanted to go there. But Wild is the Wind...");
        Sleep(2000);
        deleteMessage(34);
        windGeneration(&slope, &scale);
        if(scale != 0)
        {   gotoXY(0, 34);
            printf("This round the wind had: Slope of %d and Scale of %d", slope, scale);
            windPoint(slope);
        }
        else 
        {
            gotoXY(0, 34);
            puts("The wind was calm this round....");
        }
        Sleep(3000);
        for(int i = 0; i <= 4; i++)
        {
            deleteMessage(35 + i);
        }
        
        for(int j = 0; j < player_number; j++)
        {
         /* Οι παίκτες που έχουν χάσει τη σειρά τους δε δέχονται την επιρροή του ανέμου ενώ
            όσοι στέλνονται στα ρηχά νερά από τον άνεμο χάνουν τη σειρά τους στον επόμενο γύρο */

            if (players[j].skipRound)
            {
                players[j].skipRound = false;
            }
            else
            {
                effectFactor = windEffect(slope, scale, players[j].input);
                windPush(slope, scale, effectFactor, &(players[j]));
                if (map[players[j].Y][players[j].X] == ';') players[j].skipRound = true;
            } 
        }

     /* Γίνεται έλεγχος αν κάποιοι παίκτες βρέθηκαν στην ίδια θέση, και στην περίπτωση που 
        αυτό συμβαίνει, αυτοί οι παίκτες "τράκαραν" και πρέπει να ξαναρχίσουν από την εκκίνηση */

        for(int i = 0; i < player_number; i++)       // Ελέγχεται κάθε
        {                                            // πιθανό
            for(int j = 0; j < player_number; j++)   // ζεύγος παικτών
            {
                if((i == j) || ((players[i].hasCrushed == true) && (players[j].hasCrushed == true))) continue;
                else
                {
                    if((players[i].Y == players[j].Y) && (players[i].X == players[j].X))
                    {
                        players[i].hasCrushed = true;
                        players[j].hasCrushed = true;
                        deleteMessage(34);
                        gotoXY(0, 34);
                        printf("Oh no! %s and %s have crashed into each other!\n", players[i].name, players[j].name);
                        getBackgroundColor(players[i].X, players[i].Y, false);
                        for(int k = 0; k < 2; k++)
                        {
                            gotoXY(players[i].X, players[i].Y);
                            putch('X');
                            Sleep(1000);
                            gotoXY(players[i].X, players[i].Y);
                            putch(' ');
                            Sleep(1000);
                        }
                    } 
                }
            }
        }

        // Οι παίκτες που "τράκαραν" επαναφέρωνται στην εκκίνηση

        deleteMessage(34);
        for(int i = 0; i < player_number; i++)
        {
            if(players[i].hasCrushed)
            {
                players[i].X = 70 + 2*i;
                players[i].Y = 18;
                getBackgroundColor(players[i].X, players[i].Y, false);
                gotoXY(players[i].X, players[i].Y);
                putch(players[i].symbol);
                players[i].hasCrushed = false;
                players[i].checkpoint1 = false;
                players[i].checkpoint2 = false;
                players[i].finish = false;
                players[i].skipRound = false;
            }
        }
    }

    return 0;
}

void HideConsoleCursor()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void getBackgroundColor(int x, int y, bool red)  // Η συνάρτηση δέχεται ως παραμέτρους μια θέση στον χάρτη
{                                                // και μια μεταβλητή bool και ορίζει το χρώμα της κονσόλας:
    int extra;                                   // το background παίρνει το χρώμα της θέσης του χάρτη
    if (red) extra = 4;                          // το foreground γίνεται μαύρο αν η bool είναι ψευδής και κόκκινο αν η bool είναι αληθής
    else extra = 0;
    switch (map[y][x])
    {
    case ' ':
        SetConsoleTextAttribute(hConsole, 16 + extra);
        break;
    case '1':
        SetConsoleTextAttribute(hConsole, 16 + extra);
        break;
    case '2':
        SetConsoleTextAttribute(hConsole, 16 + extra);
        break;
    case 'f':
        SetConsoleTextAttribute(hConsole, 16 + extra);
        break;
    case 'o':
        SetConsoleTextAttribute(hConsole, 160 + extra);
        break;
    case 'X':
        SetConsoleTextAttribute(hConsole, 96 + extra);
        break;
    case ':':
        SetConsoleTextAttribute(hConsole, 32 + extra);
        break;
    case ';':
        SetConsoleTextAttribute(hConsole, 48 + extra);
        break;
    
    default:
        SetConsoleTextAttribute(hConsole, 07);
        break;
    }
}

void drawMap()   // Τυπώνεται ο χάρτης χαρακτήρα προς χαρακτήρα και τα χρώματα
{                // αλλάζουν ανάλογα με τον χαρακτήρα που βρίσκεται σε κάθε θέση
    for(int i = 0; i < 33; i++)
    {
        for(int j = 0; j < 100; j++)
        {
            getBackgroundColor(j, i, false);
            putch(' ');
        }
        SetConsoleTextAttribute(hConsole, 07);
        putch('\n');
    }
}

void deleteMessage(int line)  // Διαγράφεται μία σειρά από την κονσόλα
{
    SetConsoleTextAttribute(hConsole, 07);
    for (int i = 0; i < 100; i++)
    {
        gotoXY(i, line);
        putch(' ');
    }
}

void gotoXY(short column, short line)
{
    COORD coord;
    coord.X = column;
    coord.Y = line;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void move(struct player * pplayer)   // Η συνάρτηση δέχεται ως παράμετρο τη θέση μνήμης ενός παίκτη και τον
{                                    // μετακινεί σύμφωνα με την κίνηση που εισήγαγε στον τελευταίο γύρο
    char * pinput = (*pplayer).input;

    // Ο χαρακτήρας του παίκτη διαγράφεται από την αρχική του θέση

    gotoXY((*pplayer).X, (*pplayer).Y);
    getBackgroundColor((*pplayer).X, (*pplayer).Y, false);
    putch(' ');

    int counter = 0;

    // Οι συντεταγμένες του παίκτη αλλάζουν σύμφωνα με την κίνηση που εισήγαγε
    // χωρίς να του επιτρέπεται να πάει στα ρηχά ή στη στεριά

    while ((*pplayer).input[counter] != ENTER)
    {
        
        switch ((*pplayer).input[counter]) 
        {
        case 'w':
            ((*pplayer).Y)--;
            if ((map[(*pplayer).Y][(*pplayer).X] == ';') || (map[(*pplayer).Y][(*pplayer).X] == 'X'))
            {
                ((*pplayer).Y)++;
            }
            break;
        case 's':
            ((*pplayer).Y)++;
            if ((map[(*pplayer).Y][(*pplayer).X] == ';') || (map[(*pplayer).Y][(*pplayer).X] == 'X'))
            {
                ((*pplayer).Y)--;
            }
            break;
        case 'a':
            ((*pplayer).X)--;
            if ((map[(*pplayer).Y][(*pplayer).X] == ';') || (map[(*pplayer).Y][(*pplayer).X] == 'X'))
            {
                ((*pplayer).X)++;
            }
            break;
        case 'd':
            ((*pplayer).X)++;
            if ((map[(*pplayer).Y][(*pplayer).X] == ';') || (map[(*pplayer).Y][(*pplayer).X] == 'X'))
            {
                ((*pplayer).X)--;
            }
            break;
        default:
            break;
        }

        // Αν ο παίκτης πέρασε από κάποιο checpoint ή από τον
        // τερματισμό αυτό αποθηκεύεται στις πληροφορίες του 

        if ((map[(*pplayer).Y][(*pplayer).X] == '1'))
        {
            (*pplayer).checkpoint1 = true;
        }
        if ((map[(*pplayer).Y][(*pplayer).X] == '2') && ((*pplayer).checkpoint1))
        {
            (*pplayer).checkpoint2 = true;
        }
        if ((map[(*pplayer).Y][(*pplayer).X] == 'f') && ((*pplayer).checkpoint1) && ((*pplayer).checkpoint2))
        {
            (*pplayer).finish = true;
        }

        counter++;
    }
    (*pplayer).input[counter] = '\0';

    // Ο παίκτης εμφανίζεται στη νέα του θέση

    gotoXY((*pplayer).X, (*pplayer).Y);
    getBackgroundColor((*pplayer).X, (*pplayer).Y, true);
    putch((*pplayer).symbol);
}

int windGeneration(int *slope, int *scale)   // Δημιουργούνται, με κάποια τυχαιότητα και με βάση
{                                            // τις παραμέτρους του προηγούμενου γύρου, οι παράμετροι
    int A[3] = {-2, -1, 0};                  // που καθορίζουν την κατεύθυνση και την ένταση του ανέμου
    static int prevSlope = 1;
    static int prevScale = 0;

    int variability1 = rand()%3;
    *slope = 1 + (abs(prevSlope + A[variability1]))%8;
    prevSlope = *slope;

    int variability2 = rand()%3;
    *scale = (abs(prevScale + A[variability2]))%3;
    prevScale = *scale;
}

int windEffect(int slope, int scale, char input[100])   // Δημιουργείται μια επιπλέον παράμετρος, που κάνει τον άνεμο να επηρεάζει
{                                                       // περισσότερο τους παίκτες ανάλογα με το πλήθος των βημάτων που κάνουν
    int steps = strlen(input);
    int effectFactor = 1 + steps/4;

    return effectFactor;
}

void windPush(int slope, int scale, int effectFactor, struct player * pplayer)
{                                                                // Η συνάρτηση δέχεται ως παραμέτρους τις παραμέτρους
    gotoXY((*pplayer).X, (*pplayer).Y);                          // του ανέμου και τη θέση μνήμης ενός παίκτη και
    getBackgroundColor((*pplayer).X, (*pplayer).Y, false);       // μετακινεί τον παίκτη σύμφωνα με τον άνεμο
    putch(' ');

    // Ο παίκτης διαγράφεται από την αρχική του θέση και οι συντεταγμενες του αλλάζουν σύμφωνα με τον άνεμο

    switch(slope)
    {
        case 1:
            ((*pplayer).Y) = ((*pplayer).Y) - effectFactor*scale;
            break;
        case 2:
            ((*pplayer).Y) = ((*pplayer).Y) - effectFactor*scale; 
            ((*pplayer).X) = ((*pplayer).X) + effectFactor*scale;
            break;
        case 3: 
            ((*pplayer).X) = ((*pplayer).X) + effectFactor*scale;
            break;    
        case 4:
            ((*pplayer).Y) = ((*pplayer).Y) + effectFactor*scale; 
            ((*pplayer).X) = ((*pplayer).X) + effectFactor*scale;
            break;
        case 5:
            ((*pplayer).Y) = ((*pplayer).Y) + effectFactor*scale;
            break;
        case 6:
            ((*pplayer).Y) = ((*pplayer).Y) + effectFactor*scale; 
            ((*pplayer).X) = ((*pplayer).X) - effectFactor*scale;
            break;
        case 7: 
            ((*pplayer).X) = ((*pplayer).X) - effectFactor*scale;
            break;
        case 8:
            ((*pplayer).Y) = ((*pplayer).Y) - effectFactor*scale; 
            ((*pplayer).X) = ((*pplayer).X) - effectFactor*scale;
            break;
    }

    // Στην περίπτωση που ο άνεμος στείλει κάποιον παίκτη στη στερίά ή εκτός του χάρτη, αυτος επαναφέρεται στο νερό

    if ((map[(*pplayer).Y][(*pplayer).X] == 'X') || (map[(*pplayer).Y][(*pplayer).X] == 'o') || ((*pplayer).X < 0) || ((*pplayer).X > 99) || ((*pplayer).Y < 0) || ((*pplayer).Y > 32))
    {
        getOutOfLand(pplayer, slope);
    }

    // Ο παίκτης εμφανίζεται στην νέα του θέση

    gotoXY((*pplayer).X, (*pplayer).Y);
    getBackgroundColor((*pplayer).X, (*pplayer).Y, false);
    putch((*pplayer).symbol);
    SetConsoleTextAttribute(hConsole, 07);
}

void getOutOfLand(struct player * pplayer, int slope)
{
    while ((map[(*pplayer).Y][(*pplayer).X] == 'X') || (map[(*pplayer).Y][(*pplayer).X] == 'o') || ((*pplayer).X < 0) || ((*pplayer).X > 99) || ((*pplayer).Y < 0) || ((*pplayer).Y > 32))
    {
        switch (slope)
        {
        case 1:
            (*pplayer).Y += 1;         // Ο πάικτης επαναφλερεται στο νερό, κινούμενος
            break;                     // μία θέση αντίθετα από την την κατεύθυνση του
        case 2:                        // ανέμου μέχρις ότου να φτάσει στο νερό
            (*pplayer).Y += 1;
            (*pplayer).X -= 1;
            break;
        case 3:
            (*pplayer).X -= 1;
            break;
        case 4:
            (*pplayer).Y -= 1;
            (*pplayer).X -= 1;
            break;
        case 5:
            (*pplayer).Y -= 1;
            break;
        case 6:
            (*pplayer).Y -= 1;
            (*pplayer).X += 1;
            break;
        case 7:
            (*pplayer).X += 1;
            break;
        case 8:
            (*pplayer).Y += 1;
            (*pplayer).X += 1;
            break;
        default:
            break;
        }
    }
}

void moveConstraint(char key, bool* limitReached, int* countX, int* countY, bool* XlimitReached, bool* YlimitReached)
{
    switch(key)                                           // Όσο ο παίκτης εισάγει την κίνησή του, γίνεται συνεχώς
    {                                                     // έλεγχος για το πόσα βήματα ακόμα μπορεί να κάνει
        case 'w':                                         // και τυπώνεται αντίστοιχο μηνυμα
            (*countY)++;
            if(*YlimitReached == true) (*countY)--;
            break;
        case 's':
            (*countY)++;
            if(*YlimitReached == true) (*countY)--;
            break;
        case 'a':
            (*countX)++;
            if(*XlimitReached == true) (*countX)--;
            break;
        case 'd':
            (*countX)++;
            if(*XlimitReached == true) (*countX)--;
            break;
        default:
            break;
    } 
    deleteMessage(35);
    SetConsoleTextAttribute(hConsole, 07);
    gotoXY(0, 35);
    printf("Horizontal Steps: %d | Vertical Steps: %d", MAX_XSTEPS-*countX, MAX_YSTEPS-*countY);  

    // Όταν ο παίκτης δε μπορεί να κάνει άλλα βήματα σε κάποια ή και στις δύο κατευθύνσεις τυπώνεται αντίστοιχο μηνυμα

    if((*countX == MAX_XSTEPS) && (*countY == MAX_YSTEPS))
    {
        deleteMessage(36);
        gotoXY(0, 36);
        SetConsoleTextAttribute(hConsole, 07);
        puts("You can't make more steps!\n");
        *limitReached = true;
        Sleep(2000);
        deleteMessage(36);
        
    }

    else if(*countX == MAX_XSTEPS)
    {
        deleteMessage(36);
        gotoXY(0, 36);
        SetConsoleTextAttribute(hConsole, 07);
        puts("You can't make more steps in the horizontal direction!\n");
        *XlimitReached = true;
    }
    else if(*countY == MAX_YSTEPS)
    {
        deleteMessage(36);
        gotoXY(0, 36);
        SetConsoleTextAttribute(hConsole, 07);
        puts("You can't make more steps in the vertical direction!\n");
        *YlimitReached = true;
    }
}

void windPoint(int slope)   // Τυπώνεται μια πυξίδα που δείχνει την
{                           // κατύθυνση του ανέμου
    switch(slope)
    {
        case 1:
            gotoXY(0, 35);
            puts("    N\n    |\nW   |   E\n\n    S");
            break;
        case 2:
            gotoXY(0, 35);
            puts("    N\n     /\nW   /   E\n\n    S");
            break;
        case 3:
            gotoXY(0, 35);
            puts("    N\n\nW   --- E\n\n    S");
            break;
        case 4:
            gotoXY(0, 35);
            puts("    N\n\nW   \\   E\n     \\\n    S");
            break;
        case 5:
            gotoXY(0, 35);
            puts("    N\n\nW   |   E\n    |\n    S");
            break;
        case 6:
            gotoXY(0, 35);
            puts("    N\n\nW   /   E\n   /\n    S");
            break;
        case 7:
            gotoXY(0, 35);
            puts("    N\n\nW ---   E\n\n    S");
            break;
        case 8:
            gotoXY(0, 35);
            puts("    N\n   \\\nW   \\   E\n\n    S");
            break;
    }
}