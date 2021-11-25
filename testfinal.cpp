#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <ctime>
#include <cstdlib>
#include <chrono>

using namespace std;

class Point {
    private:
        int x;
        int y;
public:
    Point() : x(0), y(0) {} Point(int x, int y) : x(x), y(y) {}
    void setX(int x) {
        this->x = x;
    }
    void setY(int y) {
        this->y = y;
    }
    int getX(void) {
        return x;
    }
    int getY(void) {
        return y;
    }
};

class Room {
    protected:
        Point l;
        Point r;
        Point namel;
        Point namer;
    public:
        Room(Point l, Point r) : l(l), r(r) {}
        virtual void drawName(void) = 0;
        virtual void drawDoor(void) = 0;
        void drawBorder(void) {
            int width = r.getX() - l.getX() + 1;
            int height = r.getY() - l.getY() + 1;
            move(l.getY(), l.getX());
            for (int j = 0; j < width; j++) {
                addch('=');
            }
            move(l.getY()+height-1, l.getX());
            for (int j = 0; j < width; j++) {
                addch('=');
            }
            for (int i = 0; i < height; i++) {
                mvaddch(l.getY()+i, l.getX(), '|');
                mvaddch(l.getY()+i, r.getX(), '|');
            }
        }
        void draw() {
            drawBorder();
            drawDoor();
            drawName();
        }
        Point getLeft(void) {
            return l;
        }
        Point getRight(void) {
            return r;
        }
        bool checkInNameBorder(Point pos) {
            int x = pos.getX();
            int y = pos.getY();
            if (x >= namel.getX()-1 && x <= r.getX() &&
                y >= l.getY() && y <= namer.getY()) return true;
            return checkInBorder(pos);
        }
        bool checkInBorder(Point pos) {
            int x = pos.getX();
            int y = pos.getY();
            if (x == l.getX() && y == l.getY() + 3) return false;
            if (x >= l.getX() && x <= r.getX()) {
                if (y == l.getY() || y == r.getY()) return true;
            }
            if (y >= l.getY() && y <= r.getY()) {
                if (x == l.getX() || x == r.getX()) return true;
            }
            return false;
        }
        bool inArea(Point pos) {
            int _x = pos.getX();
            int _y = pos.getY();
            if (_x >= l.getX() && _x <= r.getX() && _y >= l.getY() && _y <= r.getY()) return true;
            return false;
        }
};

bool checkCanMove(Room *rooms[3], Point p) 
{
    int in_ch = mvinch(p.getY(), p.getX());
    if (in_ch == 'C') return false;
    for (int i = 0; i < 3; i++) {
        if ((*rooms[i]).checkInNameBorder(p)) {
            return false;
        }
    }
    return true;
}

class Citizen {
    private:
    Point pos;
    static int alives;
    bool dd;
    public:
    Citizen(Point p) : pos(p) 
    {
        alives++;
    }
    static Point generate(Point l, Point r) {
        int _x = (rand() % (r.getX()-3)) + (l.getX() + 2);
        int _y = (rand() % (r.getY()-3)) + (l.getY() + 2);
        Point pos(_x, _y);
        return pos;
    }
    static bool isAlive(void) {
        return alives > 0;
    }
    static void dead(void) {
        alives--;
    }
    void die(void) {
        mvaddch(pos.getY(), pos.getX(), ' ');
        dd = true;
        alives--;
    }
    bool isDie(void) {
        return dd;
    }
    void setPos(Point p) {
        this->pos = p;
    }
    Point getPos(void) {
        return pos;
    }
    void draw(void) {
        mvaddch(pos.getY(), pos.getX(), 'C');
    }
    void move(Room *rooms[3]) {
        int m;
        Point p;
        int in_ch;
        do {
            m = rand() % 4;
            p = pos;
            if (m == 0) {
                p.setY(p.getY()+1);
                if (checkCanMove(rooms, p)) {
                    pos.setY(p.getY());
                    mvaddch(p.getY()-1, p.getX(), ' ');
                    mvaddch(p.getY(), p.getX(), 'C');
                    break;
                }
            } else if (m == 1) {
                p.setY(p.getY()-1);
                if (checkCanMove(rooms, p)) {
                    pos.setY(p.getY());
                    mvaddch(p.getY()+1, p.getX(), ' ');
                    mvaddch(p.getY(), p.getX(), 'C');
                    break;
                }
            } else if (m == 2) {
                p.setX(p.getX()+1);
                if (checkCanMove(rooms, p)) {
                    pos.setX(p.getX());
                    mvaddch(p.getY(), p.getX()-1, ' ');
                    mvaddch(p.getY(), p.getX(), 'C');
                    break;
                }
            } else {
                p.setX(p.getX()-1);
                if (checkCanMove(rooms, p)) {
                    pos.setX(p.getX());
                    mvaddch(p.getY(), p.getX()+1, ' ');
                    mvaddch(p.getY(), p.getX(), 'C');
                    break;
                }
            }
        } while (true);
    }
};
int Citizen::alives = 0;

