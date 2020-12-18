#ifndef __PROC_H__
#define __PROC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define LOCTYPE float // 좌표 타입
#define IMGTYPE char // 이미지 타입
#define CASTTYPE int // casting 타입
#define NEW2019PLATE true // 2019 번호판 여부

#define DIVPAR_R 1//4.705 //  10000 % 0.2125
#define DIVPAR_G 1//1.397
#define DIVPAR_B 1//13.86
#define DIVPAR_BLACK 1

#define LBOXNUM 5 // 테스트용 l 모델 결과 갯수
#define RBOXNUM 7 // 테스트용 r 모델 결과 갯수

#define LLABELNUM 7 // l 모델 레이블 갯수
#define RLABELNUM 107 // r 모델 레이블 갯수
#define DLABELNUM 24 // d 모델 레이블 갯수
#define Y3LABELNUM 107 // yolo3 모델 레이블 갯수

#define MAXCHARLEN 30 // 문자열 최대 길이
#define MAXRESULTLEN 100 // 최대 r 결과값 길이
#define MAXLBOXNUM 100 // 최대 l 모델 바운딩 박스 갯수
#define MAXRBOXNUM 100 // 최대 r 모델 바운딩 박스 갯수
#define NULL ((void*)0) // 널값
#define LIOUTHRESH 0.3 // l모델 iou threshhold
#define LIOSTHRESH 0.8 // l모델 ios threshhold
#define RIOUTHRESH 0.3 // r모델 iou threshhold
#define RIOSTHRESH 0.8 // r모델 ios threshhold
#define REGIONNUM 17 // 지역 갯수
#define HANGULNUM 78 // 한글 갯수
#define DIGITNUM 10 // 숫자 갯수
#define YOUNGNUM 1 // 영업 갯수
#define IMGWIDTH (float)300 // 이미지 너비
#define IMGHEIGHT (float)300 // 이미지 높이
#define MAXIMGNAMELEN 100 // 이미지 경로 길이

#define PORTREGIONX 0.3
#define LANDREGIONY 0.3
#define SUBSTYOUNGX 0.3
#define SUBSTYOUNGY 0.3

// standard box info
#define LENOFSTANDARDBBOX 10
#define REGIONIDX 0
#define DIGITHEADIDX 1
#define KORCODEIDX 4
#define DIGITTAILIDX 5
#define YOUNGIDX 9

// macro
#define MIN(Y,X) Y>X?X:Y
#define MAX(Y,X) Y>X?Y:X

// define bool
typedef enum {false, true} bool;

// min, max값
typedef struct _Line {
	LOCTYPE low;
	LOCTYPE high;
}Line;

// x,y 좌표.
typedef struct _Rect {
	Line* yline;
	Line* xline;
}Rect;

// 바운딩 박스
typedef struct _Bbox {
	Rect* rect;
	float conf;
	char* name;
}Bbox;

typedef struct _Node{
	struct _Node* next;
	struct _Node* prev;
	Bbox* bbox;
}Node;

typedef struct _NodeInfo{
	Node* head;
	Node* tail;
	int size;
}NodeInfo;

