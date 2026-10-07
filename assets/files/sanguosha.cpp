#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for system("cls") or system("clear")
#include <ctime>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <functional>

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

using namespace std;

//卡牌类
struct card{
	string name;
	string suit;
	string point;
}; 
//装备区
struct eqpmt{
	vector<card> arm;
	vector<card> armor;
	vector<card> horsePlus;
	vector<card> horseMinus;
}; 
//附加buff(判定+铁索)
 struct buff{
 	vector<card> lightning;
	vector<card> happy;
 };
//玩家类 
struct Player{
    string name;
    int hp = 4;
    int hpLimit=4;
    vector<card> hand;
    bool alive = true;
    //杀限制
    bool usedSha = false;
    //判定区 
    buff bf;
    bool lightning=false;
	bool happy=false;
	//装备区 
	eqpmt eq;
	bool bargraph=false;
	bool plus1=false;
	bool minus1=false;
	bool cixiong=false;
	bool hanbin=false;
	bool renwang=false;
	bool liannu=false;
	bool shemao=false;
	bool qilin=false;
	bool fangtian=false;
	bool qinlong=false;
};
//关键变量 
vector<Player> players;
int currentIndex;
int cardIndex;
//随机数生成
int random(int a,int b){
	random_device rd;
    mt19937 gen(rd());  // 梅森旋转引擎，常用于高质量随机数
    uniform_int_distribution<> dis(a, b);  // 生成 a 到 b 的整数
    return dis(gen); 
} 
//牌堆
vector<card> fullDeck; // 全局牌堆
vector<card> drawPile; // 当前摸牌堆
//洗牌
void washDrawPile(){
	random_device rd;
    mt19937 g(rd());
    shuffle(drawPile.begin(), drawPile.end(), g); 
}
void initDeck() {
    fullDeck.clear();
    drawPile.clear();

    struct Entry { string suit, point, name; } entries[] = {
        {"Spade", "A", "闪电"}, {"Spade", "A", "决斗"},
        {"Club", "A", "诸葛连弩"}, {"Club", "A", "决斗"},
        {"Spade", "2", "八卦阵"}, {"Spade", "2", "雌雄双股剑"}, {"Spade", "2", "寒冰剑"},
        {"Club", "2", "八卦阵"}, {"Club", "2", "杀"}, {"Club", "2", "仁王盾"},
        {"Spade", "3", "过河拆桥"}, {"Spade", "3", "顺手牵羊"},
        {"Club", "3", "过河拆桥"}, {"Club", "3", "杀"},
        {"Spade", "4", "过河拆桥"}, {"Spade", "4", "顺手牵羊"},
        {"Club", "4", "过河拆桥"}, {"Club", "4", "杀"},
        {"Spade", "5", "青龙偃月刀"}, {"Spade", "5", "绝影"},
        {"Club", "5", "的卢"}, {"Club", "5", "杀"},
        {"Spade", "6", "乐不思蜀"}, {"Spade", "6", "青釭剑"},
        {"Club", "6", "乐不思蜀"}, {"Club", "6", "杀"},
        {"Spade", "7", "南蛮入侵"}, {"Spade", "7", "杀"},
        {"Club", "7", "南蛮入侵"}, {"Club", "7", "杀"},
        {"Spade", "8", "杀"}, {"Spade", "8", "杀"},
        {"Club", "8", "杀"}, {"Club", "8", "杀"},
        {"Spade", "9", "杀"}, {"Spade", "9", "杀"},
        {"Club", "9", "杀"}, {"Club", "9", "杀"},
        {"Spade", "10", "杀"}, {"Spade", "10", "杀"},
        {"Club", "10", "杀"}, {"Club", "10", "杀"},
        {"Spade", "J", "无懈可击"}, {"Spade", "J", "顺手牵羊"},
        {"Club", "J", "杀"}, {"Club", "J", "杀"},
        {"Spade", "Q", "丈八蛇矛"}, {"Spade", "Q", "过河拆桥"},
        {"Club", "Q", "借刀杀人"}, {"Club", "Q", "无懈可击"},
        {"Spade", "K", "大宛"}, {"Spade", "K", "南蛮入侵"},
        {"Club", "K", "借刀杀人"}, {"Club", "K", "无懈可击"},
        {"Heart", "A", "桃园结义"}, {"Heart", "A", "万箭齐发"},
        {"Diamond", "A", "诸葛连弩"}, {"Diamond", "A", "决斗"},
        {"Heart", "2", "闪"}, {"Diamond", "2", "闪"},
        {"Heart", "3", "桃"}, {"Diamond", "3", "闪"},
        {"Heart", "4", "桃"}, {"Diamond", "4", "闪"},
        {"Heart", "5", "麒麟弓"}, {"Diamond", "5", "闪"},
        {"Heart", "6", "桃"}, {"Diamond", "6", "闪"},
        {"Heart", "7", "桃"}, {"Diamond", "7", "闪"},
        {"Heart", "8", "桃"}, {"Diamond", "8", "闪"},
        {"Heart", "9", "桃"}, {"Diamond", "9", "闪"},
        {"Heart", "10", "杀"}, {"Diamond", "10", "闪"},
        {"Heart", "J", "杀"}, {"Diamond", "J", "闪"},
        {"Heart", "Q", "桃"}, {"Heart", "Q", "过河拆桥"}, {"Heart", "Q", "闪电"},
        {"Diamond", "Q", "桃"}, {"Diamond", "Q", "方天画戟"}, {"Diamond", "Q", "无懈可击"},
        {"Heart", "K", "爪黄飞电"}, {"Heart", "K", "闪"},
        {"Diamond", "K", "杀"}, {"Diamond", "K", "紫骍"},
    };

    for (auto& e : entries) {
        fullDeck.push_back({e.name, e.suit, e.point});
    }
    drawPile = fullDeck;
    washDrawPile();
}

