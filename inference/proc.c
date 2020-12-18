/*
#추가한 로직
2줄인데 지역명만 있고 1줄에 숫자가 6개일때

#추가해야할것
동적할당해준것들 해제해주기
*/

// stb liabrary
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC

#include"proc.h"
#include <ctype.h>
#include "../stb/stb_image.h"
#include "../stb/stb_image_write.h"
#include "../stb/stb_image_resize.h"
#include <setjmp.h>

#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) ){
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1)

// l 모델 NodeInfo
NodeInfo lNodeInfo;
// r 모델 NodeInfo
NodeInfo rNodeInfo;
// digit 모델 NodeInfo
NodeInfo dNodeInfo;
// yolo3 모델 NodeInfo
NodeInfo y3NodeInfo;
/*save용 인덱스*/
int save_idx=-1;

// l 레이블 매핑 배열
static const char l_labelMap[LLABELNUM][MAXCHARLEN] = { "background"
,"class0","class1","class2","class3","class4","class5"};
// r 레이블 매핑 배열
static const char r_labelMap[RLABELNUM][MAXCHARLEN] = { "background"
, "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
, "seoul", "daegu", "gwangju", "ulsan", "gyeonggi"
, "chungbuk", "jeonbuk", "gyeongbuk", "jeju", "busan", "incheon"
, "daejeon", "sejong", "gangwon", "chungnam", "jeonnam", "gyeongnam"
, "ga", "na", "da", "ra", "ma", "ba", "sa"
, "aa", "ja", "cha", "ka", "ta", "pa", "ha"
, "goe", "noe", "doe", "roe", "moe", "boe", "soe", "aoe"
, "joe", "choe", "koe", "toe", "poe", "hoe", "go"
, "no", "do", "ro", "mo", "bo", "so", "oo", "jo", "cho"
, "ko", "to", "po", "ho", "gu", "nu", "du", "ru", "mu"
, "bu", "su", "wu", "ju", "chu", "ku", "tu"
, "pu", "hu", "bae", "young", "-", "ke", "oikyo", "ge"
, "ne", "de", "re", "me", "be", "se", "oe", "je"
, "ch", "te", "pe", "he", "yuk", "hae", "gong", "guk", "hap" };
// d 레이블 매핑 배열 area는 지역명 code는 가나다라
static const char d_labelMap[DLABELNUM][MAXCHARLEN] = {"background","1","2","3","4","5","6","7","8","9","0","area","code"};
// yolo3 레이블 매핑 배열
static const char y3_labelMap[Y3LABELNUM][MAXCHARLEN] = { "background"
, "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
, "seoul", "daegu", "gwangju", "ulsan", "gyeonggi"
, "chungbuk", "jeonbuk", "gyeongbuk", "jeju", "busan", "incheon"
, "daejeon", "sejong", "gangwon", "chungnam", "jeonnam", "gyeongnam"
, "ga", "na", "da", "ra", "ma", "ba", "sa"
, "aa", "ja", "cha", "ka", "ta", "pa", "ha"
, "goe", "noe", "doe", "roe", "moe", "boe", "soe", "aoe"
, "joe", "choe", "koe", "toe", "poe", "hoe", "go"
, "no", "do", "ro", "mo", "bo", "so", "oo", "jo", "cho"
, "ko", "to", "po", "ho", "gu", "nu", "du", "ru", "mu"
, "bu", "su", "wu", "ju", "chu", "ku", "tu"
, "pu", "hu", "bae", "young", "-", "ke", "oikyo", "ge"
, "ne", "de", "re", "me", "be", "se", "oe", "je"
, "ch", "te", "pe", "he", "yuk", "hae", "gong", "guk", "hap" };

