#include<stdio.h>
#include<stdlib.h> 
#include<time.h>
#include<math.h>


#define N 2000
#define simplex 4	 //	simplex大小 
#define m 3          //每个时间步长活跃节点发出的边数 
#define gama 2.5     //定义活跃度幂律分布指数 
#define lBound 0.01  //定义活跃度的下界 
#define uBound 1     //定义活跃度的上界
#define eta 1
#define step 10000    //稳态终止时间 
#define AVE 10       //活跃度+SIS流行病过程平均次数
#define infectseeds 0.01 
#define p_M 0.6 
#define miurecover 0.01
#define lanbtainter 0.001   //感染率每次变化步长 
#define lanbtaifect0 0.001  //初始感染率 
#define lanbtamax 0.1      //最大感染率 
#define q_M  0.1
#define q_S  0.2
#define c_beta  0.6
#define eta_miu  0.6

typedef struct person
{
	int ID;
	double active;
	int Social_way;   //0代表成对社交  1代表群体社交 
	double source;
	int donate_state; //0代表未捐赠  1代表捐赠 
	struct neighborList *currentNeighbor;
	int activestate;
	int infectstate;      //感染状态 0易感者，1感染者， 2免疫者 
	int infectstate_next; //感染状态 0易感者，1感染者， 2免疫者 
}person;

typedef struct neighborList
{
	int ID;
	struct neighborList *next;
}neighborList;

typedef struct BeselectedID
{
	int ID;
	struct 	BeselectedID *next;
}BeselectedID;

//记录感染者密度随时间步长的变化 
typedef struct RecordIstateandT
{
	int t;
	double Idensity;
	struct RecordIstateandT *next;
}RecordIstateandT; 

//记录稳态感染者密度随感染率的变化
typedef struct RecordIstateandLanbta
{
	double Lanbta;
	double Idensity;
	//double p_M;
	struct RecordIstateandLanbta *next;
}RecordIstateandLanbta; 


typedef struct Node
{
	int ID;
	double active;
	double probilitydis;
	double weight;
}Node, *Nodepr; 
 
 
struct person *PersonNode[N];
int i, j, numOfActiveState, numberofinfect, randomID, t, seed, ave, count;
int degree[N];
double degreepd[N];
//double p_M;
double Istatedensity, SteadyIdensity;
double randomNumber, active, averactive, sumactive, lanbtainfect, sumofIdensityvalue; 
neighborList* p, * q, * s;
BeselectedID *BeselectedHead, *BeselectedSeedHead;
BeselectedID* ppp, * sss, * qqq;
RecordIstateandT* IstateandTHead, * p1, * q1, * s1;
RecordIstateandLanbta* IstateandLanbtaHead, * AverIstateandLanbtaHead, * p2, * q2, * q3;



//活跃度模型 
void InitialPersonNode();
void InitialCurrentNeighbor();
void InitialBeselectedNode();
void CalculateDegreeDistribution();           //计算平均度和度的概率分布 
void CalculateAveractive();                   //计算平均活跃度 
void FireNode();                              //概率激活节点 
void ConnectotherNode(int ID);                //激活的节点随机连接 m个其他种群节点 
void ConnectselfNode(int ID);                //激活的节点随机连接 s个本种群其他节点 
int isAlreadyNeighbor(int ID, int ppID);      //判断连接节点是否已为其邻居 
void BeNeighbor(int ID1, int ID2);            //两节点连接为邻居 
void freePersonNode(); 
void freeCurrentNeighbor();
void freeBeselectedNode();

//流行病过程 
void EpidemicProcess();              //个体感染状态发生变化
void InitialAllagentstate();         //初始化所有个体的感染态 
void RandomdistriIseeds();           //随机选择个体作为感染者 
void RandomdistriSocial_M();         //随机选择个体为群体社交 
void changeAgentstate(int ID);       //改变个体状态 
void InitialIstateandTList();        //初始化时刻及对应感染者密度链表
void InsertIstateandTtoList(double Idensity);      //将时刻及对应感染者密度存入链表
void writeIstateandTListtofile();
void InitialIstateandLanbtaList();                 //初始化变化的感染率值及对应的稳态感染者密度链表
void InsertIstateandLanbtatoList(double lanbtaValue, double IDensityValue, int ave);      //将变化的感染率值及对应的稳态感染者密度存入链表
void InitialAverIstateandLanbtaList();             //初始化平均AVE次后的稳态感染者密度及感染率值表 
void InsertAverIstateandLanbtatoList();            //将平均AVE次后的稳态感染者密度及感染率值存入链表  
void writeAverIstateandLanbtatoListtofile();
double CalculateIdensity();
int NeighborNumofinfectNode(int ID);
void freeIstateandTList();
void freeIstateandLanbtaList();
void freeAverIstateandLanbtatoList();
void freeBeselectedSeedHead();


