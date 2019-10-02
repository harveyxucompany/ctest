/**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

#define DEBUG 0
#define TOTAL_ROTATE	8

typedef struct {
	int Rx;
	int Ry;
} BlockPosition;

struct Node {
	BlockPosition* pPattern;
	struct Node *next;
};

int repeated = 0;
int xleft = 0;
int ytop = 0;
int iNumber = 0;

int TotalPattern = 0;
struct Node* headNode = NULL;
struct Node* currentNode = NULL;
struct Node* tempNode = NULL;

BlockPosition* pTempPatternPos;
BlockPosition* pCurrPatternBlockPos;
BlockPosition* pCurrPatternBlockPosNew;
BlockPosition* pRotatedPatternPos;

void MovetoLattice(void) {
	int i;
	int minX = iNumber;
	int minY = iNumber;

	for (i = 0; i < iNumber; i++) {
		if ((pRotatedPatternPos + i)->Rx < minX) {
			minX = (pRotatedPatternPos + i)->Rx;
		}
		if ((pRotatedPatternPos + i)->Ry < minY) {
			minY = (pRotatedPatternPos + i)->Ry;
		}
	}

	for (i = 0; i < iNumber; i++) {
		(pRotatedPatternPos + i)->Rx -= minX - 1;
		(pRotatedPatternPos + i)->Ry -= minY - 1;
	}
}

void Rotate0(void) {

	memcpy(pRotatedPatternPos, pCurrPatternBlockPos,
			iNumber * sizeof(BlockPosition));
	MovetoLattice();
}

void RotateX(void) {
	for (int i = 0; i < iNumber; i++) {
		(pRotatedPatternPos + i)->Rx = (pCurrPatternBlockPos + i)->Rx;
		(pRotatedPatternPos + i)->Ry = -1 * (pCurrPatternBlockPos + i)->Ry;
	}
	MovetoLattice();
}

void RotateY(void) {
	for (int i = 0; i < iNumber; i++) {
		(pRotatedPatternPos + i)->Rx = -1 * (pCurrPatternBlockPos + i)->Rx;
		(pRotatedPatternPos + i)->Ry = (pCurrPatternBlockPos + i)->Ry;
	}
	MovetoLattice();
}

void RotateXY(void) {
	for (int i = 0; i < iNumber; i++) {
		(pRotatedPatternPos + i)->Rx = -1 * (pCurrPatternBlockPos + i)->Rx;
		(pRotatedPatternPos + i)->Ry = -1 * (pCurrPatternBlockPos + i)->Ry;
	}
	MovetoLattice();
}

void Rotate90(void) {
	int i;

	for (i = 0; i < iNumber; i++) {
		(pRotatedPatternPos + i)->Rx = (pCurrPatternBlockPos + i)->Ry;
		(pRotatedPatternPos + i)->Ry = -1 * (pCurrPatternBlockPos + i)->Rx;
	}
	MovetoLattice();

	for (i = 0; i < iNumber; i++) {
		(pCurrPatternBlockPos + i)->Rx = (pRotatedPatternPos + i)->Rx;
		(pCurrPatternBlockPos + i)->Ry = (pRotatedPatternPos + i)->Ry;
	}

}

void (*RotateFunc[TOTAL_ROTATE])(void) =
{
	Rotate0,
	RotateX,
	RotateY,
	RotateXY,
	Rotate90,
	RotateX,
	RotateY,
	RotateXY
};

bool IsBlocksPosSameAsRotated(BlockPosition* pTempPatternPos) {
	int i, j, k;
	int SameCount;

	for (k = 0; k < TOTAL_ROTATE; k++) {
		RotateFunc[k]();

#if DEBUG
		printf("\n after rotated:");
		for (int m = 0; m < iNumber; m++) {
			printf("k=%d m=%d: %d,%d ", k, m, (pRotatedPatternPos + m)->Rx,
					(pRotatedPatternPos + m)->Ry);
		}
#endif
		SameCount = 0;
		for (i = 0; i < iNumber; i++) {
			for (j = 0; j < iNumber; j++) {
				if (((pTempPatternPos + i)->Rx == (pRotatedPatternPos + j)->Rx)
						&& ((pTempPatternPos + i)->Ry
								== (pRotatedPatternPos + j)->Ry)) {
					SameCount++;
					// printf("wc=%d,i=%d,j=%d %d,%d
					// ",SameCount,i,j,(pTempPatternPos+i)->Rx,(pTempPatternPos+i)->Ry);

				}
			}
		}
		if (SameCount == iNumber) {
#if DEBUG
			printf("\nsame after rotate k=%d\n", k);
			printf("\nsaved");
			for (int m = 0; m < iNumber; m++) {
				printf("%d,%d ", (pTempPatternPos + m)->Rx,
						(pTempPatternPos + m)->Ry);
			}
			printf("\n rotated");
			for (int m = 0; m < iNumber; m++) {
				printf(" %d,%d ", (pRotatedPatternPos + m)->Rx,
						(pRotatedPatternPos + m)->Ry);
			}
#endif
			return true;
		}
	}
	return false;
}

bool IsPatternExist() {
	tempNode = headNode;
	if (tempNode == NULL)
		Rotate0();
	while (tempNode) {
		if (IsBlocksPosSameAsRotated(tempNode->pPattern)) {
			return true;
		}
		tempNode = tempNode->next;
	}

	return false;
}

bool getMemory(void) {
	BlockPosition* p;

	p = (BlockPosition*) malloc(iNumber * sizeof(BlockPosition));
	if (p == NULL) {
		printf("Memory for Current Pattern malloc failed！");
		return false;
	}
	memset(p, 0, sizeof(iNumber * sizeof(BlockPosition)));
	pCurrPatternBlockPos = p;

	p = (BlockPosition*) malloc(iNumber * sizeof(BlockPosition));
	if (p == NULL) {
		printf("Memory for Current Pattern malloc failed！");
		return false;
	}
	memset(p, 0, sizeof(iNumber * sizeof(BlockPosition)));
	for (int k = 0; k < iNumber; k++) {
		p[k].Rx = 1;
		p[k].Ry = 1;
	}
	pCurrPatternBlockPosNew = p;

	p = (BlockPosition*) malloc(iNumber * sizeof(BlockPosition));
	if (p == NULL) {
		printf("Memory for Rotated malloc failed！");
		return false;
	}
	memset(p, 0, sizeof(iNumber * sizeof(BlockPosition)));
	pRotatedPatternPos = p;
	return true;
}

void freeMemory(void) {
	struct Node* nextNode = NULL;

	free(pCurrPatternBlockPos);
	free(pRotatedPatternPos);
	free(pCurrPatternBlockPosNew);

	nextNode = headNode;
	while (currentNode != NULL) {
		nextNode = currentNode->next;
		free(currentNode->pPattern);
		free(currentNode);
		currentNode = nextNode;
	}

}

void printAllPattern(void) {
	int x, y, m;
	int sum = 0;
	bool found;

	currentNode = headNode;
	while (currentNode != NULL) {
		sum = 0;

		for (y = 1; y <= iNumber; y++) {
			for (x = 1; x <= iNumber; x++) {
				found = false;

				for (m = 0; m < iNumber; m++) {

					if (((currentNode->pPattern + m)->Rx == x)
							&& ((currentNode->pPattern + m)->Ry == y)) {
						printf("#");
						found = true;
						sum++;
						if (sum >= iNumber)
							break;
					}
				}
				if (!found)
					printf(" ");

			}
			if (sum >= iNumber)
				break;
			printf("\n");
		}
		printf("\n\n");
		currentNode = currentNode->next;
	}

}
#if DEBUG
void printAllPatternNumber(void) {
	int x, y, m;
	int minx = iNumber;
	int miny = iNumber;
	int NewlineCount = 0;
	bool stop = false;
	bool found;

	currentNode = headNode;
	while (currentNode != NULL) {
		for (m = 0; m < iNumber; m++) {
			printf("%d:%d ", (currentNode->pPattern + m)->Rx,
					(currentNode->pPattern + m)->Ry);
		}
		printf("\n");
		currentNode = currentNode->next;
	}

}
#endif
bool IsRepeatBlock(void) {
	int i, j;
	int x, y;

	for (i = 0; i < iNumber; i++) {
		x = pCurrPatternBlockPos[i].Rx;
		y = pCurrPatternBlockPos[i].Ry;
		for (j = i + 1; j < iNumber; j++) {
			if ((pCurrPatternBlockPos[j].Rx == x)
					&& (pCurrPatternBlockPos[j].Ry == y)) {

				return true;
			}
		}
	}
	return false;
}
bool isNeighbour(BlockPosition* first, BlockPosition* second) {
	if (abs(second->Rx - first->Rx) > 1)
		return false;
	if (abs(second->Ry - first->Ry) > 1)
		return false;
	return true;
}

bool HasOrphon(void) {
	int i, j;
	bool isconnected;

	for (i = iNumber - 1; i > 0; i--) {
		isconnected = false;
		for (j = i - 1; j >= 0; j--) {
			if (isNeighbour(pCurrPatternBlockPos + i,
					pCurrPatternBlockPos + j)) {
				isconnected = true;
				break;
			}
		}
		if (false == isconnected)
			return true;
	}
	return false;
}
int main(void) {
	int i = 0;
	struct timeval start;
	struct timeval end;
	float diff;
//	int iNumber_plus1 = 0;
	struct Node* pnewnode = NULL;
	BlockPosition* pbp = NULL;

	printf("Please input number 1-65532\n");

	scanf("%d", &iNumber);

	if (iNumber <= 0) {
		printf("Please provide valid number 1-65532\n");
		return -1;
	}

	if (1 == iNumber) {
		printf("Totaly number of allowable arrangements is: 1");
		printf("\n#\n");
		return -1;
	}

	gettimeofday(&start, NULL);

	if(!getMemory()) return -1;

	//iNumber_plus1 = iNumber + 1;

	while (pCurrPatternBlockPosNew[iNumber - 1].Ry <= iNumber / 2) {

		for (i = 0; i < iNumber; i++) {
			if (pCurrPatternBlockPosNew[i].Rx > iNumber) {
				pCurrPatternBlockPosNew[i].Rx %= iNumber;
				pCurrPatternBlockPosNew[i].Ry++;
			}
			if (pCurrPatternBlockPosNew[i].Ry > iNumber) {
				pCurrPatternBlockPosNew[i].Ry %= iNumber;
				if (i < iNumber - 1)
					pCurrPatternBlockPosNew[i + 1].Rx++;
			}

		}
		//	printf("pCurrPatternBlockPosNew[iNumber].Ry=%d",pCurrPatternBlockPosNew[iNumber-1].Ry);
		memcpy(pCurrPatternBlockPos, pCurrPatternBlockPosNew,
				iNumber * sizeof(BlockPosition));
		pCurrPatternBlockPosNew[0].Rx++;
#if DEBUG
		printf("\nNew Pattern:");
		for (i = 0; i < iNumber; i++) {
			printf(" %d,%d ", (pCurrPatternBlockPos + i)->Rx,
					(pCurrPatternBlockPos + i)->Ry);
		}
#endif

		if (IsRepeatBlock()) {
			//printf("isRepeatBlock\n");
			continue;
		}
		if (HasOrphon()) {
			//printf("HasOrphon\n");
			continue;
		}

		if (IsPatternExist()) {
#if DEBUG
			printf("PatternExist ");
			for (int m = 0; m < iNumber; m++) {
				printf("%d,%d ", (pCurrPatternBlockPos + m)->Rx,
						(pCurrPatternBlockPos + m)->Ry);
			}
#endif
			continue;
		}

		TotalPattern++;
		pnewnode = (struct Node*) malloc(sizeof(struct Node));
		if (pnewnode == NULL) {
			printf("Memory for struct Node* malloc failed！");
			return -1;
		} else {
			if (headNode == NULL) {
				headNode = currentNode = pnewnode;
			} else {
				currentNode->next = pnewnode;
				currentNode = currentNode->next;
			}
		}
		pbp = (BlockPosition *) malloc(iNumber * sizeof(BlockPosition));
		if (pbp == NULL) {
			perror("Memory for existing blockPosition malloc failed！");
			return -1;
		} else {
			memcpy(pbp, pRotatedPatternPos, iNumber * sizeof(BlockPosition));
			currentNode->pPattern = pbp;
#if DEBUG
			printf("Saved pattern:");
			for (int k = 0; k < iNumber; k++)
			printf("%d,%d ", (currentNode->pPattern + k)->Rx,
					(currentNode->pPattern + k)->Ry);
			printf("\n");
#endif
			currentNode->next = NULL;
		}
	}

	printf("Total number of allowable arrangements are %d.\r\n", TotalPattern);

	printAllPattern();
#if DEBUG
	printAllPatternNumber();
#endif
	printf("Total number of allowable arrangements are %d.\r\n", TotalPattern);
	freeMemory();
	gettimeofday(&end, NULL);

	diff = (end.tv_sec - start.tv_sec)
			+ (end.tv_usec - start.tv_usec) / 1000000.0;
	printf("Time spent is %lf seconds\n", diff);

	return 0;
}
