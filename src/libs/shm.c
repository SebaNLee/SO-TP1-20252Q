#include "shm.h"

void *createSHM(const char *name, size_t size, bool create, bool write)
{

    int oflag = write ? O_RDWR : O_RDONLY;
    if (create)
    {
        oflag |= O_CREAT; // Indicar que además lo cree
    }

    // Creación de memoria
    int fd = shm_open(name, oflag, 0666);
    if (fd == ERROR)
    {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }

    // Ajuste de tamaño
    if (create && ftruncate(fd, size) == ERROR)
    {
        perror("Failed to truncate shared memory");
        exit(EXIT_FAILURE);
    }

    int prot = PROT_READ | (write ? PROT_WRITE : 0);
    // Mapeo
    void *ptr = mmap(0, size, prot, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("Failed to map memory");
        exit(EXIT_FAILURE);
    }

    // Ya no lo necesitamos
    if (close(fd) == ERROR)
    {
        perror("Error in close");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void closeSHM(void *ptr, int size)
{
    if (munmap(ptr, size) == ERROR)
    {
        perror("munmap() error\n");
        exit(EXIT_FAILURE);
    }
}

void unlinkSHM(const char *name)
{
    if (shm_unlink(name) == ERROR)
    {
        perror("shm_unlink() error\n");
        exit(EXIT_FAILURE);
    }

    return;
}
