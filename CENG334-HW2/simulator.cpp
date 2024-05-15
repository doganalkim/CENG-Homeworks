#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <queue>
#include <semaphore.h>
#include "helper.h"
#include "WriteOutput.h"
#include "monitor.h"
#include <errno.h>

#define S(X) (sizeof(X))
#define SLP(ts, tv , t)\
                        gettimeofday(&tv, NULL); \
                        ts.tv_sec = tv.tv_sec + (t / 1000); \
                        ts.tv_nsec = (tv.tv_usec + 1000UL * ((t % 1000)) )* 1000UL ; \
                        if( ts.tv_nsec >= 1000000000UL ){ \
                            ts.tv_nsec -= 1000000000; \
                            ts.tv_sec += 1; \
                        }\


int numbOfNarrowBridges, numbOfFerries, numbOfCrossroads,numbOfCars;
int Nt,Nm;
int Ft, Fm, Fc;
int Ct, Cm;
int CAt, CAp;
char C;
int Cid, FCM, TCM;
int c;

pthread_t *tid;


class NarrowBridge : public Monitor{
    int turn,carpassing, carsOnBridge,onetozero, zerotoone;
    std::queue<int> q1to0wait, q0to1wait;
    Condition *c,*w, *sleep; 
public:
    int NarrowBridgeTravelTime;
    int NarrowBridgeMaxWaitTime;
    NarrowBridge(){ 
        turn = 0;
        carpassing = 0;
        carsOnBridge = 0;
        onetozero = 0;
        zerotoone = 0;
    }

    void createConditionArray(int cars){
        __synchronized__ ;
        c = (Condition *) malloc(cars * S(Condition));
        for(int i = 0; i < cars ; i++) c[i] = Condition(this);
        //for(int i = 0; i < cars ; i++) t[i] = Condition(this);
        w = (Condition *) malloc(S(Condition));
        *w = Condition(this);
    }

