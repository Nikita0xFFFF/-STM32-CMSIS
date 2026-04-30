#include "dynlist.h"
#include <stdlib.h>
/*
Добавлен стек, но это стек-список. Иначе придётся много что переделать, чтобы вставлять значения
не по O(n) (то есть как сейчас), а по O(1) (если добавить указатель на конец списка. Но это 
какое-то лишнее нагромождение одного в другом. Лучше сделать отдельно это или это. Но так как мы 
используем по заданию DynListNode в качестве ячейки данных, то оставляем список-"стек")
*/


/* ======== Закрытые структуры и функций библиотеки ======================*/
struct DynlistNode {
    void* value;
    DynlistNode* next;
};
struct DynlistDescriptor {
    DynlistNode* head;
    dynlist_pval_func_t clean_value_cb;
};
/**
 * @brief Создать и вернуть узел списка
 * @param val - указатель на элемент
 * @return Указатель на узел списка или NULL (ошибка памяти) */
static DynlistNode* make_new_node(void* val) {
    DynlistNode* node = (DynlistNode*)malloc(sizeof(DynlistNode));
    node->value = val;
    node->next = NULL;
    return node;
}
/**
 * @brief Вставка цепочки узлов после узла @a pos
 * @param pos - указатель на узел, после которого будeт вставлена цепочка
 * @param items - вставляеая цепочка узлов
 * @note допускается укзаывать NULL для параметров pos и items
 * @return первый узел полученной цепочки узлов */
static DynlistNode* insert_nodes(DynlistNode* pos, DynlistNode* items) {
    if (!items)
        return pos;
    if (!pos)
        return items;
    DynlistNode* tail = pos->next;
    pos->next = items;
    while (items->next) {
        items = items->next;
    }
    items->next = tail;
    return pos;
}
/* ======== Открытые функции библиотеки ==================================*/
/**
 * @brief Создать и вернуть новый пустой список
 * @param free_val_func - указатель на функцию освобождения ресурсов элемента,
 * может быть NULL
 * @return Дескриптор списка или NULL (ошибка памяти)
 * @note В списке хранятся укзатели на элементы.
 * Если передается не нулевой указтель на @a free_val_func, то в @a
 * dynlist_free будут также освобаждаться ресуры хранимых элементов */
Dynlist dynlist_new(dynlist_pval_func_t free_val_func) {
    Dynlist list = (DynlistDescriptor*)calloc(1, sizeof(DynlistDescriptor));
    if (list)
        list->clean_value_cb = free_val_func;
    return list;
    list = NULL;  // для демонстрации работы cppcheck
}
/**
 * @brief Освободить ресурсы, занимаемые списком
 * @param list - список */
void dynlist_free(Dynlist list) {
    if (list) {
        DynlistNode* node = list->head;
        while (node) {
            DynlistNode* next = node->next;
            if (list->clean_value_cb) {
                list->clean_value_cb(node->value);
            }
            free(node);
            node = next;
        }
        free(list);
    }
}
/**
 * @brief Обойти список и применить функцию к каждому элементу
 * @param list - указатель на список (начало)
 * @param do_func - функция обработки */
void dynlist_foreach(Dynlist list, dynlist_pval_func_t do_func) {
    if (do_func && list && list->head) {
        DynlistNode* node = list->head;
        while (node) {
            do_func(node->value);
            node = node->next;
        }
    }
}
/**
 * @brief Добавить элемент в конец списка
 * @param list - список
 * @param value - указатель на элемент */
void dynlist_append(Dynlist list, void* value) {
    if (!list)
        return;
    if (!list->head) {
        list->head = insert_nodes(list->head, make_new_node(value));
    } else {
        DynlistNode* tail = list->head;
        while (tail->next) {
            tail = tail->next;
        }
        insert_nodes(tail, make_new_node(value));
    }
}
/**
 * @brief Добавить элемент в начало списка
 * @param list - список
 * @param value - указатель на элемент */
void dynlist_prepend(Dynlist list, void* value) {
    if (list) {
        DynlistNode* new_head = make_new_node(value);
        insert_nodes(new_head, list->head);
        list->head = new_head;
    }
}
/**
 * @brief Получить указатель на первый узел (итератор) списка
 * @return NULL, если список пустой или не существует, либо
 * указатель на первый узел (итератор) */
struct DynlistNode* dynlist_begin(Dynlist list) {
    return list ? list->head : NULL;
}
/**
 * @brief Получить указатель на следующий узел (итератор) списка
 * @return NULL, если достигнут конец списка */
struct DynlistNode* dynlist_next(struct DynlistNode* node) {
    return node ? node->next : NULL;
}
/**
 * @brief Получить указатель на значение, хранимое в узле списка */
void* dynlist_value(struct DynlistNode* node) {
    return node ? node->value : NULL;
}




/*
*	STACK
*/

/* ======== Удаление последнего элемента ===================*/
void* dynlist_poplast(Dynlist list) {
    if (!list || !list->head)
        return NULL;

    DynlistNode *prev = NULL;
    DynlistNode *curr = list->head;

    // Ищем последний узел
    while (curr->next) {
        prev = curr;
        curr = curr->next;
    }

    void *value = curr->value;

    if (prev) {
        prev->next = NULL;          // отсоединяем последний узел
    } else {
        list->head = NULL;           // в списке был только один элемент
    }

    free(curr);                      // освобождаем узел, значение остаётся у вызывающего
    return value;
}

istack_handle istack_new(void) {
    // Стек хранит целые числа, упакованные в void*, память под них не выделяется,
    // поэтому функция очистки не нужна.
    return (istack_handle)dynlist_new(NULL);
}

void istack_push(istack_handle handle, int value) {
    if (handle) {
        // Упаковываем int в void* через intptr_t (без потери данных)
		// но мы ограничены размером int
        void *packed = (void*)(intptr_t)value;
        dynlist_append((Dynlist)handle, packed);
    }
}

int istack_pop(istack_handle handle) {
    if (!handle)
        return 0;

    void *packed = dynlist_poplast((Dynlist)handle);
    	
	// Распаковываем обратно в int. Для пустого стека dynlist_poplast вернёт NULL,
    // что после приведения даст 0 (условно считаем значением по умолчанию).
    return (int)(intptr_t)packed;
}

int istack_empty(istack_handle handle) {
    if (!handle) return 1; // несуществующий стек считаем пустым
    return ((Dynlist)handle)->head == NULL;
}