//计算节点邻居数
int NeighborNumofNode(int ID);                

//开始模拟 
int main()
{
	srand((unsigned)time(NULL));
	InitialPersonNode();
	InitialAverIstateandLanbtaList();
	CalculateAveractive();

	for(lanbtainfect = lanbtaifect0; lanbtainfect<=lanbtamax; lanbtainfect+=lanbtainter)
	{
		InitialIstateandLanbtaList();
		for(ave=1; ave<=AVE; ave++)
		{
			InitialAllagentstate();
			RandomdistriIseeds();
			RandomdistriSocial_M(); 
			EpidemicProcess();
			InsertIstateandLanbtatoList(lanbtainfect, SteadyIdensity, ave);
			//free(ImmuneNode);
		}
		
	/* 
		for(i=1; i<count; i++)
			printf("\n%d ",ImmuneNode[i]);
		free(ImmuneNode);
	*/
		InsertAverIstateandLanbtatoList();
		freeIstateandLanbtaList();			 
		writeAverIstateandLanbtatoListtofile();
	}
	freeAverIstateandLanbtatoList();
	freePersonNode();
	return 0;
} 
/*
活跃度模型 
每个节点以活跃度a的概率被激活
激活后的节点会随机链接m个邻居节点 
*/ 
void InitialPersonNode()
{
	printf("--------------------------节点初始化开始--------------------------\n");
	//初始化PersonNode节点
	/*在时序网络演化开始前配置网络参数
	1.分配内存空间
	2.初始化节点活跃度
	*/
	FILE *f_A;
	if((f_A = fopen("nodeactive_A.txt", "w")) == NULL)
	{
		printf("   打开文件失败\n"); 
		exit(0);
	}
	for(i=0; i<N; i++)
	{
		PersonNode[i] = (struct person*)malloc(sizeof(struct person));
		if(!(PersonNode[i]))
		{
			printf("   分配空间失败\n");
			exit(0);
		}
		PersonNode[i]->source = 1; 
		PersonNode[i]->donate_state = 0; 
		PersonNode[i]->ID = i;//记录节点i的ID号 
		randomNumber = rand() / (double)(RAND_MAX); //生成0-1随机数 
		active = pow(((pow(uBound, (-gama + 1)) - pow(lBound, (-gama + 1)))*randomNumber + pow(lBound, (-gama + 1))), (1 / (-gama + 1)));
		PersonNode[i]->active = active*eta;
		printf("节点%d的活跃度为%1f\n", i+1, PersonNode[i]->active);
		fprintf(f_A, "%d %1f\n", i+1, PersonNode[i]->active);
	}
	printf("--------------------------已完成活跃度分配--------------------------\n");
	printf("--------------------------节点初始化结束----------------------------\n");
	fclose(f_A);
}

void InitialCurrentNeighbor()
{
	for(i=0; i<N; i++)
	{
		PersonNode[i]->currentNeighbor = NULL;
	}
}


void InitialBeselectedNode()
{
	BeselectedHead = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
	BeselectedHead->next = NULL; 
}

//统计时变网络的平均度及概率分布 
void CalculateDegreeDistribution()
{
	FILE *f2;
	for(i=0; i<N; i++)
	{
		degree[i] = 0;
		p = PersonNode[i]->currentNeighbor;
		while(p != NULL)
		{
			degree[i]++;
			p = p->next;
		}
	}
	for(i=0; i<N; i++)
		degreepd[i] = 0.0;
	for(i=0; i<N; i++)
	{
		degreepd[degree[i]]++;
	}
	for(i=0; i<N; i++)
	{
		degreepd[i] = degreepd[i] / (double)N;
	}
	if((f2 = fopen("degree.txt", "w")) == NULL)
	{
		printf("   打开文件失败\n");
		exit(0);
	}
	for(i=0; i<N; i++)
	{
		fprintf(f2, "%d %1f\n", i+1, degreepd[i]);
	}
	fclose(f2);
	
}

