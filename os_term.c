#include <stdio.h>
#include <stdlib.h>


int WaitingTime(process);	// ���μ����� waiting time
int TurnaroundTime(process); // ���μ����� turnaround time
void compare(fcfs, pre_sjf,np_sjf,pre_pri,np_pri, rr,multi,hrrn); // compare
//void printGantt(Gantt,time,num_of_process);

typedef struct process {
	int p_id; // Process ID
	int waiting_time; // process�� ��ٸ��� �ð�
	int turnaround_time; // process�� 
	int start_time; // round robin���� time quantum��ŭ �����ϱ� ���� �������
	int cpu_burst_time;
	int io_create; // io �� �߻��ϴ��� ���ϴ��� ����. 1�̸� �߻��ϰ� 0�̸� �߻� ����
	int io_burst_time; //io burst �ð�
	int io_start_time; //io�� �����ϴ� �ð�
	int io_end_time; //io ������ �ð�
	int arrival_time; // Queue�� �����ϴ� �ð�
	int remain_time; // process �����ϸ鼭 ���� burst �ð�
	int end_time; //process ������ ������ �ð�
	int priority; //process�� �켱����
	int age; // starvation �����ϱ� ���� age ���� �߰�.
	int intermediate; // Round robin����quantum��ŭ �����ϱ� ���� �߰� �����ϴ� ����
	float hrrn_priority; //HRRN scheduler������ �켱����
} process;


typedef struct queue {
	process Q[100];
	int p_num;
}Queue;	// process 100ĭ¥�� queue process�� 20���� �޴µ� 100���� �� ������ round robin�� multilevel queue ����

typedef struct wait_turn {
	float avg_waiting;	
	float avg_turnaround;
}W_and_T; // �� schedule ��� �Լ��鿡�� ����� ��. return���� ��� waiting�� turnaround �� �ϱ����� ������ ����ü

