#include <dynlist.h>
#include <unity.h>

/*======= Фикстуры для тестирования стека =========================*/
#define STACK_TEST_SIZE 5
const int stack_test_data[STACK_TEST_SIZE] = {10, 20, 30, 40, 50};
istack_handle stack;

/*===================================================================*/
void setUp(void) {
    stack = istack_new();
}

void tearDown(void) {
    // Обратите внимание: istack не имеет своей функции free,
    // используем dynlist_free так как стек основан на Dynlist
    dynlist_free((Dynlist)stack);
}

/*===================================================================*/
// Тест 1: Создание нового стека
void test_istack_new_created_empty(void) {
    TEST_ASSERT_TRUE_MESSAGE(istack_empty(stack), 
                            "New stack should be empty");
}

// Тест 2: Добавление одного элемента
void test_istack_push_one_element(void) {
    istack_push(stack, 42);
    
    TEST_ASSERT_FALSE_MESSAGE(istack_empty(stack), 
                              "Stack should not be empty after push");
}

// Тест 3: Извлечение элемента (LIFO порядок)
void test_istack_pop_lifo_order(void) {
    // Помещаем данные в стек
    for (int i = 0; i < STACK_TEST_SIZE; i++) {
        istack_push(stack, stack_test_data[i]);
    }
    
    // Извлекаем и проверяем обратный порядок
    for (int i = STACK_TEST_SIZE - 1; i >= 0; i--) {
        int value = istack_pop(stack);
        TEST_ASSERT_EQUAL_INT_MESSAGE(stack_test_data[i], value,
                                      "Stack should return elements in LIFO order");
    }
    
    // После всех извлечений стек должен быть пуст
    TEST_ASSERT_TRUE_MESSAGE(istack_empty(stack), 
                            "Stack should be empty after popping all elements");
}

// Тест 4: Попытка извлечь из пустого стека
void test_istack_pop_empty_stack(void) {
    // Из пустого стека должно вернуться 0
    int value = istack_pop(stack);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, value, 
                                  "Pop from empty stack should return 0");
    TEST_ASSERT_TRUE_MESSAGE(istack_empty(stack), 
                            "Stack should remain empty");
}

// Тест 5: Множественные операции push/pop
void test_istack_multiple_operations(void) {
    istack_push(stack, 5);
    istack_push(stack, 10);
    TEST_ASSERT_EQUAL_INT(10, istack_pop(stack));
    
    istack_push(stack, 15);
    istack_push(stack, 20);
    TEST_ASSERT_EQUAL_INT(20, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(15, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(5, istack_pop(stack));
    
    TEST_ASSERT_TRUE(istack_empty(stack));
}

// Тест 6: Проверка граничных значений int
void test_istack_boundary_values(void) {
    // Максимальное значение int
    istack_push(stack, 2147483647);
    TEST_ASSERT_EQUAL_INT(2147483647, istack_pop(stack));
    
    // Минимальное значение int
    istack_push(stack, -2147483648);
    TEST_ASSERT_EQUAL_INT(-2147483648, istack_pop(stack));
    
    // Ноль
    istack_push(stack, 0);
    TEST_ASSERT_EQUAL_INT(0, istack_pop(stack));
}

// Тест 7: Проверка istack_empty на разных этапах
void test_istack_empty_states(void) {
    TEST_ASSERT_TRUE(istack_empty(stack));
    
    istack_push(stack, 1);
    TEST_ASSERT_FALSE(istack_empty(stack));
    
    istack_push(stack, 2);
    TEST_ASSERT_FALSE(istack_empty(stack));
    
    istack_pop(stack);
    TEST_ASSERT_FALSE(istack_empty(stack));
    
    istack_pop(stack);
    TEST_ASSERT_TRUE(istack_empty(stack));
}

// Тест 8: Проверка с NULL указателем
void test_istack_with_null_handle(void) {
    // Должно работать без падения
    istack_push(NULL, 42);
    int value = istack_pop(NULL);
    TEST_ASSERT_EQUAL_INT(0, value);
    TEST_ASSERT_TRUE(istack_empty(NULL));
}

// Тест 9: Проверка целостности данных при множественных операциях
void test_istack_data_integrity(void) {
    // Заполняем стек
    for (int i = 0; i < 100; i++) {
        istack_push(stack, i);
    }
    
    // Проверяем, что извлекается в правильном порядке
    for (int i = 99; i >= 0; i--) {
        TEST_ASSERT_EQUAL_INT(i, istack_pop(stack));
    }
}

// Тест 10: Перемешивание операций
void test_istack_mixed_operations(void) {
    istack_push(stack, 1);
    istack_push(stack, 2);
    TEST_ASSERT_EQUAL_INT(2, istack_pop(stack));
    
    istack_push(stack, 3);
    TEST_ASSERT_EQUAL_INT(3, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(1, istack_pop(stack));
    
    istack_push(stack, 4);
    istack_push(stack, 5);
    TEST_ASSERT_EQUAL_INT(5, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(4, istack_pop(stack));
    TEST_ASSERT_TRUE(istack_empty(stack));
}

/*===================================================================*/
int main(void) {
    UNITY_BEGIN();
    
    // Тесты создания и базовых операций
    RUN_TEST(test_istack_new_created_empty);
    RUN_TEST(test_istack_push_one_element);
    RUN_TEST(test_istack_empty_states);
    
    // Тесты LIFO поведения
    RUN_TEST(test_istack_pop_lifo_order);
    RUN_TEST(test_istack_multiple_operations);
    RUN_TEST(test_istack_mixed_operations);
    
    // Граничные случаи
    RUN_TEST(test_istack_pop_empty_stack);
    RUN_TEST(test_istack_boundary_values);
    RUN_TEST(test_istack_with_null_handle);
    
    // Нагрузочный тест
    RUN_TEST(test_istack_data_integrity);
    
    return UNITY_END();
}