#include <stdio.h>
#include <stdlib.h>


int WaitingTime(process);	// 프로세스별 waiting time
int TurnaroundTime(process); // 프로세스별 turnaround time
void compare(fcfs, pre_sjf,np_sjf,pre_pri,np_pri, rr,multi,hrrn); // compare
//void printGantt(Gantt,time,num_of_process);

typedef struct process {
	int p_id; // Process ID
	int waiting_time; // process가 기다리는 시간
	int turnaround_time; // process가 
	int start_time; // round robin에서 time quantum만큼 실행하기 위해 사용했음
	int cpu_burst_time;
	int io_create; // io 가 발생하는지 안하는지 상태. 1이면 발생하고 0이면 발생 안함
	int io_burst_time; //io burst 시간
	int io_start_time; //io가 시작하는 시간
	int io_end_time; //io 끝나는 시간
	int arrival_time; // Queue에 도착하는 시간
	int remain_time; // process 실행하면서 남는 burst 시간
	int end_time; //process 실행이 끝나는 시간
	int priority; //process의 우선순위
	int age; // starvation 방지하기 위해 age 변수 추가.
	int intermediate; // Round robin에서quantum만큼 실행하기 위해 중간 저장하는 변수
	float hrrn_priority; //HRRN scheduler에서의 우선순위
} process;


typedef struct queue {
	process Q[100];
	int p_num;
}Queue;	// process 100칸짜리 queue process를 20개만 받는데 100개를 한 이유는 round robin과 multilevel queue 때문

typedef struct wait_turn {
	float avg_waiting;	
	float avg_turnaround;
}W_and_T; // 각 schedule 방식 함수들에서 사용할 것. return값을 평균 waiting과 turnaround 로 하기위해 생성한 구조체

