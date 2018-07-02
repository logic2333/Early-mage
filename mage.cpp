#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <deque>
using namespace std;

//方向 若两个方向之和为7，代表它们互为相反方向；有优先次序，右下方向是最优先考虑的
typedef enum { rightdown, Right, down, leftdown, rightup, up, Left, leftup } dirtype;
typedef set<dirtype> dirset;

//将所有方向放入一个集合
dirtype tempdirs[] = { rightdown, Right, down, leftdown, rightup, up, Left, leftup };
const dirset alldirs(tempdirs, tempdirs + 7);

//x是纵坐标，y是横坐标！
const int zx[8] = { 1, 0, 1, 1, -1, -1, 0, -1 };
const int zy[8] = { 1, 1, 0, -1, 1, 0, -1, -1 };

//为了测试走进死胡同（3，5）后能够回溯继续探索路径，此迷宫与题目所给迷宫略有不同
//将（6，6）改为1可测试无解迷宫
const bool mg[8][8] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 1, 1, 0, 1,
	1, 0, 1, 1, 0, 1, 1, 1,
	1, 1, 1, 0, 1, 0, 1, 1,
	1, 1, 0, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 1, 0, 1, 1,
	1, 1, 0, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};

//坐标类，因为所有属性和方法全是public所以用struct代替
struct coord {
	unsigned x, y;
	coord() {}
	coord(unsigned X, unsigned Y) {
		x = X; y = Y;
	}
	bool operator<(const coord& another) const {		//此比较器仅用于使map正常工作，无实际意义
		if (x < another.x) return true;
		else if (x == another.x) {
			if (y < another.y) return true;
		}
		return false;
	}
};

map<coord, dirset> all_banned_dirs;		//每个位置，和它所尝试过的，走不通的方向存在一一对应关系，用map存放

class posittype {
	friend void walk();
private:
	coord c;
	dirset getavaildirs() {
		//已知当前位置和从当前位置已经走过的不能走的方向（banneddirs），观察周围8个位置，给出所有可以走的方向
		dirset res = alldirs;
		for (auto i : alldirs) {
			if (mg[c.x + zx[i]][c.y + zy[i]] == 1) res.erase(i);						//排除撞墙的方向
			else if (all_banned_dirs[c].find(i) != all_banned_dirs[c].end()) res.erase(i);  //排除已经尝试过的，走不通的方向
		}
		return res;
	}
public:
	posittype() {}
	posittype(unsigned X, unsigned Y) {
		c = coord(X, Y);
	}
	void print();
	bool operator==(const posittype& another) {
		if (c.x == another.c.x && c.y == another.c.y) return true;
		return false;
	}
};

const posittype depart(1, 1);
const posittype dest(6, 6);
deque<posittype> route;			//queue、stack均无法遍历，不方便输出，因此使用deque

void posittype::print() {
	if (c.x == 1 && c.y == 1) cout << "入口 -> ";
	else if (*this == dest) cout << "出口" << endl;
	else printf("(%d, %d) -> ", c.y, c.x);
}

void walk() {
	posittype rn = depart;		//迷宫入口
	route.push_back(rn);
	dirtype nowdir;
	while (!(rn == dest)) {		//能够通过循环实现的，最好不要递归
		dirset availdirs = rn.getavaildirs();
		if (availdirs.empty()) {//走入了死胡同
			if (route.back() == depart) return;		//如果发现全是死胡同最后会回退到起点，此时如果发现起点所有的方向都无法走通，表明这个迷宫无解
			route.pop_back();	//回退一步
			rn = route.back();
			all_banned_dirs[rn.c].insert(nowdir);	//将刚刚走过的方向加入此位置的不可走方向，准备从这个位置再次试探
		}
		else {										//此位置有路可走
			posittype newnode = rn;					//以此位置的数据结构为基础，准备下一个位置的数据结构
			nowdir = *availdirs.begin();			//选取最优先的方向走
			newnode.c.x += zx[nowdir];				//坐标移动
			newnode.c.y += zy[nowdir];
			all_banned_dirs[rn.c].insert(nowdir);	//不走重复路，将准备走的方向（nowdir）加入本位置的禁止方向
			all_banned_dirs[newnode.c].insert(dirtype(7 - nowdir)); //不走回头路，将准备走的方向（nowdir）的相反方向加入下一个位置的禁止方向
			route.push_back(newnode);				//将新节点放入路径
			rn = newnode;							//准备从新位置再次试探
		}
	}
}

int main()
{
	walk();
	if (route.back() == depart) cout << "走不通" << endl;
	else for (auto i : route) i.print();
	return 0;
}
