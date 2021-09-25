#include <bits/stdc++.h>

#define F first
#define S second
#define pii pair <int,int>

using namespace std;

const int INF=1e7+9,N=30;

// fixed game elements
int vision,bombDelay,maxBombRange,zoneStart,zoneDelay,maxStep;
// situationcentralize()
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
    bool isfree(int x,int y){
        return !(isbox(x,y) || iswall(x,y) ||  isenemy(x,y));
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
                if((boom[x+i][y]==-1 || boom[x+i][y]==step || boom[x+i][y]==step+1) && !down_has_wall) return true;
            }
            // up wing
            if(isinside(x-i, y)){
                if((iswall(x-i,y) || isbox(x-i,y)) && !up_has_wall) up_has_wall = true;
                if((boom[x-i][y]==-1 || boom[x-i][y]==step || boom[x-i][y]==step+1) && !up_has_wall) return true;
            }
            // right wing
            if(isinside(x, y+i)){
                if((iswall(x,y+i) || isbox(x,y+i)) && !right_has_wall) right_has_wall = true;
                if((boom[x][y+i]==-1 || boom[x][y+i]==step || boom[x][y+i]==step+1) && !right_has_wall) return true;
            }
            // left wing
            if(isinside(x, y-i)){
                if((iswall(x,y-i) || isbox(x,y-i)) && !left_has_wall) left_has_wall = true;
                if((boom[x][y-i]==-1 || boom[x][y-i]==step || boom[x][y-i]==step+1) && !left_has_wall) return true;
            }
        }
        return false;
    }
    // refreshes the distances and paths after the new information (Using the Floyd-Warshal algorithm)
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
    pair <int,int> center(){
		return {height/2 , width/2};
	}
} mp;