W_and_T FCFS(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Pre_SJF(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Nonp_SJF(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Pre_Priority(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Nonp_Priority(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Round_Robin(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Multilevel_Queue(Waiting_Q, multi_Ready_Q1,multi_Ready_Q2,Result_Q,num_of_process);
W_and_T HRRN(Waiting_Q, Ready_Q, Result_Q, num_of_process); // Highest Response Ratio Next, priority를 (waiting time + cpu burst) / cpu burst 로 하는 스케쥴링 기법.


Queue Initialize_Q(Queue que) {
	int i;
	for (i = 0; i < que.p_num; i++) {
		que.Q[i].p_id = i + 1;
		que.Q[i].p_id = 0;
		que.Q[i].cpu_burst_time = 0;
		que.Q[i].start_time = 0;
		que.Q[i].io_burst_time = 0;
		que.Q[i].io_start_time = 0;
		que.Q[i].arrival_time = 0;
		que.Q[i].remain_time = 0;
		que.Q[i].end_time = 0;
		que.Q[i].priority = 0;
	}
	que.p_num = 0;
} // 초기화하는 함수, 사용하지 않았지만 혹시나 해서 삭제X

Queue FCFS_Sort_process(Queue que) {	// Queue를 arrival time 순서에 따라 정렬하는 함수
	int i, j;
	process tmp_process;
	for (j = 0; j < que.p_num - 1; j++) {
		for (i = 0; i < que.p_num - 1; i++) {
			if (que.Q[i].arrival_time > que.Q[i + 1].arrival_time) {
				tmp_process = que.Q[i];
				que.Q[i] = que.Q[i + 1];
				que.Q[i + 1] = tmp_process;
			} // arrival 시간 빠른 순서에 따라 sort
			else if (que.Q[i].arrival_time == que.Q[i + 1].arrival_time) {
				if (que.Q[i].priority > que.Q[i + 1].priority) {
					tmp_process = que.Q[i];
					que.Q[i] = que.Q[i + 1];
					que.Q[i + 1] = tmp_process;
				}
			} // 만약 arrival 시간이 같다면 priority가 낮은 것 먼저 수행하도록 앞으로
		}
	}
	return que;
}

Queue SJF_Sort_process(Queue que,int first, int rear) {	// Queue를 cpu_burst_time이 빠른 순서에 따라 정렬하는 함수
	int i,j;
	process tmp_process;

	for (j = first; j < rear; j++) {
			for (i = first; i < rear-1; i++) { // rear-1 까지 하는 이유? rear+1까지 비교하기 때문
				if (que.Q[i].remain_time > que.Q[i + 1].remain_time) {
					tmp_process = que.Q[i];
					que.Q[i] = que.Q[i + 1];
					que.Q[i + 1] = tmp_process;
				} // cpu_burst 시간 짧은 순서에 따라 sort
				else if (que.Q[i].remain_time == que.Q[i + 1].remain_time) {
					if (que.Q[i].priority > que.Q[i + 1].priority) {
						tmp_process = que.Q[i];
						que.Q[i] = que.Q[i + 1];
						que.Q[i + 1] = tmp_process;
					}
				} // 만약 cpu_burst 시간이 같다면 priority가 낮은 것 먼저 수행하도록 앞으로
			}
		}
	return que;
}

Queue Pri_Sort_process(Queue que,int first, int rear) {
	int i, j;
	process tmp_process;

	for (j = first; j < rear - 1; j++) {
		for (i = first; i < rear - 1; i++) {
			if (que.Q[i].priority > que.Q[i + 1].priority) {
				tmp_process = que.Q[i];
				que.Q[i] = que.Q[i + 1];
				que.Q[i + 1] = tmp_process;
			} // priority 순서에 따라 sort
			else if (que.Q[i].priority == que.Q[i + 1].priority) {
				if (que.Q[i].remain_time > que.Q[i + 1].remain_time) {
					tmp_process = que.Q[i];
					que.Q[i] = que.Q[i + 1];
					que.Q[i + 1] = tmp_process;
				}
			} // 만약 priority 같다면 cpu_burst 짧은 것 먼저 수행하도록 앞으로
		}
	}
	return que;
}

Queue HRRN_Sort_process(Queue que, int first, int rear) {
	int i, j;
	process tmp_process;

	for (j = first; j < rear - 1; j++) {
		for (i = first; i < rear - 1; i++) {
			if (que.Q[i].hrrn_priority < que.Q[i + 1].hrrn_priority) {	// hrrn_priority에 따라 정렬 여기선 큰게 좋은거라 큰 것을 앞으로
				tmp_process = que.Q[i];
				que.Q[i] = que.Q[i + 1];
				que.Q[i + 1] = tmp_process;
			} // hrrn priority 순서에 따라 sort
		}
	}
	return que;
}

void main(void) {

	int sch_type; // CPU scheduler 유형 선택
	W_and_T fcfs, np_sjf, pre_sjf, np_pri, pre_pri,rr,multi,hrrn; // 각 scheduler의 return값 저장할 구조체 생성
	
 
	int j; // for문에 사용할 변수
	int time; 

	Queue Waiting_Q, Ready_Q, Result_Q;	//waiting queue와 ready queue 생성
	Queue multi_Ready_Q1, multi_Ready_Q2; // multi-level queue 에서 사용할 queue 2개 
	// Q1은 RR, Q2는 FCFS
	
	Waiting_Q.p_num = 0;
	Ready_Q.p_num = 0;
	Result_Q.p_num = 0;
	multi_Ready_Q1.p_num = 0;
	multi_Ready_Q2.p_num = 0;
	// Q들 p_num 우선 모두 0으로 초기화

	printf("Input the number of process.\n"); // 생성할 프로세스 개수 입력받음
	scanf("%d", &Waiting_Q.p_num); // 입력받은 프로세스 수 Waiting_Q의 process 수로 입력

	printf("Process ID   CPU Burst time    Arrival time   Priority\n");
	for (j = 0; j < Waiting_Q.p_num; j++) {
		Waiting_Q.Q[j].p_id = j + 1; // process id는 Q 앞에서부터 1,2,3,...으로 부여
		Waiting_Q.Q[j].cpu_burst_time = rand() %20 + 1; // cpu_burst_time은 1~20사이 random부여
		Waiting_Q.Q[j].arrival_time = rand()%10; //arrival time은 0~9 random부여
		Waiting_Q.Q[j].intermediate = Waiting_Q.Q[j].cpu_burst_time; // round robin과 multilevel queue에서 사용할 중간변수
		Waiting_Q.Q[j].remain_time = Waiting_Q.Q[j].cpu_burst_time; // 실행 전 remain time은 cpu burst time과 같음
		Waiting_Q.Q[j].priority = rand() % Waiting_Q.p_num + 1; //우선순위는 1~프로세수 개수중 random 부여
		Waiting_Q.Q[j].age = 0; // age는 우선 모두 0으로 초기화. Ready_Q에서 실행중이 아닐 때 증가할 것임
		Waiting_Q.Q[j].io_create = rand() % 2; // io를 발생할 건지? 발생하면 1 안하면 0
		if (Waiting_Q.Q[j].io_create == 1) { Waiting_Q.Q[j].io_burst_time = rand() % 5 + 1; }// 만약 io가 발생하면 1~5사이의 burst time
		Waiting_Q.Q[j].hrrn_priority = (Waiting_Q.Q[j].age + Waiting_Q.Q[j].remain_time) / Waiting_Q.Q[j].remain_time; // hrrn 의 priority 공식
		printf(" %d\t\t %d\t\t %d\t\t %d\n",Waiting_Q.Q[j].p_id,Waiting_Q.Q[j].cpu_burst_time, Waiting_Q.Q[j].arrival_time, Waiting_Q.Q[j].priority);	
	}

	while (1) {
		printf("\nSelect the Scheduler type\n");
		printf("  1. FCFS\n  2. Preemptive SJF\n  3. Nonpreemptive SJF\n  4. Preemptive priority\n  5. Nonpreemptive priority\n  6. RR\n  7. Multilevel Queue\n  8. HRRN\n  9. Compare\n  0.EXIT\n");
		scanf("%d", &sch_type);

		if (sch_type == 0) {return;} // Exit 선택하기 전에는 종료하지 않음
		switch (sch_type) {
		case 1:			
			printf("You Select FCFS\n");
			fcfs = FCFS(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 2:
			printf("You Select Preemptive SJF\n");
			pre_sjf = Pre_SJF(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 3:
			printf("You Select Nonpreemptive SJF\n");
			np_sjf = Nonp_SJF(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 4:
			printf("You Select Preemptive Priority\n");
			pre_pri =Pre_Priority(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 5:
			printf("You Select Nonpreemptive Priority\n");
			np_pri = Nonp_Priority(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 6:
			printf("You Select Round Robin\n");
			rr = Round_Robin(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 7:
			printf("You Select multilevel queue\n");
			printf("Q1 is RR and Q2 is FCFS\n");
			multi = Multilevel_Queue(Waiting_Q, multi_Ready_Q1,multi_Ready_Q2, Result_Q, Waiting_Q.p_num);
			break;
		case 8:
			printf("You Select HRRN\n");
			hrrn = HRRN(Waiting_Q, Ready_Q, Result_Q, Waiting_Q.p_num);
			break;
		case 9:
			compare(fcfs, pre_sjf, np_sjf, pre_pri, np_pri, rr,multi,hrrn);
		} // 선택된  scheduler에 따른 switch문 실행 끝

	}

}

int WaitingTime(process process) {
	int i;
	process.waiting_time = process.end_time - process.arrival_time - process.cpu_burst_time;
	return process.waiting_time;
}
int TurnaroundTime(process process) {
	int i;
	process.turnaround_time = process.end_time - process.arrival_time;
	return process.turnaround_time;
}
void compare(W_and_T fcfs, W_and_T pre_sjf, W_and_T np_sjf, W_and_T pre_pri, W_and_T np_pri, W_and_T rr, W_and_T multi,W_and_T hrrn) {
	printf("FCFS\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", fcfs.avg_waiting, fcfs.avg_turnaround);
	
	printf("Preemptive SJF\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", pre_sjf.avg_waiting, pre_sjf.avg_turnaround);

	printf("Nonpreemptive SJF\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", np_sjf.avg_waiting, np_sjf.avg_turnaround);
	
	printf("Preemptive Priority\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", pre_pri.avg_waiting, pre_pri.avg_turnaround);
	
	printf("Nonpreemptive Priority\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", np_pri.avg_waiting, np_pri.avg_turnaround);
	
	printf("Round robin\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", rr.avg_waiting, rr.avg_turnaround);
	
	printf("Multilevel Queue\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", multi.avg_waiting, multi.avg_turnaround);
	
	printf("HRRN\n");
	printf("Average Waiting Time: %6f\t, Average Turnaround Time: %6f\n\n", hrrn.avg_waiting, hrrn.avg_turnaround);

}

W_and_T FCFS(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int i, j, k;
	int time = 0;
	int burst; //for문에서 사용할 cpu_burst변수
	int io_burst; // io 실행에 사용할 burst 변수
	int Gantt[400]; // 간트차트를 위한 배열

	W_and_T fcfs;
	float sum_of_waiting = 0, sum_of_turnaround = 0;
	printf("FCFS\n");
	Ready_Q = FCFS_Sort_process(Waiting_Q); // 프로세스들을 arrival time에 따라 정렬해서 Ready_Q에 대입
	Initialize_Q(Result_Q);
	for (i = 0; i < num_of_process; i++) {
		while (1) {
			if (time < Ready_Q.Q[i].arrival_time) { Gantt[time] = 0; printf("Time:%d		IDLE\n", time); time++; continue; } 
			else {
				for (burst = Ready_Q.Q[i].cpu_burst_time; burst > 0; burst--) {
					Gantt[time] = Ready_Q.Q[i].p_id;
					printf("Time:%d		Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[i].p_id, Ready_Q.Q[i].arrival_time, Ready_Q.Q[i].cpu_burst_time, Ready_Q.Q[i].priority, Ready_Q.Q[i].remain_time); // 과정
					time++;
					Ready_Q.Q[i].remain_time--;
				}
				Ready_Q.Q[i].end_time = time;
				Result_Q.Q[i] = Ready_Q.Q[i];
				//			Initialize_Q(Ready_Q);  // 결과를 Result_Q로 이동 후 Ready_Q 초기화
				break;
			}
		}
	}

	printf("Result\n");
	for (j = 0; j < num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]); // 모든 Queue
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	fcfs.avg_waiting = sum_of_waiting / (float)num_of_process;
	fcfs.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("FCFS \nAverage Waiting Time: %6f, Turnaround Time: %6f\n", fcfs.avg_waiting, fcfs.avg_turnaround);
	//printGantt(Result_Q, time,num_of_process);
	
	for(i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}//Gantt chart 출력
	printf("\n");

	return fcfs;
}

W_and_T Pre_SJF(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int i, j,k;
	int time=0; // 시간
	int first = 0;
	int rear = 0; // Q 이동할 때 맨 뒤에 넣기 위해
	int burst;
	int Gantt[400];
	W_and_T pre_sjf;
	float sum_of_waiting = 0, sum_of_turnaround = 0; // 평균 waiting과 turnaround time을 구하기 위해
	Waiting_Q = SJF_Sort_process(Waiting_Q, 0, num_of_process);
	printf("Preemptive SJF\n");
	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // 도착하면 ready_Q 로 넘김
			}
		} // 시간 도착하는 대로 Ready_Q 뒤쪽으로 뽑아옴
		
		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // 아직 도착한 것이 하나도 없을 때 
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++; 
			continue; 
		}
		else {
			Ready_Q = SJF_Sort_process(Ready_Q, first, rear); // 도착한 것과 실행중인 것 포함 sort
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // 과정
			Gantt[time] = Ready_Q.Q[first].p_id;
			time++;
			Ready_Q.Q[first].remain_time--;

			if (Ready_Q.Q[first].remain_time == 0) { // 실행 끝났으면
				Ready_Q.Q[first].end_time = time; // 끝난시간 저장
				Result_Q.Q[first] = Ready_Q.Q[first]; // 결과 queue로 이동
				first++; // 다음 process 실행
				Ready_Q = SJF_Sort_process(Ready_Q, first, rear); // sort
			}			
			for (k = first+1 ; k < num_of_process; k++) {// 현재 실행중이 아닌 것들에 대해서
				Ready_Q.Q[k].age++;		// age를 1씩 증가
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age가 6 증가할 때마다 priority를 1씩 줄이고, 음수가 되지 않도록 조건 추가

		}

		if (first == num_of_process) { break; } // 끝까지 다 실행했으면 while문 break
	}

	printf("Result\n");
	for (j = 0; j<num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]);
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	pre_sjf.avg_waiting = sum_of_waiting / (float)num_of_process;
	pre_sjf.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("Preemptive SJF \nAverage Waiting Time: %6f, Turnaround Time: %6f", pre_sjf.avg_waiting, pre_sjf.avg_turnaround);
	printf("\n\n Gantt Chart \n");
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}
	printf("\n");

	return pre_sjf;
}

W_and_T Nonp_SJF(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int i, j, k;
	int first = 0;
	int rear = 0; // Q 이동할 때 맨 뒤에 넣기 위해
	int time = 0; // 시간
	int burst;
	int Gantt[400];
	W_and_T np_sjf;
	Waiting_Q = SJF_Sort_process(Waiting_Q, 0, num_of_process);
	float sum_of_waiting = 0, sum_of_turnaround = 0; // 평균 waiting과 turnaround time을 구하기 위해
	printf("Nonpreemptive SJF\n");

	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) { // 도착시간이 되면
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q 로 넘김
			} //얘네를 포함해서 sort해야함
		} // 시간 도착하는 대로 Ready_Q 뒤쪽으로 뽑아옴
		if (Ready_Q.Q[first].cpu_burst_time == Ready_Q.Q[first].remain_time) {
			Ready_Q = SJF_Sort_process(Ready_Q, first, rear);
		}// first에 있는 것이 아직 실행 안했으면 현재 도착한 것들과 포함해서 같이 sort 해야함
		else { Ready_Q = SJF_Sort_process(Ready_Q, first + 1, rear); } //first에 있는 process가 현재 실행중이면 실행중인 것 빼고 sort
		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // 아직 도착 아무것도 안했거나 실행할 것이 도착X
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++; continue;
		}
		else {
			Ready_Q = SJF_Sort_process(Ready_Q, first+1, rear); // 실행중인 것 빼고 sort
			Gantt[time] = Ready_Q.Q[first].p_id;
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // 과정
			time++;
			Ready_Q.Q[first].remain_time--;
			
			if (Ready_Q.Q[first].remain_time == 0) {
				Ready_Q.Q[first].end_time = time;	//끝나는 시간 저장
				Result_Q.Q[first] = Ready_Q.Q[first];	//끝나면 result_Q로 이동
				first++; // 다음 process 실행 위해+1	
				Ready_Q = SJF_Sort_process(Ready_Q, first , rear); // 실행중 아닌 것들 한번 정렬
			}	
			for (k = first + 1; k < num_of_process; k++) {// 현재 실행중이 아닌 것들에 대해서
				Ready_Q.Q[k].age++;		// age를 1씩 증가
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age가 6 증가할 때마다 priority를 1씩 줄이고, 음수가 되지 않도록 조건을 추가
		}
		if (first == num_of_process) { break; }	// 실행이 끝나면 while문 나감
	}
	printf("Result\n");
	for (j = 0; j<num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]);
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	np_sjf.avg_waiting = sum_of_waiting / (float)num_of_process;
	np_sjf.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("Nonpreemptive SJF \nAverage Waiting Time: %6f, Turnaround Time: %6f", np_sjf.avg_waiting, np_sjf.avg_turnaround);
	printf("\n\n Gantt Chart \n");
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}
	printf("\n");

	return np_sjf;
}

W_and_T Pre_Priority(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int i, j, k;
	int time = 0; // 시간
	int first = 0;
	int rear = 0; // Q 이동할 때 맨 뒤에 넣기 위해
	int burst;
	int Gantt[400];
	W_and_T pre_pri;
	float sum_of_waiting = 0, sum_of_turnaround = 0; // 평균 waiting과 turnaround time을 구하기 위해
	Waiting_Q = Pri_Sort_process(Waiting_Q, 0, num_of_process); // 우선순위 높은 순으로 정렬, 우선수위가 같으면 shortest job first
	printf("Preemptive Priority\n");
	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q 로 넘김
			}
		} // 시간 도착하는 대로 Ready_Q 뒤쪽으로 뽑아옴

		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // 도착한 것 없으면 IDLE
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++;
			continue;
		}
		else {//실행할 것이 있으면
			Ready_Q = Pri_Sort_process(Ready_Q, first, rear); // 도착한 것과 실행중인 것 포함 sort
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // 과정
			Gantt[time] = Ready_Q.Q[first].p_id;
			time++;
			Ready_Q.Q[first].remain_time--;


			if (Ready_Q.Q[first].remain_time == 0) { // 실행 끝났으면
				Ready_Q.Q[first].end_time = time; // 끝나는 시간 저장 후
				Result_Q.Q[first] = Ready_Q.Q[first]; // result Queue로 결과 넘김
				first++;
				Ready_Q = Pri_Sort_process(Ready_Q, first, rear); // 한번 sort
			}
			for (k = first + 1; k < num_of_process; k++) {// 현재 실행중이 아닌 것들에 대해서
				Ready_Q.Q[k].age++;		// age를 1씩 증가
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age가 6 증가할 때마다 priority를 1씩 줄이고, 음수가 되지 않도록 조건을 추가
		}

		if (first == num_of_process) { break; }
	}

	printf("Result\n");
	for (j = 0; j<num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]);
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	pre_pri.avg_waiting = sum_of_waiting / (float)num_of_process;
	pre_pri.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("Preemptive Priority \nAverage Waiting Time: %6f, Turnaround Time: %6f", pre_pri.avg_waiting, pre_pri.avg_turnaround);
	printf("\n\n Gantt Chart \n");
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}
	printf("\n");

	return pre_pri;
}