    void enterBridge(int cid, int crid,int from, int to){
        __synchronized__;
        WriteOutput(cid,'N',crid,ARRIVE);
        int r = 0, alreadySlept = 0;
        if(from == 0 && to == 1){
            q0to1wait.push(cid);
            while(1){
                if(turn == 0){
                    while(q0to1wait.front() != cid){ // wait for the one in the front to pass
                        c[cid].wait();
                    }
                    if(turn  == 1){ // the turn might change when we awake
                        continue;
                    }
                    zerotoone++;
                    if(zerotoone > 1 ){
                        struct timespec ts;
                        struct timeval tv;
                        SLP(ts, tv, PASS_DELAY); 
                        w->timedwait(&ts);
                    }
                    if(turn == 1){
                        zerotoone--;
                        continue;
                    }
                    q0to1wait.pop();
                    if(turn == 0 && !q0to1wait.empty()){
                        int f = q0to1wait.front(); c[f].notify();
                    }
                    WriteOutput(cid,'N',crid,START_PASSING);
                    carpassing++;
                    struct timespec ts1;
                    struct timeval tv1;
                    SLP(ts1,tv1, NarrowBridgeTravelTime);
                    w -> timedwait(&ts1);
                    WriteOutput(cid,'N',crid,FINISH_PASSING);
                    carpassing--;
                    zerotoone--;
                    if(turn == 1 && carpassing == 0){ // turn changed while we were on the bridge. Timeout occured
                        if(!q1to0wait.empty()){
                            int f = q1to0wait.front(); c[f].notify();
                        }
                        if(!q0to1wait.empty()){
                            int f = q0to1wait.front(); c[f].notify(); w->notify();
                        }
                    }
                    else if(turn == 0 && carpassing == 0 && q0to1wait.empty()){
                        if(!q1to0wait.empty())
                        {
                            int f = q1to0wait.front(); c[f].notify();
                        }
                    }
                    else if(turn == 0 && carpassing == 0  && zerotoone == 0 &&  !q0to1wait.empty()){

                        if(!q1to0wait.empty())
                        {
                            turn = 1;
                            int f = q1to0wait.front(); c[f].notify();
                        }
                        int f = q0to1wait.front(); c[f].notify(); 
                    }
                    break;
                }
                else if(turn == 1 &&  carpassing == 0 &&  q1to0wait.empty()){
                    turn = 0;
                }
                else if(r == ETIMEDOUT ){
                    turn = 0;
                    if(carpassing > 0 )  {
                        c[cid].wait();
                    }
                    r = 0;
                }
                else{
                    struct timespec ts;
                    struct timeval tv;
                    SLP(ts,tv,NarrowBridgeMaxWaitTime);
                    if(cid == q0to1wait.front())
                        r = c[cid].timedwait(&ts);
                    else 
                        c[cid].wait();
                }
            }
        }
        else if(from == 1 && to == 0){
            q1to0wait.push(cid);
            while(1){
                if(turn == 1){
                    while(q1to0wait.front() != cid){ // wait for the one in the front to pass
                        c[cid].wait();
                    }
                    if(turn  == 0){ // the turn might change when we awake
                        continue;
                    }
                    onetozero++;
                    if(onetozero > 1){
                        struct timespec ts;
                        struct timeval tv;
                        SLP(ts, tv, PASS_DELAY); 
                        w->timedwait(&ts);
                    }
                    if(turn == 0){
                        onetozero--;
                        continue;
                    }
                    q1to0wait.pop();
                    if(turn == 1 && !q1to0wait.empty()){
                        int f = q1to0wait.front(); c[f].notify();
                    }
                    WriteOutput(cid,'N',crid,START_PASSING);
                    carpassing++;
                    struct timespec ts1;
                    struct timeval tv1;
                    SLP(ts1,tv1, NarrowBridgeTravelTime);
                    w -> timedwait(&ts1);
                    WriteOutput(cid,'N',crid,FINISH_PASSING);
                    onetozero--;
                    carpassing--;
                    if(turn == 0 && carpassing == 0){ // turn changed while we were on the bridge. Timeout occured
                        if(!q1to0wait.empty()){
                            int f = q1to0wait.front(); c[f].notify(); w->notify();
                        }
                        if(!q0to1wait.empty()){
                            int f = q0to1wait.front(); c[f].notify(); 
                        }
                    }
                    else if(turn == 1 && carpassing == 0 && q1to0wait.empty()){
                        if(!q0to1wait.empty())
                        {
                            int f = q0to1wait.front(); c[f].notify();
                        }
                    }
                    else if(turn == 1 && carpassing == 0 && onetozero == 0 && !q1to0wait.empty()){
                        if(!q0to1wait.empty()){
                             turn = 0;
                             int f = q0to1wait.front(); c[f].notify();
                        }
                        int f = q1to0wait.front(); c[f].notify();
                    }
                    break;
                }
                else if(turn == 0 &&  carpassing == 0 && q0to1wait.empty()  ){
                    turn = 1;
                }
                else if(r == ETIMEDOUT ){
                    turn = 1;
                    if(carpassing > 0)
                    {
                            c[cid].wait();
                    }
                    r = 0;
                }
                else{
                    struct timespec ts;
                    struct timeval tv;
                    SLP(ts,tv,NarrowBridgeMaxWaitTime);
                    if(cid == q1to0wait.front())
                        r = c[cid].timedwait(&ts);
                    else 
                        c[cid].wait();
                }
            }
        }
    }
};


class Ferry : public Monitor {
public:
    Condition *c,*w;
    int loadedCars0to1 , loadedCars1to0 , FerryTravelTime , FerryMaxWaitTime , FerryCapacity;
    Ferry(){
        loadedCars0to1 = 0;
        loadedCars1to0 = 0;
    }