// wich step will the zone hit this tile
int safety(int x,int y){
    return zoneStart+min(min(x+1,mp.height-x)-1,min(y+1,mp.width-y)-1)*zoneDelay;
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

// the (x,y) vector direction towards center
pair <int,int> centerdir(){
	pii center = mp.center();
	int xdir, ydir;
	if(me.x < center.F){
	    ydir = 3;
	} else {
		ydir = 2;
	}
	if (me.y < center.S){
		xdir = 1;
	} else{
		xdir = 0;
	}
	return {xdir,ydir};
}

// the (x,y) vector direction away from center
pair <int,int> centerdirnegative(){
	pii center = mp.center();
	int xdir, ydir;
	if(me.x < center.F){
	    ydir = 2;
	} else {
		ydir = 3;
	}
	if (me.y < center.S){
		xdir = 0;
	} else{
		xdir = 1;
	}
	return {xdir,ydir};
}

int dirlen = 2;
int dirstep = 0;
int dir = -1;

//another version of explore. this one uses a weighted random selection to pick a direction to follow for the next couple of moves (dirlen)
int explore1(){
	//random initialization
    srand(unsigned(time(0)));

	pii best;
	int bestdis = INF;
    pii cdir = centerdir();
    pii cdirneg = centerdirnegative();

	if (dir == -1 || !(dirstep > 0)) {
		vector<int> rnd;
		int x = me.x;
		int y = me.y;
		for(int i = 0; i < mp.height + mp.width ; i ++){
			if(i < min(x,mp.height - x) * 2) {
			rnd.push_back(cdirneg.F);
			} else if (i < mp.height){
			rnd.push_back(cdir.F);
			} else if(i < min(y,mp.width - y) * 2 + mp.height){
			rnd.push_back(cdirneg.S);
			} else {
			rnd.push_back(cdirneg.S);
			}
		}

		random_shuffle(rnd.begin(), rnd.end());
		dir = rnd[0];
		dirstep = dirlen;
	}

	for (pii tile:sight){
		int x = tile.F;
		int y = tile.S;
		int dis = mp.distance(me.x,me.y,x,y);

		if(dir == 0) {
			if(y >= me.y) continue;
			if(dis < bestdis) best = tile;
		} else if (dir == 1){
			if(y <= me.y) continue;
			if(dis < bestdis) best = tile;
		} else if (dir == 2){
			if(x >= me.x) continue;
			if(dis < bestdis) best = tile;
		} else if (dir == 3) {
			if(x >= me.x) continue;
			if(dis < bestdis) best = tile;
		} else {
			return -1;
		}
	}

	dirstep --;
	return mp.nextmove(me.x,me.y,best.F,best.S);
}

//yet another version of explore. in this one, the character checks to see if there is any other box remaining in the map that we know of. (which there probably is, since the map is mirrored around a point)
int explore2(){
    int dis=INF;
    pii best={-1,-1};
    for(int i=0;i<mp.height;i++){
        for(int j=0;j<mp.width;j++){
            if(mp.isbox(i,j) && mp.distance(me.x,me.y,i,j)<dis){
                best={i,j};
                dis=mp.distance(me.x,me.y,i,j);
            }
        }
    }
    if (best == make_pair (-1,-1)) return -1;
    return mp.nextmove(me.x,me.y,best.F,best.S);
}

// exploring the map when you see no box to destroy
int explore(){
    int dis=INF;
    pii best={-1,-1};
    for(int i=0;i<mp.height;i++){
        for(int j=0;j<mp.width;j++){
            if(mp.isdark(i,j) && mp.distance(me.x,me.y,i,j)<dis){
                best={i,j};
                dis=mp.distance(me.x,me.y,i,j);
            }
        }
    }
    if(best.F==-1) return -1;
    return mp.nextmove(me.x,me.y,best.F,best.S);
}

// when we're side by side with enemy returns the right move (part of mantoman process)
int knife(){
    if(me.trapCount>0){
        if(me.x==enemy.x && me.y==enemy.y+1) return 6;
        if(me.x==enemy.x && me.y==enemy.y-1) return 7;
        if(me.y==enemy.y && me.x==enemy.x+1) return 8;
        if(me.y==enemy.y && me.x==enemy.x-1) return 9;
    }
    // if it's not a good decision to punch and we should escape it returns -1
    return -1;
}

// handling when we meet enemy in the middle
int mantoman(){
    if(!enemySeen) return -1;
    if(safety(me.x,me.y)<safety(enemy.x,enemy.y)) return -1;
    if(knife()!=-1) return knife();
    if(((me.x==enemy.x-1)&&(me.y==enemy.y-1)) || ((me.x==enemy.x+1)&&(me.y==enemy.y-1)) || ((me.x==enemy.x-1)&&(me.y==enemy.y+1)) || ((me.x==enemy.x+1)&&(me.y==enemy.y+1))){
        return 4;
    }
    if(((me.x==enemy.x)&&(me.y==enemy.y-2)) || ((me.x==enemy.x)&&(me.y==enemy.y+2)) || ((me.x==enemy.x-2)&&(me.y==enemy.y)) || ((me.x==enemy.x+2)&&(me.y==enemy.y))){
        return 4;
    }
    if(((me.x==enemy.x+3)&&(me.y==enemy.y)) || ((me.x==enemy.x+2)&&(me.y==enemy.y-1)) || ((me.x==enemy.x+2)&&(me.y==enemy.y+1))){
        if(!mp.isfree(me.x-1,me.y)) return -1;
        return 2;
    }
    if(((me.x==enemy.x-3)&&(me.y==enemy.y)) || ((me.x==enemy.x-2)&&(me.y==enemy.y-1)) || ((me.x==enemy.x-2)&&(me.y==enemy.y+1))){
        if(!mp.isfree(me.x+1,me.y)) return -1;
        return 3;
    }
    if(((me.x==enemy.x)&&(me.y==enemy.y+3)) || ((me.x==enemy.x-1)&&(me.y==enemy.y+2)) || ((me.x==enemy.x+1)&&(me.y==enemy.y+2))){
        if(!mp.isfree(me.x,me.y-1)) return -1;
        return 0;
    }
    if(((me.x==enemy.x)&&(me.y==enemy.y-3)) || ((me.x==enemy.x-1)&&(me.y==enemy.y-2)) || ((me.x==enemy.x+1)&&(me.y==enemy.y-2))){
        if(!mp.isfree(me.x,me.y+1)) return -1;
        return 1;
    }
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
        if(chosen==make_pair(-1,-1)){
            if(explore2()!=-1) return explore2();
            else if(explore()!=-1) return explore();
            else return centralize();
        } 
    } 
    if(me.x==chosen.F && me.y==chosen.S){
        return 5;
    }
    return mp.nextmove(me.x,me.y,chosen.F,chosen.S);
}

bool U,D,R,L;
// evaluating the phase we are in and what functions to use (this should be completed last)
int evaluate(){
    if(enemySeen && knife()!=-1) return knife();
    // to place bomb at the end of the game
    if(centralize()==4 && mp.iszone(me.x,me.y) && me.hp<3) return 5;
    // to place traps if left at the end
    if(me.trapCount>0 && centralize()==4 && step>safety(me.x,me.y)-zoneDelay){
        if(!U && mp.isfree(me.x-1,me.y)){
            U=1;
            return 8;
        }
        if(!D && mp.isfree(me.x+1,me.y)){
            D=1;
            return 9;
        }
        if(!L && mp.isfree(me.x,me.y-1)){
            L=1;
            return 6;
        }
        if(!R && mp.isfree(me.x,me.y+1)){
            R=1;
            return 7;
        }
    }
    int res;
    if(step>zoneStart-20 && enemySeen && me.trapCount>0 && mp.distance(me.x,me.y,enemy.x,enemy.y)<5 && mantoman()!=-1) res=mantoman();
    if(step<zoneStart-max(mp.height,mp.width)) res=mine();
    else res=centralize();
    int newx=me.x,newy=me.y;
    if(res==0) newy--;
    if(res==1) newy++;
    if(res==2) newx--;
    if(res==3) newx++;
    if(mp.bombcheck(newx,newy)) res=escape();
    return res;
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
        else enemy.x=-1,enemy.y=-1,enemy.hp=-1;
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
