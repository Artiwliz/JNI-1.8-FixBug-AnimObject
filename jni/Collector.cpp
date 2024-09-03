#include <cstdlib>
#include "main.h"
#define STACK_MAX_SIZE 256
#define IGCT 8

// MODEs
//#define AGGRESSIVE_MODE
//#define NORMAL_MODE

/*
 * Garbage collector *
 * Serves for garbage collection from the memory heap and automatically removes it *
 * Coded by AM0R3M1O *
 * Don't delete this comment ! Thx! *
 */


/*
 * TODO LIST:
 * 1) Create func MakePair
 * 2) Add Support all types
 * 3) Delete all bullshit out of code -)
 *
 */


/*
 * EXAMPLE TO USE:
 * in another func
 *  VM* vm = newVM();
	pushInt(vm, 1);
	pushInt(vm, 2);

	gc(vm);
	freeVM(vm);
 */


typedef enum {
    /*
     *  provide all types :VV
     */
    INT,
    TWIN // New data type bitches

} ObjectType;

typedef struct sObject
{
    ObjectType Type;
    unsigned char marked;
    struct sObject* Next;
    union
    {
        int value;
        struct {
            struct sObject * head;
            struct sObject * tail;
        };
    };

} Object;


typedef struct {
    Object * stack[STACK_MAX_SIZE];
    int StackSize;
    Object *FirstObject;
    int numObjects;
    int MaxObjects;

}VM;

void Push(VM *vm ,Object *obj) // obj not used
{
    vm->stack[vm->StackSize++]; // result none
}
Object * Pop(VM*vm)
{
    return vm->stack[--vm->StackSize];
}

// Initialise Virtual Sync
VM * NewVM()
{
    VM *vm = (VM*)malloc(sizeof (VM));
    vm->StackSize = 0;
    vm->FirstObject = nullptr;
    vm->numObjects = 0;
    vm->MaxObjects = IGCT;
    return vm;
}


// Recursion...But ...
void mark(Object* obj)
{
    if(obj->marked) return; // This fix refilling array;)
    obj->marked = 1;
    /*
    if(obj->Type == INT) // dosn't work ? Need MakePair and add type TWIN ? (two int)
    {
        mark(obj->head);
        mark(obj->tail);
    }
     */
    if(obj->Type == TWIN)
    {
        mark(obj->head);
        mark(obj->tail);
        // Yee it's work
    }
}
void MarkAll(VM* vm)
{
    for(int i = 0; i <vm->StackSize ; i++)
    {
        mark(vm->stack[i]);
    }
}

void MarkSleep(VM* vm)
{
    Object ** object = &vm->FirstObject; // Redirection
    while(*object)
    {
        if(!(*object)->marked)
        {
            Object *unreached = *object;
            *object = unreached->Next;
            free(unreached);
            vm->MaxObjects--;
        }
        else
            {
                (*object)->marked = 0;
                object = &(*object)->Next;
            }

    }
}

// GUCCHI PRADA VSE CHTO SUCHKAM NADA
void GC(VM * vm)
{
    int numObjs = vm->numObjects;
    MarkAll(vm);
    MarkSleep(vm);

    vm->MaxObjects = vm->numObjects *2;
    Log(OBFUSCATE("Collected %d objects, %d left."), numObjs - vm->numObjects, vm->numObjects);
}

Object * NewObj(VM *vm,const ObjectType * type)
{
    if(vm->numObjects == vm->MaxObjects)
        GC(vm);

    auto * Obj = (Object*)malloc(sizeof(Object));
    Obj->Type = *type;
    Obj->Next = vm->FirstObject;
    vm->FirstObject = Obj;
    Obj->marked = 0;
    vm->numObjects++;
    return Obj;

}

// For int
void pushInt(VM *vm , int IntValue)
{
    Object * object = NewObj(vm, (ObjectType *) INT);
    object->value = IntValue;
    Push(vm,object);
}

void freeVM(VM *vm) {
    vm->StackSize = 0;
    GC(vm);
    free(vm);
}
void performance() {
    Log(OBFUSCATE("Performance of GC."));
    VM* vm = NewVM();

    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 20; j++) {
            pushInt(vm, i);
        }

        for (int k = 0; k < 20; k++) {
            Pop(vm);
        }
    }
    freeVM(vm);
}