    void initConditions(){
        __synchronized__ ;
        c = (Condition *) malloc( 2 * S(Condition));
        c[0] = Condition(this);
        c[1] = Condition(this);
        w = (Condition *) malloc( S(Condition));
        *(w) = Condition(this);
    }

    
    void enterFerry(int cid , int crid, int f, int t){
        __synchronized__
        WriteOutput(cid,'F',crid,ARRIVE);
        if(f == 0 && t == 1){
            loadedCars0to1++;
            if(loadedCars0to1 == 1){
                struct timespec ts;
                struct timeval tv;
                SLP(ts, tv, FerryMaxWaitTime);
                int res = c[0].timedwait(&ts);
                if(res == ETIMEDOUT){
                    c[0].notifyAll();
                    loadedCars0to1 = 0; // unloaded is done at the same time for each car       
                }
            }
            else if(loadedCars0to1 == FerryCapacity){ // if you are the last on the ferry, notify other cars
                c[0].notifyAll();  // notify other cars
                loadedCars0to1 = 0;
            }
            else{
                c[0].wait();
            }

            WriteOutput(cid,'F',crid,START_PASSING);
            struct timespec ts1;
            struct timeval tv1;
            SLP(ts1, tv1, FerryTravelTime);
            int check = w->timedwait(&ts1);
            WriteOutput(cid,'F',crid,FINISH_PASSING);     
        }
        else if(f == 1 && t == 0){
            loadedCars1to0++;
            if(loadedCars1to0 == 1){
                struct timespec ts;
                struct timeval tv;
                SLP(ts, tv, FerryMaxWaitTime);
                int res = c[1].timedwait(&ts);
                if(res == ETIMEDOUT){
                    loadedCars1to0 = 0; // unloaded is d
                    c[1].notifyAll();
                }
            }
            else if(loadedCars1to0 == FerryCapacity){ // if you are the last on the ferry, notify other cars
                loadedCars1to0 = 0;
                c[1].notifyAll();  // notify other cars 
            }
            else{
                c[1].wait();
            }
            WriteOutput(cid,'F',crid,START_PASSING);
            struct timespec ts1;
            struct timeval tv1;
            SLP(ts1, tv1, FerryTravelTime);
            int check = w->timedwait(&ts1);
            WriteOutput(cid,'F',crid,FINISH_PASSING);  
        }
    }
};

class CrossRoad : public Monitor{

public:
    int CrossRoadTravelTime, CrossRoadMaxWaitTime,turn,carStartedPassing,carPassing, timeOut0, timeOut1, timeOut2, timeOut3, p0,p1,p2,p3 ;
    std::queue<int> q0,q1,q2,q3;
    Condition *c,*w,*sleep;

    CrossRoad(){
        turn = -1;
        carStartedPassing = 0;
        carPassing = 0;
        timeOut0 = 0; timeOut1 = 0; timeOut2 = 0; timeOut3 = 0;
        p0 = 0; p1 = 0; p2 = 0; p3 = 0;
    }

    void initCR(int cars){
        __synchronized__ ;
        c = (Condition *) malloc( cars *  S(Condition));
        for(int i = 0; i < cars ; i++) c[i] = Condition(this);
        w = (Condition *) malloc(S(Condition));  
        *(w) = Condition(this);
        sleep = (Condition *) malloc(S(Condition));  
        *(sleep) = Condition(this);
    }

