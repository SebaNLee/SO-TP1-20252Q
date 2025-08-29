#include "shm.h"




void * createSHM(const char *name, size_t size, bool create, bool write) {


    int oflag = write ? O_RDWR : O_RDONLY;
    if (create) {
        oflag |= O_CREAT;       // Indicar que además lo cree
    }

    // Creación de memoria
    int fd = shm_open(name, oflag, 0666);
    if (fd == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }

    // Ajuste de tamaño
    if (create && ftruncate(fd, size) == -1) {
        perror("Failed to truncate shared memory");
        exit(1);
    }

    int prot = PROT_READ | (write ? PROT_WRITE : 0);
    // Mapeo
    void * ptr = mmap(0, size, prot, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Failed to map memory");
        exit(1);
    }

    // Ya no lo necesitamos
    close(fd);
    return ptr;

}


void closeSHM(void * ptr, int size)
{
    if(munmap(ptr, size) == -1)
    {
        perror("munmap() error\n");
        exit(1);
    }
}


void unlinkSHM(const char * name)
{
    if(shm_unlink(name) == -1)
    {
        perror("shm_unlink() error\n");
        exit(1);
    }

    return;
}


    
    
    
    