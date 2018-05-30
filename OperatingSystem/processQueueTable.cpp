#include "processQueueTable.h";

void PcbQueueTable::PushBack(PCB *pcb)
{
	if (pcb == nullptr) return;
	// ������Ӧ�����ȼ�����
	pcbList[pcb->getPriority()].push_back(pcb);
	pcb->Ready();
}

PCB* PcbQueueTable::PopFront()
{
	for (int i = 2; i >= 0; i--) 
	{
		if (pcbList[i].size() != 0) 
		{
			// ������Ϊ��������ȼ����еĵ�һ��pcb
			PCB* pcb = pcbList[i].front();
			pcbList[i].pop_front();
			pcb->Run();
			return pcb;
		}
	}
	return nullptr;
}

void PcbQueueTable::Delete(PCB *pcb) {
	int p = pcb->getPriority();
	for (PCBLIST::iterator iter = pcbList[p].begin(); iter != pcbList[p].end(); iter++) {
		if ((*iter) == pcb) {
			pcbList[p].erase(iter);
			return;
		}
	}
}