    void enterCR0(int cid , int crid, int f, int t){
        __synchronized__ ;
        WriteOutput(cid,'C',crid,ARRIVE);
        int alreadySlept = 0, r = 0;
        q0.push(cid); // push the queue at the begginig
        while(1){
            if(turn == 0){ // if active lane is your lane
                while(q0.front() != cid){  // if you are not at the front of the queue, you should wait
                    c[cid].wait();
                }
                if(turn != 0){ // if turn just swapped while we sleep, just go sleep again
                    continue;
                }
                p0++;
                if(p0 > 1){ // if you are not the first car, you do not have to wait for pass delay
                    struct timespec ts;
                    struct timeval tv;
                    SLP(ts, tv, PASS_DELAY); 
                    w->timedwait(&ts);
                }
                if(turn != 0){
                    p0--; continue;
                }
                if(timeOut1 == 1 || timeOut2 == 1 || timeOut3 == 1 ){
                    if(carPassing > 0 ) {
                        p0--;
                        continue;
                    }

                    if(!q1.empty()){
                        turn = 1;   
                    }
                    else if(!q2.empty()){
                        turn = 2; 
                    }
                    else if(!q3.empty()){
                        turn = 3;   
                    }

                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                        int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    p0--;
                    timeOut1 = timeOut2 = timeOut3 = 0;
                    continue;
                }

                q0.pop();


                if(turn == 0 && !q0.empty()){ // awake the next dude if your lane is still active
                    int f = q0.front(); c[f].notify();
                }
                WriteOutput(cid,'C',crid,START_PASSING);
                carPassing++;
                struct timespec ts1;
                struct timeval tv1;
                SLP(ts1,tv1,CrossRoadTravelTime);
                w->timedwait(&ts1);
                WriteOutput(cid,'C',crid,FINISH_PASSING);
                p0--;
                carPassing--;
                if(timeOut0 == 0 && turn == 0 && carPassing == 0 && carPassing == 0 && q0.empty()){ // you are the last guy in your lane. Change turn
                    if(!q1.empty()){
                        turn = 1; 
                    }
                    else if(!q2.empty()){
                        turn = 2; 
                    }
                    else if(!q3.empty()){
                        turn = 3; 
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                        int f = q3.front();  c[f].notify();
                    }
                    timeOut1 = timeOut2 = timeOut3 = 0;
                }
                else if( carPassing == 0 && (timeOut1 == 1 || timeOut2 == 1 || timeOut3 == 1 )){
                    if(!q1.empty()){
                        turn = 1;   
                    }
                    else if(!q2.empty()){
                        turn = 2; 
                    }
                    else if(!q3.empty()){
                        turn = 3;   
                    }

                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                        int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    
                    timeOut1 = timeOut2 = timeOut3 = 0;
                }
                break;
            }
            else if(r == ETIMEDOUT){ // timeout occured, obtain the road if possible. Turn will be arranged from quitting lane
                timeOut0 = 1;  // notify that timeout has occured, go back to sleeping
                r = 0;
            }
            else if( turn != 0  && carPassing == 0 &&  q1.empty() && q2.empty() && q3.empty() ){ // this is not your turn, but you can enter since no one is there
                turn = 0;
            }
            else { // you cannot obtain the road, wait for timeOut
                struct timespec ts;
                struct timeval tv;
                SLP(ts,tv,CrossRoadMaxWaitTime);
                if(cid == q0.front())
                    r = c[cid].timedwait(&ts); // for resetting the timer
                else
                    c[cid].wait();
            }
        } 
    }

