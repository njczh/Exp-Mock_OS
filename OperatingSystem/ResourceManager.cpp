#include "ResourceManager.h"

ResourceManager * resourceManager = ResourceManager::GetInstance();

int ResourceManager::CreateNewResource(int totalNum)
{
	RCB *rcb = new RCB(totalNum);
	rcbTable.push_back(rcb);
	std::cout << "����Դ" << rcb->GetRid() << "����\n";
	return rcb->GetRid();
}

rcb_iter ResourceManager::RetrieveResource(int rid)
{
	rcb_iter iter;

	for (iter = rcbTable.begin(); iter != rcbTable.end(); iter++) 
		if ((*iter)->GetRid() == rid)
			return iter;
	
	return iter;
}

RCB * ResourceManager::GetResourceRcb(int rid)
{
	rcb_iter iter = RetrieveResource(rid);
	if (iter == rcbTable.end())
		return nullptr;
	else
		return *iter;
}

// ���ܣ�����������Դ��������Դ����������
// ����ֵ��false��ʾ������Ҫ����������
int ResourceManager::Request(PCB * pcb, int rid, int reqNum)
{
	if (pcb == nullptr)
	{
		std::cout << "��ǰ�����������У�������Դ�Ƿ���" << std::endl;
		return 3;
	}
	// ��ȡrid��Ӧ��rcb
	RCB *rcb = GetResourceRcb(rid);
	if (rcb == nullptr) {
		std::cout << "��RIDΪ" << rid << "����Դ��\n";
		return 1;	// ʧ�ܣ��޴���Դ
	}

	// ������Դ����
	switch (rcb->RequestRes(reqNum))
	{
	case OK:	// �ɹ���������Դ
		pcb->AcqResource(rcb, reqNum);
		return -1;
	case BLOCK:	// ������Դ���㣬�������
		rcb->pushBackWaitList(pcb);
		return 0;
	case ERROR:	// ʧ�ܣ��������
		std::cout << "��Դ������������Դ" << rcb->GetRid() << "����������޷����룡" << std::endl;
		return 2;
	}

}

PCB * ResourceManager::Release(PCB * pcb, int rid, int relNum)
{
	if (pcb == nullptr)
	{
		std::cout << "��ǰ�����������У�������Դ�Ƿ���" << std::endl;
		return nullptr;
	}
	// ��ȡrid��Ӧ��rcb
	RCB *rcb = GetResourceRcb(rid);
	if (rcb == nullptr) {
		std::cout << "��RIDΪ" << rid << "����Դ��\n";
		return nullptr;	// ʧ�ܣ��޴���Դ
	}

	switch (pcb->RelResource(rcb, relNum))
	{
	case 1:
		std::cout << "����" << pcb->getPid()
			<< "�ͷ���Դ" << rcb->GetRid() <<"������ӵ������\n";
		return nullptr;
	case 2:
		std::cout << "����" << pcb->getPid()
			<< "����Դ" << rcb->GetRid() << "!\n";
		return nullptr;
	default:
		break;
	}

	switch (rcb->ReleaseRes(relNum))
	{
	case true:
		return rcb->IsProcessReady();
	case false:
		std::cout << "��Դ�黹ʧ�ܣ�\n";
		return nullptr;
	default:
		break;
	}

	return false;
}

void ResourceManager::ShowAllResources()
{
	std::cout << "************ Resource List ************\n";
	for (rcb_iter iter = rcbTable.begin(); iter != rcbTable.end(); iter++) 
		(*iter)->ShowBriefInfo();
	std::cout << "***************************************\n";


}

void ResourceManager::ShowOneResource(int rid)
{
	GetResourceRcb(rid)->ShowAllInfo();
}

ResourceManager * ResourceManager::GetInstance()
{
	static ResourceManager instance;
	return &instance;
}

ResourceManager::ResourceManager()
{
	for (int i = 0; i < 4; i++) {
		RCB *rcb = new RCB(3);
		rcbTable.push_back(rcb);
	}

}


ResourceManager::~ResourceManager()
{
}