//全员状态显示 
void printStatus() {
    cout << "\n【当前状态】" << endl;
    for (const auto& p : players) {
        cout << p.name << " HP: " << p.hp << (p.alive ? " (存活)" : " (阵亡)") << " | 手牌数: " << p.hand.size() <<" | 装备区：";
        if(p.eq.arm.size()>0) cout<<p.eq.arm[0].name<<" ";
    	if(p.eq.armor.size()>0) cout<<p.eq.armor[0].name<<" ";
		if(p.eq.horsePlus.size()>0) cout<<p.eq.horsePlus[0].name<<" ";
		if(p.eq.horseMinus.size()>0) cout<<p.eq.horseMinus[0].name<<" ";
		cout<<" | 状态：";
		if(p.lightning) cout<<"闪电"<<" ";
		if(p.happy) cout<<"乐不思蜀"<<" ";
		cout<<endl;
		}
		
}
//回车清屏前的提示语 
void pause_and_clear(string message = "请将电脑交给对方，按回车继续...") {
    cout << message << endl;
    cin.ignore(); cin.get();
    system(CLEAR_COMMAND);
    printStatus();
    cout<<"\n---------------------------------------\n";
}
//摸牌
card drawCard() {
    card c = drawPile.back();
    drawPile.pop_back();
    return c;
}
//装备牌效果去除 
void liannuoff(Player p){
	p.liannu=false;
}
// 清除装备状态(牌名 → 功能 的映射表)
unordered_map<std::string, std::function<void(Player&)>> turnoff = {
        {"诸葛连弩",liannuoff},
};
//弃牌并放回牌堆洗乱 
//手牌 
void throwCard(Player& p, int idx){
	card c = p.hand[idx];
	drawPile.push_back(c);
	washDrawPile();
	p.hand.erase(p.hand.begin() + idx);
} 
//武器 
void throwCard1(Player& p){
	string name=p.eq.arm[0].name;
	if (turnoff.count(name)) {
        turnoff[name](p);  // 执行牌的效果
	}
	else cout<<"装备效果无法找到，因此无法脱落\n";
	card c = p.eq.arm[0];
	drawPile.push_back(c);
	washDrawPile();
	p.eq.arm.erase(p.eq.arm.begin() + 0);
}
//防具 
void throwCard2(Player& p){
	string name=p.eq.armor[0].name;
	if (turnoff.count(name)) {
        turnoff[name](p);  // 执行牌的效果
	}
	else cout<<"装备效果无法找到，因此无法脱落\n";
	card c = p.eq.armor[0];
	drawPile.push_back(c);
	washDrawPile();
	p.eq.armor.erase(p.eq.armor.begin() + 0);
} 
//加一马 
void throwCard3(Player& p){
	string name=p.eq.horsePlus[0].name;
	if (turnoff.count(name)) {
        turnoff[name](p);  // 执行牌的效果
	}
	else cout<<"装备效果无法找到，因此无法脱落\n";
	card c = p.eq.horsePlus[0];
	drawPile.push_back(c);
	washDrawPile();
	p.eq.horsePlus.erase(p.eq.horsePlus.begin() + 0);
} 
//减一马 
void throwCard4(Player& p){
	string name=p.eq.horseMinus[0].name;
	if (turnoff.count(name)) {
        turnoff[name](p);  // 执行牌的效果
	}
	else cout<<"装备效果无法找到，因此无法脱落\n";
	card c = p.eq.horseMinus[0];
	drawPile.push_back(c);
	washDrawPile();
	p.eq.horseMinus.erase(p.eq.horseMinus.begin() + 0);
} 
//buff
void throwCard5(Player& p,string name){
	if(name=="sd"){
		card c = p.bf.lightning[0];
		drawPile.push_back(c);
		washDrawPile();
		p.bf.lightning.erase(p.bf.lightning.begin() + 0);
	}
	if(name=="le"){
		card c = p.bf.happy[0];
		drawPile.push_back(c);
		washDrawPile();
		p.bf.happy.erase(p.bf.happy.begin() + 0);
	}
} 
//顺走牌(1:手牌，2：武器，3:防具，4：加一马，5：减一马，6：雷电，7：乐不思蜀)
card drawCardfromsb(Player& p,int type){
	card c;
	int n=random(0,p.hand.size()-1);
	switch (type) {
		case 1:
			c=p.hand[n];
			p.hand.erase(p.hand.begin()+n);
			break;
		case 2:
			c=p.eq.arm[0];
			if (turnoff.count(c.name)) {
		        turnoff[c.name](p);  // 执行牌的效果
			}
			else cout<<"装备效果无法找到，因此无法去除效果\n";
			p.eq.arm.pop_back();
			break;
		case 3:
			c=p.eq.armor[0];
			if (turnoff.count(c.name)) {
		        turnoff[c.name](p);  // 执行牌的效果
			}
			else cout<<"装备效果无法找到，因此无法去除效果\n";
			p.eq.armor.pop_back();
			break;
		case 4:
			c=p.eq.horsePlus[0];
			if (turnoff.count(c.name)) {
		        turnoff[c.name](p);  // 执行牌的效果
			}
			else cout<<"装备效果无法找到，因此无法去除效果\n";
			p.eq.horsePlus.pop_back();
			break;
		case 5:
			c=p.eq.horseMinus[0];
			if (turnoff.count(c.name)) {
		        turnoff[c.name](p);  // 执行牌的效果
			}
			else cout<<"装备效果无法找到，因此无法去除效果\n";
			p.eq.horseMinus.pop_back();
			break;
		case 6:
			c=p.bf.lightning[0];
			p.bf.lightning.pop_back();
			break;
		case 7:
			c=p.bf.happy[0];
			p.bf.happy.pop_back();
			break;
	}
	return c;
} 
//展示p的手牌 
void printHand(const Player &p) {
    cout << p.name << " 的手牌: \n";
    for(int i = 0; i < p.hand.size(); ++i)
        cout << "[" << i+1 << "]" << p.hand[i].name << "(" << p.hand[i].suit << " " << p.hand[i].point << ")\n";
    cout << endl;
}
 //是否有该牌 