    void enterCR1(int cid , int crid, int f, int t){
        __synchronized__ ;
        WriteOutput(cid,'C',crid,ARRIVE);
        int alreadySlept = 0, r = 0;
        q1.push(cid); // push the queue at the begginig
        while(1){
            if(turn == 1){ // if active lane is your lane
                while(q1.front() != cid){  // if you are not at the front of the queue, you should wait
                    c[cid].wait();
                }
                if(turn != 1){ // if turn just swapped while we sleep, just go sleep again
                    continue;
                }
                p1++;
                if(p1 > 1){ // if you are not the first car, you do not have to wait for pass delay
                    struct timespec ts;
                    struct timeval tv;
                    SLP(ts, tv, PASS_DELAY); 
                    w->timedwait(&ts);
                }
                if(turn != 1){
                    p1--; continue;
                }
                if(timeOut2 == 1 || timeOut3 == 1 || timeOut0 == 1){
                    if(carPassing > 0){
                        p1--;
                        continue;
                    }
                    if(!q2.empty()){
                        turn = 2;  
                    }
                    else if(!q3.empty()){
                        turn = 3;   
                    }
                    else if(!q0.empty()){
                        turn = 0;  
                    }

                     if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                         int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }

                    if(!q1.empty()){
                        int f = q1.front(); c[f].notify();
                    }
                    p1--;
                    timeOut0 = timeOut2 = timeOut3 = 0;
                    continue;
                }
                q1.pop();
                if(turn == 1 && !q1.empty()){ // awake the next dude if your lane is still active
                    int f = q1.front(); c[f].notify();
                }
                WriteOutput(cid,'C',crid,START_PASSING);
                carPassing++;
                struct timespec ts1;
                struct timeval tv1;
                SLP(ts1,tv1,CrossRoadTravelTime);
                w->timedwait(&ts1);
                WriteOutput(cid,'C',crid,FINISH_PASSING);
                p1--;
                carPassing--;
                
                if(timeOut1 == 0 && turn == 1 && carPassing == 0 && q1.empty()){ // you are the last guy in your lane. Change turn
                   if(!q2.empty()){
                        turn = 2;  
                    }
                    else if(!q3.empty()){
                        turn = 3;   
                    }
                    else if(!q0.empty()){
                        turn = 0;  
                    }

                     if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                         int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }

                    timeOut0 = timeOut2 = timeOut3 = 0;
                }
                else if(carPassing == 0 && (timeOut0 == 1 || timeOut2 == 1 || timeOut3 == 1) ){
                    if(!q2.empty()){
                        turn = 2;  
                    }
                    else if(!q3.empty()){
                        turn = 3;   
                    }
                    else if(!q0.empty()){
                        turn = 0;  
                    }

                     if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                         int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }

                    if(!q1.empty()){
                        int f = q1.front(); c[f].notify();
                    }

                    timeOut0 = timeOut2 = timeOut3 = 0;
                }
                break;

            }
            else if(r == ETIMEDOUT){ // timeout occured, obtain the road if possible. Turn will be arranged from quitting lane
                timeOut1 = 1;  // notify that timeout has occured, go back to sleeping
                r = 0;
            }
            else if( turn != 1  &&  carPassing == 0 && q0.empty() && q2.empty() && q3.empty() ){ // this is not your turn, but you can enter since no one is there
                turn = 1;
            }
            else { // you cannot obtain the road, wait for timeOut
                struct timespec ts;
                struct timeval tv;
                SLP(ts,tv,CrossRoadMaxWaitTime);
                if(cid == q1.front())
                    r = c[cid].timedwait(&ts); // for resetting the timer
                else
                    c[cid].wait();
            }
        } 
    }

    void enterCR2(int cid , int crid, int f, int t){
        __synchronized__ ;
        WriteOutput(cid,'C',crid,ARRIVE);
        int alreadySlept = 0, r = 0;
        q2.push(cid); // push the queue at the begginig
        while(1){
            if(turn == 2){ // if active lane is your lane
                while(q2.front() != cid){  // if you are not at the front of the queue, you should wait
                    c[cid].wait();
                }
                if(turn != 2){ // if turn just swapped while we sleep, just go sleep again
                    continue;
                }
                p2++;
                if(p2 > 1){ // if you are not the first car, you do not have to wait for pass delay
                    struct timespec ts;
                    struct timeval tv;
                    SLP(ts, tv, PASS_DELAY); 
                    w->timedwait(&ts);
                }

                if(turn != 2){
                    p2--; continue;
                }
                if(timeOut3 == 1 || timeOut0 == 1 || timeOut1 == 1){
                    if(carPassing > 0 ){
                        p2--;
                        continue;
                    }
                     if(!q3.empty()){
                        turn = 3; 
                    }
                    else if(!q0.empty()){
                        turn = 0; 
                    }
                    else if(!q1.empty()){
                        turn = 1;  
                    }

                    if(!q3.empty()){
                        int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front(); c[f].notify();
                    }
                    p2--;
                    timeOut0 = timeOut1 = timeOut3 = 0;
                    continue;

                }
                q2.pop();
                if(turn == 2 && !q2.empty()){ // awake the next dude if your lane is still active
                    int f = q2.front(); c[f].notify();
                }
                WriteOutput(cid,'C',crid,START_PASSING);
                carPassing++;
                struct timespec ts1;
                struct timeval tv1;
                SLP(ts1,tv1,CrossRoadTravelTime);
                w->timedwait(&ts1);
                WriteOutput(cid,'C',crid,FINISH_PASSING);
                carPassing--;
                p2--; 
                
                if(timeOut2 == 0 && turn == 2 && carPassing == 0 && q2.empty()){ // you are the last guy in your lane. Change turn
                    if(!q3.empty()){
                        turn = 3; 
                    }
                    else if(!q0.empty()){
                        turn = 0; 
                    }
                    else if(!q1.empty()){
                        turn = 1;  
                    }

                    if(!q3.empty()){
                        int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }

                    timeOut0 = timeOut1 = timeOut3 = 0;

                }
                else if(carPassing == 0 && (timeOut0 == 1 || timeOut1 == 1 || timeOut3 == 1)){
                    if(!q3.empty()){
                        turn = 3; 
                    }
                    else if(!q0.empty()){
                        turn = 0; 
                    }
                    else if(!q1.empty()){
                        turn = 1;  
                    }

                    if(!q3.empty()){
                        int f = q3.front();  c[f].notify();
                    }
                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front(); c[f].notify();
                    }

                    timeOut0 = timeOut1 = timeOut3 = 0;
                }
                break;

            }
            else if(r == ETIMEDOUT){ // timeout occured, obtain the road if possible. Turn will be arranged from quitting lane
                timeOut2 = 1;  // notify that timeout has occured, go back to sleeping
                r = 0;
            }
            else if( turn != 2  &&  carPassing == 0 && q0.empty() && q1.empty() && q3.empty() ){ // this is not your turn, but you can enter since no one is there
                turn = 2;
            }
            else { // you cannot obtain the road, wait for timeOut
                struct timespec ts;
                struct timeval tv;
                SLP(ts,tv,CrossRoadMaxWaitTime);
                if(cid == q2.front())
                    r = c[cid].timedwait(&ts); // for resetting the timer
                else
                    c[cid].wait();
            }
        } 
    }

    void enterCR3(int cid , int crid, int f, int t){
        __synchronized__ ;
        WriteOutput(cid,'C',crid,ARRIVE);
        int alreadySlept = 0, r = 0;
        q3.push(cid); // push the queue at the begginig
        while(1){
            if(turn == 3){ // if active lane is your lane
                while(q3.front() != cid){  // if you are not at the front of the queue, you should wait
                    c[cid].wait();
                }
                if(turn != 3){ // if turn just swapped while we sleep, just go sleep again
                    continue;
                }
                p3++;
                if(p3 > 1 ){ // if you are not the first car, you do not have to wait for pass delay
                    struct timespec ts;
                    struct timeval tv;
                    SLP(ts, tv, PASS_DELAY); 
                    w->timedwait(&ts);
                }
                if(turn != 3){
                    p3--; continue;
                }
                if(timeOut0 == 1 || timeOut1 == 1 || timeOut2 == 1){
                    if(carPassing > 0){
                        p3--;
                        continue;
                    }
                    if(!q0.empty()){
                        turn = 0; 
                    }
                    else if(!q1.empty()){
                        turn = 1;  
                    }
                    else if(!q2.empty()){
                        turn = 2;   
                    }

                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                        int f = q3.front(); c[f].notify();
                    }
                    p3--;
                    timeOut0 = timeOut1 = timeOut2 = 0;
                    continue;
                }
                 
                q3.pop();
                if(turn == 3 && !q3.empty()){ // awake the next dude if your lane is still active
                    int f = q3.front(); c[f].notify();
                }
                WriteOutput(cid,'C',crid,START_PASSING);
                carPassing++;
                struct timespec ts1;
                struct timeval tv1;
                SLP(ts1,tv1,CrossRoadTravelTime);
                w->timedwait(&ts1);
                WriteOutput(cid,'C',crid,FINISH_PASSING);
                carPassing--;
                 p3--;
                
                if(timeOut3 == 0 && turn == 3 && carPassing == 0 && q3.empty()){ // you are the last guy in your lane. Change turn
                    if(!q0.empty()){
                        turn = 0; 
                    }
                    else if(!q1.empty()){
                        turn = 1;  
                    }
                    else if(!q2.empty()){
                        turn = 2;   
                    }

                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    timeOut0 = timeOut1 = timeOut2 = 0;
                }
                else if( carPassing == 0 && (timeOut0 == 1 || timeOut1 == 1 || timeOut2 == 1)){
                    if(!q0.empty()){
                        turn = 0; 
                    }
                    else if(!q1.empty()){
                        turn = 1;  
                    }
                    else if(!q2.empty()){
                        turn = 2;   
                    }

                    if(!q0.empty()){
                        int f = q0.front();  c[f].notify();
                    }
                    if(!q1.empty()){
                        int f = q1.front();  c[f].notify();
                    }
                    if(!q2.empty()){
                        int f = q2.front();  c[f].notify();
                    }
                    if(!q3.empty()){
                        int f = q3.front(); c[f].notify();
                    }
                    timeOut0 = timeOut1 = timeOut2 = 0;
                }
                break;

            }
            else if(r == ETIMEDOUT){ // timeout occured, obtain the road if possible. Turn will be arranged from quitting lane
                timeOut3 = 1;  // notify that timeout has occured, go back to sleeping
                r = 0;
            }
            else if( turn != 3  && carPassing == 0 &&  q0.empty() && q1.empty() && q2.empty() ){ // this is not your turn, but you can enter since no one is there
                turn = 3;
            }
            else { // you cannot obtain the road, wait for timeOut
                struct timespec ts;
                struct timeval tv;
                SLP(ts,tv,CrossRoadMaxWaitTime);
                if(cid == q3.front())
                    r = c[cid].timedwait(&ts); // for resetting the timer
                else
                    c[cid].wait();
            }
        } 
    }
};

