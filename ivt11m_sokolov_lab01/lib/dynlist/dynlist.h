#pragma once
#ifdef __cplusplus
extern "C" {
#endif
// Тип-дескриптор для стека целых чисел


typedef struct DynlistDescriptor DynlistDescriptor;
typedef struct DynlistNode DynlistNode;
typedef struct DynlistDescriptor* Dynlist;

typedef void (*dynlist_pval_func_t)(void* val);
__attribute__((warn_unused_result)) Dynlist dynlist_new(dynlist_pval_func_t clean_val_func);
void dynlist_free(Dynlist plist);
void dynlist_append(Dynlist list, void* value);
void dynlist_prepend(Dynlist list, void* value);
void dynlist_foreach(Dynlist list, dynlist_pval_func_t do_func);

struct DynlistNode* dynlist_begin(Dynlist list);
struct DynlistNode* dynlist_next(struct DynlistNode* node);
void* dynlist_value(struct DynlistNode* node);



/* Новые функции для работы со стеком */
void* dynlist_poplast(Dynlist list);

/* Типы и функции для стека целых чисел */
typedef DynlistDescriptor* istack_handle;

/* Создать новый стек */
istack_handle istack_new();

/* Поместить число в стек */
void istack_push(istack_handle handle, int value);

/* Забрать число из стека */
int istack_pop(istack_handle handle);

#ifdef __cplusplus
}
#endif