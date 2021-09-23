#include <bits/stdc++.h>

#define F first
#define S second
#define pii pair <int,int>

using namespace std;

const int INF=1e7+9,N=30;

// fixed game elements
int vision,bombDelay,maxBombRange,zoneStart,zoneDelay,maxStep;
// situation
int step,lastmove=4;
bool enemySeen;
// list of tiles that are currently in our vision in format of {x,y}
vector <pair <int,int> > sight;

struct Player{
    int x,y,hp,bombRange,trapCount,hpupcnt;
} me,enemy;

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
        if(stt[height-x-1][width-y-1]&(1<<9)) stt[height-x-1][width-y-1]=val&13;
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
    bool isenemy(int x,int y){
        return (enemy.x==x && enemy.y==y);
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
        return !(isbox(ind/width,ind%width) || iswall(ind/width,ind%width) ||  isenemy(ind/width,ind%width));
    }
    // is the tile in range of a bomb (regardless of the time)
    bool issafe(int x,int y){
        // returning TRUE if tile can go boom! FALSE if safe
        if(isbomb(x,y)==1) return 0;
        bool  left_has_wall = false, right_has_wall = false, up_has_wall = false,down_has_wall = false;
        int bmbRange = me.bombRange; // can be changed by strategy
        for (int i=1; i <= bmbRange; i++){
            // down wing
            if(isinside(x+i, y)){
                if((iswall(x+i,y) || isbox(x+i,y)) && !down_has_wall) down_has_wall = true; //if there is a wall in this wings dont's check behind of it
                if(isbomb(x+i,y) && !down_has_wall) return 0;
            }
            // up wing
            if(isinside(x-i, y)){
                if((iswall(x-i,y) || isbox(x-i,y)) && !up_has_wall) up_has_wall = true;
                if(isbomb(x-i,y) && !up_has_wall) return 0;
            }
            // right wing
            if(isinside(x, y+i)){
                if((iswall(x,y+i) || isbox(x,y+i)) && !right_has_wall) right_has_wall = true;
                if(isbomb(x,y+i) && !right_has_wall) return 0;
            }
            // left wing
            if(isinside(x, y-i)){
                if((iswall(x,y-i) || isbox(x,y-i)) && !left_has_wall) left_has_wall = true;
                if(isbomb(x,y-i) && !left_has_wall) return 0;
            }
        }
        return 1;
    }
    // if the tile will explode next step
    bool bombcheck(int x,int y){
        // returning TRUE if tile can go boom! FALSE if safe
        if(boom[x][y]==-1 || boom[x][y]==step) return true;
        bool  left_has_wall = false, right_has_wall = false, up_has_wall = false,down_has_wall = false;
        int bmbRange = me.bombRange; // can be changed by strategy
        for (int i=1; i <= bmbRange; i++){
            // down wing
            if(isinside(x+i, y)){
                if((iswall(x+i,y) || isbox(x+i,y)) && !down_has_wall) down_has_wall = true; //if there is a wall in this wings dont's check behind of it
                if((boom[x+i][y]==-1 || boom[x+i][y]==step) && !down_has_wall) return true;
            }
            // up wing
            if(isinside(x-i, y)){
                if((iswall(x-i,y) || isbox(x-i,y)) && !up_has_wall) up_has_wall = true;
                if((boom[x-i][y]==-1 || boom[x-i][y]==step) && !up_has_wall) return true;
            }
            // right wing
            if(isinside(x, y+i)){
                if((iswall(x,y+i) || isbox(x,y+i)) && !right_has_wall) right_has_wall = true;
                if((boom[x][y+i]==-1 || boom[x][y+i]==step) && !right_has_wall) return true;
            }
            // left wing
            if(isinside(x, y-i)){
                if((iswall(x,y-i) || isbox(x,y-i)) && !left_has_wall) left_has_wall = true;
                if((boom[x][y-i]==-1 || boom[x][y-i]==step) && !left_has_wall) return true;
            }
        }
        return false;
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
        for(int k=0;k<surface;k++){
            for(int j=0;j<surface;j++){
                for(int i=0;i<surface;i++){
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
    int nextmove(int x1,int y1,int x2,int y2){
        int ind1=x1*width+y1,ind2=x2*width+y2;
        return nxt[ind1][ind2];
    }
} mp;

// wich step will the zone hit this tile
int safety(int x,int y){
    return zoneStart+min(min(x+1,mp.height-x),min(y+1,mp.width-y))*zoneDelay;
}

// checking whether we should move to centre and finding the best way to do so
int centralize(){
    int dis=INF,safe=0;
    pair <int,int> best;
    for(int i=0;i<mp.height;i++){
        for(int j=0;j<mp.width;j++){
            if(!mp.isdark(i,j)){
                if((safety(i,j)>safe && mp.distance(me.x,me.y,i,j)!=INF) || (safety(i,j)==safe && mp.distance(me.x,me.y,i,j)<dis)){
                    dis=mp.distance(me.x,me.y,i,j);
                    best={i,j};
                    safe=safety(i,j);
                }
            }
        }
    }
    return mp.nextmove(me.x,me.y,best.first,best.second);
}

// exploring the map when you see no box to destroy
int explore(){
    
}

// handling when we meet enemy in the middle
int mantoman(){
    if(!enemySeen) return -1;
    if(safety(me.x,me.y)<safety(enemy.x,enemy.y)) return -1;
    if(((me.x^me.y)&1)==((enemy.x^enemy.y)&1)) return 4;
}

// escaping from enemy in dire situations
int runaway(){
    
}

// when we're side by side with enemy returns the right move
int knife(){
    if(me.hp>=enemy.hp && me.trapCount>0){
        if(me.x==enemy.x && me.y==enemy.y+1) return 6;
        if(me.x==enemy.x && me.y==enemy.y-1) return 7;
        if(me.y==enemy.y && me.x==enemy.x+1) return 8;
        if(me.y==enemy.y && me.x==enemy.x-1) return 9;
    }
    // if it's not a good decision to punch and we should escape it returns -1
    return -1;
}

// finding the best bomb to place to collect the most boxes (part of mining process)
pair <int,int> bestbomb(){
    int num=1,len=INF;
    pair <int,int> best={-1,-1};
    for(pair <int,int> tile:sight){
        int x=tile.first,y=tile.second;
        if(mp.iswall(x,y) || mp.isbox(x,y)) continue ;
        int broken=0,dis=mp.distance(me.x,me.y,x,y);
        if(dis==INF) continue;
        for(int i=x+1;i<=min(x+me.bombRange,mp.height-1);i++){
            if(mp.isbox(i,y)){
                broken++;
                break;
            }
            if(mp.iswall(i,y)) break;
        }
        for(int i=x-1;i>=max(x-me.bombRange,0);i--){
            if(mp.isbox(i,y)){
                broken++;
                break;
            }
            if(mp.iswall(i,y)) break;
        }
        for(int i=y+1;i<=min(y+me.bombRange,mp.width-1);i++){
            if(mp.isbox(x,i)){
                broken++;
                break;
            }
            if(mp.iswall(x,i)) break;
        }
        for(int i=y-1;i>=max(y-me.bombRange,0);i--){
            if(mp.isbox(x,i)){
                broken++;
                break;
            }
            if(mp.iswall(x,i)) break;
        }
        if(make_pair(broken,-dis)>make_pair(num,-len)){
            best=tile;
            num=broken;
            len=dis;
        }
    }
    return best;
}

// escaping from the bomb we just placed (part of the mining process)
int escape(){
    int dis=INF;
    pii best;
    for(int i=0;i<mp.height;i++){
        for(int j=0;j<mp.width;j++){
            if(!mp.isdark(i,j) && mp.issafe(i,j) && mp.distance(me.x,me.y,i,j)<dis){
                dis=mp.distance(me.x,me.y,i,j);
                best={i,j};
            }
        }
    }
    return mp.nextmove(me.x,me.y,best.F,best.S);
}

// gathering the newly exploded boxes (part of the mining process)
int collect(){
    int dis=INF;
    pii best;
    for(pii tile:sight){
        int  x=tile.F,y=tile.S;
        if((mp.ishp(x,y) || mp.istrap(x,y) || mp.isbombupgrade(x,y)) && mp.distance(me.x,me.y,x,y)<dis){
            dis=mp.distance(me.x,me.y,x,y);
            best={x,y};
        }
    }
    return mp.nextmove(me.x,me.y,best.F,best.S);
}

// handling the mining process
pair <int,int> chosen={-1,-1}; // the currnt target of placing the bomb
int mine(){
    if(chosen!=make_pair(-1,-1) && mp.isfire(chosen.F,chosen.S)){
        chosen={-1,-1};
    }
    if(chosen!=make_pair(-1,-1) && mp.isbomb(chosen.F,chosen.S) && mp.issafe(me.x,me.y)){
        return 4;
    }
    for(pii tile:sight){
        if(mp.ishp(tile.F,tile.S) || mp.istrap(tile.F,tile.S) || mp.isbombupgrade(tile.F,tile.S)){
            chosen={-1,-1};
            return collect();
        } 
    }
    if(!mp.issafe(me.x,me.y)){
        return escape();
    } 
    if(chosen==make_pair(-1,-1)){
        chosen=bestbomb();
        if(chosen==make_pair(-1,-1)) return centralize();
    } 
    if(me.x==chosen.F && me.y==chosen.S){
        return 5;
    }
    return mp.nextmove(me.x,me.y,chosen.F,chosen.S);
}

// evaluating the phase we are in and what functions to use (this should be completed last)
int evaluate(){
    if(enemySeen && knife()!=-1) return knife();
    if(step<zoneStart) return mine();
    else return centralize();
}

int main(){
    string init;
    cin>>init>>mp.height>>mp.width>>me.x>>me.y>>me.hp>>me.bombRange>>me.trapCount>>vision>>bombDelay>>maxBombRange>>zoneStart>>zoneDelay>>maxStep;
    cout<<"init confirm"<<endl;
    mp.init();
    while(true){
        cin>>step>>lastmove>>me.x>>me.y>>me.hp>>me.hpupcnt>>me.bombRange>>me.trapCount;
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
        int res=evaluate();
        cout<<res<<endl;
    }
    return 0;
}