W_and_T FCFS(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Pre_SJF(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Nonp_SJF(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Pre_Priority(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Nonp_Priority(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Round_Robin(Waiting_Q, Ready_Q, Result_Q, num_of_process);
W_and_T Multilevel_Queue(Waiting_Q, multi_Ready_Q1,multi_Ready_Q2,Result_Q,num_of_process);
W_and_T HRRN(Waiting_Q, Ready_Q, Result_Q, num_of_process); // Highest Response Ratio Next, priority�� (waiting time + cpu burst) / cpu burst �� �ϴ� �����층 ���.


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
} // �ʱ�ȭ�ϴ� �Լ�, ������� �ʾ����� Ȥ�ó� �ؼ� ����X

Queue FCFS_Sort_process(Queue que) {	// Queue�� arrival time ������ ���� �����ϴ� �Լ�
	int i, j;
	process tmp_process;
	for (j = 0; j < que.p_num - 1; j++) {
		for (i = 0; i < que.p_num - 1; i++) {
			if (que.Q[i].arrival_time > que.Q[i + 1].arrival_time) {
				tmp_process = que.Q[i];
				que.Q[i] = que.Q[i + 1];
				que.Q[i + 1] = tmp_process;
			} // arrival �ð� ���� ������ ���� sort
			else if (que.Q[i].arrival_time == que.Q[i + 1].arrival_time) {
				if (que.Q[i].priority > que.Q[i + 1].priority) {
					tmp_process = que.Q[i];
					que.Q[i] = que.Q[i + 1];
					que.Q[i + 1] = tmp_process;
				}
			} // ���� arrival �ð��� ���ٸ� priority�� ���� �� ���� �����ϵ��� ������
		}
	}
	return que;
}

Queue SJF_Sort_process(Queue que,int first, int rear) {	// Queue�� cpu_burst_time�� ���� ������ ���� �����ϴ� �Լ�
	int i,j;
	process tmp_process;

	for (j = first; j < rear; j++) {
			for (i = first; i < rear-1; i++) { // rear-1 ���� �ϴ� ����? rear+1���� ���ϱ� ����
				if (que.Q[i].remain_time > que.Q[i + 1].remain_time) {
					tmp_process = que.Q[i];
					que.Q[i] = que.Q[i + 1];
					que.Q[i + 1] = tmp_process;
				} // cpu_burst �ð� ª�� ������ ���� sort
				else if (que.Q[i].remain_time == que.Q[i + 1].remain_time) {
					if (que.Q[i].priority > que.Q[i + 1].priority) {
						tmp_process = que.Q[i];
						que.Q[i] = que.Q[i + 1];
						que.Q[i + 1] = tmp_process;
					}
				} // ���� cpu_burst �ð��� ���ٸ� priority�� ���� �� ���� �����ϵ��� ������
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
			} // priority ������ ���� sort
			else if (que.Q[i].priority == que.Q[i + 1].priority) {
				if (que.Q[i].remain_time > que.Q[i + 1].remain_time) {
					tmp_process = que.Q[i];
					que.Q[i] = que.Q[i + 1];
					que.Q[i + 1] = tmp_process;
				}
			} // ���� priority ���ٸ� cpu_burst ª�� �� ���� �����ϵ��� ������
		}
	}
	return que;
}

Queue HRRN_Sort_process(Queue que, int first, int rear) {
	int i, j;
	process tmp_process;

	for (j = first; j < rear - 1; j++) {
		for (i = first; i < rear - 1; i++) {
			if (que.Q[i].hrrn_priority < que.Q[i + 1].hrrn_priority) {	// hrrn_priority�� ���� ���� ���⼱ ū�� �����Ŷ� ū ���� ������
				tmp_process = que.Q[i];
				que.Q[i] = que.Q[i + 1];
				que.Q[i + 1] = tmp_process;
			} // hrrn priority ������ ���� sort
		}
	}
	return que;
}

void main(void) {

	int sch_type; // CPU scheduler ���� ����
	W_and_T fcfs, np_sjf, pre_sjf, np_pri, pre_pri,rr,multi,hrrn; // �� scheduler�� return�� ������ ����ü ����
	
 
	int j; // for���� ����� ����
	int time; 

	Queue Waiting_Q, Ready_Q, Result_Q;	//waiting queue�� ready queue ����
	Queue multi_Ready_Q1, multi_Ready_Q2; // multi-level queue ���� ����� queue 2�� 
	// Q1�� RR, Q2�� FCFS
	
	Waiting_Q.p_num = 0;
	Ready_Q.p_num = 0;
	Result_Q.p_num = 0;
	multi_Ready_Q1.p_num = 0;
	multi_Ready_Q2.p_num = 0;
	// Q�� p_num �켱 ��� 0���� �ʱ�ȭ

	printf("Input the number of process.\n"); // ������ ���μ��� ���� �Է¹���
	scanf("%d", &Waiting_Q.p_num); // �Է¹��� ���μ��� �� Waiting_Q�� process ���� �Է�

	printf("Process ID   CPU Burst time    Arrival time   Priority\n");
	for (j = 0; j < Waiting_Q.p_num; j++) {
		Waiting_Q.Q[j].p_id = j + 1; // process id�� Q �տ������� 1,2,3,...���� �ο�
		Waiting_Q.Q[j].cpu_burst_time = rand() %20 + 1; // cpu_burst_time�� 1~20���� random�ο�
		Waiting_Q.Q[j].arrival_time = rand()%10; //arrival time�� 0~9 random�ο�
		Waiting_Q.Q[j].intermediate = Waiting_Q.Q[j].cpu_burst_time; // round robin�� multilevel queue���� ����� �߰�����
		Waiting_Q.Q[j].remain_time = Waiting_Q.Q[j].cpu_burst_time; // ���� �� remain time�� cpu burst time�� ����
		Waiting_Q.Q[j].priority = rand() % Waiting_Q.p_num + 1; //�켱������ 1~���μ��� ������ random �ο�
		Waiting_Q.Q[j].age = 0; // age�� �켱 ��� 0���� �ʱ�ȭ. Ready_Q���� �������� �ƴ� �� ������ ����
		Waiting_Q.Q[j].io_create = rand() % 2; // io�� �߻��� ����? �߻��ϸ� 1 ���ϸ� 0
		if (Waiting_Q.Q[j].io_create == 1) { Waiting_Q.Q[j].io_burst_time = rand() % 5 + 1; }// ���� io�� �߻��ϸ� 1~5������ burst time
		Waiting_Q.Q[j].hrrn_priority = (Waiting_Q.Q[j].age + Waiting_Q.Q[j].remain_time) / Waiting_Q.Q[j].remain_time; // hrrn �� priority ����
		printf(" %d\t\t %d\t\t %d\t\t %d\n",Waiting_Q.Q[j].p_id,Waiting_Q.Q[j].cpu_burst_time, Waiting_Q.Q[j].arrival_time, Waiting_Q.Q[j].priority);	
	}

	while (1) {
		printf("\nSelect the Scheduler type\n");
		printf("  1. FCFS\n  2. Preemptive SJF\n  3. Nonpreemptive SJF\n  4. Preemptive priority\n  5. Nonpreemptive priority\n  6. RR\n  7. Multilevel Queue\n  8. HRRN\n  9. Compare\n  0.EXIT\n");
		scanf("%d", &sch_type);

		if (sch_type == 0) {return;} // Exit �����ϱ� ������ �������� ����
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
		} // ���õ�  scheduler�� ���� switch�� ���� ��

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
	int burst; //for������ ����� cpu_burst����
	int io_burst; // io ���࿡ ����� burst ����
	int Gantt[400]; // ��Ʈ��Ʈ�� ���� �迭

	W_and_T fcfs;
	float sum_of_waiting = 0, sum_of_turnaround = 0;
	printf("FCFS\n");
	Ready_Q = FCFS_Sort_process(Waiting_Q); // ���μ������� arrival time�� ���� �����ؼ� Ready_Q�� ����
	Initialize_Q(Result_Q);
	for (i = 0; i < num_of_process; i++) {
		while (1) {
			if (time < Ready_Q.Q[i].arrival_time) { Gantt[time] = 0; printf("Time:%d		IDLE\n", time); time++; continue; } 
			else {
				for (burst = Ready_Q.Q[i].cpu_burst_time; burst > 0; burst--) {
					Gantt[time] = Ready_Q.Q[i].p_id;
					printf("Time:%d		Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[i].p_id, Ready_Q.Q[i].arrival_time, Ready_Q.Q[i].cpu_burst_time, Ready_Q.Q[i].priority, Ready_Q.Q[i].remain_time); // ����
					time++;
					Ready_Q.Q[i].remain_time--;
				}
				Ready_Q.Q[i].end_time = time;
				Result_Q.Q[i] = Ready_Q.Q[i];
				//			Initialize_Q(Ready_Q);  // ����� Result_Q�� �̵� �� Ready_Q �ʱ�ȭ
				break;
			}
		}
	}

	printf("Result\n");
	for (j = 0; j < num_of_process; j++) {
		printf("Process ID: %d, Waiting Time: %d, Turnaround Time: %d\n", Result_Q.Q[j].p_id, WaitingTime(Result_Q.Q[j]), TurnaroundTime(Result_Q.Q[j]));
		sum_of_waiting += WaitingTime(Result_Q.Q[j]); // ��� Queue
		sum_of_turnaround += TurnaroundTime(Result_Q.Q[j]);
	}
	fcfs.avg_waiting = sum_of_waiting / (float)num_of_process;
	fcfs.avg_turnaround = sum_of_turnaround / (float)num_of_process;
	printf("FCFS \nAverage Waiting Time: %6f, Turnaround Time: %6f\n", fcfs.avg_waiting, fcfs.avg_turnaround);
	//printGantt(Result_Q, time,num_of_process);
	
	for(i = 0; i < time; i++) {
		printf("%d ", Gantt[i]);
	}//Gantt chart ���
	printf("\n");

	return fcfs;
}

W_and_T Pre_SJF(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) {
	int i, j,k;
	int time=0; // �ð�
	int first = 0;
	int rear = 0; // Q �̵��� �� �� �ڿ� �ֱ� ����
	int burst;
	int Gantt[400];
	W_and_T pre_sjf;
	float sum_of_waiting = 0, sum_of_turnaround = 0; // ��� waiting�� turnaround time�� ���ϱ� ����
	Waiting_Q = SJF_Sort_process(Waiting_Q, 0, num_of_process);
	printf("Preemptive SJF\n");
	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // �����ϸ� ready_Q �� �ѱ�
			}
		} // �ð� �����ϴ� ��� Ready_Q �������� �̾ƿ�
		
		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // ���� ������ ���� �ϳ��� ���� �� 
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++; 
			continue; 
		}
		else {
			Ready_Q = SJF_Sort_process(Ready_Q, first, rear); // ������ �Ͱ� �������� �� ���� sort
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // ����
			Gantt[time] = Ready_Q.Q[first].p_id;
			time++;
			Ready_Q.Q[first].remain_time--;

			if (Ready_Q.Q[first].remain_time == 0) { // ���� ��������
				Ready_Q.Q[first].end_time = time; // �����ð� ����
				Result_Q.Q[first] = Ready_Q.Q[first]; // ��� queue�� �̵�
				first++; // ���� process ����
				Ready_Q = SJF_Sort_process(Ready_Q, first, rear); // sort
			}			
			for (k = first+1 ; k < num_of_process; k++) {// ���� �������� �ƴ� �͵鿡 ���ؼ�
				Ready_Q.Q[k].age++;		// age�� 1�� ����
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age�� 6 ������ ������ priority�� 1�� ���̰�, ������ ���� �ʵ��� ���� �߰�

		}

		if (first == num_of_process) { break; } // ������ �� ���������� while�� break
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
	int rear = 0; // Q �̵��� �� �� �ڿ� �ֱ� ����
	int time = 0; // �ð�
	int burst;
	int Gantt[400];
	W_and_T np_sjf;
	Waiting_Q = SJF_Sort_process(Waiting_Q, 0, num_of_process);
	float sum_of_waiting = 0, sum_of_turnaround = 0; // ��� waiting�� turnaround time�� ���ϱ� ����
	printf("Nonpreemptive SJF\n");

	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) { // �����ð��� �Ǹ�
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q �� �ѱ�
			} //��׸� �����ؼ� sort�ؾ���
		} // �ð� �����ϴ� ��� Ready_Q �������� �̾ƿ�
		if (Ready_Q.Q[first].cpu_burst_time == Ready_Q.Q[first].remain_time) {
			Ready_Q = SJF_Sort_process(Ready_Q, first, rear);
		}// first�� �ִ� ���� ���� ���� �������� ���� ������ �͵�� �����ؼ� ���� sort �ؾ���
		else { Ready_Q = SJF_Sort_process(Ready_Q, first + 1, rear); } //first�� �ִ� process�� ���� �������̸� �������� �� ���� sort
		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // ���� ���� �ƹ��͵� ���߰ų� ������ ���� ����X
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++; continue;
		}
		else {
			Ready_Q = SJF_Sort_process(Ready_Q, first+1, rear); // �������� �� ���� sort
			Gantt[time] = Ready_Q.Q[first].p_id;
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // ����
			time++;
			Ready_Q.Q[first].remain_time--;
			
			if (Ready_Q.Q[first].remain_time == 0) {
				Ready_Q.Q[first].end_time = time;	//������ �ð� ����
				Result_Q.Q[first] = Ready_Q.Q[first];	//������ result_Q�� �̵�
				first++; // ���� process ���� ����+1	
				Ready_Q = SJF_Sort_process(Ready_Q, first , rear); // ������ �ƴ� �͵� �ѹ� ����
			}	
			for (k = first + 1; k < num_of_process; k++) {// ���� �������� �ƴ� �͵鿡 ���ؼ�
				Ready_Q.Q[k].age++;		// age�� 1�� ����
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age�� 6 ������ ������ priority�� 1�� ���̰�, ������ ���� �ʵ��� ������ �߰�
		}
		if (first == num_of_process) { break; }	// ������ ������ while�� ����
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
	int time = 0; // �ð�
	int first = 0;
	int rear = 0; // Q �̵��� �� �� �ڿ� �ֱ� ����
	int burst;
	int Gantt[400];
	W_and_T pre_pri;
	float sum_of_waiting = 0, sum_of_turnaround = 0; // ��� waiting�� turnaround time�� ���ϱ� ����
	Waiting_Q = Pri_Sort_process(Waiting_Q, 0, num_of_process); // �켱���� ���� ������ ����, �켱������ ������ shortest job first
	printf("Preemptive Priority\n");
	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q �� �ѱ�
			}
		} // �ð� �����ϴ� ��� Ready_Q �������� �̾ƿ�

		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // ������ �� ������ IDLE
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++;
			continue;
		}
		else {//������ ���� ������
			Ready_Q = Pri_Sort_process(Ready_Q, first, rear); // ������ �Ͱ� �������� �� ���� sort
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // ����
			Gantt[time] = Ready_Q.Q[first].p_id;
			time++;
			Ready_Q.Q[first].remain_time--;


			if (Ready_Q.Q[first].remain_time == 0) { // ���� ��������
				Ready_Q.Q[first].end_time = time; // ������ �ð� ���� ��
				Result_Q.Q[first] = Ready_Q.Q[first]; // result Queue�� ��� �ѱ�
				first++;
				Ready_Q = Pri_Sort_process(Ready_Q, first, rear); // �ѹ� sort
			}
			for (k = first + 1; k < num_of_process; k++) {// ���� �������� �ƴ� �͵鿡 ���ؼ�
				Ready_Q.Q[k].age++;		// age�� 1�� ����
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age�� 6 ������ ������ priority�� 1�� ���̰�, ������ ���� �ʵ��� ������ �߰�
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
	int rear = 0; // Q �̵��� �� �� �ڿ� �ֱ� ����
	int time = 0; // �ð�
	int burst;
	int Gantt[400];
	W_and_T np_pri;
	Waiting_Q = Pri_Sort_process(Waiting_Q, 0, num_of_process);
	float sum_of_waiting = 0, sum_of_turnaround = 0; // ��� waiting�� turnaround time�� ���ϱ� ����
	printf("Nonpreemptive Priority\n");

	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) { // �����ð��� �Ǹ�
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q �� �ѱ�
			} //��׸� �����ؼ� sort�ؾ���
		} // �ð� �����ϴ� ��� Ready_Q �������� �̾ƿ�
		if (Ready_Q.Q[first].cpu_burst_time == Ready_Q.Q[first].remain_time) { 
			Ready_Q = Pri_Sort_process(Ready_Q, first, rear);
		}// first�� �ִ� ���� ���� ���� ���� �������� ���� ������ �͵�� �����ؼ� ���� sort �ؾ���
		else { Ready_Q = Pri_Sort_process(Ready_Q, first + 1, rear); } // ���� �������̾����� ���� sort

		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // ���� ���� �ƹ��͵� ���߰ų� ������ ���� ����X
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++; continue;
		}
		else {
			Ready_Q = Pri_Sort_process(Ready_Q, first + 1, rear); // ������ �Ͱ� �������� �� ���� sort
			Gantt[time] = Ready_Q.Q[first].p_id;
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // ����
			time++;
			Ready_Q.Q[first].remain_time--;

			if (Ready_Q.Q[first].remain_time == 0) {
				Ready_Q.Q[first].end_time = time;	//������ �ð� ����
				Result_Q.Q[first] = Ready_Q.Q[first];	//������ result_Q�� �̵�
				first++; // ���� process ���� ����		
				Ready_Q = Pri_Sort_process(Ready_Q, first, rear); // ������ �ƴ� �͵� �ѹ� ����
			}
			for (k = first + 1; k < num_of_process; k++) {// ���� �������� �ƴ� �͵鿡 ���ؼ�
				Ready_Q.Q[k].age++;		// age�� 1�� ����
				if (((Ready_Q.Q[k].age) % 6 == 0) && (Ready_Q.Q[k].age != 0) && Ready_Q.Q[k].priority>0) { Ready_Q.Q[k].priority--; }
			}// age�� 6 ������ ������ priority�� 1�� ���̰�, ������ ���� �ʵ��� ������ �߰�
		}
		if (first == num_of_process) { break; }	// ������ ������ while�� ����
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
	int condition; // ��� �۾����� ������ �� Ȯ���� �� ����� �Լ�
	int i, j, k;
	int burst;
	float sum_of_waiting=0, sum_of_turnaround=0;
	int Gantt[400];
	W_and_T rr;

	printf("Round Robin\n");
	printf("Input time quantum\n");
	scanf("%d",&time_quantum);

	Waiting_Q = FCFS_Sort_process(Waiting_Q); // �켱 arrival time ������ ����, ���� arrival time ���ٸ� �켱������ ����

	while (1) {
				condition = 0; // üũ�Ҷ����� 0���� �ʱ�ȭ���־����
				for (i = 0; i < num_of_process; i++) {
					if (Waiting_Q.Q[i].arrival_time == time) {
						Ready_Q.Q[rear++] = Waiting_Q.Q[i];// �����Ҷ����� Ready_Q �� �ڷ� ���� ���� ������ sort�� ���ִ� ������ �켱������ �־��ַ���..
					}
				}
				if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // ���� �����Ѱ� ������
					Gantt[time] = 0; 
					printf("Time:%d		IDLE\n", time); 
					time++; 
					continue;
				}
				else { // ������ ���� ������
					printf("Time:%d			Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].priority, Ready_Q.Q[first].remain_time); // ����
					if (Ready_Q.Q[first].remain_time == Ready_Q.Q[first].intermediate) { // �����Ҷ�
						Ready_Q.Q[first].start_time = time;// �ð��� start time���� ����
					}
					Gantt[time] = Ready_Q.Q[first].p_id;
					time++;
					Ready_Q.Q[first].remain_time--; 

					if (Ready_Q.Q[first].remain_time == 0) {
						Ready_Q.Q[first].end_time = time; // ������ ��� �ð� ����
						Result_Q.Q[rear_for_result++] = Ready_Q.Q[first++]; // ����� �̵� �� first++
					}
					else if ((time - Ready_Q.Q[first].start_time) == time_quantum) {//time quantume��ŭ �������� ���
						Ready_Q.Q[first].intermediate = Ready_Q.Q[first].remain_time; //�߰��ð�. time quantum��ŭ �����ϱ� ���� �߰��� ����.
						Ready_Q.Q[rear] = Ready_Q.Q[first++]; // �ǵڷ� �̵� �� first++
						rear++;
					}
				}

				for (k = 0; k < rear_for_result; k++) {
					if (Result_Q.Q[k].remain_time == 0) {
						condition++; // ���� process�� ���� ��� üũ�ؼ� 
					}
				}
				if (condition == num_of_process) { break; }// ��� process���� ������ while�� break
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
	int first1=0, first2=0, rear1=0,rear2=0; // �� Q1,2�� ���� ��
	int rear_for_result=0; // ��� queue�� ���� ��, while ������ ���ǹ������� �����
	int rear_tmp = 0; // tmp�� �ð���� ������ ���� �Լ�
	float sum_of_waiting = 0, sum_of_turnaround = 0;
	int condition;
	int time_quantum; // Q1 quantum
	int Gantt_Q1[400], Gantt_Q2[400];
	Queue tmp_Q; // �� Q�� process ������ ���� �ӽ� Queue 
	W_and_T multi;

	printf("Input time quantum for Q1\n");
	scanf("%d", &time_quantum);
	Waiting_Q = FCFS_Sort_process(Waiting_Q); // �켱������ �־��ַ��� ������
	while (1) {
		
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				tmp_Q.Q[rear_tmp] = Waiting_Q.Q[i]; // Queue 2���� ���ǿ� ���� ������ ���� �켱 �ӽ� Queue�� �Ҵ�
				if (rear_tmp % 2 == 0) { multi_Ready_Q1.Q[rear1++] = tmp_Q.Q[rear_tmp++]; } // Ȧ����°�� ������ ���� Q1�� �Ҵ�
				else { multi_Ready_Q2.Q[rear2++] = tmp_Q.Q[rear_tmp++]; } //¦����° ������ Q2�� �Ҵ�
			}
		} // Q1 �� Q2�� ����������� ����

		if ((rear1 == 0) || (time < multi_Ready_Q1.Q[first1].arrival_time) || (first1==rear1)) { // Q1�� ����X �Ǵ� ������ ���� ������
			if ((rear2 == 0) || (time < multi_Ready_Q2.Q[first2].arrival_time) ) { // Q2���� ���� ������ ���� ���� �� 
				Gantt_Q1[time] = 0;
				Gantt_Q2[time] = 0;
				printf("Time:%d		IDLE\n", time); time++; continue; // �Ѵ� ������ ���� ������
			}
			else { // ���� Q1�� ������ ���� ���� Q2�� ������ ���� ���� ��
				//���� ������� �̹� ���ĵǾ� �ֱ� ������ ���� ������ �ʿ� ���� �ٷ� �������ָ� �ȴ�.
				printf("Time:%d		Queue2		Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, multi_Ready_Q2.Q[first2].p_id, multi_Ready_Q2.Q[first2].arrival_time, multi_Ready_Q2.Q[first2].cpu_burst_time, multi_Ready_Q2.Q[first2].priority, multi_Ready_Q2.Q[first2].remain_time); // ����
				Gantt_Q1[time] = 0;
				Gantt_Q2[time] = multi_Ready_Q2.Q[first2].p_id;
				time++;
				multi_Ready_Q2.Q[first2].remain_time--;
				if (multi_Ready_Q2.Q[first2].remain_time == 0) {
					multi_Ready_Q2.Q[first2].end_time = time; // ������ ��� �ð� ����
					Result_Q.Q[rear_for_result++] = multi_Ready_Q2.Q[first2];
					first2++;
				}
			}
		}
		else {//Q1�� ������ ���� ���� ��
			printf("Time:%d		Queue1		Process ID: %d, Arrival Time: %d, CPU Burst Time: %d, Priority: %d, Remain Time: %d\n", time, multi_Ready_Q1.Q[first1].p_id, multi_Ready_Q1.Q[first1].arrival_time, multi_Ready_Q1.Q[first1].cpu_burst_time, multi_Ready_Q1.Q[first1].priority, multi_Ready_Q1.Q[first1].remain_time); // ����
			if (multi_Ready_Q1.Q[first1].remain_time == multi_Ready_Q1.Q[first1].intermediate) {//�����Ҷ�
				multi_Ready_Q1.Q[first1].start_time = time;// �ð��� start time���� ����
			}
			Gantt_Q1[time] = multi_Ready_Q1.Q[first1].p_id;
			Gantt_Q2[time] = 0;
			time++;
			multi_Ready_Q1.Q[first1].remain_time--;
			if (multi_Ready_Q1.Q[first1].remain_time == 0) { // �� �������� ���
				multi_Ready_Q1.Q[first1].end_time = time;
				Result_Q.Q[rear_for_result++] = multi_Ready_Q1.Q[first1++];
			}
			else if ((time - multi_Ready_Q1.Q[first1].start_time) == time_quantum) {//time quantume��ŭ �������� ���
				multi_Ready_Q1.Q[first1].intermediate = multi_Ready_Q1.Q[first1].remain_time;
				multi_Ready_Q1.Q[first1].age = 0; // Q2�� �������� �켱 age �ʱ�ȭ
				multi_Ready_Q2.Q[rear2++] = multi_Ready_Q1.Q[first1++]; // Q2�� �̵�
				
			}
		}

		for (j = first2+1; j < rear2; j++) { //�������ΰ��� ���� �����ϱ� age 1�� ����
			multi_Ready_Q2.Q[j].age++; // Q2���� ������ �ƴ� �͵��� age��� �߰�
			if (multi_Ready_Q2.Q[j].age == 15) {
				multi_Ready_Q1.Q[rear1++] = multi_Ready_Q2.Q[j]; // age�� 15�� �Ѵ� �͵��� ������ starvation ������ ���� Q1�� �Ѱ���
			}
		}

		condition = 0;

		for (k = 0; k < rear_for_result;k++) {	// result queue ��� ������ condition == process�� �̹Ƿ� while�� ������ ��������
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

	printf("\n\n Q1 Gantt Chart \n"); // Queue1 ��Ʈ
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt_Q1[i]);
	}
	printf("\n");

	printf("\n\n Q2 Gantt Chart \n");	//Queue2 ��Ʈ
	for (i = 0; i < time; i++) {
		printf("%d ", Gantt_Q2[i]);
	}
	printf("\n");

	return multi;
}