W_and_T Nonp_Priority(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int i, j, k;
	int first = 0;
	int rear = 0; // Q 이동할 때 맨 뒤에 넣기 위해
	int time = 0; // 시간
	int burst;
	int Gantt[400];
	W_and_T np_pri;
	Waiting_Q = Pri_Sort_process(Waiting_Q, 0, num_of_process);
	float sum_of_waiting = 0, sum_of_turnaround = 0; // 평균 waiting과 turnaround time을 구하기 위해
	printf("Nonpreemptive Priority\n");

	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) { // 도착시간이 되면
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q 로 넘김
			} //얘네를 포함해서 sort해야함
		} // 시간 도착하는 대로 Ready_Q 뒤쪽으로 뽑아옴
		if (Ready_Q.Q[first].cpu_burst_time == Ready_Q.Q[first].remain_time) { 
			Ready_Q = Pri_Sort_process(Ready_Q, first, rear);
		}// first에 있는 것이 아직 실행 시작 안했으면 현재 도착한 것들과 포함해서 같이 sort 해야함
		else { Ready_Q = Pri_Sort_process(Ready_Q, first + 1, rear); } // 현재 실행중이었으면 빼고 sort

		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // 아직 도착 아무것도 안했거나 실행할 것이 도착X
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++; continue;
		}
		else {
			Ready_Q = Pri_Sort_process(Ready_Q, first + 1, rear); // 도착한 것과 실행중인 것 포함 sort
			Gantt[time] = Ready_Q.Q[first].p_id;
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // 과정
			time++;
			Ready_Q.Q[first].remain_time--;

			if (Ready_Q.Q[first].remain_time == 0) {
				Ready_Q.Q[first].end_time = time;	//끝나는 시간 저장
				Result_Q.Q[first] = Ready_Q.Q[first];	//끝나면 result_Q로 이동
				first++; // 다음 process 실행 위해		
				Ready_Q = Pri_Sort_process(Ready_Q, first, rear); // 실행중 아닌 것들 한번 정렬
			}
			for (k = first + 1; k < num_of_process; k++) {// 현재 실행중이 아닌 것들에 대해서
				Ready_Q.Q[k].age++;		// age를 1씩 증가
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age가 6 증가할 때마다 priority를 1씩 줄이고, 음수가 되지 않도록 조건을 추가
		}
		if (first == num_of_process) { break; }	// 실행이 끝나면 while문 나감
	}
	printf("Result\n");
	for (j = 0; j<num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]);
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	np_pri.avg_waiting = sum_of_waiting / (float)num_of_process;
	np_pri.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("Nonpreemptive Priority \nAverage Waiting Time: %6f, Turnaround Time: %6f", np_pri.avg_waiting, np_pri.avg_turnaround);
	printf("\n\n Gantt Chart \n");
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}
	printf("\n");
	return np_pri;
}

