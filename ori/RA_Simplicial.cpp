#include<stdio.h>
#include<stdlib.h> 
#include<time.h>
#include<math.h>


#define N 2000
#define simplex 4	 //	simplex��С 
#define m 3          //ÿ��ʱ�䲽����Ծ�ڵ㷢���ı��� 
#define gama 2.5     //�����Ծ�����ɷֲ�ָ�� 
#define lBound 0.01  //�����Ծ�ȵ��½� 
#define uBound 1     //�����Ծ�ȵ��Ͻ�
#define eta 1
#define step 10000    //��̬��ֹʱ�� 
#define AVE 10       //��Ծ��+SIS���в�����ƽ������
#define infectseeds 0.01 
#define p_M 0.6 
#define miurecover 0.01
#define lanbtainter 0.001   //��Ⱦ��ÿ�α仯���� 
#define lanbtaifect0 0.001  //��ʼ��Ⱦ�� 
#define lanbtamax 0.1      //����Ⱦ�� 
#define q_M  0.1
#define q_S  0.2
#define c_beta  0.6
#define eta_miu  0.6

typedef struct person
{
	int ID;
	double active;
	int Social_way;   //0����ɶ��罻  1����Ⱥ���罻 
	double source;
	int donate_state; //0����δ����  1������� 
	struct neighborList *currentNeighbor;
	int activestate;
	int infectstate;      //��Ⱦ״̬ 0�׸��ߣ�1��Ⱦ�ߣ� 2������ 
	int infectstate_next; //��Ⱦ״̬ 0�׸��ߣ�1��Ⱦ�ߣ� 2������ 
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

//��¼��Ⱦ���ܶ���ʱ�䲽���ı仯 
typedef struct RecordIstateandT
{
	int t;
	double Idensity;
	struct RecordIstateandT *next;
}RecordIstateandT; 

//��¼��̬��Ⱦ���ܶ����Ⱦ�ʵı仯
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



//��Ծ��ģ�� 
void InitialPersonNode();
void InitialCurrentNeighbor();
void InitialBeselectedNode();
void CalculateDegreeDistribution();           //����ƽ���ȺͶȵĸ��ʷֲ� 
void CalculateAveractive();                   //����ƽ����Ծ�� 
void FireNode();                              //���ʼ���ڵ� 
void ConnectotherNode(int ID);                //����Ľڵ�������� m��������Ⱥ�ڵ� 
void ConnectselfNode(int ID);                //����Ľڵ�������� s������Ⱥ�����ڵ� 
int isAlreadyNeighbor(int ID, int ppID);      //�ж����ӽڵ��Ƿ���Ϊ���ھ� 
void BeNeighbor(int ID1, int ID2);            //���ڵ�����Ϊ�ھ� 
void freePersonNode(); 
void freeCurrentNeighbor();
void freeBeselectedNode();

//���в����� 
void EpidemicProcess();              //�����Ⱦ״̬�����仯
void InitialAllagentstate();         //��ʼ�����и���ĸ�Ⱦ̬ 
void RandomdistriIseeds();           //���ѡ�������Ϊ��Ⱦ�� 
void RandomdistriSocial_M();         //���ѡ�����ΪȺ���罻 
void changeAgentstate(int ID);       //�ı����״̬ 
void InitialIstateandTList();        //��ʼ��ʱ�̼���Ӧ��Ⱦ���ܶ�����
void InsertIstateandTtoList(double Idensity);      //��ʱ�̼���Ӧ��Ⱦ���ܶȴ�������
void writeIstateandTListtofile();
void InitialIstateandLanbtaList();                 //��ʼ���仯�ĸ�Ⱦ��ֵ����Ӧ����̬��Ⱦ���ܶ�����
void InsertIstateandLanbtatoList(double lanbtaValue, double IDensityValue, int ave);      //���仯�ĸ�Ⱦ��ֵ����Ӧ����̬��Ⱦ���ܶȴ�������
void InitialAverIstateandLanbtaList();             //��ʼ��ƽ��AVE�κ����̬��Ⱦ���ܶȼ���Ⱦ��ֵ�� 
void InsertAverIstateandLanbtatoList();            //��ƽ��AVE�κ����̬��Ⱦ���ܶȼ���Ⱦ��ֵ��������  
void writeAverIstateandLanbtatoListtofile();
double CalculateIdensity();
int NeighborNumofinfectNode(int ID);
void freeIstateandTList();
void freeIstateandLanbtaList();
void freeAverIstateandLanbtatoList();
void freeBeselectedSeedHead();


//����ڵ��ھ���
int NeighborNumofNode(int ID);                

//��ʼģ�� 
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
��Ծ��ģ�� 
ÿ���ڵ��Ի�Ծ��a�ĸ��ʱ�����
�����Ľڵ���������m���ھӽڵ� 
*/ 
void InitialPersonNode()
{
	printf("--------------------------�ڵ��ʼ����ʼ--------------------------\n");
	//��ʼ��PersonNode�ڵ�
	/*��ʱ�������ݻ���ʼǰ�����������
	1.�����ڴ�ռ�
	2.��ʼ���ڵ��Ծ��
	*/
	FILE *f_A;
	if((f_A = fopen("nodeactive_A.txt", "w")) == NULL)
	{
		printf("   ���ļ�ʧ��\n"); 
		exit(0);
	}
	for(i=0; i<N; i++)
	{
		PersonNode[i] = (struct person*)malloc(sizeof(struct person));
		if(!(PersonNode[i]))
		{
			printf("   ����ռ�ʧ��\n");
			exit(0);
		}
		PersonNode[i]->source = 1; 
		PersonNode[i]->donate_state = 0; 
		PersonNode[i]->ID = i;//��¼�ڵ�i��ID�� 
		randomNumber = rand() / (double)(RAND_MAX); //����0-1����� 
		active = pow(((pow(uBound, (-gama + 1)) - pow(lBound, (-gama + 1)))*randomNumber + pow(lBound, (-gama + 1))), (1 / (-gama + 1)));
		PersonNode[i]->active = active*eta;
		printf("�ڵ�%d�Ļ�Ծ��Ϊ%1f\n", i+1, PersonNode[i]->active);
		fprintf(f_A, "%d %1f\n", i+1, PersonNode[i]->active);
	}
	printf("--------------------------����ɻ�Ծ�ȷ���--------------------------\n");
	printf("--------------------------�ڵ��ʼ������----------------------------\n");
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

//ͳ��ʱ�������ƽ���ȼ����ʷֲ� 
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
		printf("   ���ļ�ʧ��\n");
		exit(0);
	}
	for(i=0; i<N; i++)
	{
		fprintf(f2, "%d %1f\n", i+1, degreepd[i]);
	}
	fclose(f2);
	
}