W_and_T HRRN(Queue Waiting_Q, Queue Ready_Q, Queue Result_Q, int num_of_process) { // preemptive�̰�, sjf�� ������ ���̴�.
	int i, j, k;																	// hrrn�� priority = (age + burst)/burst.
	int time = 0; // �ð�
	int first = 0;
	int rear = 0; // Q �̵��� �� �� �ڿ� �ֱ� ����
	int burst;
	int Gantt[400];
	W_and_T hrrn;
	float sum_of_waiting = 0, sum_of_turnaround = 0; // ��� waiting�� turnaround time�� ���ϱ� ����

	Waiting_Q = SJF_Sort_process(Waiting_Q,0,num_of_process); // ó���� ��� hrrn_pri�� 1�̱� ������ hrrn sort�� �ص� �ǹ̾���. ���� sjf sort

	while (1) {
		for (i = 0; i < num_of_process; i++) {
			if (Waiting_Q.Q[i].arrival_time == time) {
				Ready_Q.Q[rear++] = Waiting_Q.Q[i]; // ready_Q �� �ѱ�
			}
		} // �ð� �����ϴ� ��� Ready_Q �������� �̾ƿ�
		Ready_Q = HRRN_Sort_process(Ready_Q, first, rear);
		if (rear == 0 || (time < Ready_Q.Q[first].arrival_time)) { // ���� ������ ���� �ϳ��� ���� �� 
			Gantt[time] = 0;
			printf("Time:%d		IDLE\n", time);
			time++;
			continue;
		}
		else {
			Ready_Q = HRRN_Sort_process(Ready_Q, first, rear); // ������ �Ͱ� �������� �� ���� sort
			printf("Time: %d     Process ID: %d, Arrival Time: %d, CPU Burst Time: %d,  HRRN priority: %6f, Remain Time: %d\n", time, Ready_Q.Q[first].p_id, Ready_Q.Q[first].arrival_time, Ready_Q.Q[first].cpu_burst_time, Ready_Q.Q[first].hrrn_priority, Ready_Q.Q[first].remain_time); // ����
			Gantt[time] = Ready_Q.Q[first].p_id;
			time++;
			Ready_Q.Q[first].remain_time--;

			if (Ready_Q.Q[first].remain_time == 0) {
				Ready_Q.Q[first].end_time = time;
				Result_Q.Q[first] = Ready_Q.Q[first];
				first++;
			//	Ready_Q = HRRN_Sort_process(Ready_Q, first, rear);
			}
			for (i = first + 1; i < rear; i++) { // ������ �ƴ� �͵��� age �ϳ��� �ø��� ����
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