class Imposter {
    private:
    Point pos;
    public:
    Imposter(Point p) : pos(p) {}
    void setPos(Point p) {
        this->pos = p;
    }
    void draw(void) {
        mvaddch(pos.getY(), pos.getX(), 'I');
    }
    int getX(void) {
        return pos.getX();
    }
    int getY(void) {
        return pos.getY();
    }
    void setX(int x) {
        pos.setX(x);
    }
    void setY(int y) {
        pos.setY(y);
    }
    Point getPos(void) {
        return pos;
    }
    bool checkCivilInArea(void) {
        int in_ch;
        int y = pos.getY();
        int x = pos.getX();
        for (int i = y - 5; i < y + 5; i++) {
            for (int j = x - 5; j < x + 5; j++) {
                in_ch = mvinch((i < 0 ? 0 : i), (j < 0 ? 0 : j));
                if (in_ch == 'C') {
                    return true;
                }
            }
        }
        return false;
    }
    bool check(Point p1, Point p2) {
        return p1.getX() == p2.getX() && p1.getY() == p2.getY();
    }
    bool kill(Citizen *citizen[5], Room *cctv) {
        Point pos = this->pos;
        int x = pos.getX();
        int y = pos.getY();
        int in_ch = mvinch(y+1, x);
        bool killed = false;
        for (int i = 0; i < 5; i++) {
            if (citizen[i] == NULL) continue; 
            do {
                pos.setY(y+1);
                if (check(citizen[i]->getPos(), pos)) {
                    mvaddch(y+1, x, ' ');
                    citizen[i]->die();
                    killed = true;
                    break;
                }
                pos.setY(y-1);
                if (check(citizen[i]->getPos(), pos)) {
                    mvaddch(y-1, x, ' ');
                    citizen[i]->die();
                    killed = true;
                    break;
                }
                pos.setY(y);
                pos.setX(x-1);
                if (check(citizen[i]->getPos(), pos)) {
                    mvaddch(y, x-1, ' ');
                    citizen[i]->die();
                    killed = true;
                    break;
                }
                pos.setX(x+1);
                if (check(citizen[i]->getPos(), pos)) {
                    citizen[i]->die();
                    mvaddch(y, x+1, ' ');
                    killed = true;
                    break;
                }
                pos.setX(x);
            } while (0);
            if (killed) {
                citizen[i] = NULL;
                break;
            }
        }
        if (killed) {
            for (int i = 0; i < 5; i++) {
                if (citizen[i] == NULL) continue;
                if (cctv->inArea(citizen[i]->getPos())) return true;
            }
            return checkCivilInArea();
        }
        return false;
    }
};


class CCTVRoom : public Room {
    public:
    CCTVRoom(Point l, Point r) : Room(l, r) { }
    void drawDoor(void) {
        mvaddch(l.getY() + 3, l.getX(), ' ');
    }
    void drawName(void) {
        namel.setX(r.getX()-6);
        namel.setY(l.getY());
        namer.setX(r.getX());
        namer.setY(l.getY()+2);
        mvaddstr(l.getY()+1, r.getX()-6, "CCTV");
        mvaddstr(l.getY()+2, r.getX()-6,  "ROOM");
    }
};

class NormalRoom : public Room {
    public:
    NormalRoom(Point l, Point r) : Room(l, r) {}
    void drawDoor(void) {
        mvaddch(l.getY() + 3, l.getX(), ' ');
    }
    void drawName(void) {
        namel.setX(r.getX()-6);
        namel.setY(l.getY());
        namer.setX(r.getX());
        namer.setY(l.getY()+2);
        mvaddstr(l.getY()+1, r.getX()-6, "ROOM");
    }
};

class EntireRoom : public Room {
    public:
    EntireRoom(Point l, Point r) : Room(l, r) {}
    void drawDoor(void) {}
    void drawName(void) {}
};


void drawRoom(Room *rooms[3]) {
    EntireRoom *entire = new EntireRoom(Point(0,0), Point(81,21));
    CCTVRoom *cctv = new CCTVRoom(Point(4,3), Point(33,9));
    NormalRoom *normal = new NormalRoom(Point(4,9), Point(38,16));
    rooms[0] = entire;
    rooms[1] = cctv;
    rooms[2] = normal;

    for (int i = 0; i < 3; i++) {
        (*rooms[i]).draw();
    }
}