bool hasCard(const Player &p, const string &card) {
	for (int i = 0; i < p.hand.size(); ++i)
		if (p.hand[i].name == card) return true;
	return false;
}
//定位牌
int findCard(const Player &p, const string &card) {
	for (int i = 0; i < p.hand.size(); ++i) 
		if (p.hand[i].name == card) return i;
	return -1;
}
//挑选对象
int select(){
	for (int i = 0; i < players.size(); ++i) {
	    if (i != currentIndex && players[i].alive)
	    	cout << i+1 << ": " << players[i].name << "\n";
    }
    int targetIndex;
    cin >> targetIndex;
    targetIndex--;
    while (targetIndex < 0 || targetIndex >= players.size() || !players[targetIndex].alive || targetIndex == currentIndex) {
        cout << "无效目标\n";
        cin >> targetIndex;
        targetIndex--;
	}
	return targetIndex;
}
//控制选择必须为(y/n)
string cindcs(){
	string dcs;
	cin >> dcs;
    while(dcs != "y" && dcs != "n"){
		cout<<"请回答(y/n):";
		cin>>dcs;
	}
	return dcs;
} 
//装备装备
//武器 
void loadUp1(){
	if(players[currentIndex].eq.arm.size()>0) throwCard1(players[currentIndex]);
	players[currentIndex].eq.arm.push_back(players[currentIndex].hand[cardIndex]);
	players[currentIndex].hand.erase(players[currentIndex].hand.begin()+cardIndex);
}
//防具 
void loadUp2(){
	if(players[currentIndex].eq.armor.size()>0) throwCard2(players[currentIndex]);
	players[currentIndex].eq.armor.push_back(players[currentIndex].hand[cardIndex]);
	players[currentIndex].hand.erase(players[currentIndex].hand.begin()+cardIndex);
}
//加一马 
void loadUp3(){
	if(players[currentIndex].eq.horsePlus.size()>0) throwCard3(players[currentIndex]);
	players[currentIndex].eq.horsePlus.push_back(players[currentIndex].hand[cardIndex]);
	players[currentIndex].hand.erase(players[currentIndex].hand.begin()+cardIndex);
}
//减一马 
void loadUp4(){
	if(players[currentIndex].eq.horseMinus.size()>0) throwCard4(players[currentIndex]);
	players[currentIndex].eq.horseMinus.push_back(players[currentIndex].hand[cardIndex]);
	players[currentIndex].hand.erase(players[currentIndex].hand.begin()+cardIndex);
}
//添加buff
void adding(Player& p,string name){
	if(name=="sd") p.bf.lightning.push_back(players[currentIndex].hand[cardIndex]);
	if(name=="le") p.bf.happy.push_back(players[currentIndex].hand[cardIndex]);
	players[currentIndex].hand.erase(players[currentIndex].hand.begin()+cardIndex);
} 
//无懈寻牌
bool haswx(){
	for(int i=0;i<players.size();i++)
	{
		if(hasCard(players[i],"无懈可击")){
			return true;
		}
	}
	return false;
} 
//牌的效果（杀，桃，闪电）
//濒死桃
void rescue(Player& dying) {
    for (int i = 1; i <= players.size(); ++i) {
        int idx = (currentIndex + i) % players.size();
        if (!players[idx].alive) continue;
        auto& p = players[idx];
        for (int j = 0; j < p.hand.size(); ++j) {
            if (p.hand[j].name == "桃") {
                cout << p.name << " 是否使用桃救 " << dying.name << "? (y/n): ";
                string choice=cindcs();
                if (choice == "y") {
                    throwCard(p, j);
                    dying.hp++;
                    cout << dying.name << "血量为："<<dying.hp<<endl;
                    break;
                }
                else break; 
            }
        }
    }
}
//伤害造成效果
void hurt(Player& p, int damage){
	p.hp-=damage;
	if(p.hp<=0){
		cout<<p.name<<"濒死！\n";
		for(int i=1;i<=1-p.hp;i++) rescue(p);
		if(p.hp<=0)
		{
			p.alive=false;
			cout<<"\n"<<p.name << " 阵亡！\n";
			for(int j=0;j<p.hand.size();j++) throwCard(p,j);
			if(p.eq.arm.size()>0) throwCard1(p);
	    	if(p.eq.armor.size()>0) throwCard2(p);
			if(p.eq.horsePlus.size()>0) throwCard3(p);
			if(p.eq.horseMinus.size()>0) throwCard4(p);
			if(p.bf.lightning.size()>0){
				throwCard5(p,"sd");
				p.lightning=false;
			} 
			if(p.bf.happy.size()>0){
				throwCard5(p,"le");
				p.happy=false;
			}
		}
	}
}
//无懈可击
bool usewuxie(bool rslt){
	if(haswx()){
		cout<<"开始无懈可击的选择\n"; 
		cout<<"从1号开始选择是否发动无懈可击，直到最后一位，没有此牌的人无论选什么都没用，走流程\n"; 
		pause_and_clear("点回车开始...");
		for(int j=0;j<players.size();j++){
			if(!players[j].alive) continue; 
			pause_and_clear();
			cout<<j+1;
			pause_and_clear("号的选择回合，按回车开始选择");
			printHand(players[j]); 
			cout<<players[j].name<<"是否使用无懈可击(y/n):";
			string dcs=cindcs();
			if(hasCard(players[j],"无懈可击")){
				if(dcs=="y"){
					throwCard(players[j],findCard(players[j],"无懈可击"));
					rslt=usewuxie(rslt);
					cout<<"上一张牌被无懈了\n";
					rslt=!rslt;
					break; 
				}
			}
		}
		pause_and_clear();
	}
	return rslt;
}
void wxkj(){
	cout<<"无懈可击无法主动使用\n";
}
//杀
void useSha() {
    cout << "选择攻击目标编号:\n";
    int targetIndex=select();
	players[currentIndex].usedSha=true;
	throwCard(players[currentIndex], cardIndex);
	pause_and_clear("按回车进入杀的响应阶段");
	pause_and_clear(players[targetIndex].name + "，你被“杀”了，请防御，按回车继续...");
    
	//杀的响应阶段
	Player& defender = players[targetIndex];
    cout << "======== " << defender.name << " 的防御阶段 ========\n";
    cout << "你的手牌：\n";
    printHand(defender);

    if (hasCard(defender, "闪")) {
        cout << "你有“闪”，是否使用？(y/n): ";
        string defend=cindcs();
        if (defend == "y") {
        	int idx=findCard(defender, "闪");
            throwCard(defender, idx);
            pause_and_clear("你使用了“闪”，成功躲避伤害！\n按回车后转交回出牌者");
            cout<<players[targetIndex].name<<"使用了“闪”，成功躲避伤害！\n" ;
            pause_and_clear("按回车后继续");
        } else{
            pause_and_clear("你选择不闪，受到1点伤害！\n按回车后转交回出牌者");
            cout<<players[targetIndex].name<<"不使用“闪”，成功造成伤害！\n" ;
            hurt(defender,1);
            pause_and_clear("按回车后继续");
        }	
    } else {
        pause_and_clear("你没有“闪”，受到1点伤害！\n按回车后转交回出牌者");
        cout<<players[targetIndex].name<<"不使用“闪”，成功造成伤害！\n" ;
        hurt(defender,1);
        pause_and_clear("按回车后继续");
    }
}
//闪
void useshan(){
	cout<<"闪无法主动使用\n";
} 
//桃 
void usePeach() {
    if (players[currentIndex].hp < players[currentIndex].hpLimit) {
        players[currentIndex].hp++;
        cout << players[currentIndex].name << " 使用桃，恢复1点体力，当前体力：" << players[currentIndex].hp << endl;
        throwCard(players[currentIndex], cardIndex);
        pause_and_clear("按回车后继续");
    } else {
        cout << "当前体力已满，无法使用桃\n";
        pause_and_clear("按回车后继续");
    }
}
//闪电
void uselightning(){
	players[currentIndex].lightning=true;
	adding(players[currentIndex],"sd");
} 
//判闪电
bool lightningPoint(const string& numberStr) {
    static const unordered_set<string> valid = {
        "2", "3", "4", "5", "6", "7", "8", "9"
    };
    return valid.count(numberStr) > 0;
}
//乐不思蜀
void usele(){
	cout<<"选择乐不思蜀的对象：";
    int targetIndex=select();
    players[targetIndex].happy=true;
    adding(players[targetIndex],"le");
} 
//诸葛连弩
void liannu(){
	players[currentIndex].liannu=true;
	cout<<"已装备诸葛连弩\n";
	loadUp1();
}
//决斗
void juedou(){
	throwCard(players[currentIndex],cardIndex);
	cout<<"选择决斗对象：";
	int targetIndex=select();
	cout<<players[currentIndex].name<<"选择和"<<players[targetIndex].name<<"进行决斗!\n";
	if(usewuxie(false)) return;
	pause_and_clear("按回车继续...");
	cout<<"请将设备转至"<<players[targetIndex].name<<"手中\n\n";
	cout<<players[currentIndex].name<<"对你发起决斗\n";
	pause_and_clear("按回车后进入决斗...");
	int i=1;
	while(true){
		if(i%2==1)
		{
			printHand(players[targetIndex]);
			if(!hasCard(players[targetIndex],"杀")){
				hurt(players[targetIndex],1);
				pause_and_clear("你没有杀，你无法出杀,扣除1点血\n按回车结束...");
				cout<<players[targetIndex].name<<"在决斗中失败，扣除一点血\n";
				pause_and_clear("按回车后继续...");
				break;
			}
			cout<<players[targetIndex].name<<"是否出杀(y/n):";
			string dcs=cindcs();
			if(dcs=="y"){
					throwCard(players[targetIndex],findCard(players[targetIndex],"杀"));
					pause_and_clear("你已出杀，按回车进入对方回合");
					pause_and_clear("你的回合");
			}else{
				hurt(players[targetIndex],1);
				pause_and_clear("你选择不出杀,扣除1点血\n按回车结束...");
				cout<<players[targetIndex].name<<"在决斗中失败，扣除一点血\n";
				pause_and_clear("按回车后继续...");
				break;
			}
		}
		else{
			printHand(players[currentIndex]);
			if(!hasCard(players[currentIndex],"杀")){
				hurt(players[currentIndex],1);
				pause_and_clear("你没有杀，你无法出杀,扣除1点血\n按回车结束...");
				cout<<players[currentIndex].name<<"在决斗中失败，扣除一点血\n";
				pause_and_clear("按回车后继续...");
				break;
			}
			cout<<players[currentIndex].name<<"是否出杀(y/n):";
			string dcs=cindcs();
			if(dcs=="y"){
					throwCard(players[currentIndex],findCard(players[currentIndex],"杀"));
					pause_and_clear("你已出杀，按回车进入对方回合");
					pause_and_clear("你的回合，按回车开始");
			}
			else{
				hurt(players[currentIndex],1);
				pause_and_clear("你选择不出杀,扣除1点血\n按回车结束...");
				cout<<players[currentIndex].name<<"在决斗中失败，扣除一点血\n";
				pause_and_clear("按回车后继续...");
				break;
			}
		}
		i++;
	}
}
//桃园结义 
void taoyuan(){
	for(int i=0;i<players.size();i++){
		if(players[i].alive)
		{
			cout<<players[i].name<<"的加血回合\n";
			if(!usewuxie(false)&&players[i].hp<players[i].hpLimit) players[i].hp++;
		}
		
	}
	throwCard(players[currentIndex],cardIndex);
}
//无中生有
void wzsy(){
	throwCard(players[currentIndex],cardIndex);
	if(!usewuxie(false))
	{
		cout<<players[currentIndex].name<<"摸两张牌\n"; 
		players[currentIndex].hand.push_back(drawCard());
		players[currentIndex].hand.push_back(drawCard());
	}
}
//南蛮入侵
void nmrq(){
	throwCard(players[currentIndex],cardIndex);
	pause_and_clear("按回车后开始...");
	for(int i=1;i<players.size();i++)
	{
		int idx=(currentIndex+i)%players.size();
		if(!players[idx].alive) continue;
		cout<<players[idx].name;
		pause_and_clear("的出杀回合");
		printHand(players[idx]);
		if(!usewuxie(false)){
			if(!hasCard(players[idx],"杀")){
				cout<<"你没有杀,扣除一点血\n";
				hurt(players[idx],1);
				pause_and_clear("按回车后转交下一人以继续...");
			}
			else{
				cout<<"是否选择出杀(y/n):";
				string dcs=cindcs();
				if(dcs=="y"){
					throwCard(players[i],findCard(players[i],"杀"));
					pause_and_clear("已出杀以避免伤害，按回车后给下一人以继续...");
				}
				else{
					cout<<"你没有杀,扣除一点血\n";
					hurt(players[idx],1);
					pause_and_clear("按回车后转交下一人以继续...");
				}
			}
		}
	}
}
void wjqf(){
	throwCard(players[currentIndex],cardIndex);
	pause_and_clear("按回车后开始...");
	for(int i=1;i<players.size();i++)
	{
		int idx=(currentIndex+i)%players.size();
		if(!players[idx].alive) continue;
		cout<<players[idx].name;
		pause_and_clear("的出闪回合");
		if(!usewuxie(false)){
			printHand(players[idx]);
			if(!hasCard(players[idx],"闪")){
				cout<<"你没有闪,扣除一点血\n";
				hurt(players[idx],1);
				pause_and_clear("按回车后转交下一人以继续...");
			}
			else{
				cout<<"是否选择出闪(y/n):";
				string dcs=cindcs();
				if(dcs=="y"){
					throwCard(players[i],findCard(players[i],"闪"));
					pause_and_clear("已出闪以避免伤害，按回车后给下一人以继续...");
				}
				else{
					cout<<"你没有闪,扣除一点血\n";
					hurt(players[idx],1);
					pause_and_clear("按回车后转交下一人以继续...");
				}
			}
		}
	}
}
//过河拆桥
void ghcq(){
	throwCard(players[currentIndex],cardIndex);
	cout<<"挑选要拆的对象\n" ;
	int targetIdx=select();
	pause_and_clear("点回车开始");
	cout<<players[currentIndex].name<<"将要拆掉"<<players[targetIdx].name<<"的一张牌\n";
	if(usewuxie(false)) return;
	if(players[targetIdx].lightning){
		cout<<"是否拆掉闪电(y/n):";
		string dcs=cindcs();
		if(dcs=="y"){
			players[targetIdx].lightning=false;
			throwCard5(players[targetIdx],"sd"); 
			return;
		}
	}
	if(players[targetIdx].happy){
		cout<<"是否拆掉乐不思蜀(y/n):";
		string dcs=cindcs();
		if(dcs=="y"){
			players[targetIdx].happy=false;
			throwCard5(players[targetIdx],"le"); 
			return;
		}
	}
	if(players[targetIdx].hand.size()>0){
		if(players[targetIdx].eq.arm.size()>0||players[targetIdx].eq.armor.size()>0||players[targetIdx].eq.horseMinus.size()>0||players[targetIdx].eq.horsePlus.size()>0){
			cout<<"选择拆手牌选y，选择拆装备选n:";
			string dcs=cindcs();
			if(dcs=="y"){
				throwCard(players[targetIdx],random(0,players[targetIdx].hand.size()-1));
				cout<<"成功拆除"<<players[targetIdx].name<<"的一张手牌\n"; 
			} 
			else{
				while(true){
					cout<<"\n选择拆武器（1），防具（2），加一马（3），减一马（4）:";
					string choice;
					cin>> choice;
					while(choice!="1"&&choice!="2"&&choice!="3"&&choice!="4"){
						cout<<"请在1，2，3，4中选一个输入：";
						cin>>choice;
					}
					if(choice=="1"){
						if(players[targetIdx].eq.arm.size()>0){
							throwCard1(players[targetIdx]);
							break;
						}
						else{
							cout<<"没有装备武器，请选择装备的牌\n";
							continue; 
						}
					}else if(choice=="2"){
						if(players[targetIdx].eq.armor.size()>0){
							throwCard2(players[targetIdx]);
							break;
						}
						else{
							cout<<"没有装备防具，请选择装备的牌\n";
							continue; 
						}
					}else if(choice=="3"){
						if(players[targetIdx].eq.horsePlus.size()>0){
							throwCard3(players[targetIdx]);
							break;
						}
						else{
							cout<<"没有装备加一马，请选择装备的牌\n";
							continue; 
						}
					}else if(choice=="4"){
						if(players[targetIdx].eq.horseMinus.size()>0){
							throwCard4(players[targetIdx]);
							break;
						}
						else{
							cout<<"没有装备减一马，请选择装备的牌\n";
							continue; 
						}
					}
				}
			}
		}else{
			throwCard(players[targetIdx],random(0,players[targetIdx].hand.size()-1));
			cout<<"已随机拆掉"<<players[targetIdx].name<<"一张牌\n" ;
		}
	}
	else if(players[targetIdx].eq.arm.size()>0||players[targetIdx].eq.armor.size()>0||players[targetIdx].eq.horseMinus.size()>0||players[targetIdx].eq.horsePlus.size()>0){
		while(true){
			cout<<"\n选择拆武器（1），防具（2），加一马（3），减一马（4）:";
			string choice;
			cin>> choice;
			while(choice!="1"&&choice!="2"&&choice!="3"&&choice!="4"){
				cout<<"请在1，2，3，4中选一个输入：";
				cin>>choice;
			}
			if(choice=="1"){
				if(players[targetIdx].eq.arm.size()>0){
					throwCard1(players[targetIdx]);
					break;
				}
				else{
					cout<<"没有装备武器，请选择装备的牌\n";
					continue; 
				}
			}else if(choice=="2"){
				if(players[targetIdx].eq.armor.size()>0){
					throwCard2(players[targetIdx]);
					break;
				}
				else{
					cout<<"没有装备防具，请选择装备的牌\n";
					continue; 
				}
			}else if(choice=="3"){
				if(players[targetIdx].eq.horsePlus.size()>0){
					throwCard3(players[targetIdx]);
					break;
				}
				else{
					cout<<"没有装备加一马，请选择装备的牌\n";
					continue; 
				}
			}else if(choice=="4"){
				if(players[targetIdx].eq.horseMinus.size()>0){
					throwCard4(players[targetIdx]);
					break;
				}
				else{
					cout<<"没有装备减一马，请选择装备的牌\n";
					continue; 
				}
			}
		}
	}
	else{
		cout<<"此角色无牌可拆，此牌视作无效\n"; 
	}
} 
//drawCardfromsb(players[targetIdx],);
//顺手牵羊
void ssqy(){
	throwCard(players[currentIndex],cardIndex);
	cout<<"挑选要牵的对象\n" ;
	int targetIdx=select();
	pause_and_clear("点回车开始");
	cout<<players[currentIndex].name<<"将要牵走"<<players[targetIdx].name<<"的一张牌\n";
	if(usewuxie(false)) return;
	cout<<"请输入（1~7）以选择顺走什么牌(1:手牌，2：武器，3:防具，4：加一马，5：减一马，6：雷电，7：乐不思蜀)（只能顺有的牌）:";
	int type;
	cin>>type;
	while(type!=1&&type!=2&&type!=3&&type!=4&&type!=5&&type!=6&&type!=7){
		cout<<"请在（1~7）中选择：";
		cin>>type;
	}
	switch (type) {
		case 1:
			if(players[targetIdx].hand.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],1));
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"的一张牌\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n"; 
			break;
		case 2:
			if(players[targetIdx].eq.arm.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],2));
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"装备区的武器牌\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n"; 
			break;
		case 3:
			if(players[targetIdx].eq.armor.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],3));
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"装备区的防具牌\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n";
			break;
		case 4:
			if(players[targetIdx].eq.horsePlus.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],4));
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"装备区的加一马牌\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n";
			break;
		case 5:
			if(players[targetIdx].eq.horseMinus.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],5));
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"装备区的减一马牌\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n";
			break;
		case 6:
			if(players[targetIdx].bf.lightning.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],6));
				players[targetIdx].lightning=false;
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"状态区的闪电\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n";
			break;
		case 7:
			if(players[targetIdx].bf.happy.size()>0){
				players[currentIndex].hand.push_back(drawCardfromsb(players[targetIdx],7));
				players[targetIdx].happy=false;
				cout<<players[currentIndex].name<<"已牵走"<<players[targetIdx].name<<"状态区的乐不思蜀\n";
			}
			else cout<<"你牵的对象区域为空，此牌视作无效\n";
			break;
	}
}
//未知
void unknownCard(){
    std::cout << "未知牌，无法出牌。\n";
}