W_and_T Round_Robin(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int time_quantum;
	int time=0;
	int first = 0, rear = 0;
	int rear_for_result=0;
	int condition; // 모든 작업들이 끝났는 지 확인할 때 사용할 함수
	int i, j, k;
	int burst;
	float sum_of_waiting=0, sum_of_turnaround=0;
	int Gantt[400];
	W_and_T rr;

	printf("Round Robin\n");
	printf("Input time quantum\n");
	scanf("%d",&time_quantum);

	Waiting_Q = FCFS_Sort_process(Waiting_Q); // 우선 arrival time 순으로 정렬, 만약 arrival time 같다면 우선순위로 결정

	while (1) {
				condition = 0; // 체크할때마다 0으로 초기화해주어야함
				for (i = 0; i < num_of_process; i++) {
					if (Waiting_Q.Q[i].arrival_time == time) {
						Ready_Q.Q[rear++] = Waiting_Q.Q[i];// 도착할때마다 Ready_Q 맨 뒤로 보냄 굳이 위에서 sort를 해주는 이유는 우선순위도 넣어주려고..
					}
				}
				if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // 아직 도착한게 없으면
					Gantt[time] = 0; 
					printf("Time:%d		IDLE\n", time); 
					time++; 
					continue;
				}
				else { // 실행할 것이 있으면
					printf("Time:%d			Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // 과정
					if (Ready_Q.Q[first].remain_time == Ready_Q.Q[first].intermediate) { // 시작할때
						Ready_Q.Q[first].start_time = time;// 시간을 start time으로 저장
					}
					Gantt[time] = Ready_Q.Q[first].p_id;
					time++;
					Ready_Q.Q[first].remain_time--; 

					if (Ready_Q.Q[first].remain_time == 0) {
						Ready_Q.Q[first].end_time = time; // 끝났을 경우 시간 저장
						Result_Q.Q[rear_for_result++] = Ready_Q.Q[first++]; // 결과로 이동 후 first++
					}
					else if ((time - Ready_Q.Q[first].start_time) == time_quantum) {//time quantume만큼 실행했을 경우
						Ready_Q.Q[first].intermediate = Ready_Q.Q[first].remain_time; //중간시간. time quantum만큼 실행하기 위해 추가한 변수.
						Ready_Q.Q[rear] = Ready_Q.Q[first++]; // 맨뒤로 이동 후 first++
						rear++;
					}
				}

				for (k = 0; k < rear_for_result; k++) {
					if (Result_Q.Q[k].remain_time == 0) {
						condition++; // 끝난 process들 개수 모두 체크해서 
					}
				}
				if (condition == num_of_process) { break; }// 모든 process들이 끝나면 while문 break
			}
	printf("Result\n");
	for (j = 0; j < num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]); 
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	rr.avg_waiting = sum_of_waiting / (float)num_of_process;
	rr.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("Round Robin \nAverage Waiting Time: %6f, Turnaround Time: %6f", rr.avg_waiting, rr.avg_turnaround);
	printf("\n\n Gantt Chart \n");
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}
	printf("\n");
	return rr;
}