typedef struct{
    int id;
    int travelTime;
    int pathLen;
    char *path;
} Car;


NarrowBridge *NarrowBridges;
Ferry *Ferries;
CrossRoad *CrossRoads;

void take_input();
void *carthread(void *ptr);
void *timerthreadForBridge(void *ptr);
void *timerthreadForFerry1to0(void *ptr);
void *timerthreadForFerry0to1(void *ptr);
void waitForReaping();
void pass(char CurrentConnector,int cid , int CurrentConnectorId,int from, int to);
void passNarrowBridge(int cid, int crid, int f,int t);
void passFerry(int cid, int crid , int f ,int t);
void PassCR(int cid, int crid , int f ,int t);

int main(){
    take_input();
    waitForReaping();
    return 0;
}

void take_input(){
    scanf("%d",&numbOfNarrowBridges);
    if(numbOfNarrowBridges)
        NarrowBridges = (NarrowBridge *) malloc(numbOfNarrowBridges * S(NarrowBridge));
    for(int i = 0; i < numbOfNarrowBridges ; i++){ // Bridge handling
        scanf("%d %d",&Nt,&Nm);
        NarrowBridges[i] = NarrowBridge();
        NarrowBridges[i].NarrowBridgeTravelTime = Nt;
        NarrowBridges[i].NarrowBridgeMaxWaitTime = Nm;
    }
    scanf("%d",&numbOfFerries);
    if(numbOfFerries)
        Ferries = (Ferry *) malloc(numbOfFerries * S(Ferry));
    for(int i = 0; i < numbOfFerries ; i++){ // Ferry handli g
        scanf("%d %d %d",&Ft,&Fm,&Fc);
        Ferries[i] = Ferry();
        Ferries[i].initConditions();
        Ferries[i].FerryTravelTime = Ft;
        Ferries[i].FerryMaxWaitTime = Fm;
        Ferries[i].FerryCapacity = Fc;
    }
    scanf("%d",&numbOfCrossroads);
    if(numbOfCrossroads)
        CrossRoads = (CrossRoad *) malloc(numbOfCrossroads * S(CrossRoad));
    for(int i = 0; i < numbOfCrossroads ; i++){
        scanf("%d %d",&Ct, &Cm);
        CrossRoads[i] = CrossRoad();
        CrossRoads[i].CrossRoadTravelTime = Ct;
        CrossRoads[i].CrossRoadMaxWaitTime = Cm;
    }

    scanf("%d",&numbOfCars);
    if(numbOfCars)
        tid = (pthread_t *) malloc(numbOfCars*S(pthread_t));
    for(int i = 0; i < numbOfNarrowBridges ; i++){
        NarrowBridges[i].createConditionArray(numbOfCars);
        
    }
    for(int i = 0; i < numbOfCrossroads ; i++){
        CrossRoads[i].initCR(numbOfCars);
    }
    for(int i = 0; i < numbOfCars ; i++){
        scanf("%d %d",&CAt,&CAp);
        char c;
        while( (c = getchar() ) != '\n' ); // consume new line character for proper getline operation
        char *input;
        size_t len;
        ssize_t read = getline(&input,&len,stdin);
        Car *car = (Car *) malloc(S(Car));
        car->id = i; car->travelTime = CAt;  car->pathLen = CAp;
        car->path = (char *) malloc(strlen(input));
        strcpy(car->path,input);
        InitWriteOutput();
        pthread_create(&tid[i],NULL,carthread,car);
    }
}


