#include "dynlist.h"  // Подключаем заголовочный файл нашей библиотеки
#include <stdlib.h>   // Подключаем стандартную библиотеку для malloc/free/calloc

/* ======== Закрытые структуры и функций библиотеки ======================*/

// Определение структуры узла связного списка
struct DynlistNode {
    void* value;        // Указатель на значение (любого типа)
    DynlistNode* next;  // Указатель на следующий узел в списке
};

// Определение структуры дескриптора списка (содержит мета-информацию)
struct DynlistDescriptor {
    DynlistNode* head;                   // Указатель на первый узел списка
    dynlist_pval_func_t clean_value_cb;  // Callback-функция для очистки значений
};

/**
 * @brief Создать и вернуть узел списка
 * @param val - указатель на элемент
 * @return Указатель на узел списка или NULL (ошибка памяти) */
static DynlistNode* make_new_node(void* val) {
    DynlistNode* node = (DynlistNode*)malloc(sizeof(DynlistNode));  // Выделяем память под узел
    node->value = val;  // Сохраняем переданный указатель в поле value
    node->next = NULL;  // Инициализируем указатель на следующий узел как NULL
    return node;        // Возвращаем указатель на созданный узел
}

/**
 * @brief Вставка цепочки узлов после узла @a pos
 * @param pos - указатель на узел, после которого будeт вставлена цепочка
 * @param items - вставляеая цепочка узлов
 * @note допускается укзаывать NULL для параметров pos и items
 * @return первый узел полученной цепочки узлов */
static DynlistNode* insert_nodes(DynlistNode* pos, DynlistNode* items) {
    if (!items)        // Если вставляемая цепочка пустая
        return pos;    // Возвращаем исходный узел без изменений
    if (!pos)          // Если позиция вставки NULL (вставка в пустой список)
        return items;  // Возвращаем саму цепочку как новый список

    DynlistNode* tail = pos->next;  // Сохраняем указатель на хвост исходного списка
    pos->next = items;              // Присоединяем цепочку items после узла pos

    while (items->next) {     // Проходим до конца вставляемой цепочки
        items = items->next;  // Переходим к следующему узлу
    }
    items->next = tail;  // Присоединяем сохраненный хвост к концу цепочки
    return pos;          // Возвращаем исходный узел
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
    Dynlist list =
        (DynlistDescriptor*)calloc(1, sizeof(DynlistDescriptor));  // Выделяем память под дескриптор
    if (list)                                                      // Если выделение памяти успешно
        list->clean_value_cb = free_val_func;                      // Сохраняем callback-функцию очистки
    return list;  // Возвращаем созданный дескриптор списка
    list = NULL;  // Мертвый код (никогда не выполнится), для демонстрации cppcheck
}

/**
 * @brief Освободить ресурсы, занимаемые списком
 * @param list - список */
void dynlist_free(Dynlist list) {
    if (list) {                                     // Проверяем, что список существует
        DynlistNode* node = list->head;             // Начинаем с головы списка
        while (node) {                              // Пока есть узлы в списке
            DynlistNode* next = node->next;         // Сохраняем указатель на следующий узел
            if (list->clean_value_cb) {             // Если задана callback-функция очистки
                list->clean_value_cb(node->value);  // Вызываем ее для освобождения значения
            }
            free(node);   // Освобождаем память узла
            node = next;  // Переходим к следующему узлу
        }
        free(list);  // Освобождаем память дескриптора списка
    }
}

/**
 * @brief Обойти список и применить функцию к каждому элементу
 * @param list - указатель на список (начало)
 * @param do_func - функция обработки */
void dynlist_foreach(Dynlist list, dynlist_pval_func_t do_func) {
    if (do_func && list && list->head) {  // Проверяем все входные параметры
        DynlistNode* node = list->head;   // Начинаем с головы списка
        while (node) {                    // Пока есть узлы в списке
            do_func(node->value);         // Вызываем функцию обработки для значения
            node = node->next;            // Переходим к следующему узлу
        }
    }
}

/**
 * @brief Добавить элемент в конец списка
 * @param list - список
 * @param value - указатель на элемент */
void dynlist_append(Dynlist list, void* value) {
    if (!list)  // Проверяем, что список существует
        return;
    if (!list->head) {                                                // Если список пустой
        list->head = insert_nodes(list->head, make_new_node(value));  // Вставляем первый элемент
    } else {                                                          // Если список не пустой
        DynlistNode* tail = list->head;                               // Начинаем поиск с головы
        while (tail->next) {    // Пока не достигнем последнего узла
            tail = tail->next;  // Переходим к следующему узлу
        }
        insert_nodes(tail, make_new_node(value));  // Вставляем новый узел после последнего
    }
}

/**
 * @brief Добавить элемент в начало списка
 * @param list - список
 * @param value - указатель на элемент */
void dynlist_prepend(Dynlist list, void* value) {
    if (list) {                                        // Проверяем, что список существует
        DynlistNode* new_head = make_new_node(value);  // Создаем новый узел
        insert_nodes(new_head, list->head);            // Вставляем старый список после нового узла
        list->head = new_head;                         // Обновляем голову списка на новый узел
    }
}

/**
 * @brief Получить указатель на первый узел (итератор) списка
 * @return NULL, если список пустой или не существует, либо
 * указатель на первый узел (итератор) */
struct DynlistNode* dynlist_begin(Dynlist list) {
    return list ? list->head : NULL;  // Возвращаем голову списка или NULL
}

/**
 * @brief Получить указатель на следующий узел (итератор) списка
 * @return NULL, если достигнут конец списка */
struct DynlistNode* dynlist_next(struct DynlistNode* node) {
    return node ? node->next : NULL;  // Возвращаем следующий узел или NULL
}

/**
 * @brief Получить указатель на значение, хранимое в узле списка */
void* dynlist_value(struct DynlistNode* node) {
    return node ? node->value : NULL;  // Возвращаем значение узла или NULL
}


/*МОЙ ГГ КОД*/
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