//����ƽ����Ծ�� 
void CalculateAveractive()
{
	sumactive = 0.0;
	for(i=0; i<N; i++)
	{
		sumactive += PersonNode[i]->active;  
	}
	averactive = sumactive / (double)N;
}

//���нڵ��Ը��Ի�Ծ�ȼ���
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

//ѡ��m�������ڵ�����
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

//�γ�(s-1)-simpelex�ṹ 
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

//ppID�Ƿ���Ϊ�ھӽڵ� 
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
		//	printf("%d�Ѿ����Լ��ĵ�ǰ�ھӽڵ�\n", ppID);
	}			
}

//ID1��ID2������Ϊ�ھ� 
void BeNeighbor(int ID1, int ID2)
{
	/*��ID2��ΪID1���ھӽڵ���뵽ID1���ھ������У�������ID1�Ķ���Ϣ*/
	p = (neighborList *)malloc(sizeof(neighborList));
	p->ID = ID2;
	p->next = PersonNode[ID1]->currentNeighbor;
	PersonNode[ID1]->currentNeighbor = p; 
	/*��ID1��ΪID2���ھӽڵ���뵽ID2���ھ������У�������ID2�Ķ���Ϣ*/
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
���в����� 
��ʼʱS I�ֲ��Ѿ����úã���Ⱦ״̬��0�׸У�1��Ⱦ
����ÿ���ڵ㣬
1.�����S̬�����ݵ�ǰ�ھ���I̬�ڵ����������һ������ת��ΪS̬
2.�����I̬����recoverRate�����ָ�ΪS̬
*/ 
void EpidemicProcess()
{	
	double InstantIdensity = 0.0;
	InitialIstateandTList();//��¼ʱ��T��I״̬
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
		//������Դ 
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
//��ʼ�����и���ĸ�Ⱦ̬���罻��ʽ 
void InitialAllagentstate()
{
	for(i=0; i<N; i++)
	{
		PersonNode[i]->Social_way = 0; //��ʼ�����и���Ϊ�ɶ��罻 
		PersonNode[i]->infectstate = 0; //��ʼ�����и���Ϊ����״̬ 
		PersonNode[i]->infectstate_next = 0; //��ʼ�����и���Ϊ����״̬ 
	} 
}

//��������Ⱦ������ 
void RandomdistriIseeds()
{
	BeselectedSeedHead = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
	BeselectedSeedHead->next = NULL;
	seed = N*infectseeds;
	while(seed)
	{
		randomID = rand() % N;
		if (PersonNode[randomID]->infectstate == 1 && PersonNode[randomID]->infectstate == 2)//����Ǹ�Ⱦ�ڵ�����߽ڵ㣬����ѡ
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

//���ѡ��������Ⱥ���罻 
void RandomdistriSocial_M()
{
	BeselectedSeedHead = (struct BeselectedID*)malloc(sizeof(struct BeselectedID));
	BeselectedSeedHead->next = NULL;
	seed = N*(1-p_M);
	while(seed)
	{
		randomID = rand() % N;
		if (PersonNode[randomID]->Social_way == 1)//����ǽڵ��ѱ�ѡΪȺ���罻 
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

//�ı��Ⱦ��״̬ 
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

//��¼ʱ��T��I״̬
void InitialIstateandTList()
{
	IstateandTHead = (struct RecordIstateandT*)malloc(sizeof(struct RecordIstateandT));
	if (IstateandTHead == NULL) 
	{
		printf("IstateandTHead�����ڴ�ʧ��\n");
	}
	IstateandTHead->next = NULL;
}

//��ʱ�̼���Ӧ��Ⱦ���ܶȴ�������
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
		printf("  �ļ���ʧ��\n");
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

//��ʼ���仯�ĸ�Ⱦ��ֵ����Ӧ����̬��Ⱦ���ܶ�����
void InitialIstateandLanbtaList()
{
	IstateandLanbtaHead = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	if (IstateandLanbtaHead == NULL) 
	{
	printf("IstateandLanbtaHead�����ڴ�ʧ��\n");
	}
	IstateandLanbtaHead->next = NULL;
}

//���仯�ĸ�Ⱦ��ֵ����Ӧ����̬��Ⱦ���ܶȴ�������
void InsertIstateandLanbtatoList(double lanbtaValue, double IDensityValue, int ave)
{
	p2 = IstateandLanbtaHead;
	q2 = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	q2->Idensity = IDensityValue;
	q2->Lanbta = lanbtaValue;
	q2->next = p2->next;
	p2->next = q2;
	printf("--------------��%d�ν���,lanbtaֵΪ��%lf, lanbta/miuֵΪ%lf����Ⱦ�ܶ�Ϊ%lf\n", ave, lanbtaValue, lanbtaValue / miurecover, IDensityValue);	
}

//��ʼ��ƽ��AVE�κ����̬��Ⱦ���ܶȼ���Ⱦ��ֵ��
void InitialAverIstateandLanbtaList()
{
	AverIstateandLanbtaHead = (struct RecordIstateandLanbta *)malloc(sizeof(struct RecordIstateandLanbta));
	if (AverIstateandLanbtaHead == NULL) 
	{
	printf("IstateandLanbtaHead�����ڴ�ʧ��\n");
	}
	AverIstateandLanbtaHead->next = NULL;	
} 

//��ƽ��AVE�κ����̬��Ⱦ���ܶȼ���Ⱦ��ֵ�������� 
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
	printf("lanbtaȡֵΪ%lfʱ��lanbta/miu=%lf,��ƽ��%d��I̬��Ⱦ�ܶ�Ϊ��%lf\n", q2->Lanbta, q2->Lanbta / miurecover, AVE, q2->Idensity);
	q2->next = AverIstateandLanbtaHead->next;
	AverIstateandLanbtaHead->next = q2;
	//freeIstateandLanbtaList(); 
}

void writeAverIstateandLanbtatoListtofile()
{
	FILE *f;
	if((f = fopen("AverIstateandLanbta.txt", "w")) == NULL)
	{
		printf("   ���ļ�ʧ��\n");
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

//�ͷŸ�Ⱦ���ӵı�ű� 
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

//����ڵ��Ⱦ������

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
����ڵ��ھ��� 
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