void waitForReaping(){
    for(int i = 0; i < numbOfCars ; i++){
        pthread_join(tid[i],NULL);
    }
}

void *carthread(void *ptr){
    Car *car = (Car *) ptr;
    int index = 0;
    for(int i = 0; i < car -> pathLen  ; i++){ // [index] = Type
        char CurrentConnector = car->path[index], *numb;
        index++;
        int index2 = index, digitSize = 0, numbInd = 0;
        while(car->path[index2] != ' '){
            index2++; digitSize++;
        }
        numb = (char *) malloc(digitSize * S(char));
        while(car->path[index] != ' '){
            numb[numbInd++] = car->path[index++];
        }
        int CurrentConnectorId = atoi(numb);
        index++;
        int from = atoi(&car->path[index]);
        index+=2;
        int to = atoi(&car->path[index]);
        index+=2;
        WriteOutput(car->id,CurrentConnector,CurrentConnectorId,TRAVEL);
        sleep_milli(car->travelTime);
        pass(CurrentConnector,car->id,CurrentConnectorId, from,to);
    }
    return NULL;
}

void pass(char CurrentConnector, int cid ,int CurrentConnectorId,int from, int to){
    if(CurrentConnector == 'N'){
        NarrowBridges[CurrentConnectorId].enterBridge(cid,CurrentConnectorId,from,to);
        return;
    }
    else if(CurrentConnector == 'F'){
        Ferries[CurrentConnectorId].enterFerry(cid,CurrentConnectorId,from,to);
        return;
    }
    else if(CurrentConnector == 'C'){
        PassCR(cid,CurrentConnectorId,from , to);
        return;
    }
}

void PassCR(int cid, int crid , int f ,int t){
    switch(f){
        case 0:
            CrossRoads[crid].enterCR0(cid,crid,f,t); break;
        case 1:
            CrossRoads[crid].enterCR1(cid,crid,f,t); break;
        case 2:
             CrossRoads[crid].enterCR2(cid,crid,f,t); break;
        case 3:
             CrossRoads[crid].enterCR3(cid,crid,f,t); break;
    }
    return;
}


