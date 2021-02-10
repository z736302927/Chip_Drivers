#include "algorithm/slist.h"
#include "linux/gfp.h"
#include "kinetis/memory.h"

/* The following program is modified by the user according to the hardware device, otherwise the driver cannot run. */

/**
  * @step 1:  Modify the corresponding function according to the modified area and the corresponding function name.
  * @step 2:  Modify four areas: GPIO_PORT/GPIO_PIN/Led_TypeDef/LEDn.
  * @step 3:  .
  * @step 4:  .
  * @step 5:
  */

#include "kinetis/idebug.h"

typedef struct SListNode {
    void *Data;
    struct SListNode *Next; /* ָ����һ����� */
} SListNode;

void SListInit(SListNode **ppFirst)
{
    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    *ppFirst = NULL;
}

/* ��ӡ���� */
void SListPrint(SListNode *First)
{
    SListNode *Current;

    for (Current = First; Current != NULL; Current = Current->Next)
        kinetis_print_trace(KERN_DEBUG, "@%p", Current->Data);
}

/* �����½ڵ� */
static SListNode *CreateNode(void *Data)
{
    SListNode *Node = (SListNode *)kmalloc(sizeof(SListNode), GFP_KERNEL);
    Node->Data = Data;
    Node->Next = NULL;

    return Node;
}
/*  β������ */
void SListPushBack(SListNode **ppFirst, void *Data)
{
    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    SListNode *Node = CreateNode(Data);

    if (*ppFirst == NULL) { /* �ж�����Ϊ�� */
        *ppFirst = Node;
        return;
    }

    /* �������е����һ���ڵ� */
    SListNode *Current = *ppFirst;

    while (Current->Next != NULL)
        Current = Current->Next;

    Current->Next = Node;/* ����������Ľڵ� */
}

void SListPushFront(SListNode **ppFirst, void *Data)
{
    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    SListNode *Node = CreateNode(Data);
    Node->Next = *ppFirst;
    *ppFirst = Node;
}

/* β��ɾ�� */
void SListPopBack(SListNode **ppFirst)
{
    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    if (*ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "*ppFirst can not be null!");

    if ((*ppFirst)->Next == NULL) {
        kfree(*ppFirst);
        *ppFirst = NULL;
        return;
    }

    SListNode *Current = *ppFirst;

    while (Current->Next->Next != NULL)
        Current = Current->Next;

    kfree(Current->Next);
    Current->Next = NULL;
}

/* ͷ��ɾ�� */
void SListPopFront(SListNode **ppFirst)
{
    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    if (*ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "*ppFirst can not be null!");/* �����ǿ����� */

    SListNode *first = *ppFirst;
    *ppFirst = (*ppFirst)->Next;
    kfree(first);
}

void SListInsert(SListNode **ppFirst, SListNode *pPos, void *Data)
{
    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    if (*ppFirst == pPos) {
        SListPushFront(ppFirst, Data);
        return;
    }

    SListNode *newNode = CreateNode(Data);
    SListNode *Current;

    /* �ҵ�posǰ��һ���ڵ� */
    for (Current = *ppFirst; Current->Next != pPos; Current = Current->Next) { }

    /* �ı�����ֶ��ڵ�ֵ��������ָ���ֵ */
    Current->Next = newNode;
    newNode->Next = pPos;
}

/* �������ɾ�� */
void SListErase(SListNode **ppFirst, SListNode *pPos)
{
    if (*ppFirst == pPos) {
        SListPopFront(ppFirst);
        return;
    }

    SListNode *Current = *ppFirst;

    while (Current->Next != pPos)
        Current = Current->Next;

    Current->Next = pPos->Next;
    kfree(pPos);
}

/* ��ֵɾ����ֻɾ�����ĵ�һ�� */
void SListRemove(SListNode **ppFirst, void *Data)
{
    SListNode *Current = *ppFirst;
    SListNode *Previous = NULL;

    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    if (*ppFirst == NULL)
        return;

    while (Current) {
        if (Current->Data == Data) {
            /* ɾ�� */
            /* 1.ɾ�����ǵ�һ���ڵ� */
            if (*ppFirst == Current) {
                *ppFirst = Current->Next;
                kfree(Current);
                Current = NULL;
            } else { /* ɾ���м�ڵ� */
                Previous->Next = Current->Next;
                kfree(Current);
                Current = NULL;
            }

            break;
        }

        Previous = Current;
        Current = Current->Next;
    }
}

/*  ��ֵɾ����ɾ�����е� */
void SListRemoveAll(SListNode **ppFirst, void *Data)
{
    SListNode *Current = NULL;
    SListNode *Previous = NULL;

    if (ppFirst != NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    if (*ppFirst == NULL)
        return;

    Current = *ppFirst;

    while (Current) {
        if (Current->Data == Data) {
            /* ɾ�� */
            /* 1.ɾ�����ǵ�һ���ڵ� */
            if (*ppFirst == Current) {
                *ppFirst = Current->Next;
                kfree(Current);
                Current = *ppFirst;
            } else { /* ɾ���м�ڵ� */
                Previous->Next = Current->Next;
                kfree(Current);
                Current = Previous;
            }
        }

        Previous = Current;
        Current = Current->Next;
    }
}

/*  ���� ����Ҫ����ÿһ���ڵ� */
void SListDestroy(SListNode **ppFirst)
{
    SListNode *Current = NULL;
    SListNode *Object = NULL;

    if (ppFirst == NULL)
        kinetis_print_trace(KERN_DEBUG, "ppFirst can not be null!");

    Current = *ppFirst;

    while (Current) {
        Object = Current;
        Current = Current->Next;
        kfree(Object);
        Object = NULL;
    }

    *ppFirst = NULL;
}

/* ��ֵ���ң����ص�һ���ҵ��Ľ��ָ�룬���û�ҵ������� NULL */
SListNode *SListFind(SListNode *pFirst, void *Data)
{
    for (SListNode *Current = pFirst; Current != NULL; Current = Current->Next) {
        if (Current->Data == Data)
            return Current;
    }

    return NULL;
}

/* The above procedure is modified by the user according to the hardware device, otherwise the driver cannot run. */
