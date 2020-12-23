#include "stdio.h"
#include "balance.h"
#include "pch.h"

int main() {
	int a[101] = { 0,-1 }; // 초기 33번 비교를 위해, 3개의 동전(0,1,2)을 미리 올려놓음.
	int b[101] = { 1,-1 };
	int c[101] = { 2,-1 };
	int sum = 1;
	int tmp_a[2] = { -1,-1 };
	int tmp_b[2] = { -1,-1 };
	int tmp_c[2] = { -1,-1 };
	int faulty[101] = { -1 };	// 불량(5) 동전들
	int real[101] = { -1 };		// 진짜(7) 동전들
	int equals[101] = { -1 };	// 초기 33번 비교후, 무게가 같은 동전들
	int faulty_count = 0, equals_count = 0, real_count = 0;
	int balance_check = 0, re_balance_check = 0;
	int flag1 = -1, flag2 = -1; // 분명한 동전을 찾음. flag1 = 7, flag2 = 5(불량)
	int i, j;


	//불량동전을 초기화한다.
	initialize(30);
	printf("----------- 초기화 완료 ---------\n");

	// 인덱스를 3개씩(ex (0,1,2),(3,4,5)...(96,97,98) 쪼개서 저울에 1개씩 비교. 
	// 총 33번 비교.
	// 인덱스 (0~98) 까지 완료 후, (99)는 나중에 equals에 넣어서 체크.
	for (i = 0; i < 99; i += 3, a[0] += 3, b[0] += 3, c[0] += 3) {
		balance_check = balance(a, b, c);
		switch (balance_check) {
		case 0:
			if (flag1 == -1) flag1 = a[0];
			if (flag2 == -1) flag2 = b[0];
			real[real_count++] = a[0];
			faulty[faulty_count++] = b[0];
			faulty[faulty_count++] = c[0];
			printf("	faulty = %d, %d\n", b[0], c[0]);
			break;
		case 1:
			if (flag1 == -1) flag1 = b[0];
			if (flag2 == -1) flag2 = a[0];
			real[real_count++] = b[0];
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = c[0];
			printf("	faulty = %d, %d\n", a[0], c[0]);
			break;
		case 2:
			if (flag1 == -1) flag1 = c[0];
			if (flag2 == -1) flag2 = a[0];
			real[real_count++] = c[0];
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = b[0];
			printf("	faulty = %d, %d\n", a[0], b[0]);
			break;
		case 3:
			if (flag1 == -1) flag1 = a[0];
			if (flag2 == -1) flag2 = c[0];
			real[real_count++] = a[0];
			real[real_count++] = b[0];
			faulty[faulty_count++] = c[0];
			printf("	faulty = %d\n", c[0]);
			break;
		case 4:
			if (flag1 == -1) flag1 = b[0];
			if (flag2 == -1) flag2 = a[0];
			real[real_count++] = b[0];
			real[real_count++] = c[0];
			faulty[faulty_count++] = a[0];
			printf("	faulty = %d\n", a[0]);
			break;
		case 5:
			if (flag1 == -1) flag1 = c[0];
			if (flag2 == -1) flag2 = b[0];
			real[real_count++] = a[0];
			real[real_count++] = c[0];
			faulty[faulty_count++] = b[0];
			printf("	faulty = %d\n", b[0]);
			break;
		case 6:
			equals[equals_count++] = a[0];
			equals[equals_count++] = b[0];
			equals[equals_count++] = c[0];
			break;
		}

		// 99개 coin이 모두 무게가 같을 경우
		if (equals_count == 99) {
			equals[equals_count] = -1;
			b[0] = -1;
			c[0] = -1;
			if (balance(equals, b, c) == SUCCESS) return 0; // 99개 불량인지 확인
			else {
				a[0] = 99; a[1] = -1;
				balance(a, b, c); // 아니면 1개 불량
			}
		}
	}
	printf("-----------flag1, flag2 설정 완료---------\n");
	printf("//////////////////////////////////////////\n");
	printf(" faulty_count = %d, real_count = %d\n", faulty_count, real_count);

	b[0] = -1;
	c[0] = -1;
	faulty[faulty_count] = -1;

	// 초기비교수 진짜가 많으면 equals그룹 전부 진짜로 가정
	if (faulty_count <= real_count) {
		if (faulty_count <= 30) {
			printf("Real_count Big\n");
			if (balance(faulty, b, c) == SUCCESS) 			// 현재 불량동전이 전체인지 체크
				return 0;
			else {
				if (faulty_count <= 24) {					// 실패하면 99번 포함하고 체크
					printf("check - 99coin\n");
					faulty[faulty_count++] = 99;
					faulty[faulty_count] = -1;
					if (balance(faulty, b, c) == SUCCESS)
						return 0;
					else
						faulty[--faulty_count] = -1;
				}
			}
		}
	}
	// 초기비교수 불량이 많으면 equals그룹 전부 불량으로 가정
	else if (faulty_count > real_count) {
		if (real_count <= 30) {
			printf("Faulty_count Big\n");
			for (i = 0; i < equals_count; i++)              // equals그룹 불량에 포함 체크
				faulty[faulty_count++] = equals[i];
			faulty[faulty_count++] = 99;					// 99번째도 불량으로 가정
			faulty[faulty_count] = -1;
			printf(" faulty_count = %d\n", faulty_count);

			if (balance(faulty, b, c) == SUCCESS)    //체크
				return 0;
			else {                                              // 실패하면 99번 제외하고 체크
				if (real_count <= 24) {
					printf("check - 99coin\n");
					faulty[--faulty_count] = -1;
					if (balance(faulty, b, c) == SUCCESS)
						return 0;
					else
						faulty_count++;
				}
				for (i = 0; i <= equals_count; i++)             // 실패하면 equals그룹 불량에서 빼고 진행
					faulty[--faulty_count] = -1;
			}
		}
	}

	// 가정이 모두 실패의 경우, equals 추가 비교
	printf("----------- EQUALS CHECK ---------\n");
	a[2] = -1;
	b[2] = -1;
	c[2] = -1;
	equals[equals_count++] = 99;
	equals[equals_count++] = -1;	// equals의 마지막에 넣어서 같이 체크해줌
	equals[equals_count++] = -1;	// equals_count값을 3의 배수로 맞추기 위해 -1을 넣어줌
	printf(" equals_count = %d\n", equals_count);

	// 위에 그룹에서 나눴을 때 EQUALS 다시 체크
	// 4개씩 쪼개서 체크한다
	// 나머지가 1개, 2개, 3개일 경우, for문 종료 후 따로 체크
	// equals_count값이 12의 배수일 경우, 마지막 b[1] = 99이므로 if문으로 따로 체크해줌. 
	for (i = 11; i < equals_count; i = i + 12) {
		a[0] = equals[i - 11];
		a[1] = equals[i - 8];
		b[0] = equals[i - 5];
		b[1] = equals[i - 2];
		c[0] = flag1; // 확실한 진짜(7) 동전
		c[1] = flag2; // 확실한 불량(5) 동전 

		balance_check = balance(a, b, c);
		switch (balance_check) {
		case 0:									// a[0],a[1] = real(7)
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			real[real_count++] = a[1];
			real[real_count++] = a[1] + sum;
			real[real_count++] = a[1] + sum * 2;

			// b[0],b[1]를 판단하기 위해 tmp에 저장 후 re_check
			// tmp_c에 flag1을 넣어서 확실히 체크
			tmp_a[0] = b[0];
			tmp_b[0] = b[1];
			tmp_c[0] = flag1;
			re_balance_check = balance(tmp_a, tmp_b, tmp_c);
			switch (re_balance_check) {
			case 2:
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				faulty[faulty_count++] = b[1];
				if (b[1] != 99) {
					faulty[faulty_count++] = b[1] + sum;
					faulty[faulty_count++] = b[1] + sum * 2;
				}
				printf("	result = %d %d %d, %d %d %d \n", b[0], b[0] + sum, b[0] + sum * 2, b[1], b[1] + sum, b[1] + sum * 2);
				break;
			case 4:
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				real[real_count++] = b[1];
				if (b[1] != 99) {
					real[real_count++] = b[1] + sum;
					real[real_count++] = b[1] + sum * 2;
				}
				printf("	result = %d %d %d\n", b[0], b[0] + sum, b[0] + sum * 2);
				break;
			case 5:
				faulty[faulty_count++] = b[1];
				if (b[1] != 99) {
					faulty[faulty_count++] = b[1] + sum;
					faulty[faulty_count++] = b[1] + sum * 2;
				}
				real[real_count++] = b[0];
				real[real_count++] = b[0] + sum;
				real[real_count++] = b[0] + sum * 2;
				printf("	result = %d %d %d\n", b[1], b[1] + sum, b[1] + sum * 2);
				break;
			case 6:
				real[real_count++] = b[0];
				real[real_count++] = b[0] + sum;
				real[real_count++] = b[0] + sum * 2;
				real[real_count++] = b[1];
				if (b[1] != 99) {
					real[real_count++] = b[1] + sum;
					real[real_count++] = b[1] + sum * 2;
				}
				break;
			}
			break;

		case 1:	// b[0],b[1] = real(7)
			real[real_count++] = b[0];
			real[real_count++] = b[0] + sum;
			real[real_count++] = b[0] + sum * 2;
			real[real_count++] = b[1];
			if (b[1] != 99) {
				real[real_count++] = b[1] + sum;
				real[real_count++] = b[1] + sum * 2;
			}

			// a[0],a[1]를 판단하기 위해 tmp_a,b에 저장 후 re_check
			// tmp_c에 flag1을 넣어서 확실히 체크
			tmp_a[0] = a[0];
			tmp_b[0] = a[1];
			tmp_c[0] = flag1;
			re_balance_check = balance(tmp_a, tmp_b, tmp_c);
			switch (re_balance_check) {
			case 2:
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				faulty[faulty_count++] = a[1];
				faulty[faulty_count++] = a[1] + sum;
				faulty[faulty_count++] = a[1] + sum * 2;
				printf("	result = %d %d %d, %d %d %d \n", a[0], a[0] + sum, a[0] + sum * 2, a[1], a[1] + sum, a[1] + sum * 2);
				break;
			case 4:
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				real[real_count++] = a[1];
				real[real_count++] = a[1] + sum;
				real[real_count++] = a[1] + sum * 2;
				printf("	result = %d %d %d\n", a[0], a[0] + sum, a[0] + sum * 2);
				break;
			case 5:
				faulty[faulty_count++] = a[1];
				faulty[faulty_count++] = a[1] + sum;
				faulty[faulty_count++] = a[1] + sum * 2;
				real[real_count++] = a[0];
				real[real_count++] = a[0] + sum;
				real[real_count++] = a[0] + sum * 2;
				printf("	result = %d %d %d\n", a[1], a[1] + sum, a[1] + sum * 2);
				break;
			case 6:
				real[real_count++] = a[0];
				real[real_count++] = a[0] + sum;
				real[real_count++] = a[0] + sum * 2;
				real[real_count++] = a[1];
				real[real_count++] = a[1] + sum;
				real[real_count++] = a[1] + sum * 2;
				break;
			}
			break;

		case 2:	// a[0],a[1],b[0],b[1] = faulty(5)
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			faulty[faulty_count++] = a[1];
			faulty[faulty_count++] = a[1] + sum;
			faulty[faulty_count++] = a[1] + sum * 2;
			faulty[faulty_count++] = b[0];
			faulty[faulty_count++] = b[0] + sum;
			faulty[faulty_count++] = b[0] + sum * 2;
			faulty[faulty_count++] = b[1];
			if (b[1] != 99) {
				faulty[faulty_count++] = b[1] + sum;
				faulty[faulty_count++] = b[1] + sum * 2;
			}
			printf("	result = %d %d %d, %d %d %d, %d %d %d, %d %d %d\n", a[0], a[0] + sum, a[0] + sum * 2,
				a[1], a[1] + sum, a[1] + sum * 2, b[0], b[0] + sum, b[0] + sum * 2, b[1], b[1] + sum, b[1] + sum * 2);
			break;

		case 3:	// a[0],a[1],b[0],b[1] = real(7)
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			real[real_count++] = a[1];
			real[real_count++] = a[1] + sum;
			real[real_count++] = a[1] + sum * 2;
			real[real_count++] = b[0];
			real[real_count++] = b[0] + sum;
			real[real_count++] = b[0] + sum * 2;
			real[real_count++] = b[1];
			if (b[1] != 99) {
				real[real_count++] = b[1] + sum;
				real[real_count++] = b[1] + sum * 2;
			}
			break;

		case 4:	// a[0],a[1] = faulty(5)
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			faulty[faulty_count++] = a[1];
			faulty[faulty_count++] = a[1] + sum;
			faulty[faulty_count++] = a[1] + sum * 2;
			printf("	result = %d %d %d, %d %d %d \n", a[0], a[0] + sum, a[0] + sum * 2, a[1], a[1] + sum, a[1] + sum * 2);

			// b[0],b[1]를 판단하기 위해 tmp_a,b에 저장 후 re_check
			// tmp_c에 flag1을 넣어서 확실히 체크
			tmp_a[0] = b[0];
			tmp_b[0] = b[1];
			tmp_c[0] = flag1;
			re_balance_check = balance(tmp_a, tmp_b, tmp_c);
			switch (re_balance_check) {
			case 2:
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				faulty[faulty_count++] = b[1];
				if (b[1] != 99) {
					faulty[faulty_count++] = b[1] + sum;
					faulty[faulty_count++] = b[1] + sum * 2;
				}
				printf("	result = %d %d %d, %d %d %d \n", b[0], b[0] + sum, b[0] + sum * 2, b[1], b[1] + sum, b[1] + sum * 2);
				break;
			case 4:
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				real[real_count++] = b[1];
				if (b[1] != 99) {
					real[real_count++] = b[1] + sum;
					real[real_count++] = b[1] + sum * 2;
				}
				printf("	result = %d %d %d\n", b[0], b[0] + sum, b[0] + sum * 2);
				break;
			case 5:
				faulty[faulty_count++] = b[1];
				if (b[1] != 99) {
					faulty[faulty_count++] = b[1] + sum;
					faulty[faulty_count++] = b[1] + sum * 2;
				}
				real[real_count++] = b[0];
				real[real_count++] = b[0] + sum;
				real[real_count++] = b[0] + sum * 2;
				printf("	result = %d %d %d\n", b[1], b[1] + sum, b[1] + sum * 2);
				break;
			case 6:
				real[real_count++] = b[0];
				real[real_count++] = b[0] + sum;
				real[real_count++] = b[0] + sum * 2;
				real[real_count++] = b[1];
				if (b[1] != 99) {
					real[real_count++] = b[1] + sum;
					real[real_count++] = b[1] + sum * 2;
				}
				break;
			}
			break;

		case 5:	// b[0],b[1] = faulty(5)
			faulty[faulty_count++] = b[0];
			faulty[faulty_count++] = b[0] + sum;
			faulty[faulty_count++] = b[0] + sum * 2;
			faulty[faulty_count++] = b[1];
			if (b[1] != 99) {
				faulty[faulty_count++] = b[1] + sum;
				faulty[faulty_count++] = b[1] + sum * 2;
			}
			printf("	result = %d %d %d, %d %d %d \n", b[0], b[0] + sum, b[0] + sum * 2, b[1], b[1] + sum, b[1] + sum * 2);

			// a[0],a[1]를 판단하기 위해 tmp_a,b에 저장 후 re_check
			// tmp_c에 flag1을 넣어서 확실히 체크
			tmp_a[0] = a[0];
			tmp_b[0] = a[1];
			tmp_c[0] = flag1;
			re_balance_check = balance(tmp_a, tmp_b, tmp_c);
			switch (re_balance_check) {
			case 2:
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				faulty[faulty_count++] = a[1];
				faulty[faulty_count++] = a[1] + sum;
				faulty[faulty_count++] = a[1] + sum * 2;
				printf("	result = %d %d %d, %d %d %d \n", a[0], a[0] + sum, a[0] + sum * 2, a[1], a[1] + sum, a[1] + sum * 2);
				break;
			case 4:
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				real[real_count++] = a[1];
				real[real_count++] = a[1] + sum;
				real[real_count++] = a[1] + sum * 2;
				printf("	result = %d %d %d\n", a[0], a[0] + sum, a[0] + sum * 2);
				break;
			case 5:
				faulty[faulty_count++] = a[1];
				faulty[faulty_count++] = a[1] + sum;
				faulty[faulty_count++] = a[1] + sum * 2;
				real[real_count++] = a[0];
				real[real_count++] = a[0] + sum;
				real[real_count++] = a[0] + sum * 2;
				printf("	result = %d %d %d\n", a[1], a[1] + sum, a[1] + sum * 2);
				break;
			case 6:
				real[real_count++] = a[0];
				real[real_count++] = a[0] + sum;
				real[real_count++] = a[0] + sum * 2;
				real[real_count++] = a[1];
				real[real_count++] = a[1] + sum;
				real[real_count++] = a[1] + sum * 2;
				break;
			}
			break;

		case 6:
			// a[0],a[1],b[0],b[1]를 판단하기 위해 tmp_a = a[0], tmp_b = b[0]에 저장 후 re_check
			// a[0],a[1]과 b[0],b[1]은 서로 반대되는 무게 (ex a[0]=5, -> a[1]=7) 
			// tmp_c에 flag1을 넣어서 확실히 체크
			tmp_a[0] = a[0];
			tmp_b[0] = b[0];
			tmp_c[0] = flag1;
			re_balance_check = balance(tmp_a, tmp_b, tmp_c);
			switch (re_balance_check) {
			case 2:
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				real[real_count++] = a[1];
				real[real_count++] = a[1] + sum;
				real[real_count++] = a[1] + sum * 2;
				real[real_count++] = b[1];
				if (b[1] != 99) {
					real[real_count++] = b[1] + sum;
					real[real_count++] = b[1] + sum * 2;
				}
				printf("	result = %d %d %d, %d %d %d \n", a[0], a[0] + sum, a[0] + sum * 2, b[0], b[0] + sum, b[0] + sum * 2);
				break;
			case 4:
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				faulty[faulty_count++] = b[1];
				if (b[1] != 99) {
					faulty[faulty_count++] = b[1] + sum;
					faulty[faulty_count++] = b[1] + sum * 2;
				}
				real[real_count++] = a[1];
				real[real_count++] = a[1] + sum;
				real[real_count++] = a[1] + sum * 2;
				real[real_count++] = b[0];
				real[real_count++] = b[0] + sum;
				real[real_count++] = b[0] + sum * 2;
				printf("	result = %d %d %d, %d %d %d \n", a[0], a[0] + sum, a[0] + sum * 2, b[1], b[1] + sum, b[1] + sum * 2);
				break;
			case 5:
				faulty[faulty_count++] = a[1];
				faulty[faulty_count++] = a[1] + sum;
				faulty[faulty_count++] = a[1] + sum * 2;
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				real[real_count++] = a[0];
				real[real_count++] = a[0] + sum;
				real[real_count++] = a[0] + sum * 2;
				real[real_count++] = b[1];
				if (b[1] != 99) {
					real[real_count++] = b[1] + sum;
					real[real_count++] = b[1] + sum * 2;
				}
				printf("	result = %d %d %d, %d %d %d \n", a[1], a[1] + sum, a[1] + sum * 2, b[0], b[0] + sum, b[0] + sum * 2);
				break;
			case 6:
				faulty[faulty_count++] = a[1];
				faulty[faulty_count++] = a[1] + sum;
				faulty[faulty_count++] = a[1] + sum * 2;
				faulty[faulty_count++] = b[1];
				if (b[1] != 99) {
					faulty[faulty_count++] = b[1] + sum;
					faulty[faulty_count++] = b[1] + sum * 2;
				}
				real[real_count++] = a[0];
				real[real_count++] = a[0] + sum;
				real[real_count++] = a[0] + sum * 2;
				real[real_count++] = b[0];
				real[real_count++] = b[0] + sum;
				real[real_count++] = b[0] + sum * 2;
				printf("	result = %d %d %d, %d %d %d \n", a[1], a[1] + sum, a[1] + sum * 2, b[1], b[1] + sum, b[1] + sum * 2);
				break;
			}
		}
	}

	// EQUALS 체크 후 나머지가 1개, 2개, 3개일 경우, 체크.
	// i - equals_count값이 8일 경우, equals 1개 남음. 1개는 99번째 동전.
	// i - equals_count값이 5일 경우, equals 2개 남음. 마지막 동전은 99번째 동전.
	// i - equals_count값이 2일 경우, equals 3개 남음. 마지막 동전은 99번째 동전.
	printf("i - equals_count = %d\n", i - equals_count);
	a[1] = -1;
	b[1] = -1;
	c[1] = -1;
	if (i - equals_count == 8) {		// equals가 1개 남았을 때, 마지막에 넣어준 동전은 99번째.
		printf("남은 equals 1개 체크\n");
		faulty[faulty_count] = -1;
		b[0] = -1;
		c[0] = -1;
		if (balance(faulty, b, c) == SUCCESS) return 0;		// 1개 남은 동전(99)은 안넣고 체크.
		else {												// SUCCESS가 아니면 1개 남은 동전(99)은 faulty 동전.
			faulty[faulty_count++] = 99;
			printf("	result = 99 \n");

		}
	}
	else if (i - equals_count == 5) {	// equals가 2개 남았을 때, 마지막 동전은 99번째.
		printf("남은 equals 2개 체크\n");
		a[0] = equals[i - 11];			// a[0],b[0]에 동전을 넣고, c[0]에 flag1을 넣어서 체크.
		b[0] = 99;
		c[0] = flag1;
		balance_check = balance(a, b, c);
		switch (balance_check) {
		case 2:
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			faulty[faulty_count++] = b[0];
			printf("	result = %d %d %d 99 \n", a[0], a[0] + sum, a[0] + sum * 2);
			break;
		case 4:
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			real[real_count++] = b[0];
			printf("	result = %d %d %d\n", a[0], a[0] + sum, a[0] + sum * 2);
			break;
		case 5:
			faulty[faulty_count++] = b[0];
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			printf("	result = 99 \n");
			break;
		case 6:
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			real[real_count++] = b[0];
			break;
		default:break;
		}
	}
	else if (i - equals_count == 2) {	// equals가 3개 남았을 때, 마지막 동전은 99번째.
		printf("남은 equals 3개 체크\n");
		a[0] = equals[i - 11];			// a[0],b[0],c[0]에 동전을 넣어서 체크.
		b[0] = equals[i - 8];
		c[0] = 99;
		balance_check = balance(a, b, c);
		switch (balance_check) {
		case 0:
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			faulty[faulty_count++] = b[0];
			faulty[faulty_count++] = b[0] + sum;
			faulty[faulty_count++] = b[0] + sum * 2;
			faulty[faulty_count++] = c[0];
			printf("	faulty = %d %d %d 99 \n", b[0], b[0] + sum, b[0] + sum * 2);
			break;
		case 1:
			real[real_count++] = b[0];
			real[real_count++] = b[0] + sum;
			real[real_count++] = b[0] + sum * 2;
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			faulty[faulty_count++] = c[0];
			printf("	faulty = %d %d %d 99 \n", a[0], a[0] + sum, a[0] + sum * 2);
			break;
		case 2:
			real[real_count++] = c[0];
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			faulty[faulty_count++] = b[0];
			faulty[faulty_count++] = b[0] + sum;
			faulty[faulty_count++] = b[0] + sum * 2;
			printf("	faulty = %d %d %d %d %d %d\n", a[0], a[0] + sum, a[0] + sum * 2, b[0], b[0] + sum, b[0] + sum * 2);
			break;
		case 3:
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			real[real_count++] = b[0];
			real[real_count++] = b[0] + sum;
			real[real_count++] = b[0] + sum * 2;
			faulty[faulty_count++] = c[0];
			printf("	faulty = %d\n", c[0]);
			break;
		case 4:
			real[real_count++] = b[0];
			real[real_count++] = b[0] + sum;
			real[real_count++] = b[0] + sum * 2;
			real[real_count++] = c[0];
			faulty[faulty_count++] = a[0];
			faulty[faulty_count++] = a[0] + sum;
			faulty[faulty_count++] = a[0] + sum * 2;
			printf("	faulty = %d %d %d\n", a[0], a[0] + sum, a[0] + sum * 2);
			break;
		case 5:
			real[real_count++] = a[0];
			real[real_count++] = a[0] + sum;
			real[real_count++] = a[0] + sum * 2;
			real[real_count++] = c[0];
			faulty[faulty_count++] = b[0];
			faulty[faulty_count++] = b[0] + sum;
			faulty[faulty_count++] = b[0] + sum * 2;
			printf("	faulty = %d %d %d\n", b[0], b[0] + sum, b[0] + sum * 2);
			break;
		case 6:
			// 남은 equals 3개가 모두 같은 무게일 경우
			faulty[faulty_count] = -1;
			tmp_b[0] = -1;
			tmp_c[0] = -1;
			if (balance(faulty, tmp_b, tmp_c) == SUCCESS) return 0;		// 남은 equals 동전을 안넣고 체크.
			else {												// SUCCESS가 아니면 남은 equals는 모두 faulty 동전.
				faulty[faulty_count++] = a[0];
				faulty[faulty_count++] = a[0] + sum;
				faulty[faulty_count++] = a[0] + sum * 2;
				faulty[faulty_count++] = b[0];
				faulty[faulty_count++] = b[0] + sum;
				faulty[faulty_count++] = b[0] + sum * 2;
				faulty[faulty_count++] = 99;
				printf("	faulty = %d %d %d %d %d %d 99 \n", a[0], a[0] + sum, a[0] + sum * 2, b[0], b[0] + sum, b[0] + sum * 2);

			}
			break;
		}
	}

	printf("//////////////////////////////////////////\n");
	for (i = 0; i < faulty_count; i++)
	{
		//printf("result = %d\n", faulty[i]);
		a[i] = faulty[i];
	}
	printf("\n끝!!!\n");
	printf("result_count = %d\n", faulty_count);
	a[i] = -1; b[0] = -1; c[0] = -1;
	balance(a, b, c);

	return 0;
}