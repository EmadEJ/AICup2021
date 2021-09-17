#include <bits/stdc++.h>

using namespace std;

const int INF=1e7+9,N=30;

// fixed game elements
int vision,bombDelay,maxBombRange,zoneStart,zoneDelay,maxStep;
// situation
int step;
bool enemySeen;
// list of tiles that are currently in our vision in format of {x,y}
vector <pair <int,int> > sight;

struct Map{
    int height,width;
    int stt[N][N],boom[N][N];
    bool insight[N][N];
    int dis[N*N][N*N],nxt[N*N][N*N];
    // initialization
    void init(){
        for(int i=0;i<N;i++) fill(stt[i],stt[i]+N,(1<<9));
        for(int i=0;i<N;i++) fill(boom[i],boom[i]+N,INF);
        for(int i=0;i<N*N;i++) fill(dis[i],dis[i]+N*N,INF);
    }
    // updating the state of a tile
    void update(int x,int y,int val){
        if((stt[x][y]>>4)%2==0 && (val>>4)%2==1){
            if(insight[x][y]) boom[x][y]=step+bombDelay;
            else boom[x][y]=-1;
        }
        if((val>>4)%2==0){
            boom[x][y]=INF;
        }
        insight[x][y]=1;
        stt[x][y]=val;
    }
    // checking the state
    bool iszone(int x,int y){
        return (stt[x][y])%2;
    }
    bool isfire(int x,int y){
        return (stt[x][y]>>1)%2;
    }
    bool isbox(int x,int y){
        return (stt[x][y]>>2)%2;
    }
    bool iswall(int x,int y){
        return (stt[x][y]>>3)%2;
    }
    bool isbomb(int x,int y){
        return (stt[x][y]>>4)%2;
    }
    bool isbombupgrade(int x,int y){
        return (stt[x][y]>>5)%2;
    }
    bool ishp(int x,int y){
        return (stt[x][y]>>6)%2;
    }
    bool istrap(int x,int y){
        return (stt[x][y]>>7)%2;
    }
    bool isplayer(int x,int y){
        return (stt[x][y]>>8)%2;
    }
    bool isdark(int x,int y){
        return (stt[x][y]>>9)%2;
    }
    // is currently in sight
    bool isvisible(int x,int y){
        return insight[x][y];
    }
    // is inside the boundries of the map
    bool isinside(int x,int y){
        return (x>=0 && x<height && y>=0 && y<width);
    }
    bool isfree(int ind){
        return !(isbox(ind/width,ind%width) || iswall(ind/width,ind%width));
    }
    // refreshes the distances and paths after the new information
    void clean(){
        for(int i=0;i<N*N;i++) fill(dis[i],dis[i]+N*N,INF);
        for(int i=0;i<height;i++){
            for(int j=0;j<width;j++){
                int ind=i*width+j;
                if(!isfree(ind) || isdark(ind/width,ind%width)) continue;
                dis[ind][ind]=0;
                nxt[ind][ind]=4;
                if(i>0 && isfree(ind-width)) dis[ind][ind-width]=1,nxt[ind][ind-width]=2;
                if(i<height-1 && isfree(ind+width)) dis[ind][ind+width]=1,nxt[ind][ind+width]=3;
                if(j>0 && isfree(ind-1)) dis[ind][ind-1]=1,nxt[ind][ind-1]=0;
                if(j<width-1 && isfree(ind+1)) dis[ind][ind+1]=1,nxt[ind][ind+1]=1;
            }
        }
        int surface=width*height;
        for(int i=0;i<surface;i++){
            for(int j=0;j<surface;j++){
                for(int k=0;k<surface;k++){
                    if(dis[i][k]+dis[k][j]<dis[i][j]){
                        dis[i][j]=dis[i][k]+dis[k][j];
                        nxt[i][j]=nxt[i][k];
                    }
                }
            }
        }
    }
    // the minimum distance between (x1,y1) and (x2,y2)
    int distance(int x1,int y1,int x2,int y2){
        int ind1=x1*width+y1,ind2=x2*width+y2;
        return dis[ind1][ind2];
    }
    // getting the first move from (x1,y1) to (x2,y2)
    int nxtmove(int x1,int y1,int x2,int y2){
        int ind1=x1*width+y1,ind2=x2*width+y2;
        return nxt[ind1][ind2];
    }
} mp;

