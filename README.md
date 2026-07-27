# Примитивы конкурентности на C++

Проект реализует основные примитивы конкурентного и асинхронного программирования: от синхронизации потоков и futures до stackful-корутин и каналов.

## Что реализовано

- thread pool, manual event loop и таймер;
- mutex, spinlock, semaphore, condition variable, barrier и wait group;
- `Promise`/`Future`, передача результата и композиция асинхронных операций;
- fibers: переключение контекста, планирование, `yield`, `sleep_for`;
- buffered/rendezvous channels и `select`.

## Как читать репозиторий

Каждое задание хранится в отдельной ветке — `master` служит витриной:

- [`runtime/thread_pool`](https://github.com/idOgItt/concurrency_course/tree/runtime/thread_pool) — пул потоков;
- [`future/fun`](https://github.com/idOgItt/concurrency_course/tree/future/fun) — комбинаторы futures;
- [`fiber/mutex`](https://github.com/idOgItt/concurrency_course/tree/fiber/mutex) — runtime fibers и синхронизация;
- [`fiber/channel`](https://github.com/idOgItt/concurrency_course/tree/fiber/channel) — каналы и `select`;
- ветки `sync/*`, `runtime/*`, `future/*`, `fiber/*` — остальные этапы курса.

Стек: C++20, CMake, атомики и стандартная библиотека.