W_and_T Multilevel_Queue(Queue Waiting_Q, Queue multi_Ready_Q1, Queue multi_Ready_Q2, Queue Result_Q, int num_of_process) {
	int i, j, k;
	int time=0; 
	int burst1, burst2;
	int first1=0, first2=0, rear1=0,rear2=0; // 각 Q1,2을 위한 것
	int rear_for_result=0; // 결과 queue를 위한 것, while 나가는 조건문에서도 사용함
	int rear_tmp = 0; // tmp로 시간대로 보내기 위한 함수
	float sum_of_waiting = 0, sum_of_turnaround = 0;
	int condition;
	int time_quantum; // Q1 quantum
	int Gantt_Q1[400], Gantt_Q2[400];
	Queue tmp_Q; // 각 Q로 process 나누기 위한 임시 Queue 
	W_and_T multi;

	printf("Input time quantum for Q1\n");
	scanf("%d", &time_quantum);
	Waiting_Q = FCFS_Sort_process(Waiting_Q); // 우선순위도 넣어주려고 정렬함
	while (1) {
		
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				tmp_Q.Q[rear_tmp] = Waiting_Q.Q[i]; // Queue 2개로 조건에 맞춰 나누기 위해 우선 임시 Queue로 할당
				if (rear_tmp % 2 == 0) { multi_Ready_Q1.Q[rear1++] = tmp_Q.Q[rear_tmp++]; } // 홀수번째에 도착한 것은 Q1에 할당
				else { multi_Ready_Q2.Q[rear2++] = tmp_Q.Q[rear_tmp++]; } //짝수번째 도착은 Q2에 할당
			}
		} // Q1 과 Q2로 나누기까지의 과정

		if ((rear1 == 0) || (time < multi_Ready_Q1.Q[first1].arrival_time) || (first1==rear1)) { // Q1에 도착X 또는 끝까지 실행 했을때
			if ((rear2 == 0) || (time < multi_Ready_Q2.Q[first2].arrival_time) ) { // Q2에도 아직 도착한 것이 없을 때 
				Gantt_Q1[time] = 0;
				Gantt_Q2[time] = 0;
				printf("Time:%d		IDLE\n", time); time++; continue; // 둘다 실행할 것이 없으면
			}
			else { // 만약 Q1은 실행할 것이 없고 Q2에 실행할 것이 있을 때
				//도착 순서대로 이미 정렬되어 있기 때문에 따로 정렬할 필요 없이 바로 실행해주면 된다.
				printf("Time:%d		Queue2		Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, multi_Ready_Q2.Q[first2].p_id, multi_Ready_Q2.Q[first2].arrival_time, multi_Ready_Q2.Q[first2].cpu_burst_time, multi_Ready_Q2.Q[first2].priority, multi_Ready_Q2.Q[first2].remain_time); // 과정
				Gantt_Q1[time] = 0;
				Gantt_Q2[time] = multi_Ready_Q2.Q[first2].p_id;
				time++;
				multi_Ready_Q2.Q[first2].remain_time--;
				if (multi_Ready_Q2.Q[first2].remain_time == 0) {
					multi_Ready_Q2.Q[first2].end_time = time; // 끝났을 경우 시간 저장
					Result_Q.Q[rear_for_result++] = multi_Ready_Q2.Q[first2];
					first2++;
				}
			}
		}
		else {//Q1에 실행할 것이 있을 때
			printf("Time:%d		Queue1		Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, multi_Ready_Q1.Q[first1].p_id, multi_Ready_Q1.Q[first1].arrival_time, multi_Ready_Q1.Q[first1].cpu_burst_time, multi_Ready_Q1.Q[first1].priority, multi_Ready_Q1.Q[first1].remain_time); // 과정
			if (multi_Ready_Q1.Q[first1].remain_time == multi_Ready_Q1.Q[first1].intermediate) {//시작할때
				multi_Ready_Q1.Q[first1].start_time = time;// 시간을 start time으로 저장
			}
			Gantt_Q1[time] = multi_Ready_Q1.Q[first1].p_id;
			Gantt_Q2[time] = 0;
			time++;
			multi_Ready_Q1.Q[first1].remain_time--;
			if (multi_Ready_Q1.Q[first1].remain_time == 0) { // 다 실행했을 경우
				multi_Ready_Q1.Q[first1].end_time = time;
				Result_Q.Q[rear_for_result++] = multi_Ready_Q1.Q[first1++];
			}
			else if ((time - multi_Ready_Q1.Q[first1].start_time) == time_quantum) {//time quantume만큼 실행했을 경우
				multi_Ready_Q1.Q[first1].intermediate = multi_Ready_Q1.Q[first1].remain_time;
				multi_Ready_Q1.Q[first1].age = 0; // Q2로 가기전에 우선 age 초기화
				multi_Ready_Q2.Q[rear2++] = multi_Ready_Q1.Q[first1++]; // Q2로 이동
				
			}
		}

		for (j = first2+1; j < rear2; j++) { //실행중인것이 뺴고 있으니까 age 1씩 증가
			multi_Ready_Q2.Q[j].age++; // Q2에서 실행중 아닌 것들은 age계속 추가
			if (multi_Ready_Q2.Q[j].age == 15) {
				multi_Ready_Q1.Q[rear1++] = multi_Ready_Q2.Q[j]; // age가 15가 넘는 것들이 있으면 starvation 방지를 위해 Q1로 넘겨줌
			}
		}

		condition = 0;

		for (k = 0; k < rear_for_result;k++) {	// result queue 모두 끝나면 condition == process수 이므로 while문 나가는 조건으로
			if (Result_Q.Q[k].remain_time == 0) { condition++; }
		}
		if (condition == num_of_process) { break; }
	}

	printf("Result\n");
	for (j = 0; j<num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]);
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	multi.avg_waiting = sum_of_waiting / (float)num_of_process;
	multi.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("Multilevel Queue\nAverage Waiting Time: %6f, Turnaround Time: %6f", multi.avg_waiting, multi.avg_turnaround);

	printf("\n\n Q1 Gantt Chart \n"); // Queue1 간트
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt_Q1[i]);
	}
	printf("\n");

	printf("\n\n Q2 Gantt Chart \n");	//Queue2 간트
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt_Q2[i]);
	}
	printf("\n");

	return multi;
}