//计算平均活跃度 
void CalculateAveractive()
{
	sumactive = 0.0;
	for(i=0; i<N; i++)
	{
		sumactive += PersonNode[i]->active;  
	}
	averactive = sumactive / (double)N;
}

//所有节点以各自活跃度激活
void FireNode()
{
	numOfActiveState = 0;
	for(i=0; i<N; i++)
	{
		randomNumber = rand() / (double)(RAND_MAX);
		if(randomNumber < PersonNode[i]->active)
		{
			PersonNode[i]->activestate = 1;
			numOfActiveState++;
		}
		else
			PersonNode[i]->activestate = 0;
	}
}

//选择m个其他节点连接
void ConnectotherNode(int ID, int numOfContact)
{
	InitialBeselectedNode();
	j = numOfContact;

	while(j)
	{
		randomID = rand() % N; 
		if(isAlreadyNeighbor(ID, randomID))
		{
			continue;
		}
		else
		{
			ppp = BeselectedHead->next;
			while(ppp && ppp->ID != randomID)
			{
				ppp = ppp->next;
			} 
			if(!ppp)
			{
				sss = (struct BeselectedID *)malloc(sizeof(struct BeselectedID));
				sss->ID = randomID;
				sss->next = BeselectedHead->next;
				BeselectedHead->next = sss;
				j--;
			}
			else
			{
				continue;
			}	
		} 
	}
	ppp = BeselectedHead->next;
	while(ppp)
	{
		BeNeighbor(ID, ppp->ID);
		//		printf("%d %d %d\n", t, ID, ppp->ID);
		ppp = ppp->next;
	}
	freeBeselectedNode(); 
}

//形成(s-1)-simpelex结构 
void ConnectselfNode(int ID, int numOfContact)
{
	InitialBeselectedNode();
	j = numOfContact;

	while(j)
	{
		randomID = rand() % N; 
		if(randomID == ID)
		{
			continue;
		} 
		if(isAlreadyNeighbor(ID, randomID))
		{
			continue;
		}
		else
		{
			ppp = BeselectedHead->next;
			while(ppp && ppp->ID != randomID)
			{
				ppp = ppp->next;
			} 
			if(!ppp)
			{
				sss = (struct BeselectedID *)malloc(sizeof(struct BeselectedID));
				sss->ID = randomID;
				sss->next = BeselectedHead->next;
				BeselectedHead->next = sss;
				j--;
			}
			else
			{
				continue;
			}	
		} 
	}
	
	ppp = BeselectedHead->next;
	while(ppp)
	{
		BeNeighbor(ID, ppp->ID);
		//		printf("%d %d %d\n", t, ID, ppp->ID); 
		ppp = ppp->next;
	}
	sss = BeselectedHead->next;
	ppp = sss->next;
	while(sss && sss->next)
	{
		while(ppp)
		{
			if(!(isAlreadyNeighbor(sss->ID, ppp->ID)))
				BeNeighbor(sss->ID, ppp->ID);
			ppp = ppp->next;
		}
		sss = sss->next;
		ppp = sss->next;
	}
	freeBeselectedNode(); 
}

//ppID是否已为邻居节点 
int isAlreadyNeighbor(int ID, int ppID)
{
	p = PersonNode[ID]->currentNeighbor;
	while(p && p->ID != ppID)
	{
		p = p->next;
	}
	if(!p)
		return 0;
	else
	{
		return 1;
		//	printf("%d已经是自己的当前邻居节点\n", ppID);
	}			
}

//ID1和ID2互连接为邻居 
void BeNeighbor(int ID1, int ID2)
{
	/*将ID2作为ID1的邻居节点插入到ID1的邻居链表中，并更新ID1的度信息*/
	p = (neighborList *)malloc(sizeof(neighborList));
	p->ID = ID2;
	p->next = PersonNode[ID1]->currentNeighbor;
	PersonNode[ID1]->currentNeighbor = p; 
	/*将ID1作为ID2的邻居节点插入到ID2的邻居链表中，并更新ID2的度信息*/
	p = (neighborList *)malloc(sizeof(neighborList));
	p->ID = ID1;
	p->next = PersonNode[ID2]->currentNeighbor;
	PersonNode[ID2]->currentNeighbor = p; 
}