int showMenu(Room *rooms[3]) {
    addstr("> START GAME\n");
    addstr("  END GAME\n");
    move(0, 0);
    timeout(-1);
    int in_ch;
    int start = 1;
    while (1) {
        in_ch = getch();
        switch (in_ch)
        {
            case KEY_DOWN:
                mvaddch(0, 0, ' ');
                mvaddch(1, 0, '>');
                start = 0;
                break;
            case KEY_UP:
                mvaddch(1, 0, ' ');
                mvaddch(0, 0, '>');
                start = 1;
                break;
            case 10: /* enter */
                if (start) {
                    clear();
                    drawRoom(rooms);
                    return 1;
                } else {
                    return 0;
                }
                break;
        }
        refresh();
    }
    return 0;
}

void doImposter(Citizen *citizen[5], Room *rooms[3]) {
    Imposter *imposter = new Imposter(Point(40, 10));
    imposter->draw();
    refresh();
    Point mov;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    chrono::steady_clock::time_point end;
    timeout(1000);

    while (1) {

        /* check time over */
        end = chrono::steady_clock::now();
        if ((chrono::duration_cast<chrono::seconds>(end - begin).count()) > 120) {
            clear();
            addstr("TIME OVER!!");
            refresh();
            sleep(3);
            return;
        }

        /* move one citizen randomly */
        do {
            int r = rand() % 5;
            Citizen *cit = citizen[r];
            if (cit == NULL) continue;
            cit->move(rooms);
            break;
        } while (true);

        /* move imposter */
        mov = imposter->getPos();
        move(mov.getY(), mov.getX());
        int in_ch = getch();
        switch (in_ch)
        {
            case KEY_DOWN:
                mov.setY(imposter->getY()+1);
                if (!checkCanMove(rooms, mov)) {
                    continue;
                }
                mvaddch(imposter->getY(), imposter->getX(), ' ');
                mvaddch(imposter->getY()+1, imposter->getX(), 'I');
                imposter->setY(imposter->getY()+1);
                break;
            case KEY_UP:
                mov.setY(imposter->getY()-1);
                if (!checkCanMove(rooms, mov)) {
                    continue;
                }
                mvaddch(imposter->getY(), imposter->getX(), ' ');
                mvaddch(imposter->getY()-1, imposter->getX(), 'I');
                imposter->setY(imposter->getY()-1);
                break;
            case KEY_LEFT:
                mov.setX(imposter->getX()-1);
                if (!checkCanMove(rooms, mov)) {
                    continue;
                }
                mvaddch(imposter->getY(), imposter->getX(), ' ');
                mvaddch(imposter->getY(), imposter->getX()-1, 'I');
                imposter->setX(imposter->getX()-1);
                break;
            case KEY_RIGHT:
                mov.setX(imposter->getX()+1);
                if (!checkCanMove(rooms, mov)) {
                    continue;
                }
                mvaddch(imposter->getY(), imposter->getX(), ' ');
                mvaddch(imposter->getY(), imposter->getX()+1, 'I');
                imposter->setX(imposter->getX()+1);
                break;
            case 'a':
                if (imposter->kill(citizen, rooms[1])) {
                    clear();
                    addstr("FAIL!!");
                    refresh();
                    sleep(3);
                    free(imposter);
                    return;
                } else {
                    if (!Citizen::isAlive()) {
                        clear();
                        addstr("CLEAR!!");
                        refresh();
                        sleep(3);
                        free(imposter);
                        return;
                    }
                }
                break;
        }
        refresh();
    }
}

int main(void)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    Room *rooms[3];
    int start;
    do {
        start = showMenu(rooms);
        if (start) {
            srand((unsigned int)time(NULL));
            Citizen *citizen[5];
            Point pos;
            for (int i = 0; i < 5; i++) {
                while (true) {
                    pos = Citizen::generate(rooms[0]->getLeft(), rooms[0]->getRight());
                    if (rooms[1]->checkInNameBorder(pos) || rooms[2]->checkInNameBorder(pos)) {
                        continue;
                    }
                    citizen[i] = new Citizen(pos);
                    citizen[i]->draw();
                    break;
                }
            }
            doImposter(citizen, rooms);

            /* cleanup */
            for (int i = 0; i < 3; i++) {
                free(rooms[i]);
            }
            for (int i = 0; i < 5; i++) {
                free(citizen[i]);
            }
        }
        clear();
        refresh();
    } while (start);
    endwin();
    return 0;
}
