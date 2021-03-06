//
// Created by guillaume on 30/12/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "list.h"
#include "sscanner.h"


static struct s_celt *celt_create(void *data);

static struct s_celt *celt_prepend(struct s_celt *old, void *data);

static struct s_celt *celt_append(struct s_celt *celt, void *data);

static struct s_celt *celt_remove_first(struct s_celt *celt);

static void celt_free(struct s_celt *celt);

static size_t celt_length(struct s_celt *celt);

static struct s_celt *celt_search(struct s_celt *celt, void *data, bool (*function)(void *, void *));

struct s_celt {
    struct s_celt *next;
    void *data;
};

struct s_celt *celt_create(void *data) {

    struct s_celt *celt = malloc(sizeof(struct s_celt));
    if (celt == NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    celt->data = data;
    celt->next = NULL;

    return celt;
}


struct s_celt *celt_prepend(struct s_celt *old, void *data) {

    struct s_celt *celt = celt_create(data);

    celt->next = old;

    return celt;
}


struct s_celt *celt_append(struct s_celt *celt, void *data) {

    struct s_celt **pCelt = &celt;

    while (*pCelt != NULL) {
        pCelt = &(*pCelt)->next;
    }

    *pCelt = celt_create(data);

    return *pCelt;
}


struct s_celt *celt_remove_first(struct s_celt *celt) {

    struct s_celt *first = celt;
    celt = celt->next;
    free(first);

    return celt;
}


void celt_free(struct s_celt *celt) {

    while (celt != NULL) {
        celt = celt_remove_first(celt);
    }
}


size_t celt_length(struct s_celt *celt) {

    size_t length = 0;
    while (celt != NULL) {
        length++;
        celt = celt->next;
    }

    return length;
}


struct s_celt *celt_search(struct s_celt *celt, void *data, bool (*function)(void *, void *)) {

    while (celt != NULL) {

        if (function(celt->data, data)) {
            return celt;
        }

        celt = celt->next;
    }

    return NULL;
}


list_t *list_create() {

    list_t *list = malloc(sizeof(list_t));
    if (list == NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&list->mutex, NULL);

    list->root = NULL;

    return list;
}


void list_add(list_t *list, void *data) {

    pthread_mutex_lock(&list->mutex);

    if (list->root == NULL) {
        list->root = celt_create(data);
    } else {
        celt_append(list->root, data);
    }

    pthread_mutex_unlock(&list->mutex);
}


void *list_get(list_t *list, unsigned int i) {

    pthread_mutex_lock(&list->mutex);

    struct s_celt *celt = list->root;

    size_t count = 0;
    while (celt != NULL) {

        if (count == i) {

            pthread_mutex_unlock(&list->mutex);

            return celt->data;
        }

        celt = celt->next;
        count++;
    }

    pthread_mutex_unlock(&list->mutex);

    return NULL;
}


void *list_remove(list_t *list, unsigned int i) {

    pthread_mutex_lock(&list->mutex);

    void *data = NULL;

    if (celt_length(list->root) > 0) {

        struct s_celt *celt = list->root;
        struct s_celt *beforeCelt = list->root;

        if (i == 0) {
            data = list->root->data;
            list->root = celt_remove_first(list->root);
        } else {

            size_t count = 0;
            while (celt != NULL) {

                if (count == i) {
                    data = celt->data;
                    beforeCelt->next = celt_remove_first(celt);
                    celt = NULL;
                } else {
                    beforeCelt = celt;
                    celt = celt->next;
                    count++;
                }
            }
        }
    }

    pthread_mutex_unlock(&list->mutex);

    return data;
}

void *list_remove_data(list_t *list, void *data) {


    pthread_mutex_lock(&list->mutex);

    if (celt_length(list->root) > 0) {

        struct s_celt *celt = list->root;
        struct s_celt *beforeCelt = list->root;

        if (list->root->data == data) {
            data = list->root->data;
            list->root = celt_remove_first(list->root);
        } else {
            while (celt != NULL) {

                if (celt->data == data) {

                    data = celt->data;
                    beforeCelt->next = celt_remove_first(celt);
                    celt = NULL;
                } else {
                    beforeCelt = celt;
                    celt = celt->next;
                }
            }
        }
    }

    pthread_mutex_unlock(&list->mutex);
    return data;
}


void list_free(list_t *list) {

    if (list == NULL) {
        return;
    }

    celt_free(list->root);

    pthread_mutex_destroy(&list->mutex);

    free(list);
}


size_t list_size(list_t *list) {

    pthread_mutex_lock(&list->mutex);

    size_t size = celt_length(list->root);

    pthread_mutex_unlock(&list->mutex);

    return size;
}


void list_clear(list_t *list) {

    pthread_mutex_lock(&list->mutex);

    celt_free(list->root);
    list->root = NULL;

    pthread_mutex_unlock(&list->mutex);
}


bool list_contains(list_t *list, void *data) {

    pthread_mutex_lock(&list->mutex);

    struct s_celt *celt = list->root;

    bool contains = false;

    while (celt != NULL && !contains) {

        if (celt->data == data) {
            contains = true;
        }

        celt = celt->next;
    }

    pthread_mutex_unlock(&list->mutex);

    return contains;
}

void *list_random_get(list_t *list) {

    pthread_mutex_lock(&list->mutex);


    size_t i = (size_t) randint(0, (long) celt_length(list->root - 1));
    size_t count = 0;

    struct s_celt *celt = list->root;

    void *data = NULL;

    while (celt != NULL) {

        if (count == i) {
            data = celt->data;
            celt = NULL;
        } else {
            celt = celt->next;
            count++;
        }
    }

    pthread_mutex_unlock(&list->mutex);
    return data;
}


void *list_search(list_t *list, void *data, bool (*function)(void *, void *)) {


    pthread_mutex_lock(&list->mutex);

    struct s_celt *celt = celt_search(list->root, data, function);

    pthread_mutex_unlock(&list->mutex);

    return celt == NULL ? celt : celt->data;
}


void list_add_if_not_found(list_t *list, void *data, bool (*function)(void *, void *)) {

    pthread_mutex_lock(&list->mutex);

    struct s_celt *celt = celt_search(list->root, data, function);

    if (celt == NULL) {

        if (list->root == NULL) {
            list->root = celt_create(data);
        } else {
            celt_append(list->root, data);
        }
    }

    pthread_mutex_unlock(&list->mutex);
}