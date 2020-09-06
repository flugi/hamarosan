#include "graphics.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cmath>
using namespace genv;
using namespace std;

const int XX=1920;
const int YY=1080;

struct ido {
    int ora, perc, masodperc;
};

chrono::time_point<chrono::system_clock> mai_napon_maskor(int ora, int perc) {
    chrono::time_point<chrono::system_clock> most = chrono::system_clock::now();
    time_t tmp = chrono::system_clock::to_time_t(most);
	//C++20 teljes implementáció majd támogat szebb módszert is, jelenleg a C struktúrák felülírása a szabványt leginkább követő megoldás
    tm local_tm = *localtime(&tmp);
    local_tm.tm_hour = ora; //nincs naptári napon átívelő várakozás támogatva
    local_tm.tm_min = perc;
    local_tm.tm_sec = 0;
    tmp = mktime(&local_tm);
    chrono::time_point<chrono::system_clock> vege =  chrono::system_clock::from_time_t(tmp);
    return vege;
}

ido hatralevo_ido(chrono::time_point<chrono::system_clock> vege) {
    ido d;
    chrono::time_point<chrono::system_clock> curr = chrono::system_clock::now();
    chrono::system_clock::duration difi = vege - curr;
    if (difi.count()>=0) {
        size_t ms = difi.count()/1000000;
        int sec = ms/1000;
        int min = sec/60;
        int hour = min/60;
        sec %= 60;
        min %= 60;
        d.ora = hour;
        d.perc = min;
        d.masodperc = sec;
        return d;
    }
    d.ora=-1;
    return d;
}

int main(int argc, char*argv[]) {
    stringstream ss;
    for (int i=1;i<argc;i++) {
        ss << argv[i] << " ";
    }
    ss << endl;
    int ora, perc;
    ss >> ora >> perc;
    string uzenet;
    getline(ss,uzenet);
    if (!ss.good()) {
        cerr << "usage: " << argv[0] << " hour minute message in a sentence" << endl;
        cerr << "example: " << argv[0] << " 10 15 the meeting" << endl;
        exit(1);
    }
    chrono::time_point<chrono::system_clock> vege = mai_napon_maskor(ora,perc);
    gout.open(XX,YY);
    gout.set_title("hamarosan");
    gout << font("LiberationSans-Bold.ttf",80);
    
    event ev;
    gin.timer(100);
    while (gin >> ev && ev.keycode != key_escape) {
        ido d = hatralevo_ido(vege);
        if (d.ora>=0) {
            stringstream sss;
            sss <<d.ora << ":" << (d.perc<10?"0":"") << d.perc << ":" <<(d.masodperc<10?"0":"") << d.masodperc;
            string timestamp = sss.str();
            gout << move_to(0,0) << color(100,100,100) << box(XX,YY) << color(255,255,255);

            if (d.ora<1) {
            for (int i=0;i<60;i++ ) {
                int x = -sin(i/30.0*M_PI)*YY/5+XX/2;
                int y = -cos(i/30.0*M_PI)*YY/5+2*YY/3;
                int c = (i < d.perc)*155+100;
                gout << move_to(x-5,y-5) <<color(c,c,c) << box(11,11);
            }
                for (int i=0;i<60;i++ ) {
                    int x = -sin(i/30.0*M_PI)*YY/5+XX/2;
                    int y = -cos(i/30.0*M_PI)*YY/5+2*YY/3;
                    int c = (i < d.masodperc)*64+115;
                    gout << move_to(x-2,y-2) <<color(c,c,c) << box(5,5);
                }
            }
            gout << color(255,255,255);


            gout << move_to(XX/2-gout.twidth(timestamp)/2,2*YY/3-gout.cascent()/2);
            gout << text(timestamp);
            gout << move_to(XX/2-gout.twidth(uzenet)/2,YY/4);
            gout << text(uzenet);
        } else {
            gout << move_to(0,0) << color(100,100,100) << box(XX,YY) << color(255,255,255);
            gout << move_to(XX/2-gout.twidth(uzenet)/2,YY/2);
            gout << text(uzenet);
        }
        gout << refresh;
    }
}