void freePersonNode()
{
	for(i=0; i<N; i++)
	{
		free(PersonNode[i]);
	}
}

void freeCurrentNeighbor()
{
	for(i=0; i<N; i++)
	{
		p = PersonNode[i]->currentNeighbor;
		while(p)
		{
			s = p->next;
			free(p);
			p = s;
		}
	}
}

void freeBeselectedNode()
{
	ppp = BeselectedHead->next;
	while(ppp)
	{
		sss = ppp->next;
		free(ppp);
		ppp = sss;
	}
	free(BeselectedHead);
}


/*
流行病过程 
初始时S I分布已经设置好；感染状态：0易感，1感染
遍历每个节点，
1.如果是S态，根据当前邻居中I态节点的数量，以一定比例转化为S态
2.如果是I态，以recoverRate比例恢复为S态
*/ 
void EpidemicProcess()
{	
	double InstantIdensity = 0.0;
	InitialIstateandTList();//记录时间T和I状态
	SteadyIdensity = 0.0;
	for(t=1; t<=step; t++)
	{
		for(i=0; i<N; i++)
		{
			if(PersonNode[i]->infectstate == 1)
				PersonNode[i]->source = 1;
			else
			{
				PersonNode[i]->source = 1;
				PersonNode[i]->donate_state = 0;
			}
				
		}
		FireNode();
		InitialCurrentNeighbor();
		for(i=0; i<N; i++)
		{
			if(PersonNode[i]->activestate == 1)
			{
				if(PersonNode[i]->Social_way == 0)
					ConnectotherNode(i, m);
				else
					ConnectselfNode(i, simplex-1);
			}
		}
		//分配资源 
		for(i=0; i<N; i++)
		{
			if(PersonNode[i]->infectstate == 1)
			{
				//numberofinfect = 0;
				p = PersonNode[i]->currentNeighbor;
				while(p)
				{
					if(PersonNode[p->ID]->infectstate == 0)
					{
						int num_infectnode = NeighborNumofinfectNode(p->ID);
						if(PersonNode[p->ID]->Social_way == 0)
						{
							randomNumber = rand() / (double)(RAND_MAX);
							if(randomNumber < q_M)
							{
								PersonNode[i]->source += 1.0/(double)num_infectnode;
								PersonNode[p->ID]->donate_state = 1;
							}
						
						}
						else
						{
							randomNumber = rand() / (double)(RAND_MAX);
							if(randomNumber < q_S)
							{
								PersonNode[i]->source += 1.0/(double)num_infectnode;
								PersonNode[p->ID]->donate_state = 1;
								
							}	
						}
					}
					p = p->next;
				}
			}		
		} 
		
		
		for(i=0; i<N; i++)
		{
			changeAgentstate(i);
		}
		for(i=0; i<N; i++)
			PersonNode[i]->infectstate = PersonNode[i]->infectstate_next;
		InstantIdensity = CalculateIdensity();
		InsertIstateandTtoList(InstantIdensity);
		//CalculateDegreeDistribution();
		if(t > 9000)
			SteadyIdensity += InstantIdensity;
		freeCurrentNeighbor();
	}
	SteadyIdensity = SteadyIdensity/1000;
	writeIstateandTListtofile();	
}
//初始化所有个体的感染态和社交方式 
void InitialAllagentstate()
{
	for(i=0; i<N; i++)
	{
		PersonNode[i]->Social_way = 0; //初始化所有个体为成对社交 
		PersonNode[i]->infectstate = 0; //初始化所有个体为健康状态 
		PersonNode[i]->infectstate_next = 0; //初始化所有个体为健康状态 
	} 
}