struct Player{
    int x,y,hp,bombRange,trapCount,hpupcnt;
} me,enemy;

// wich step will the zone hit this tile
int safety(int x,int y){
    return zoneStart+min(min(x+1,mp.height-x),min(y+1,mp.width-y))*zoneDelay;
}

// handling when we meet enemy in the middle
int mantoman(){
    
}

// when we're side by side with enemy retruns the right move
int punching(){
    if(me.hp>=enemy.hp){
        if(me.x==enemy.x && me.y==enemy.y+1) return 6;
        if(me.x==enemy.x && me.y==enemy.y-1) return 7;
        if(me.y==enemy.y && me.x==enemy.x+1) return 8;
        if(me.y==enemy.y && me.x==enemy.x-1) return 9;
    }
    // if it's not a good decision to punch and we should escape it returns -1
    else{
        return -1;
    }
}

// checking if the next move is dangerous (will go into bomb) and if yes doing something safe
int bombcheck(int x, int y){
    // returning TRUE if tile can go boom! FALSE if safe
    bool  lelf_has_wall = false, right_has_wall = false, up_has_wall = false,down_has_wall = false;
    int bmbRange = maxBombRange; // can be changed by strategy
    for (int i=1; i <= bmbRange; i++){

        // down wing
        if(mp.isinside(x+i, y)){
        if(((mp.iswall(x+i,y) || mp.isbox(x+i,y)) && !down_has_wall) down_has_wall = true; //if there is a wall in this wings dont's check behind of it
        if((mp.isbomb(x+i,y) && !down_has_wall) return true;}
        
        // up wing
        if(mp.isinside(x-i, y)){
        if((mp.iswall(x-i,y) || mp.isbox(x-i,y)) && !down_has_wall) down_has_wall = true;
        if((mp.isbomb(x-i,y) && !down_has_wall) return true;}
        
        // right wing
        if(mp.isinside(x, y+i)){
        if((mp.iswall(x,y+i) || mp.isbox(x,y+i)) && !down_has_wall) down_has_wall = true;
        if(mp.isbomb(x,y+i) && !down_has_wall) return true;}
        
        // left wing
        if(mp.isinside(x, y-i)){
        if((mp.iswall(x,y-i) || mp.isbox(x,y-i)) && !down_has_wall) down_has_wall = true;
        if(mp.isbomb(x,y-i) && !down_has_wall) return true;}
    }
    return false;
    
}

// finding the best bomb to place to collect the most boxes
int mining(){
    
}

// checking whether we should move to centre and finding the best way to do so
int centralizing(){
    
}

// evaluating the phase we are in and what functions to use (this should be completed last)
int evaluate(){
    
}

int main(){
    string init;
    cin>>init>>mp.height>>mp.width>>me.x>>me.y>>me.hp>>me.bombRange>>me.trapCount>>vision>>bombDelay>>maxBombRange>>zoneStart>>zoneDelay>>maxStep;
    cout<<"init confirm"<<endl;
    mp.init();
    while(true){
        int step,last;
        cin>>step>>last>>me.x>>me.y>>me.hp>>me.hpupcnt>>me.bombRange>>me.trapCount;
        cin>>enemySeen;
        if(enemySeen) cin>>enemy.x>>enemy.y>>enemy.hp;
        int n;
        cin>>n;
        sight.clear();
        for(int i=0;i<N;i++) fill(mp.insight[i],mp.insight[i]+N,0);
        for(int i=0;i<n;i++){
            int x,y,val;
            cin>>x>>y>>val;
            sight.push_back({x,y});
            mp.update(x,y,val);
        }
        mp.clean();
        string eom;
        cin>>eom;
        cout<<evaluate()<<endl;
    }
    return 0;
}