W_and_T HRRN(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) { // preemptive이고, sjf를 보완한 것이다.
	int i, j, k;																	// hrrn의 priority = (age + burst)/burst.
	int time = 0; // 시간
	int first = 0;
	int rear = 0; // Q 이동할 때 맨 뒤에 넣기 위해
	int burst;
	int Gantt[400];
	W_and_T hrrn;
	float sum_of_waiting = 0, sum_of_turnaround = 0; // 평균 waiting과 turnaround time을 구하기 위해

	Waiting_Q = SJF_Sort_process(Waiting_Q,0,num_of_process); // 처음엔 모두 hrrn_pri가 1이기 때문에 hrrn sort를 해도 의미없다. 따라서 sjf sort

	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q 로 넘김
			}
		} // 시간 도착하는 대로 Ready_Q 뒤쪽으로 뽑아옴
		Ready_Q = HRRN_Sort_process(Ready_Q, first, rear);
		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // 아직 도착한 것이 하나도 없을 때 
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++;
			continue;
		}
		else {
			Ready_Q = HRRN_Sort_process(Ready_Q, first, rear); // 도착한 것과 실행중인 것 포함 sort
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  HRRN priority: %6f, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].hrrn_priority, Ready_Q.Q[first].remain_time); // 과정
			Gantt[time] = Ready_Q.Q[first].p_id;
			time++;
			Ready_Q.Q[first].remain_time--;

			if (Ready_Q.Q[first].remain_time == 0) {
				Ready_Q.Q[first].end_time = time;
				Result_Q.Q[first] = Ready_Q.Q[first];
				first++;
			//	Ready_Q = HRRN_Sort_process(Ready_Q, first, rear);
			}
			for (i = first + 1; i < rear; i++) { // 실행중 아닌 것들의 age 하나씩 올리기 위해
				Ready_Q.Q[i].age++;
				Ready_Q.Q[i].hrrn_priority = (float)(Ready_Q.Q[i].age + Ready_Q.Q[i].remain_time) / Ready_Q.Q[i].remain_time; 
			}
		}

		if (first == num_of_process) { break; }

	}
	printf("Result\n");
	for (j = 0; j<num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]);
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	hrrn.avg_waiting = sum_of_waiting / (float)num_of_process;
	hrrn.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("HRRN \nAverage Waiting Time: %6f, Turnaround Time: %6f", hrrn.avg_waiting, hrrn.avg_turnaround);
	printf("\n\n Gantt Chart \n");
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}
	printf("\n");

	return hrrn;
}