//随机分配感染者种子 
void RandomdistriIseeds()
{
	BeselectedSeedHead = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
	BeselectedSeedHead->next = NULL;
	seed = N*infectseeds;
	while(seed)
	{
		randomID = rand() % N;
		if (PersonNode[randomID]->infectstate == 1 && PersonNode[randomID]->infectstate == 2)//如果是感染节点和免疫节点，重新选
		{
			continue;
		}
		ppp = BeselectedSeedHead->next;
		while(ppp && ppp->ID != randomID)
		{
			ppp = ppp->next;
		}
		if(!ppp)
		{
			sss = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
			sss->ID = randomID;
			sss->next = BeselectedSeedHead->next;
			BeselectedSeedHead->next = sss;
			PersonNode[randomID]->infectstate = 1;
			PersonNode[randomID]->infectstate_next = 1;
			seed--;	
		}
		else
		{
			continue;
		}

	}
	freeBeselectedSeedHead();
}

//随机选择个体进行群体社交 
void RandomdistriSocial_M()
{
	BeselectedSeedHead = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
	BeselectedSeedHead->next = NULL;
	seed = N*(1-p_M);
	while(seed)
	{
		randomID = rand() % N;
		if (PersonNode[randomID]->Social_way == 1)//如果是节点已被选为群体社交 
		{
			continue;
		}
		ppp = BeselectedSeedHead->next;
		while(ppp && ppp->ID != randomID)
		{
			ppp = ppp->next;
		}
		if(!ppp)
		{
			sss = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
			sss->ID = randomID;
			sss->next = BeselectedSeedHead->next;
			BeselectedSeedHead->next = sss;
			PersonNode[randomID]->Social_way = 1;
			seed--;	
		}
		else
		{
			continue;
		}

	}
	freeBeselectedSeedHead();
}

//改变感染者状态 
void changeAgentstate(int ID)
{
	if(PersonNode[ID]->infectstate == 1)
	{
		randomNumber = rand() / (double)(RAND_MAX);
		double miu = 1 - powl(1 - miurecover, eta_miu * PersonNode[ID]->source);
		if(randomNumber < miu)
		{
			PersonNode[ID]->infectstate_next = 0;
		}
	}
	else if(PersonNode[ID]->infectstate == 0)
	{
		numberofinfect = 0;
		p = PersonNode[ID]->currentNeighbor;
		while(p)
		{
			if(PersonNode[p->ID]->infectstate == 1)
			{
				numberofinfect++;
			}
			p = p->next;
		}
		if(PersonNode[ID]->donate_state == 1)
		{
			randomNumber = rand() / (double)(RAND_MAX);
			if(randomNumber < (1 - powl(1 - lanbtainfect, numberofinfect)))
			{
				PersonNode[ID]->infectstate_next = 1;
			}	
		}
		else
		{
			randomNumber = rand() / (double)(RAND_MAX);
			if(randomNumber < (1 - powl(1 - c_beta * lanbtainfect, numberofinfect)))
			{
				PersonNode[ID]->infectstate_next = 1;
			}
		}
	
	}
	else if(PersonNode[ID]->infectstate == 2)
		return;

}

//记录时间T和I状态
void InitialIstateandTList()
{
	IstateandTHead = (struct RecordIstateandT*)malloc(sizeof(struct RecordIstateandT));
	if (IstateandTHead == NULL) 
	{
		printf("IstateandTHead申请内存失败\n");
	}
	IstateandTHead->next = NULL;
}

//将时刻及对应感染者密度存入链表
void InsertIstateandTtoList(double Idensity)
{
	p1 = IstateandTHead;
	q1 = (struct RecordIstateandT*)malloc(sizeof(struct RecordIstateandT));
	q1->Idensity = Idensity;
	q1->t = t;
	q1->next = p1->next;
	p1->next = q1;
}

void writeIstateandTListtofile()
{
	FILE *f;
	if((f = fopen("IdensityandT.txt", "w")) == NULL)
	{
		printf("  文件打开失败\n");
		exit(0);
	}
	
	p1 = IstateandTHead->next;
	while(p1)
	{
		 fprintf(f, "%d %1f\n", p1->t, p1->Idensity);
		 p1 = p1->next;
	}
	fclose(f);
	freeIstateandTList();
}

//初始化变化的感染率值及对应的稳态感染者密度链表
void InitialIstateandLanbtaList()
{
	IstateandLanbtaHead = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	if (IstateandLanbtaHead == NULL) 
	{
	printf("IstateandLanbtaHead申请内存失败\n");
	}
	IstateandLanbtaHead->next = NULL;
}