// 노드 초기화
void initNodeInfo(NodeInfo* nodeInfo);
// 노드들 해제
void clearNodeInfo(NodeInfo* nodeInfo);
// bbox 삽입하며 노드 생성
Node* createNode(Bbox* bbox);
// 노드 끝에 삽입
void insertNodeLast(NodeInfo* nodeInfo,Node* newNode);
// 노드 idx에 삽입
void insertNodeAt(NodeInfo* nodeInfo,Node* newNode,int idx);
// 빈 노드 num개 삽입
NodeInfo* getVacantNodeInfo(int num);
// 노드 idx에 제거
Node* deleteNodeAt(NodeInfo* nodeInfo,int idx);
// 노드 마지막에서 제거
Node* deleteNodeLast(NodeInfo* nodeInfo);
// 노드 해제
void freeNode(Node* deleteNode);
// 노드 info 해제
void freeNodeInfo(NodeInfo* nodeInfo);
// idx 노드 반환
Node* atIdxNode(NodeInfo* nodeInfo,int idx);
// i번째까지만 노드를 남겨두고 나머지 삭제
void leaveNodeAt(NodeInfo* nodeinfo,int idx);
// nodeInfo 복사
NodeInfo* copyNodeInfo(NodeInfo* nodeInfo);
// nodeInfo1 nodeInfo2 merge
NodeInfo* mergeNodeInfo(NodeInfo* nodeInfo1,NodeInfo* nodeInfo2);
// 실제 node 복사
Node* copyNode(Node* node);
// 노드 대체
void subsNode(NodeInfo* destNodeInfo,int destIdx,NodeInfo* sourceNodeInfo,int sourceIdx);
// 노드 하나만 대체
void subsOnlyNode(NodeInfo* destNodeInfo,int destIdx,Node* sourceNode);
// Localize 결과 반환
Bbox* getLocalResult();
// conf가 제일 높은 박스 반환
Bbox* getMaxConfBox(NodeInfo* nodeInfo);
// 13레이블(Digit Model) 텍스트 반환
char* getPlateStringDigitModel(char* recoPlateNum);
// yolo3 텍스트 반환
char* getPlateStringYOLO3(char* recoPlateNum);
// 107레이블 텍스트 반환
char* getPlateString(char* recoPlateNum);
// r1 결과 박스들중 사용할 박스만 선택
NodeInfo* parseBboxes(NodeInfo* nodeInfo);
// 지역인지
bool isRegion(char* name);
// 한글인지
bool isHangul(char* name);
// 영업용인지
bool isYoung(char* name);
// 숫자인지
bool isDigit(char* name);
// 모델에서 결과값 받기
void setBbox(int idx,int label, LOCTYPE xmin,LOCTYPE ymin,LOCTYPE xmax,LOCTYPE ymax, float conf,char type);
// 교집합 line
Line* lineIntersection(Line* line1,Line* line2);
// 교집합 rect
Rect* rectIntersection(Rect* rect1,Rect* rect2);
// line Iou
LOCTYPE lineIou(Line* line1,Line* line2);
// line Ios
LOCTYPE lineIos(Line* line1,Line* line2);
// rect Iou
LOCTYPE rectIou(Rect* rect1,Rect* rect2);
// rect Ios
LOCTYPE rectIos(Rect* rect1,Rect* rect2);
// rect Iot
LOCTYPE rectIot(Rect* rect1,Rect* targetRect);
// 박스 필터링(iou,ios)
void filterBbox(NodeInfo* nodeInfo,float iouThresh,float iosThresh);
// iou 필터링
void filterBboxIou(NodeInfo* nodeInfo,float iouThresh);
// ios 필터링
void filterBboxIos(NodeInfo* nodeInfo,float iosThresh);
// line 길이
LOCTYPE lineLength(Line* line);
// rect 넓이
LOCTYPE rectArea(Rect* rect);
// line 설정
void setLine(Line* line,LOCTYPE low,LOCTYPE high);
// rect 설정
void setRect(Rect* rect,Line* xline,Line* yline);
// center값 반환
LOCTYPE getCenter(Line* line);
// gap 반환
LOCTYPE getGap(Bbox* bbox1,Bbox* bbox2);
// quick sort Bbox by xmin
void sortBboxByXlow(NodeInfo* nodeInfo,int low,int high);
// quick sort Bbox by ymin
void sortBboxByYlow(NodeInfo* nodeInfo,int low,int high);
// quick sort Bbox by conf
void sortBboxByConf(NodeInfo* nodeInfo,int low,int high);
// quick sort Bbox by conf Reverse
void sortBboxByConfR(NodeInfo* nodeInfo,int low,int high);
// Bbox Swap
void swapBbox(Node* node1,Node* node2);
// local 모델용 bgr 만들기
int makeLocalImg(char* filename);
// recog 모델용 bgr 만들기
int makeRecogImg(Bbox* bbox,char* filename);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PROC_H__ */