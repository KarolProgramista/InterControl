#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"

#define HEIGHT 10
#define WIDTH 200

typedef struct Network {
    char name[50];
    int exist;
} Network;

typedef struct Networks {
    Network nets[100];
} Networks;

char *main_choices[] = {
    "List networks",
    "Connect",
    "Exit"
};

char *temp_filename = "/tmp/InterControl.temp";

int main_choices_n = sizeof(main_choices) / sizeof(char *);
void print_main_menu(WINDOW *menu_win, int highlight);
void list_network(WINDOW *menu_win);
void connect(WINDOW *menu_win);
void connectToWIFI(char *ssid, WINDOW *menu_win);

int main()
{
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int max_choices = main_choices_n;
    int c;

    initscr();
    clear();
    noecho();
    cbreak();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    
    menu_win = newwin(HEIGHT, WIDTH, 2, 0);
    keypad(menu_win, TRUE);

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 0, "InterControl 1.0");
    attroff(COLOR_PAIR(1) | A_BOLD);
    refresh();

    print_main_menu(menu_win, highlight);

    while(1)
    {
        c = wgetch(menu_win);

        switch(c)
        {
            case KEY_UP:
                if(highlight == 1)
                    highlight = max_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == max_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10:
                choice = highlight;
            default:
                break;
        }

        if (choice == 3)
        {
            break;
        }
        else if(choice == 1)
        {
           list_network(menu_win);
           choice = 0;
        }
        else if(choice == 2)
        {
            connect(menu_win);
            choice = 0;
            werase(menu_win);
        }
        
        
        print_main_menu(menu_win, highlight);
    }   

    clrtoeol();
    refresh();
    endwin();
    return 0;

}


void print_main_menu(WINDOW *menu_win, int highlight)
{
    int i;
    int x = 2;
    int y = 1;
    werase(menu_win);
    mvwprintw(menu_win ,0, 0, "Main Menu");
    
    for (i=0; i < main_choices_n; i++)
    {
        if(i+1 == highlight)
        {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y+i, x, main_choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        else
        {
            mvwprintw(menu_win, y+i, x, main_choices[i]);
        }
    }

    wrefresh(menu_win);
}

void list_network(WINDOW *menu_win)
{   
    werase(menu_win);
    mvwprintw(menu_win, 0, 0, "All WiFi networks");
    system("nmcli d wifi rescan > /dev/null 2>1&");
    sleep(2);
    system("nmcli d wifi list | tr '*' ' ' | awk '{print $1}' > /tmp/InterControl.temp");
    FILE *temp = fopen(temp_filename, "r");
    char buffer[200] = {0};
    int i = 0;
    int x = 2;
    
    while (fgets(buffer, 199, temp) != NULL)
    {
        if(i > 0)
            mvwprintw(menu_win, i, x, "%s\n", buffer);
        i++;
    }
    wrefresh(menu_win);
    fclose(temp);
    getch();
    return;
}

void connect(WINDOW *menu_win)
{
    werase(menu_win);
    mvwprintw(menu_win, 0, 0, "All WiFi networks");
    system("nmcli d wifi rescan > /dev/null 2>1&");
    sleep(2);
    system("nmcli d wifi list | tr '*' ' ' | awk '{print $1}' > /tmp/InterControl.temp");
    FILE *temp = fopen(temp_filename, "r");
    char buffer[50] = {0};
    int i = 0;
    int x = 2;
    int c;
    int highlight = 1;
    int choice = 0;
    int choices_n = 0;
    
    Networks *networks = malloc(sizeof(Network) * 100);

    while (fgets(buffer, 50, temp) != NULL)
    {
        if(i > 0)
        {
            choices_n++;
            strcpy(networks->nets[i-1].name, buffer);
        }
        i++;
    }

    while (1)
    {
        for(i=0; i < choices_n; i++)
        {
            if(highlight == i+1)
            {
                wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, i+1, x, networks->nets[i].name);
                wattroff(menu_win, A_REVERSE);
            }
            else
            {
                mvwprintw(menu_win, i+1, x, "%d : %s",i+1,networks->nets[i].name);
            }
        }

        wrefresh(menu_win);
        
        c = wgetch(menu_win);
        switch (c)
        {
            case KEY_UP:
                if(highlight > 1)
                    highlight--;
                else
                    highlight = choices_n;
                break;
            case KEY_DOWN:
                if (highlight < choices_n)
                    highlight++;
                else
                    highlight = 1;
                break;
            case 10:
                choice = highlight;
                break;
            case 'q':
                choice = -1;
            default:
                break;
        }
        if (choice > 0)
        {   
            connectToWIFI(networks->nets[choice-1].name, menu_win);
            break;
        }
        else if (choice < 0)
        {
            break;
        }
        
    }

    wrefresh(menu_win);
    fclose(temp);
    free(networks);
    return;
}

void connectToWIFI(char ssid[50], WINDOW *menu_win)
{
    werase(menu_win);
    char password[20];
    int i;
    char command[100];
    mvwprintw(menu_win, 0, 0, "Type your password");
    wrefresh(menu_win);
    echo();
    mvwgetstr(menu_win,1, 0, password);
    noecho();
    wrefresh(menu_win);
    // Delete new line from ssid
    for (i=0; i < strlen(ssid); i++)
    {
        if (ssid[i] == '\n')
            ssid[i] = '\0';
    }
    sprintf(command, "nmcli d wifi connect %s password %s > /dev/null 2>&1", ssid, password);
    system(command);
    return;
}