//将变化的感染率值及对应的稳态感染者密度存入链表
void InsertIstateandLanbtatoList(double lanbtaValue, double IDensityValue, int ave)
{
	p2 = IstateandLanbtaHead;
	q2 = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	q2->Idensity = IDensityValue;
	q2->Lanbta = lanbtaValue;
	q2->next = p2->next;
	p2->next = q2;
	printf("--------------第%d次结束,lanbta值为：%lf, lanbta/miu值为%lf，感染密度为%lf\n", ave, lanbtaValue, lanbtaValue / miurecover, IDensityValue);	
}

//初始化平均AVE次后的稳态感染者密度及感染率值表
void InitialAverIstateandLanbtaList()
{
	AverIstateandLanbtaHead = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	if (AverIstateandLanbtaHead == NULL) 
	{
	printf("IstateandLanbtaHead申请内存失败\n");
	}
	AverIstateandLanbtaHead->next = NULL;	
} 

//将平均AVE次后的稳态感染者密度及感染率值存入链表 
void InsertAverIstateandLanbtatoList()
{
	p2 = IstateandLanbtaHead->next;
	sumofIdensityvalue = 0.0;
	while(p2)
	{
		sumofIdensityvalue += p2->Idensity;
		p2 = p2->next;
	}
	q2 = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	q2->Idensity = sumofIdensityvalue / (double)AVE;
	q2->Lanbta = IstateandLanbtaHead->next->Lanbta;
	printf("lanbta取值为%lf时，lanbta/miu=%lf,总平均%d后I态感染密度为：%lf\n", q2->Lanbta, q2->Lanbta / miurecover, AVE, q2->Idensity);
	q2->next = AverIstateandLanbtaHead->next;
	AverIstateandLanbtaHead->next = q2;
	//freeIstateandLanbtaList(); 
}

void writeAverIstateandLanbtatoListtofile()
{
	FILE *f;
	if((f = fopen("AverIstateandLanbta.txt", "w")) == NULL)
	{
		printf("   打开文件失败\n");
		exit(0);
	}
	p2 = AverIstateandLanbtaHead->next;
	while(p2)
	{
		fprintf(f, "%1f %1f\n", p2->Lanbta, p2->Idensity);
		p2 = p2->next;
	}
	fclose(f);
	//freeAverIstateandLanbtatoList();
}

double CalculateIdensity()
{
	numberofinfect = 0;
	for(i=0; i<N; i++)
	{
		if(PersonNode[i]->infectstate == 1)
		{
			numberofinfect++;
		}
	}
	Istatedensity = numberofinfect / (double)N;
	return Istatedensity;
}

void freeIstateandTList()
{
	p1 = IstateandTHead->next;
	while(p1)
	{
		s1 = p1->next;
		free(p1);
		p1 = s1;
	}
	free(IstateandTHead);
}

void freeIstateandLanbtaList()
{
	p2 = IstateandLanbtaHead->next;
	while(p2)
	{
		q2 = p2->next;
		free(p2);
		p2 = q2;
	}
	free(IstateandLanbtaHead);
}

void freeAverIstateandLanbtatoList()
{
	p2 = AverIstateandLanbtaHead->next;
	while(p2)
	{
		q2 = p2->next;
		free(p2);
		p2 = q2;
	}
	free(AverIstateandLanbtaHead);
}

//释放感染种子的编号表 
void freeBeselectedSeedHead()
{
	ppp = BeselectedSeedHead->next;
	while(ppp)
	{
		sss = ppp->next;
		free(ppp);
		ppp = sss;
	}
	free(BeselectedSeedHead);
}

//计算节点感染者数量

int NeighborNumofinfectNode(int ID)
{

	int numberofinfectnode = 0;
	q = PersonNode[ID]->currentNeighbor;
	while(q)
	{
		if(PersonNode[q->ID]->infectstate == 1)
		{
			numberofinfectnode++;
		}
		q = q->next;
	}
	return numberofinfectnode;

} 


/*
计算节点邻居数 
*/ 
int NeighborNumofNode(int ID)
{
	int kk;
	kk = 0;
	p = PersonNode[ID]->currentNeighbor;
	while(p)
	{
		kk++;
		p = p->next;
	}
	return kk;
}