void initNodeInfo(NodeInfo* nodeInfo){
	nodeInfo->head=NULL;
	nodeInfo->tail=NULL;
	nodeInfo->size=0;
}
void clearNodeInfo(NodeInfo* nodeInfo){
	if(nodeInfo==NULL){
		return;
	}
	while(nodeInfo->size!=0){
		Node* delNode=deleteNodeLast(nodeInfo);
		freeNode(delNode);
	}
}
Node* createNode(Bbox* bbox){
	Node* newNode=(Node*)malloc(sizeof(Node));
	newNode->bbox=bbox;
	newNode->next=NULL;
	newNode->prev=NULL;
	return newNode;
}
void insertNodeLast(NodeInfo* nodeInfo,Node* newNode){
	// 에러
	if(nodeInfo==NULL){
		//printf("error : insertNodeLast nodeInfo NULL\n");
		return;
	}
	if(newNode==NULL){
		//printf("error : insertNodeLast newNode NULL\n");
		return;
	}
	// 비어있을때
	if(nodeInfo->size==0){
		nodeInfo->head=newNode;
		nodeInfo->tail=newNode;
		newNode->next=NULL;
		newNode->prev=NULL;
	}
	else{
		nodeInfo->tail->next=newNode;
		newNode->prev=nodeInfo->tail;
		nodeInfo->tail=newNode;
	}
	nodeInfo->size++;
	return;
}
void insertNodeAt(NodeInfo* nodeInfo,Node* newNode,int idx){
	// //printf("insertAt Node name : %s,size : %d, idx : %d.\n",newNode->bbox->name,nodeInfo->size,idx);
	// 범위 초과
	if(idx>nodeInfo->size||idx<0){
		//printf("error : insertNodeAt out of range\n");
		return;
	}
	// 에러
	if(nodeInfo==NULL){
		//printf("error : insertNodeAt nodeInfo NULL\n");
		return;
	}
	if(newNode==NULL){
		//printf("error : insertNodeAt newNode NULL\n");
		return;
	}
	// 공통
	nodeInfo->size++;

	// 처음 넣을때
	if(nodeInfo->size==1){
		nodeInfo->head=newNode;
		nodeInfo->tail=newNode;
		newNode->next=NULL;
		newNode->prev=NULL;
		return;
	}
	// 헤드에 넣을때
	else if(idx==0){
		nodeInfo->head->prev=newNode;
		newNode->next=nodeInfo->head;
		nodeInfo->head=newNode;
		return;
	}
	// 마지막에 넣을때
	else if(idx+1==(nodeInfo->size)){
		nodeInfo->tail->next=newNode;
		newNode->prev=nodeInfo->tail;
		nodeInfo->tail=newNode;
		return;
	}
	// 일반적인 경우
	else{
		Node* tmpNode=nodeInfo->head;
		for(int i=0;i<idx;i++){
			tmpNode=tmpNode->next;
		}
		tmpNode->prev->next=newNode;
		newNode->prev=tmpNode->prev;
		newNode->next=tmpNode;
		tmpNode->prev=newNode;
	}
}
NodeInfo* getVacantNodeInfo(int num){
	NodeInfo* nodeInfo=(NodeInfo*)malloc(sizeof(NodeInfo));
	initNodeInfo(nodeInfo);
	for(int i=0;i<num;i++){
		// 동적할당
		Bbox* bbox=(Bbox*)malloc(sizeof(Bbox));
		Rect* rect=(Rect*)malloc(sizeof(Rect));
		Line* yline=(Line*)malloc(sizeof(Line));
		Line* xline=(Line*)malloc(sizeof(Line));

		// 값 대입
		xline->low=0;
		yline->low=0;
		xline->high=0;
		yline->high=0;
		bbox->conf=0;
		bbox->name="";

		// 연결
		rect->xline=xline;
		rect->yline=yline;
		bbox->rect=rect;
		Node* vacantNode=createNode(bbox);
		insertNodeLast(nodeInfo,vacantNode);
	}
	return nodeInfo;
}
Node* deleteNodeAt(NodeInfo* nodeInfo,int idx){
	////printf("deleteNode %d idx\n",idx);
	// 범위 초과
	if(idx>=nodeInfo->size||idx<0||nodeInfo->size<=0){
		//printf("error : deleteNodeAt out of range size %d,idx %d\n",nodeInfo->size,idx);
		return NULL;
	}

	// head && tail 삭제할때
	Node* deleteNode;
	if(nodeInfo->size==1){
		deleteNode=nodeInfo->head;
		nodeInfo->head=NULL;
		nodeInfo->tail=NULL;
	}
	// head 삭제할때
	else if(idx==0){
		deleteNode=nodeInfo->head;
		deleteNode->next->prev=NULL;
		nodeInfo->head=deleteNode->next;
		deleteNode->next=NULL;
	}
	// tail 삭제할때
	else if(idx==nodeInfo->size-1){
		deleteNode=nodeInfo->tail;
		deleteNode->prev->next=NULL;
		nodeInfo->tail=deleteNode->prev;
		deleteNode->prev=NULL;
	}
	// 일반적인 경우
	else{
		Node* prevNode=nodeInfo->head;
		for(int i=0;i<idx-1;i++){
			prevNode=prevNode->next;
		}
		deleteNode=prevNode->next;
		deleteNode->prev->next=deleteNode->next;
		deleteNode->next->prev=deleteNode->prev;
		deleteNode->next=NULL;
		deleteNode->prev=NULL;
	}
	// 공통
	nodeInfo->size--;
	////printf("free node\n");
	/*
	free(deleteNode->bbox->rect->xline);
	free(deleteNode->bbox->rect->yline);
	free(deleteNode->bbox->rect);
	free(deleteNode->bbox);
	free(deleteNode);
	*/
	return deleteNode;
}
Node* deleteNodeLast(NodeInfo* nodeInfo){
	return deleteNodeAt(nodeInfo,nodeInfo->size-1);
}
void freeNode(Node* deleteNode){
	if(deleteNode==NULL){
		return;
	}
	//printf("Free Node %s\n",deleteNode->bbox->name);
	if(deleteNode->bbox->rect->xline!=NULL){
		free(deleteNode->bbox->rect->xline);
	}
	if(deleteNode->bbox->rect->yline!=NULL){
		free(deleteNode->bbox->rect->yline);
	}
	if(deleteNode->bbox->rect!=NULL){
		free(deleteNode->bbox->rect);
	}
	if(deleteNode->bbox!=NULL){
		free(deleteNode->bbox);	
	}
	if(deleteNode!=NULL){
		free(deleteNode);	
	}
}
void freeNodeInfo(NodeInfo* nodeInfo){
	if(nodeInfo==NULL){
		return;
	}
	while(nodeInfo->size!=0){
		Node* delNode=deleteNodeLast(nodeInfo);
		freeNode(delNode);
	}
	free(nodeInfo);
}
Node* atIdxNode(NodeInfo* nodeInfo,int idx){
	// 에러
	if(nodeInfo==NULL){
		//printf("error : atIdxNode nodeInfo NULL\n");
		return;
	}

	// 범위 초과
	if(idx>=nodeInfo->size||idx<0){
		//printf("error atIdxNode out of range\n");
		return NULL;
	}
	Node* node=nodeInfo->head;
	for(int i=0;i<idx;i++){
		if(node==NULL){
			//printf("error atIdxNode node NULL\n");
			return NULL;
		}
		node=node->next;
	}
	return node;
}
void leaveNodeAt(NodeInfo* nodeinfo,int idx){
	while(nodeinfo->size>idx){
		Node* delNode=deleteNodeLast(nodeinfo);
		if(delNode!=NULL){
			freeNode(delNode);
		}
	}
}
NodeInfo* copyNodeInfo(NodeInfo* nodeInfo){
	if(nodeInfo->size<0){
		return NULL;
	}
	NodeInfo* cpyNodeInfo=(NodeInfo*)malloc(sizeof(NodeInfo));
	initNodeInfo(cpyNodeInfo);
	for(Node* node=nodeInfo->head;node!=NULL;node=node->next){
		Node* newNode=copyNode(node);
		insertNodeLast(cpyNodeInfo,newNode);
	}
	return cpyNodeInfo;
}
NodeInfo* mergeNodeInfo(NodeInfo* nodeInfo1,NodeInfo* nodeInfo2){
	if(nodeInfo1->size<0||nodeInfo2->size<0){
		return NULL;
	}
	NodeInfo* cpyNodeInfo=(NodeInfo*)malloc(sizeof(NodeInfo));
	initNodeInfo(cpyNodeInfo);
	for(Node* node=nodeInfo1->head;node!=NULL;node=node->next){
		Node* newNode=copyNode(node);
		insertNodeLast(cpyNodeInfo,newNode);
	}
	for(Node* node=nodeInfo2->head;node!=NULL;node=node->next){
		Node* newNode=copyNode(node);
		insertNodeLast(cpyNodeInfo,newNode);
	}
	return cpyNodeInfo;
}
Node* copyNode(Node* node){
	if(node==NULL){
		return NULL;
	}
	Bbox* newBbox=(Bbox*)malloc(sizeof(Bbox));
	Rect* newRect=(Rect*)malloc(sizeof(Rect));
	Line* newLineX=(Line*)malloc(sizeof(Line));
	Line* newLineY=(Line*)malloc(sizeof(Line));
	char* name=(char*)malloc(sizeof(MAXCHARLEN));

	// Line
	newLineX->low=node->bbox->rect->xline->low;
	newLineY->low=node->bbox->rect->yline->low;
	newLineX->high=node->bbox->rect->xline->high;
	newLineY->high=node->bbox->rect->yline->high;
	
	// Rect
	newRect->xline=newLineX;
	newRect->yline=newLineY;

	// Bbox
	newBbox->rect=newRect;
	newBbox->conf=node->bbox->conf;
	strcpy(name,node->bbox->name);
	newBbox->name=name;

	// Node
	Node* newNode=createNode(newBbox);
	return newNode;
}
void subsNode(NodeInfo* destNodeInfo,int destIdx,NodeInfo* sourceNodeInfo,int sourceIdx){
	Node* sourceNode=atIdxNode(sourceNodeInfo,sourceIdx);
	if(sourceNode==NULL){
		//printf("subs error : NULL source node\n");
		return;
	}
	Node* destNode=copyNode(sourceNode);
	Node* delNode=deleteNodeAt(destNodeInfo,destIdx);
	if(delNode!=NULL){
		freeNode(delNode);
	}
	insertNodeAt(destNodeInfo,destNode,destIdx);
}
void subsOnlyNode(NodeInfo* destNodeInfo,int destIdx,Node* sourceNode){
	Node* destNode=copyNode(sourceNode);
	Node* delNode=deleteNodeAt(destNodeInfo,destIdx);
	if(delNode!=NULL){
		freeNode(delNode);
	}
	insertNodeAt(destNodeInfo,destNode,destIdx);
}
Bbox* getLocalResult() {
	////printf("Start getMaxConfBox\n");
	// iou/ios
	filterBbox(&lNodeInfo,LIOUTHRESH,LIOSTHRESH);
	////printf("finished filtering\n");
	////printf("filtered head node %s\n",lNodeInfo.head->bbox->name);
	// 결과로 쓸 박스
	Bbox* box=getMaxConfBox(&lNodeInfo);
	clearNodeInfo(&lNodeInfo);
	/*
	float maxConf = 0;
	for(Node* node=lNodeInfo.head;node!=NULL;node=node->next){
		////printf("conf %f\n",node->bbox->conf);
		if(node->bbox->conf>maxConf){
			maxConf=node->bbox->conf;
			box=node->bbox;
			////printf("box name %s\n",node->bbox->name);
		}
	}
	*/
	////printf("return box name %s\n",box->name);
	//printf("box : %s\n",box->name);
	return box;
}
Bbox* getMaxConfBox(NodeInfo* nodeInfo){
	if(nodeInfo->size<=0){
		return NULL;
	}
	float maxConf=0;
	Node* maxNode=NULL;
	int maxNodeIdx=-1;
	int idx=-1;
	for(Node* node=nodeInfo->head;node!=NULL;node=node->next){
		idx++;
		if(node->bbox->conf>maxConf){
			maxConf=node->bbox->conf;
			maxNodeIdx=idx;
		}
	}
	maxNode=deleteNodeAt(nodeInfo,maxNodeIdx);
	if(maxNode==NULL){
		return NULL;
	}
	//printf("maxNode : %s\n",maxNode->bbox->name);
	return maxNode->bbox;
}
char* getPlateStringDigitModel(char* recoPlateNum) {
	// 0. iou/ios 과정
	// filterBbox(&rNodeInfo,RIOUTHRESH,RIOSTHRESH);
	// 1. 박스 파싱 (7개의 박스만 나오게끔)
	NodeInfo* resNodeInfo=parseBboxes(&dNodeInfo);
	//printf("fin parse\n");

	for(Node* node=resNodeInfo->head;node!=NULL;node=node->next){
		////printf("r name %s\n", node->bbox->name);
		strcat(recoPlateNum,node->bbox->name);
	}
	clearNodeInfo(&dNodeInfo);
	////printf("recoPlateNum : %s\n", recoPlateNum);
	return recoPlateNum;
}
char* getPlateStringYOLO3(char* recoPlateNum) {
	// 0. iou/ios 과정
	// filterBbox(&rNodeInfo,RIOUTHRESH,RIOSTHRESH);
	// 1. 박스 파싱 (7개의 박스만 나오게끔)
	NodeInfo* resNodeInfo=parseBboxes(&y3NodeInfo);
	//printf("fin parse\n");

	for(Node* node=resNodeInfo->head;node!=NULL;node=node->next){
		////printf("r name %s\n", node->bbox->name);
		strcat(recoPlateNum,node->bbox->name);
	}
	clearNodeInfo(&y3NodeInfo);
	////printf("recoPlateNum : %s\n", recoPlateNum);
	return recoPlateNum;
}
char* getPlateString(char* recoPlateNum) {
	// 0. iou/ios 과정
	// filterBbox(&rNodeInfo,RIOUTHRESH,RIOSTHRESH);
	// 1. 박스 파싱 (7개의 박스만 나오게끔)
	NodeInfo* resNodeInfo=parseBboxes(&rNodeInfo);
	//printf("fin parse\n");

	for(Node* node=resNodeInfo->head;node!=NULL;node=node->next){
		////printf("r name %s\n", node->bbox->name);
		strcat(recoPlateNum,node->bbox->name);
	}
	clearNodeInfo(&rNodeInfo);
	////printf("recoPlateNum : %s\n", recoPlateNum);
	return recoPlateNum;
}
NodeInfo* parseBboxes(NodeInfo* nodeInfoOri) {
	//printf("parsing start\n");
	// nodeInfo == standardBboxes
	NodeInfo* nodeInfo=copyNodeInfo(nodeInfoOri);
	NodeInfo* standardBboxes=getVacantNodeInfo(LENOFSTANDARDBBOX);
	// 1. 박스들 카테고리 분류. 지역 한글 숫자로 구분.
	//NodeInfo* regionNodeInfo = (NodeInfo*)malloc(sizeof(NodeInfo));
	//NodeInfo* korcodeNodeInfo = (NodeInfo*)malloc(sizeof(NodeInfo));
	//NodeInfo* youngNodeInfo = (NodeInfo*)malloc(sizeof(NodeInfo));
	NodeInfo* digitNodeInfo = (NodeInfo*)malloc(sizeof(NodeInfo));
	//initNodeInfo(regionNodeInfo);
	//initNodeInfo(korcodeNodeInfo);
	//initNodeInfo(youngNodeInfo);
	initNodeInfo(digitNodeInfo);

	//float dupIouThresh=0.9;
	//float interlockIotThresh=0.7;
	//printf("check point 1\n");

	Node* maxConfRegionBox=NULL;
	Node* maxConfKorcodeBox=NULL;
	Node* maxConfYoungBox=NULL;

	Node* delNode=NULL;
	// 박스 분류
	int i=-1;
	for(Node* node=nodeInfo->head;node!=NULL;node=node->next){
		char* name=node->bbox->name;
		Node* tmpNode;
		i++;
		/*
		printf("idx : %d,name : %s,%.f %.f %.f %.f .\n",i,node->bbox->name
		,node->bbox->rect->xline->low,node->bbox->rect->yline->low
		,node->bbox->rect->xline->high,node->bbox->rect->yline->high);
		*/
		// region 인지
		if(isRegion(name)){
			if(maxConfRegionBox==NULL){
				maxConfRegionBox=node;
			}
			else if(node->bbox->conf>maxConfRegionBox->bbox->conf){
				maxConfRegionBox=node;
			}
		}
		// young(영업) 인지
		else if(isYoung(name)){
			if(maxConfYoungBox==NULL){
				maxConfYoungBox=node;
			}
			else if(node->bbox->conf>maxConfYoungBox->bbox->conf){
				maxConfYoungBox=node;
			}
		}
		// hangul 인지
		else if(isHangul(name)){
			if(maxConfKorcodeBox==NULL){
				maxConfKorcodeBox=node;
			}
			else if(node->bbox->conf>maxConfKorcodeBox->bbox->conf){
				maxConfKorcodeBox=node;
			}
		}
		// digit 인지
		else if(isDigit(name)){
			tmpNode=copyNode(atIdxNode(nodeInfo,i));
			insertNodeLast(digitNodeInfo,tmpNode);
		}
	}
	
	//printf("check point 2\n");

	// region 처리
	if(maxConfRegionBox!=NULL){
		//printf("check point 3\n");

		bool isRegion=true;

		// yline > xline 세로로 길쭉(portrait)
		if(lineLength(maxConfRegionBox->bbox->rect->yline)>lineLength(maxConfRegionBox->bbox->rect->xline)){
			// 박스 x좌표가 왼쪽에 없을때(비율을 넘어갈때)
			if(maxConfRegionBox->bbox->rect->xline->low/IMGWIDTH>PORTREGIONX){
				isRegion=false;
			}
			// yline > xline*3
			if(lineLength(maxConfRegionBox->bbox->rect->yline)>lineLength(maxConfRegionBox->bbox->rect->xline)*3){
				isRegion=false;
			}
		}
		// landscape 일때 y 센터값이 위쪽에 없을때(일정비율을 넘기면)
		else if(getCenter(maxConfRegionBox->bbox->rect->yline)/IMGHEIGHT<LANDREGIONY){
			isRegion=false;
		}

		if(!isRegion){
			maxConfRegionBox=NULL;
		}
		else{
			subsOnlyNode(standardBboxes,REGIONIDX,maxConfRegionBox);
		}
	}
	// young 처리
	if(maxConfYoungBox!=NULL){
		//printf("check point 4\n");

		bool isYoung=true;
		if(getCenter(maxConfYoungBox->bbox->rect->xline)/IMGWIDTH>SUBSTYOUNGX
		||getCenter(maxConfYoungBox->bbox->rect->yline)/IMGHEIGHT>SUBSTYOUNGY){
			isYoung=false;
		}
		if(!isYoung){
			maxConfYoungBox=NULL;
		}
		else{
			subsOnlyNode(standardBboxes,YOUNGIDX,maxConfYoungBox);
		}
	}
	// korcode 처리
	if(maxConfKorcodeBox!=NULL){
		//printf("check point 5\n");

		subsOnlyNode(standardBboxes,KORCODEIDX,maxConfKorcodeBox);
	}
	//printf("before process digit standard box\n");
	for(int i=0;i<standardBboxes->size;i++){
		//printf("st box %d : %s.\n",i,atIdxNode(standardBboxes,i)->bbox->name);
	}
	// digit 처리
	if(digitNodeInfo->size>0){
		//printf("check point 6\n");

		//filterBbox(digitNodeInfo,RIOUTHRESH,RIOSTHRESH);
		sortBboxByConfR(digitNodeInfo,0,digitNodeInfo->size-1);
		/*
		for(int i=0;i<digitNodeInfo->size;i++){
			Node* node=atIdxNode(digitNodeInfo,i);
			printf("Idx %d : name : %s, conf : %f\n",i,node->bbox->name,node->bbox->conf);
		}
		*/
		// head 박스 3개
		NodeInfo* headBboxes=getVacantNodeInfo(3);
		// tail 박스 4개
		NodeInfo* tailBboxes=getVacantNodeInfo(4);

		bool new2019Candidate=false;

		// 숫자가 4개 이하
		if(digitNodeInfo->size<=4){
			//printf("check point 7\n");
			tailBboxes=copyNodeInfo(digitNodeInfo);
			sortBboxByXlow(tailBboxes,0,tailBboxes->size-1);
		}
		// 숫자가 5개 이상
		else{
			//printf("check point 8\n");
			if(digitNodeInfo->size>=7){
				//printf("check point 9\n");

				// 지역명이 있으면 6개만 남겨둠(구형)
				if(maxConfRegionBox!=NULL){
					leaveNodeAt(digitNodeInfo,6);
				}
				// 지역명이 없으면 7개까지 남겨둠(신형)
				else{
					leaveNodeAt(digitNodeInfo,7);
				}
			}
			//printf("check point 10\n");
			NodeInfo* confSortDigits=copyNodeInfo(digitNodeInfo);
			//NodeInfo* areaSortDigits=copyNodeInfo(digitNodeInfo);
			NodeInfo* xSortDigits=copyNodeInfo(digitNodeInfo);
			NodeInfo* ySortDigits=copyNodeInfo(digitNodeInfo);

			sortBboxByConfR(confSortDigits,0,confSortDigits->size-1);
			sortBboxByXlow(xSortDigits,0,xSortDigits->size-1);
			/*
			printf("xSortDigits\n");
			for(int i=0;i<xSortDigits->size;i++){
				printf("X sort box %d : %s,xmin %f.\n",i,atIdxNode(xSortDigits,i)->bbox->name,atIdxNode(xSortDigits,i)->bbox->rect->xline->low);
			}
			*/
			sortBboxByYlow(ySortDigits,0,ySortDigits->size-1);
			/*
			printf("ySortDigits\n");
			for(int i=0;i<ySortDigits->size;i++){
				//printf("Y sort box %d : %s,ymin %f.\n",i,atIdxNode(ySortDigits,i)->bbox->name,atIdxNode(ySortDigits,i)->bbox->rect->yline->low);
			}
			*/
			NodeInfo* confHighDigit7;
			NodeInfo* headDigitNew=(NodeInfo*)malloc(sizeof(NodeInfo));
			NodeInfo* tailDigitNew=(NodeInfo*)malloc(sizeof(NodeInfo));

			initNodeInfo(headDigitNew);
			initNodeInfo(tailDigitNew);

			// 숫자 7개 일때(신형 포함)
			if(digitNodeInfo->size==7){
				// 한글이 들어가있으면
				if(maxConfKorcodeBox!=NULL){
					for(Node* node=xSortDigits->head;node!=NULL;node=node->next){
						// 왼쪽에 
						if(getCenter(node->bbox->rect->xline)<maxConfKorcodeBox->bbox->rect->xline->low){
							Node* newNode=copyNode(node);
							insertNodeLast(headDigitNew,newNode);
						}
						else if(getCenter(node->bbox->rect->xline)>maxConfKorcodeBox->bbox->rect->xline->high){
							Node* newNode=copyNode(node);
							insertNodeLast(tailDigitNew,newNode);
						}
					}
					if(headDigitNew->size==3&&tailDigitNew->size==4){
						//printf("headDigitNew+tailDigitNew\n");
						confHighDigit7=mergeNodeInfo(headDigitNew,tailDigitNew);
						sortBboxByConfR(confHighDigit7,0,confHighDigit7->size-1);
						new2019Candidate=true;
					}
				}
			}

			int xOverlapCnt=0;
			int yOverlapCnt=0;
			int topYOverLapCnt=0;
			bool isOverText=false;
			
			float botY=atIdxNode(ySortDigits,0)->bbox->rect->yline->low;
			float topY=atIdxNode(ySortDigits,ySortDigits->size-1)->bbox->rect->yline->high;
			int midY=(int)((topY+botY)/2); // round 뺌
			int lowerCnt=0;
			//printf("check point 11\n");
			for(Node* node=ySortDigits->head;node!=NULL;node=node->next){
				if(node->bbox->rect->yline->high>midY){
					lowerCnt++;
				}
			}
			// region이 중앙보다 위에 있다면 true
			if(maxConfRegionBox!=NULL){
				if(maxConfRegionBox->bbox->rect->yline->high<midY){
					isOverText=true;
				}
			}
			// korcode가 중앙보다 위에 있다면 true
			if(maxConfKorcodeBox!=NULL){
				if(maxConfKorcodeBox->bbox->rect->yline->high<midY){
					isOverText=true;
				}
			}
			for(int i=0;i<2;i++){
				for(int j=0;j<ySortDigits->size;j++){
					if(i==j){
						continue;
					}
					if((int)getCenter(atIdxNode(ySortDigits,i)->bbox->rect->xline)
					>(int)atIdxNode(ySortDigits,j)->bbox->rect->xline->low
					&&(int)getCenter(atIdxNode(ySortDigits,i)->bbox->rect->xline)
					<(int)atIdxNode(ySortDigits,j)->bbox->rect->xline->high){
						xOverlapCnt++;
					}
				}
			}
			for(int i=0;i<2;i++){
				for(int j=0;j<ySortDigits->size;j++){
					if(i==j){
						continue;
					}
					if((int)getCenter(atIdxNode(ySortDigits,i)->bbox->rect->yline)
					>(int)atIdxNode(ySortDigits,j)->bbox->rect->yline->low
					&&(int)getCenter(atIdxNode(ySortDigits,i)->bbox->rect->yline)
					<(int)atIdxNode(ySortDigits,j)->bbox->rect->yline->high){
						yOverlapCnt++;
					}
				}
			}
			for(int i=0;i<ySortDigits->size;i++){
				int lastIdx=ySortDigits->size-1;
				if(i==lastIdx){
					continue;
				}
				if((int)getCenter(atIdxNode(ySortDigits,lastIdx)->bbox->rect->yline)
				>(int)atIdxNode(ySortDigits,i)->bbox->rect->yline->low
				&&(int)getCenter(atIdxNode(ySortDigits,lastIdx)->bbox->rect->yline)
				<(int)atIdxNode(ySortDigits,i)->bbox->rect->yline->high){
					topYOverLapCnt++;
				}
			}
			//printf("lowerCnt %d\n",lowerCnt);
			//printf("xOverlapCnt %d\n",xOverlapCnt);
			//printf("yOverlapCnt %d\n",yOverlapCnt);
			//printf("topYOverLapCnt %d\n",topYOverLapCnt);
			//printf("isOverText %d\n",isOverText);
			//printf("check point 12\n");
			// 1줄 일때
			if(lowerCnt>=5&&yOverlapCnt>=8&&!isOverText){
				//printf("1줄 일때\n");
				// 신규번호판이 아닐때 5 or 6
				if(!new2019Candidate){
					// 1줄 번호 6개
					if(xSortDigits->size==6){
						subsNode(headBboxes,1,xSortDigits,0);
						subsNode(headBboxes,2,xSortDigits,1);
						subsNode(tailBboxes,0,xSortDigits,2);
						subsNode(tailBboxes,1,xSortDigits,3);
						subsNode(tailBboxes,2,xSortDigits,4);
						subsNode(tailBboxes,3,xSortDigits,5);
					}
					// 1줄 번호 5개, 1개 에러
					else{
						// head 번호 1개 미스
						if(atIdxNode(xSortDigits,1)->bbox->rect->xline->high>IMGWIDTH*0.5){
							float xLimit=getCenter(atIdxNode(xSortDigits,1)->bbox->rect->xline)*2
							-atIdxNode(xSortDigits,3)->bbox->rect->xline->high;
							if(xLimit>=atIdxNode(xSortDigits,0)->bbox->rect->xline->low
							&&xLimit<=atIdxNode(xSortDigits,0)->bbox->rect->xline->high){
								subsNode(headBboxes,2,xSortDigits,0);
							}
							else{
								subsNode(headBboxes,1,xSortDigits,0);
							}
							subsNode(tailBboxes,0,xSortDigits,1);
							subsNode(tailBboxes,1,xSortDigits,2);
							subsNode(tailBboxes,2,xSortDigits,3);
							subsNode(tailBboxes,3,xSortDigits,4);
						}
						// tail 번호 1개 미스
						else{
							subsNode(headBboxes,1,xSortDigits,0);
							subsNode(headBboxes,2,xSortDigits,1);
							
							LOCTYPE xGap0=getGap(atIdxNode(xSortDigits,2)->bbox,atIdxNode(xSortDigits,3)->bbox);
							LOCTYPE xGap1=getGap(atIdxNode(xSortDigits,3)->bbox,atIdxNode(xSortDigits,4)->bbox);
							LOCTYPE avgW=((lineLength(atIdxNode(xSortDigits,2)->bbox->rect->xline))
							+(lineLength(atIdxNode(xSortDigits,3)->bbox->rect->xline))
							+(lineLength(atIdxNode(xSortDigits,4)->bbox->rect->xline)))/3.0;
							
							// OXOO case
							if(xGap0>avgW*0.8){
								subsNode(tailBboxes,0,xSortDigits,2);
								subsNode(tailBboxes,2,xSortDigits,3);
								subsNode(tailBboxes,3,xSortDigits,4);
							}
							else{
								// OOXO case
								if(xGap1>avgW*0.8){
									subsNode(tailBboxes,0,xSortDigits,2);
									subsNode(tailBboxes,1,xSortDigits,3);
									subsNode(tailBboxes,3,xSortDigits,4);
								}
								else{
									LOCTYPE midAreaXmin=IMGWIDTH/2.0-avgW*0.6;
									LOCTYPE midAreaXmax=IMGWIDTH/2.0-avgW*0.6;
									// OOOX case
									if(getCenter(atIdxNode(xSortDigits,2)->bbox->rect->xline)>=midAreaXmin
									&&getCenter(atIdxNode(xSortDigits,2)->bbox->rect->xline)<=midAreaXmax){
										subsNode(tailBboxes,0,xSortDigits,2);
										subsNode(tailBboxes,1,xSortDigits,3);
										subsNode(tailBboxes,2,xSortDigits,4);
									}
									// XOOO case
									else{
										subsNode(tailBboxes,1,xSortDigits,2);
										subsNode(tailBboxes,2,xSortDigits,3);
										subsNode(tailBboxes,3,xSortDigits,4);
									}
								}
							}
						}
					}
				}
				// 신규번호판 일때 2019 new plate
				else{
					NodeInfo* ySortHigh7=copyNodeInfo(confHighDigit7);
					NodeInfo* xSortHigh7=copyNodeInfo(confHighDigit7);
					sortBboxByYlow(ySortHigh7,0,ySortHigh7->size-1);
					sortBboxByXlow(xSortHigh7,0,xSortHigh7->size-1);

					LOCTYPE firstDigitW=lineLength(atIdxNode(xSortHigh7,0)->bbox->rect->xline);
					LOCTYPE firstDigitH=lineLength(atIdxNode(xSortHigh7,0)->bbox->rect->yline);
					LOCTYPE firstDigitCenterY=getCenter(atIdxNode(xSortHigh7,0)->bbox->rect->yline);
					LOCTYPE midBBCenterY=getCenter(atIdxNode(ySortHigh7,4)->bbox->rect->yline);
					LOCTYPE midBBHeight=lineLength(atIdxNode(ySortHigh7,4)->bbox->rect->yline);

					if(NEW2019PLATE){
						subsNode(headBboxes,0,xSortHigh7,0);
						subsNode(headBboxes,1,xSortHigh7,1);
						subsNode(headBboxes,2,xSortHigh7,2);

						subsNode(tailBboxes,0,xSortHigh7,3);
						subsNode(tailBboxes,1,xSortHigh7,4);
						subsNode(tailBboxes,2,xSortHigh7,5);
						subsNode(tailBboxes,3,xSortHigh7,6);
					}
					// 신규번호판 x 첫번째 숫자 버림
					else{
						subsNode(headBboxes,1,xSortHigh7,1);
						subsNode(headBboxes,2,xSortHigh7,2);

						subsNode(tailBboxes,0,xSortHigh7,3);
						subsNode(tailBboxes,1,xSortHigh7,4);
						subsNode(tailBboxes,2,xSortHigh7,5);
						subsNode(tailBboxes,3,xSortHigh7,6);
					}
					//freeNodeInfo(ySortHigh7);
					//freeNodeInfo(xSortHigh7);
				}
			}
			// 2줄 일때
			else{
				//printf("2줄 일때\n");
				// 슷자 7개 이상
				if(confSortDigits->size>=7){
					//printf("confSrotDigits 7\n");
					// 6개만 남겨두고 나머지 제거
					while(confSortDigits->size==6){
						delNode=deleteNodeAt(confSortDigits,6);
						freeNode(delNode);
					}
					sortBboxByConfR(confSortDigits,0,confSortDigits->size-1);
				}
				// 숫자 5개 일때
				if(ySortDigits->size==5){
					//printf("ySortDigits 5\n");
					// 1줄 숫자가 2개가 아닐때
					if(topYOverLapCnt<=1){
						Node* upperBbox=atIdxNode(ySortDigits,0);
						NodeInfo* lowerBboxes=copyNodeInfo(ySortDigits);
						tailBboxes=copyNodeInfo(ySortDigits);

						delNode=deleteNodeAt(lowerBboxes,0);
						if(delNode!=NULL){
							freeNode(delNode);
						}
						delNode=deleteNodeAt(tailBboxes,0);
						if(delNode!=NULL){
							freeNode(delNode);
						}
						sortBboxByXlow(lowerBboxes,0,lowerBboxes->size-1);
						sortBboxByXlow(tailBboxes,0,tailBboxes->size-1);

						if(maxConfRegionBox!=NULL){
							LOCTYPE estRegionX=upperBbox->bbox->rect->xline->low-lineLength(upperBbox->bbox->rect->xline);
							if(maxConfRegionBox->bbox->rect->xline->high>estRegionX){
								/*
								delNode=deleteNodeAt(headBboxes,1);
								freeNode(delNode);
								insertNodeAt(headBboxes,1,upperBbox);
								*/
								subsNode(headBboxes,1,ySortDigits,0);
							}
							else{
								/*
								delNode=deleteNodeAt(headBboxes,2);
								freeNode(delNode);
								insertNodeAt(headBboxes,2,upperBbox);
								*/
								subsNode(headBboxes,2,ySortDigits,0);
							}
						}
						//freeNodeInfo(lowerBboxes);
					}
					// 윗줄 2개 아랫줄 3개
					else{
						NodeInfo* upperBboxes=(NodeInfo*)malloc(sizeof(NodeInfo)); // 2개
						NodeInfo* lowerBboxes=(NodeInfo*)malloc(sizeof(NodeInfo)); // 3개
						initNodeInfo(upperBboxes);
						initNodeInfo(lowerBboxes);
						subsNode(upperBboxes,0,ySortDigits,0);
						subsNode(upperBboxes,1,ySortDigits,1);
						subsNode(lowerBboxes,0,ySortDigits,2);
						subsNode(lowerBboxes,1,ySortDigits,3);
						subsNode(lowerBboxes,2,ySortDigits,4);
						
						subsNode(headBboxes,1,upperBboxes,0);
						subsNode(headBboxes,2,upperBboxes,1);

						LOCTYPE xGap0=getGap(atIdxNode(lowerBboxes,0)->bbox,atIdxNode(lowerBboxes,1)->bbox);
						LOCTYPE xGap1=getGap(atIdxNode(lowerBboxes,1)->bbox,atIdxNode(lowerBboxes,2)->bbox);

						LOCTYPE avgW=((lineLength(atIdxNode(lowerBboxes,0)->bbox->rect->xline))
							+(lineLength(atIdxNode(lowerBboxes,1)->bbox->rect->xline))
							+(lineLength(atIdxNode(lowerBboxes,2)->bbox->rect->xline)))/3.0;

						// OXOO case
						if(xGap0>avgW*0.8){
							subsNode(tailBboxes,0,lowerBboxes,0);
							subsNode(tailBboxes,2,lowerBboxes,1);
							subsNode(tailBboxes,3,lowerBboxes,2);
						}
						else{
						// OOXO case
							if(xGap1>avgW*0.8){
								subsNode(tailBboxes,0,lowerBboxes,0);
								subsNode(tailBboxes,1,lowerBboxes,1);
								subsNode(tailBboxes,3,lowerBboxes,2);
							}
							else{
								LOCTYPE estTail1CtrX;
								// OOOX case
								if(getCenter(atIdxNode(upperBboxes,0)->bbox->rect->xline)>IMGWIDTH/2){
									estTail1CtrX=atIdxNode(upperBboxes,0)->bbox->rect->xline->low
									-lineLength(atIdxNode(upperBboxes,0)->bbox->rect->xline);
								}
								// XOOO case
								else{
									estTail1CtrX=atIdxNode(upperBboxes,0)->bbox->rect->xline->low
									-lineLength(atIdxNode(upperBboxes,0)->bbox->rect->xline)*2;
								}
								if (estTail1CtrX>=atIdxNode(lowerBboxes,0)->bbox->rect->xline->low
								&&estTail1CtrX<=atIdxNode(lowerBboxes,0)->bbox->rect->xline->high){
									subsNode(tailBboxes,0,lowerBboxes,0);
									subsNode(tailBboxes,1,lowerBboxes,1);
									subsNode(tailBboxes,2,lowerBboxes,2);
								}
								else{
									subsNode(tailBboxes,1,lowerBboxes,0);
									subsNode(tailBboxes,2,lowerBboxes,1);
									subsNode(tailBboxes,3,lowerBboxes,2);
								}
							}
						}
						//freeNodeInfo(upperBboxes);
						//freeNodeInfo(lowerBboxes);
					}
				}
				// 숫자 6개 일때
				else if(ySortDigits->size==6){
					NodeInfo* upperBboxes=getVacantNodeInfo(2); // 2개
					NodeInfo* lowerBboxes=getVacantNodeInfo(4); // 4개
					// 2줄인데 지역명만 있고 1줄에 숫자가 6개일때
					if(xOverlapCnt==0){
						subsNode(headBboxes,1,xSortDigits,0);
						subsNode(headBboxes,2,xSortDigits,1);
						for(int idx=0;idx<tailBboxes->size;idx++){
							subsNode(tailBboxes,idx,xSortDigits,idx+2);
						}
					}
					else{
						subsNode(upperBboxes,0,ySortDigits,0);
						subsNode(upperBboxes,1,ySortDigits,1);
						subsNode(lowerBboxes,0,ySortDigits,2);
						subsNode(lowerBboxes,1,ySortDigits,3);
						subsNode(lowerBboxes,2,ySortDigits,4);
						subsNode(lowerBboxes,3,ySortDigits,5);
						
						sortBboxByXlow(upperBboxes,0,upperBboxes->size-1);
						sortBboxByXlow(lowerBboxes,0,lowerBboxes->size-1);

						subsNode(headBboxes,1,upperBboxes,0);
						subsNode(headBboxes,2,upperBboxes,1);

						for(int idx=0;idx<tailBboxes->size;idx++){
							subsNode(tailBboxes,idx,lowerBboxes,idx);
						}
					}
					//freeNodeInfo(upperBboxes);
					//freeNodeInfo(lowerBboxes);
				}
			}
			/*
			freeNodeInfo(headDigitNew);
			freeNodeInfo(tailDigitNew);
			freeNodeInfo(confHighDigit7);
			freeNodeInfo(confSortDigits);
			freeNodeInfo(xSortDigits);
			freeNodeInfo(ySortDigits);
			*/
		}
		//printf("sort head\n");
		//printf("head size : %d\n",headBboxes->size);
		//printf("tail size : %d\n",tailBboxes->size);
		//printf("standardBboxes size %d\n",standardBboxes->size);
		for(int idx=0;idx<headBboxes->size;idx++){ // 1 2 3
			subsNode(standardBboxes,idx+DIGITHEADIDX,headBboxes,idx);
		}
		for(int i=0;i<standardBboxes->size;i++){
			//printf("st %d : %s.\n",i,atIdxNode(standardBboxes,i)->bbox->name);
		}
		//printf("sort tail\n");
		for(int idx=0;idx<tailBboxes->size;idx++){ // 5 6 7 8
			if(idx==tailBboxes->size-1){
				for(int i=0;i<standardBboxes->size;i++){
					//printf("st %d : %s.\n",i,atIdxNode(standardBboxes,i)->bbox->name);
				}		
			}
			subsNode(standardBboxes,idx+DIGITTAILIDX,tailBboxes,idx);
			//printf("subsed st %d : %s.\n",idx+DIGITTAILIDX,atIdxNode(standardBboxes,idx+DIGITTAILIDX)->bbox->name);
		}
		for(int i=0;i<standardBboxes->size;i++){
			//printf("st %d : %s.\n",i,atIdxNode(standardBboxes,i)->bbox->name);
		}
		//freeNodeInfo(headBboxes);
		//freeNodeInfo(tailBboxes);
	}
	//freeNodeInfo(nodeInfo);
	//freeNodeInfo(digitNodeInfo);

	return standardBboxes;
}
bool isRegion(char* name){
	static const char region[REGIONNUM][MAXCHARLEN]={"seoul", "daegu", "gwangju", "ulsan", "gyeonggi"
	, "chungbuk", "jeonbuk", "gyeongbuk", "jeju", "busan", "incheon"
	, "daejeon", "sejong", "gangwon", "chungnam", "jeonnam", "gyeongnam"};
	for(int i=0;i<REGIONNUM;i++){
		if(strcmp(name,region[i])==0){
			return true;
		}
		// digit model용 area
		if(strcmp(name,"area")==0){
			return true;
		}
	}
	return false;
}
bool isHangul(char* name){
	static const char region[HANGULNUM][MAXCHARLEN]={"ga", "na", "da", "ra", "ma", "ba", "sa"
	, "aa", "ja", "cha", "ka", "ta", "pa", "ha"
	, "goe", "noe", "doe", "roe", "moe", "boe", "soe", "aoe"
	, "joe", "choe", "koe", "toe", "poe", "hoe", "go"
	, "no", "do", "ro", "mo", "bo", "so", "oo", "jo", "cho"
	, "ko", "to", "po", "ho", "gu", "nu", "du", "ru", "mu"
	, "bu", "su", "wu", "ju", "chu", "ku", "tu"
	, "pu", "hu", "bae", "-", "ke", "oikyo", "ge"
	, "ne", "de", "re", "me", "be", "se", "oe", "je"
	, "ch", "te", "pe", "he", "yuk", "hae", "gong", "guk", "hap"};
	for(int i=0;i<HANGULNUM;i++){
		if(strcmp(name,region[i])==0){
			return true;
		}
		// digit model용 code
		if(strcmp(name,"code")==0){
			return true;
		}
	}
	return false;
}
bool isYoung(char* name){
	static const char region[YOUNGNUM][MAXCHARLEN]={"young"};
	for(int i=0;i<YOUNGNUM;i++){
		if(strcmp(name,region[i])==0){
			return true;
		}
	}
	return false;
}
bool isDigit(char* name){
	static const char region[DIGITNUM][MAXCHARLEN]={"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
	for(int i=0;i<DIGITNUM;i++){
		if(strcmp(name,region[i])==0){
			return true;
		}
	}
	return false;
}
void setBbox(int idx,int label, LOCTYPE xmin,LOCTYPE ymin,LOCTYPE xmax,LOCTYPE ymax, float conf,char type){
	// l 모델일때
	if(type=='l'){
		if(idx==0){
			initNodeInfo(&lNodeInfo);
		}
		// 동적할당
		Bbox* bbox=(Bbox*)malloc(sizeof(Bbox));
		Rect* rect=(Rect*)malloc(sizeof(Rect));
		Line* yline=(Line*)malloc(sizeof(Line));
		Line* xline=(Line*)malloc(sizeof(Line));

		// 값 대입
		xline->low=xmin;
		yline->low=ymin;
		xline->high=xmax;
		yline->high=ymax;
		bbox->conf=conf;
		bbox->name=l_labelMap[label];

		// 연결
		rect->xline=xline;
		rect->yline=yline;
		bbox->rect=rect;
		Node* node=createNode(bbox);
		/*
		printf("name : %s, conf : %.4f, box info : %.0f %.0f %.0f %.0f\n"
		,node->bbox->name
		,node->bbox->conf
		,node->bbox->rect->xline->low
		,node->bbox->rect->yline->low
		,node->bbox->rect->xline->high
		,node->bbox->rect->yline->high);
		*/
		insertNodeLast(&lNodeInfo,node);
	}
	// r 모델일때
	else if(type=='r'){
		if(idx==0){
			initNodeInfo(&rNodeInfo);
		}
		// 동적할당
		Bbox* bbox=(Bbox*)malloc(sizeof(Bbox));
		Rect* rect=(Rect*)malloc(sizeof(Rect));
		Line* yline=(Line*)malloc(sizeof(Line));
		Line* xline=(Line*)malloc(sizeof(Line));

		// 값 대입
		xline->low=xmin;
		yline->low=ymin;
		xline->high=xmax;
		yline->high=ymax;
		bbox->conf=conf;
		bbox->name=r_labelMap[label];

		// 연결
		rect->xline=xline;
		rect->yline=yline;
		bbox->rect=rect;

		Node* node=createNode(bbox);
		/*
		printf("name : %s, conf : %.4f,  box info : %.0f %.0f %.0f %.0f\n"
		,node->bbox->name
		,node->bbox->conf
		,node->bbox->rect->xline->low
		,node->bbox->rect->yline->low
		,node->bbox->rect->xline->high
		,node->bbox->rect->yline->high);
		*/
		insertNodeLast(&rNodeInfo,node);
	}
	// d 모델일때
	else if(type=='d'){
		if(idx==0){
			initNodeInfo(&dNodeInfo);
		}
		// 동적할당
		Bbox* bbox=(Bbox*)malloc(sizeof(Bbox));
		Rect* rect=(Rect*)malloc(sizeof(Rect));
		Line* yline=(Line*)malloc(sizeof(Line));
		Line* xline=(Line*)malloc(sizeof(Line));

		// 값 대입
		xline->low=xmin;
		yline->low=ymin;
		xline->high=xmax;
		yline->high=ymax;
		bbox->conf=conf;
		bbox->name=d_labelMap[label];

		// 연결
		rect->xline=xline;
		rect->yline=yline;
		bbox->rect=rect;

		Node* node=createNode(bbox);
		/*
		printf("name : %s, conf : %.4f,  box info : %.0f %.0f %.0f %.0f\n"
		,node->bbox->name
		,node->bbox->conf
		,node->bbox->rect->xline->low
		,node->bbox->rect->yline->low
		,node->bbox->rect->xline->high
		,node->bbox->rect->yline->high);
		*/
		insertNodeLast(&dNodeInfo,node);
	}
	// yolo3 모델일때
	else if(type=='y'){
		if(idx==0){
			initNodeInfo(&y3NodeInfo);
		}
		// 동적할당
		Bbox* bbox=(Bbox*)malloc(sizeof(Bbox));
		Rect* rect=(Rect*)malloc(sizeof(Rect));
		Line* yline=(Line*)malloc(sizeof(Line));
		Line* xline=(Line*)malloc(sizeof(Line));

		// 값 대입
		xline->low=xmin;
		yline->low=ymin;
		xline->high=xmax;
		yline->high=ymax;
		bbox->conf=conf;
		bbox->name=y3_labelMap[label];

		// 연결
		rect->xline=xline;
		rect->yline=yline;
		bbox->rect=rect;

		Node* node=createNode(bbox);
		
		printf("name : %s, conf : %.4f,  box info : %.0f %.0f %.0f %.0f\n"
		,node->bbox->name
		,node->bbox->conf
		,node->bbox->rect->xline->low
		,node->bbox->rect->yline->low
		,node->bbox->rect->xline->high
		,node->bbox->rect->yline->high);
		
		insertNodeLast(&y3NodeInfo,node);
	}
}
Line* lineIntersection(Line *line1,Line *line2){
	////printf("lineInterSection\n");
	Line *tmp=(Line*)malloc(sizeof(Line));
	////printf("line1 low : %f, high : %f\n",line1->low,line1->high);
	////printf("line2 low : %f, high : %f\n",line2->low,line2->high);
	// line2.low가 더 크게 바꿈.
	if(line1->low>line2->low){
		// change
		tmp=line2;
		line2=line1;
		line1=tmp;
	}
	
	// 교차점이 없으면 00으로 넘김
	if(line1->high<line2->low){
		tmp->low=0;
		tmp->high=0;
		return tmp;
	}
	////printf("calc low high\n");
	int low=line2->low;
	int high=line1->high<line2->high?line1->high:line2->high;

	////printf("setLine\n");
	setLine(tmp,low,high);

	return tmp;
}
Rect* rectIntersection(Rect *rect1,Rect *rect2){
	////printf("rectIntersection\n");
	Rect *rect=malloc(sizeof(Rect));

	Line *xline=lineIntersection(rect1->xline,rect2->xline);
	Line *yline=lineIntersection(rect1->yline,rect2->yline);

	if(xline==NULL||yline==NULL){
		return;
	}
	
	////printf("setRect\n");
	setRect(rect,xline,yline);

	return rect;
}
LOCTYPE lineIou(Line *line1,Line *line2){
	Line* intersection=lineIntersection(line1,line2);

	if(intersection==NULL){
		return 0;
	}
	float deno=lineLength(line1)+lineLength(line2)-lineLength(intersection);
	if(deno==0){
		//printf("divided zero\n");
		return 0;
	}
	LOCTYPE result=lineLength(intersection)/deno;
	free(intersection);
	return result;
}
LOCTYPE lineIos(Line *line1,Line *line2){
	Line* intersection=lineIntersection(line1,line2);

	if(intersection==NULL){
		return 0;
	}
	Line* small = (lineLength(line1)<lineLength(line2))?&line1:&line2;
	float deno=lineLength(small);
	if(deno==0){
		//printf("divided zero\n");
		return 0;
	}
	LOCTYPE result=lineLength(intersection)/deno;
	free(intersection);
	return result;
}
LOCTYPE rectIou(Rect *rect1,Rect *rect2){
	////printf("Rect Iou\n");
	Rect* intersection=rectIntersection(rect1,rect2);
	if(intersection==NULL){
		return 0;
	}
	////printf("calc rect result\n");
	////printf("rect1 %f\n",rectArea(rect1));
	////printf("rect2 %f\n",rectArea(rect2));
	////printf("intersection xmin : %f, xmax : %f\n",intersection->xline->low,intersection->xline->high);
	////printf("intersection ymin : %f, ymax : %f\n",intersection->yline->low,intersection->yline->high);

	////printf("inter %f\n",rectArea(intersection));
	float deno=rectArea(rect1)+rectArea(rect2)-rectArea(intersection);
	if(deno==0){
		//printf("divided zero\n");
		return 0;
	}
	////printf("result\n");
	LOCTYPE result = rectArea(intersection)/deno;
	free(intersection);
	return result;
}
LOCTYPE rectIos(Rect *rect1,Rect *rect2){
	Rect* intersection=rectIntersection(rect1,rect2);

	if(intersection==NULL){
		return 0;
	}
	Rect* small = (rectArea(rect1)<rectArea(rect2))?&rect1:&rect2;
	float deno=rectArea(small);
	if(deno==0){
		//printf("divided zero\n");
		return 0;
	}
	LOCTYPE result = rectArea(intersection)/deno;
	free(intersection);
	return result;
}
LOCTYPE rectIot(Rect *rect1,Rect *targetRect){
	Rect* intersection=rectIntersection(rect1,targetRect);

	if(intersection==NULL){
		return 0;
	}
	float deno=rectArea(targetRect);
	if(deno==0){
		//printf("divided zero\n");
		return 0;
	}
	LOCTYPE result = rectArea(intersection)/deno;
	free(intersection);
	return result;
}
void filterBbox(NodeInfo* nodeInfo,float iouThresh,float iosThresh){
	////printf("Start filterBbox\n");
	////printf("bbox 0 %s\n",bboxes[0]->name);
	filterBboxIou(nodeInfo,iouThresh);
	////printf("finished Iou\n");
	// 반환이 제대로 안된거같으니 반환값을 변경해보자.
	////printf("filteredBbox name %s\n",bboxes[0].name);
	//*filteredBbox=filterBboxIos(bboxes,boxSize,iosThresh);
	////printf("finished Ios\n");
	//return filteredBbox;
}
void filterBboxIou(NodeInfo* nodeInfo,float iouThresh){
	////printf("Start filterBboxIou\n");
	// sort filteredBboxes by xlow
	////printf("Sort Bbox\n");
	sortBboxByXlow(nodeInfo,0,nodeInfo->size-1);
	////printf("Sorted Bbox\n");
	////printf("Box size:%d\n",nodeInfo->size);
	int leftIdx=0;
	float dightTh=0.2;

	while(leftIdx<nodeInfo->size-1){
		int rightIdx=leftIdx+1;
		while(rightIdx<nodeInfo->size){
			////printf("leftIdx:%d, rightIdx:%d\n",leftIdx,rightIdx);
			// 션 이사님한테 물어봐서 어디서 빼야할지 결정해야함.
			//Bbox* left=&(bboxes[leftIdx]);
			Node* leftNode=atIdxNode(nodeInfo,leftIdx);
			Bbox* left=leftNode->bbox;
			Node* rightNode=atIdxNode(nodeInfo,rightIdx);
			Bbox* right=rightNode->bbox;

			////printf("call rectIou\n");
			////printf("when iou, xlow : %f, xmax : %f\n",left->rect->xline->low,left->rect->xline->high);
			float iou=rectIou(left->rect,right->rect);
			////printf("iou : %f\n",iou);
			////printf("calc th\n");
			float th;
			if(isdigit(left->name[0])&&isdigit(right->name[0])){
				th=dightTh;
			}
			else{
				th=iouThresh;
			}
			if(iou<th){
				rightIdx+=1;
				continue;
			}
			////printf("calc victim\n");
			Bbox* victim;
			if(left->conf==right->conf){
				victim=(rectArea(left->rect)>rectArea(right->rect))?right:left;
			}
			else{
				victim=(left->conf>right->conf)?right:left;
			}

			Node* deleteNode;
			if(victim==left){
				// bboxes.pop(leftIdx);
				deleteNode=deleteNodeAt(nodeInfo,leftIdx);
				freeNode(deleteNode);
				break;
			}
			deleteNode=deleteNodeAt(nodeInfo,rightIdx);
			freeNode(deleteNode);
		}
		leftIdx+=1;
	}
	////printf("filteredBboxes name %s\n",nodeInfo->head->bbox->name);
	//return filteredBboxes;
}
void filterBboxIos(NodeInfo* nodeInfo,float iosThresh){
	////printf("Start filterBboxIos\n");
	//Bbox filteredBboxes[MAXLBOXNUM];
	//memcpy(&filteredBboxes,bboxes,sizeof(Bbox)*boxSize);
	////printf("filteredBboxes 0 %s\n",nodeInfo->head->bbox->name);
	// sort filteredBboxes by xlow
	sortBboxByXlow(nodeInfo,0,nodeInfo->size-1);

	int leftIdx=0;

	while(leftIdx<nodeInfo->size-1){
		int rightIdx=leftIdx+1;
		while(rightIdx<nodeInfo->size){
			// 션 이사님한테 물어봐서 어디서 빼야할지 결정해야함.
			Bbox* left=atIdxNode(nodeInfo,leftIdx);
			Bbox* right=atIdxNode(nodeInfo,rightIdx);

			float iou=rectIos(left->rect,right->rect);

			if(iou<iosThresh){
				rightIdx+=1;
				continue;
			}

			Bbox* victim;
			if(left->conf==right->conf){
				victim=(rectArea(left->rect)>rectArea(right->rect))?right:left;
			}
			else{
				victim=(left->conf>right->conf)?right:left;
			}

			Node* deleteNode;
			if(victim==left){
				// bboxes.pop(leftIdx);
				deleteNode=deleteNodeAt(nodeInfo,leftIdx);
				freeNode(deleteNode);
				break;
			}
			deleteNode=deleteNodeAt(nodeInfo,rightIdx);
			freeNode(deleteNode);
		}
		leftIdx+=1;
	}
	//return filteredBboxes;
}
LOCTYPE lineLength(Line* line){
	// +1 한건 divide zero 때매 그런가
	return (line->high-line->low)+1;
}
LOCTYPE rectArea(Rect* rect){
	return lineLength(rect->yline)*lineLength(rect->xline);
}
void setLine(Line* line,LOCTYPE low,LOCTYPE high){
	line->low=low;
	line->high=high;
}
void setRect(Rect* rect,Line* xline,Line* yline){
	rect->xline=xline;
	rect->yline=yline;
}
LOCTYPE getCenter(Line* line){
	if(line==NULL){
		//printf("error : getCenter line NULL\n");
	}
	LOCTYPE result=((line->low)+(line->high))/2.0;
	return result;
}
LOCTYPE getGap(Bbox* bbox1,Bbox* bbox2){
	LOCTYPE gap=bbox2->rect->xline->low-bbox1->rect->xline->high;
	gap=(0>gap)?0:gap;
	return gap;
}
void sortBboxByXlow(NodeInfo* nodeInfo,int low,int high){
	if(low>=high){
		return;
	}
	////printf("low %d, high %d\n",low,high);
	int pivot=0,first=0,last=0;

	pivot=low;
	first=low;
	last=high;
	Node* pivotNode=nodeInfo->head;
	Node* firstNode=nodeInfo->head;
	Node* lastNode=nodeInfo->head;
	for(int i=0;i<pivot;i++){
		pivotNode=pivotNode->next;
	}
	for(int i=0;i<first;i++){
		firstNode=firstNode->next;
	}
	for(int i=0;i<last;i++){
		lastNode=lastNode->next;
	}
	////printf("first %d, last %d, pivot %d\n",first,last,pivot);
	while(first<last){
		////printf("while\n");
		while(first<=high&&firstNode->bbox->rect->xline->low<=pivotNode->bbox->rect->xline->low){
			////printf("first %d\n",first);
			////printf("firstNode xmin %f\n",firstNode->bbox->rect->xline->low);
			////printf("pivotNode xmin %f\n",pivotNode->bbox->rect->xline->low);
			firstNode=firstNode->next;
			first++;
			if(first>high||firstNode==NULL){
				break;
			}
		}
		while(last>=low&&lastNode->bbox->rect->xline->low>pivotNode->bbox->rect->xline->low){
			////printf("last %d\n",last);
			////printf("lastNode xmin %f\n",lastNode->bbox->rect->xline->low);
			////printf("pivotNode xmin %f\n",pivotNode->bbox->rect->xline->low);
			lastNode=lastNode->prev;
			last--;
			if(last<low||lastNode==NULL){
				break;
			}
		}
		/*
		while(boxes[first].rect.xline.low<=boxes[pivot].rect.xline.low&&first<=high){
			//printf("first %d\n",first);
			//printf("first low : %f, pivot low : %f\n",boxes[first].rect.xline.low,boxes[pivot].rect.xline.low);
			first++;
		}
		while(boxes[last].rect.xline.low>boxes[pivot].rect.xline.low&&last>=low){
			//printf("last %d\n",last);
			//printf("last low : %f, pivot low : %f\n",boxes[last].rect.xline.low,boxes[pivot].rect.xline.low);
			last--;
		}
		*/
		if(first<last){
			swapBbox(firstNode,lastNode);
		}
	}
	swapBbox(lastNode,pivotNode);
	sortBboxByXlow(nodeInfo,low,last-1);
	sortBboxByXlow(nodeInfo,last+1,high);
}
void sortBboxByYlow(NodeInfo* nodeInfo,int low,int high){
	if(low>=high){
		return;
	}
	////printf("low %d, high %d\n",low,high);
	int pivot=0,first=0,last=0;

	pivot=low;
	first=low;
	last=high;
	Node* pivotNode=nodeInfo->head;
	Node* firstNode=nodeInfo->head;
	Node* lastNode=nodeInfo->head;
	for(int i=0;i<pivot;i++){
		pivotNode=pivotNode->next;
	}
	for(int i=0;i<first;i++){
		firstNode=firstNode->next;
	}
	for(int i=0;i<last;i++){
		lastNode=lastNode->next;
	}
	////printf("first %d, last %d, pivot %d\n",first,last,pivot);
	while(first<last){
		////printf("while\n");
		while(first<=high&&firstNode->bbox->rect->yline->low<=pivotNode->bbox->rect->yline->low){
			////printf("first %d\n",first);
			////printf("firstNode xmin %f\n",firstNode->bbox->rect->xline->low);
			////printf("pivotNode xmin %f\n",pivotNode->bbox->rect->xline->low);
			firstNode=firstNode->next;
			first++;
			if(first>high||firstNode==NULL){
				break;
			}
		}
		while(last>=low&&lastNode->bbox->rect->yline->low>pivotNode->bbox->rect->yline->low){
			////printf("last %d\n",last);
			////printf("lastNode xmin %f\n",lastNode->bbox->rect->xline->low);
			////printf("pivotNode xmin %f\n",pivotNode->bbox->rect->xline->low);
			lastNode=lastNode->prev;
			last--;
			if(last<low||lastNode==NULL){
				break;
			}
		}
		if(first<last){
			swapBbox(firstNode,lastNode);
		}
	}
	swapBbox(lastNode,pivotNode);
	sortBboxByYlow(nodeInfo,low,last-1);
	sortBboxByYlow(nodeInfo,last+1,high);
}
void sortBboxByConf(NodeInfo* nodeInfo,int low,int high){
	if(low>=high){
		return;
	}
	////printf("low %d, high %d\n",low,high);
	int pivot=0,first=0,last=0;

	pivot=low;
	first=low;
	last=high;
	Node* pivotNode=nodeInfo->head;
	Node* firstNode=nodeInfo->head;
	Node* lastNode=nodeInfo->head;
	for(int i=0;i<pivot;i++){
		pivotNode=pivotNode->next;
	}
	for(int i=0;i<first;i++){
		firstNode=firstNode->next;
	}
	for(int i=0;i<last;i++){
		lastNode=lastNode->next;
	}
	while(first<last){
		while(first<=high&&firstNode->bbox->conf<=pivotNode->bbox->conf){
			firstNode=firstNode->next;
			first++;
			if(first>high||firstNode==NULL){
				break;
			}
		}
		while(last>=low&&lastNode->bbox->conf>pivotNode->bbox->conf){
			lastNode=lastNode->prev;
			last--;
			if(last<low||lastNode==NULL){
				break;
			}
		}
		if(first<last){
			swapBbox(firstNode,lastNode);
		}
	}
	swapBbox(lastNode,pivotNode);
	sortBboxByConf(nodeInfo,low,last-1);
	sortBboxByConf(nodeInfo,last+1,high);
}
void sortBboxByConfR(NodeInfo* nodeInfo,int low,int high){
	if(low>=high){
		return;
	}
	////printf("low %d, high %d\n",low,high);
	int pivot=0,first=0,last=0;

	pivot=low;
	first=low;
	last=high;
	Node* pivotNode=nodeInfo->head;
	Node* firstNode=nodeInfo->head;
	Node* lastNode=nodeInfo->head;
	for(int i=0;i<pivot;i++){
		pivotNode=pivotNode->next;
	}
	for(int i=0;i<first;i++){
		firstNode=firstNode->next;
	}
	for(int i=0;i<last;i++){
		lastNode=lastNode->next;
	}
	while(first<last){
		while(first<=high&&firstNode->bbox->conf>=pivotNode->bbox->conf){
			firstNode=firstNode->next;
			first++;
			if(first>high||firstNode==NULL){
				break;
			}
		}
		while(last>=low&&lastNode->bbox->conf<pivotNode->bbox->conf){
			lastNode=lastNode->prev;
			last--;
			if(last<low||lastNode==NULL){
				break;
			}
		}
		if(first<last){
			swapBbox(firstNode,lastNode);
		}
	}
	swapBbox(lastNode,pivotNode);
	sortBboxByConfR(nodeInfo,low,last-1);
	sortBboxByConfR(nodeInfo,last+1,high);
}
void swapBbox(Node* node1,Node* node2){
	////printf("swap %s %s\n",node1->bbox->name,node2->bbox->name);
	Bbox* tmp;
	tmp=node1->bbox;
	node1->bbox=node2->bbox;
	node2->bbox=tmp;
}
int makeLocalImg(char* filename){
	TRY{
		int width,height,channels;
		IMGTYPE* in_img=stbi_load(filename,&width,&height,&channels,0); // r0g0b0 r1g1b1
		if(in_img==NULL){
			printf("NULL img\n");
			return 0;
		}
		int out_w=300,out_h=300;
		IMGTYPE* out_img=(IMGTYPE*)malloc(out_w*out_h*channels);
		// resize to 300x300
		int ret=stbir_resize_uint8(in_img,width,height,0,
		out_img,out_w,out_h,0,channels);
		
		/*
		FILE* fp_img;
		fp_img=fopen("./law_data.txt","wb");
		fwrite(out_img,1,out_w*out_h*channels,fp_img);
		fclose(fp_img);
		*/
		/*
		if(ret==0){
			printf("resize error\n");
			return 0;
		}
		*/
		/*
		if(out_img==NULL){
			printf("resize failed\n");
			return 0;
		}
		*/
		////printf("malloc int_img\n");
		CASTTYPE* int_img=(CASTTYPE*)malloc(sizeof(CASTTYPE)*out_w*out_h*channels);
		stbi_write_jpg("local_img.jpg",out_w,out_h,channels,out_img,0);
		////printf("parse int_img\n");
		int i=0;
		for(i=0;i<out_w*out_h*channels;i++){
			//int_img[i]=(int)(out_img[i]);
			int_img[i]=(CASTTYPE)(out_img[i]);
		}
		if(int_img==NULL){
			printf("parse int failed\n");
			return 0;
		}
		//printf("width : %d, height : %d, channel : %d\n",out_w,out_h,channels);
		//printf("size : %d!!\n",out_w*out_h*channels);
		FILE *fp=NULL;
		fp=fopen("./local_img.bgr","w");
		if(channels==1){
			//printf("channel 1\n");
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
		}
		else if(channels==3){
			printf("channel 3 img\n");

			for(int c=channels-1;c>=0;c--){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
		}
		fclose(fp);
		stbi_image_free(in_img);
		stbi_image_free(out_img);
		free(int_img);
		return 1;
	}
	CATCH{
		printf("localize img failed\n");
		return 0;
	}
	ETRY;
}
int makeRecogImg(Bbox* bbox,char* filename){
	TRY{
		////printf("call makeRecogImg\n");
		int width,height,channels;
		IMGTYPE* img=stbi_load(filename,&width,&height,&channels,0);
		//printf("Load IMG width : %d, height : %d, channel : %d\n",width,height,channels);
		if(img==NULL){
			printf("NULL img\n");
			return 0;
		}

		/*
		// 계수값을 곱해줌
		for(int c=0;c<channels;c++){
			for(int i=0;i<height;i++){
				for(int j=0;j<width;j++){
					if(channels==1){
						// black
						img[channels*(width*i+j)+c]/=DIVPAR_BLACK;
					}
					else if(c==0){
						// red
						img[channels*(width*i+j)+c]/=DIVPAR_R;
					}
					else if(c==1){
						// green
						img[channels*(width*i+j)+c]/=DIVPAR_G;
					}
					else if(c==2){
						// blue
						img[channels*(width*i+j)+c]/=DIVPAR_B;
					}
				}
			}
		}
		*/
		int xmin=bbox->rect->xline->low;
		int ymin=bbox->rect->yline->low;
		int xmax=bbox->rect->xline->high;
		int ymax=bbox->rect->yline->high;

		int crop_w=(width*xmax/300)-(width*xmin/300);
		int crop_h=(height*ymax/300)-(height*ymin/300);
		IMGTYPE* crop_img=(IMGTYPE*)malloc(crop_w*crop_h*channels);

		for(int c=0;c<channels;c++){
			int idx=-1;
			for(int i=height*ymin/300;i<height*ymax/300;i++){
				for(int j=width*xmin/300;j<width*xmax/300;j++){
					idx++;
					crop_img[channels*(idx)+c]=img[channels*(width*i+j)+c];
				}
			}
		}
		
		stbi_write_jpg("crop_img.jpg",crop_w,crop_h,channels,crop_img,0);
		//crop_img=stbi_load("crop_img.jpg",&crop_w,&crop_h,&channels,0);
		/*
		FILE* fp_crop;
		fp_crop=fopen("./crop_img.bgr","wb");
		for(int c=channels-1;c>=0;c--){
			for(int wh=0;wh<crop_w*crop_h;wh++){
				fwrite(&(crop_img[wh*channels+c]),1,1,fp_crop);
			}
		}
		fclose(fp_crop);
		*/
		/*
		int spc_w=crop_w;
		int spc_h=crop_h*1.5;
		idx=-1;
		IMGTYPE* spc_img=(IMGTYPE*)malloc(spc_w*spc_h*channels);
		
		for(int i=0;i<spc_w*spc_h*channels;i++){
			spc_img[i]=rand() % 256;
		}

		int spc_cnt=-1;
		for(int i=0;i<spc_w*spc_h*channels;i++){
			if(i>=spc_w*spc_h*channels*0.125 && i<spc_w*spc_h*channels*0.875){
				spc_cnt++;
				spc_img[i]=crop_img[spc_cnt];
			}
		}
		*/

		/*
		printf("crop_w : %d, crop_h:%d\n",crop_w,crop_h);
		int spc_w=crop_w;
		int spc_h=crop_w;
		int idx=-1;
		int crop_idx=-1;
		IMGTYPE* spc_img=(IMGTYPE*)malloc(spc_w*spc_h*channels);

		for(int c=0;c<channels;c++){
			for(int i=0;i<spc_h;i++){
				for(int j=0;j<spc_w;j++){
					idx++;
					if(crop_idx<crop_w*crop_h){
						crop_idx++;
						spc_img[idx]=crop_img[crop_idx];
					}
					else{
						//spc_img[idx]=rand() % 256;
						spc_img[idx]=0;
					}
				}
			}
		}
		*/

		int out_w=300,out_h=300;
		IMGTYPE* out_img=(IMGTYPE*)malloc(out_w*out_h*channels);
		int ret=stbir_resize_uint8(crop_img,crop_w,crop_h,0,out_img,out_w,out_h,0,channels);
		//int ret=stbir_resize_uint8(spc_img,spc_w,spc_h,0,out_img,out_w,out_h,0,channels);
		if(ret==0){
			printf("resize error\n");
			return 0;
		}
		/*
		int out416_w=416,out416_h=416;
		IMGTYPE* out416_img=(IMGTYPE*)malloc(out416_w*out416_h*channels);
		ret=stbir_resize_uint8(crop_img,crop_w,crop_h,0,out416_img,out416_w,out416_h,0,channels);
		*/
		CASTTYPE* int_img=(CASTTYPE*)malloc(sizeof(CASTTYPE)*out_w*out_h*channels);
		stbi_write_jpg("recog_img.jpg",out_w,out_h,channels,out_img,0);
		//unsigned char* emty_img=(unsigned char*)malloc(out_w*out_h*channels);
		for(int i=0;i<out_w*out_h*channels;i++){
			int_img[i]=(CASTTYPE)(out_img[i]);
			//float_img[i]=(float)(out_img[i]);
			//int_img[i]=int_img[i]*100/255;
		}
		/*
		CASTTYPE* int_416img=(CASTTYPE*)malloc(sizeof(CASTTYPE)*out416_w*out416_h*channels);
		stbi_write_jpg("recog_416img.jpg",out416_w,out416_h,channels,out416_img,0);
		//unsigned char* emty_img=(unsigned char*)malloc(out_w*out_h*channels);
		for(int i=0;i<out416_w*out416_h*channels;i++){
			int_416img[i]=(CASTTYPE)(out416_img[i]);
			//float_img[i]=(float)(out_img[i]);
			//int_img[i]=int_img[i]*100/255;
		}
		*/
		//printf("size : %ld\n",sizeof(float_img));

		FILE* fp;
		fp=fopen("./recog_img.bgr","wb");
		
		if(channels==1){
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
					
				}
			}
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
		}
		else if(channels==3){
			//printf("channel 3 img\n");
			//printf("width : %d, height : %d, channel : %d\n",out_w,out_h,channels);
			for(int c=channels-1;c>=0;c--){
				for(int wh=0;wh<out_w*out_h;wh++){
					fwrite(&(int_img[channels*wh+c]),1,1,fp);
				}
			}
		}
		fclose(fp);
		/*
		fp=fopen("./recog_416img.bgr","wb");
		if(channels==1){
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out416_w*out416_h;wh++){
					fwrite(&(int_416img[channels*wh+c]),1,1,fp);
				}
			}
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out416_w*out416_h;wh++){
					fwrite(&(int_416img[channels*wh+c]),1,1,fp);
				}
			}
			for(int c=0;c<channels;c++){
				for(int wh=0;wh<out416_w*out416_h;wh++){
					fwrite(&(int_416img[channels*wh+c]),1,1,fp);		
				}
			}
		}
		else if(channels==3){
			//printf("channel 3 img\n");
			//printf("width : %d, height : %d, channel : %d\n",out416_w,out416_h,channels);
			for(int c=channels-1;c>=0;c--){
				for(int wh=0;wh<out416_w*out416_h;wh++){
					fwrite(&(int_416img[channels*wh+c]),1,1,fp);
				}
			}
		}
		fclose(fp);
		*/
		stbi_image_free(img);
		stbi_image_free(crop_img);
		stbi_image_free(out_img);
		free(int_img);
		return 1;
	}
	CATCH{
		printf("make recognize img failed\n");
		return 0;
	}
	ETRY;
}