//一个回合 
void takeTurn() {
    if (!players[currentIndex].alive) return;
	players[currentIndex].usedSha = false;
	bool mopai=true;
	bool chupai=true;
	bool qipai=true;
    cout << "======== " << players[currentIndex].name << " 的回合 ========\n";
    //判定阶段
	if(players[currentIndex].lightning){
		if(!usewuxie(false)){
			cout<<"摸一张牌判闪电\n";
			card c=drawCard();
			cout<<"牌的花色为："<<c.suit<<"\n牌的点数为：" <<c.point<<endl;
			if(c.suit=="Spade"&&lightningPoint(c.point)){
				cout<<"闪电判定生效！\n";
				players[currentIndex].lightning=false;
				throwCard5(players[currentIndex],"sd");
				hurt(players[currentIndex],3);
			}
			else{
				cout<<"闪电判定未生效！已自动转至下一位\n";
				if(currentIndex==players.size()) players[0].lightning=true;
				else players[currentIndex+1].lightning=true;
			}
			drawPile.push_back(c);
			washDrawPile();
		}else{
			players[currentIndex].lightning=false;
			throwCard5(players[currentIndex],"sd");
		}
	}
	if(players[currentIndex].happy&&players[currentIndex].alive){
		if(!usewuxie(false)){
			cout<<"摸一张牌判乐不思蜀\n";
			card c=drawCard();
			cout<<"牌的花色为："<<c.suit<<"\n牌的点数为：" <<c.point<<endl;
			if(c.suit!="Heart"){
				chupai=false;
				cout<<"乐不思蜀判定生效!\n"; 
			}
			else cout<<"乐不思蜀判定未生效!\n";
			drawPile.push_back(c);
			washDrawPile();
			players[currentIndex].happy=false;
			throwCard5(players[currentIndex],"le");
		}else{
			players[currentIndex].happy=false;
			throwCard5(players[currentIndex],"le");
		}
	}
	//摸牌阶段
	if(mopai&&players[currentIndex].alive){
		players[currentIndex].hand.push_back(drawCard());
		players[currentIndex].hand.push_back(drawCard());
	    cout << "你摸了两张牌。\n";
	}
    
	//出牌阶段
	while (chupai&&players[currentIndex].alive) {
		printHand(players[currentIndex]);
		cout << "选择操作: 1. 出牌 2. 结束回合\n ";
		string op;
	    cin >> op;
	    while(op!="1"&&op!="2"){
			cout<<"请输入(1/2):";
			cin >> op;
		}
	    if (op == "2") break;
	    if (op == "1") {
	    	cout << "选择手牌编号: ";
	        cin >> cardIndex;
	        cardIndex--;
	        if (cardIndex < 0 || cardIndex >= players[currentIndex].hand.size()) {
	            cout << "无效编号\n";
	            continue;
	    	}
	        string card = players[currentIndex].hand[cardIndex].name;
	        if(!players[currentIndex].liannu)
	        {
				if(card == "杀"){
					if (players[currentIndex].usedSha) {
				        cout << "每回合只能使用一次杀\n";
				        continue;
				    }
				}
			}
			// 建立牌名 → 功能 的映射表
	        unordered_map<std::string, std::function<void()>> cardEffects = {
			        {"杀", useSha},
			        {"闪",useshan},
			        {"桃", usePeach},
			        {"闪电",uselightning},
			        {"乐不思蜀",usele},
			        {"诸葛连弩",liannu},
			        {"决斗",juedou},
			        {"桃园结义",taoyuan},
			        {"无中生有",wzsy},
			        {"无懈可击",wxkj},
			        {"南蛮入侵",nmrq},
			        {"万箭齐发",wjqf},
			        {"过河拆桥",ghcq},
			        {"顺手牵羊",ssqy},
	        }; 
	        // 查找并执行对应的函数
	        if (cardEffects.count(card)) {
			        cardEffects[card]();  // 执行牌的效果
			} else {
				unknownCard();
			}
		}
	}
    //弃牌阶段
	if(players[currentIndex].hand.size()>players[currentIndex].hp&&qipai&&players[currentIndex].alive){
		cout<<"你需要弃置" << players[currentIndex].hand.size()-players[currentIndex].hp << "张手牌\n";
		while(players[currentIndex].hand.size()>players[currentIndex].hp){
			printHand(players[currentIndex]);
			cout << "选择手牌编号: ";
			int cardIdx;
	        cin >> cardIdx;
	        cardIdx--;
	        if (cardIdx < 0 || cardIdx >= players[currentIndex].hand.size()) {
	            cout << "无效编号\n";
	            continue;
	    	}
	    	throwCard(players[currentIndex],cardIdx);
		}
	}
	//结束阶段 
    cout<<"示众\n";
    printStatus();
    pause_and_clear("回合结束，请交换操作。");
}

int main() {
    srand(time(0));
	initDeck();

    cout << "=== 三国杀 标准版 ===\n";
    cout << "请选择游戏人数（4~6）：";
    int playerCount;
    while (true) {
        cin >> playerCount;
        if (playerCount >= 4 && playerCount <= 6) break;
        cout << "人数不合法，请重新输入（4~6）：";
    }
//选角色 
    for (int i = 0; i < playerCount; ++i) {
        Player p;
        cout << "请输入第" << i + 1 << "位玩家名字：";
        cin >> p.name;
        for(int j=0;j<4;j++) p.hand.push_back(drawCard());
        players.push_back(p);
    }
//循环回合 
	printStatus();
    cout<<"\n---------------------------------------\n";
    while (count_if(players.begin(), players.end(), [](Player& p) { return p.alive; }) > 1) {
        takeTurn();
        currentIndex = (currentIndex + 1) % playerCount;
    }
//胜利宣言
    for (const auto& p : players) {
        if (p.alive) {
            cout << "\n游戏结束！" << p.name << " 获胜！\n";
            break;
        }
    }

    return 0;
}
