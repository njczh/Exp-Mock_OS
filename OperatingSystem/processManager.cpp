#include "processManager.h"

ProcessManager* processManager = ProcessManager::GetInstance();

ProcessManager * ProcessManager::GetInstance()
{
	static ProcessManager instance;
	return &instance;
}

void ProcessManager::CreateInitProcess() {
	if (processTable.size() > 0)
		std::cout << "init�����Ѵ��ڣ��޷�������" << std::endl;
	else
		CreateProcess("init", INIT);
}

// ���̴���
int ProcessManager::CreateProcess(const string pName, priorities priority)
{
	if (runningPcb == nullptr && priority != INIT) return 1;
	// ���ڴ�������һ���հ�PCB������ʼ��PCB
	PCB* pcb = new PCB(pName, priority, runningPcb);
	if (pcb->getPid() == MAX_PROCESS_NUM) return 2;
	cout << "[  CREATE  ] ";
	pcb->showBriefInfo();

	// ���´�����process image ���������̱�
	processTable.push_back(pcb);

	// ������Ӧ���ȼ��ľ�������β��
	readyList.PushBack(pcb);

	return 0;
}

// �ڽ��̱����ҵ�PID�Ľ���
pTable_iter ProcessManager::RetrieveProcess(unsigned int pid)
{
	pTable_iter iter;

	for (iter = processTable.begin(); iter != processTable.end(); iter++)
		if ((*iter)->getPid() == pid)
			return iter;		

	return iter;
}

// ����ϵͳ��ֹ���̵Ĺ������£�����ԭ���
//		���ݱ���ֹ���̵ı�ʶ��������PCB�����ж����ý��̵�״̬��
//		������ֹ���̴���ִ��״̬��������ֹ�ý��̵�ִ�У����������Դ������������̡�
//		���ý��̻����ӽ��̣���Ӧ���������ӽ�����ֹ��
//		���ý�����ӵ�е�ȫ����Դ����黹���丸���̻�黹������ϵͳ��
//		����PCB�����ڶ��У�������ɾ����
int ProcessManager::DeleteProcess(unsigned int pid)
{
	pTable_iter iter = RetrieveProcess(pid);
	if (iter == processTable.end()) return 2;

	PCB * pcb = (*iter);

	vector<PCB*> child = pcb->getChild();

	for (vector<PCB*>::iterator iter = child.begin(); iter != child.end(); iter++) 
		DeleteProcess((*iter)->getPid());

	if (pcb == runningPcb) {
		runningPcb = nullptr;
		Schedule();
	}
	else 
	{
		// �Ӿ���/����������ɾ������
		switch (pcb->getPList())
		{
		case READYLIST:
			readyList.Delete(pcb);
			break;
		default:
			break;
		};
	}

	pcb->RelResourceAll();

	// �ӽ��̱��г�������
	processTable.erase(iter);
	delete(pcb);

	cout << "[  DELETE  ] Process, PID: " << pid << endl;

	return 0;
}

void ProcessManager::ShowAllProcess()
{
	cout << left << setw(6) << "NAME"
		<< right << setw(5) << "PID"
		<< setw(5) << "PPID"
		<< setw(10) << "PRIORITY"
		<< setw(10) << "STAT"
		<< endl;
	for (pTable_iter iter = processTable.begin(); iter != processTable.end(); iter++) 
	{
		cout << left << setw(6) << (*iter)->getPName()
			<< right << setw(5) << (*iter)->getPid();

		if ((*iter)->getPPid() == MAX_PROCESS_NUM)	cout << setw(5) << "/";
		else cout << setw(5) << (*iter)->getPPid();

		cout << setw(10) << (*iter)->getPriorityS()
			<< setw(10) << (*iter)->getPTypeS()
			<< endl;
	}
	cout << endl;
}

void ProcessManager::ShowOneProcess(unsigned int pid)
{
	(*RetrieveProcess(pid))->ShowAllInfo();
}

// ���̵���
void ProcessManager::Schedule()
{
	readyList.PushBack(runningPcb);
	runningPcb = readyList.PopFront();

	if (isCpuFree())
		cout << "[ SCHEDULE ] Now NO process is running! Use 'to' to schedule process" << endl;
	else
		cout << "[ SCHEDULE ] Now Process: \" " << runningPcb->getPName()
		<< " (PID: " << runningPcb->getPid() << ") \" is running!" << endl;

}

void ProcessManager::BlockProcess(RCB* waitRcb, int waitNum)
{
	runningPcb->WaitResource(waitRcb, waitNum);
	runningPcb = nullptr;
}

void ProcessManager::WakeUpProcess(PCB* pcb)
{

	if (pcb == nullptr) return;
	readyList.PushBack(pcb);
	pcb->WakeUp();
}

int ProcessManager::getProcessNum()
{
	return processTable.size();
}

bool ProcessManager::isCpuFree()
{
	if (runningPcb == nullptr)
		return true;
	else
		return false;
}

PCB * ProcessManager::GetRunningPcb()
{
	return runningPcb;
}