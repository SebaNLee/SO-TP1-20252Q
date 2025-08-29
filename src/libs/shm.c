#include "shm.h"



// TODO tal vez se tenga que hacer con permisos de read only o con write tmb
// tipo sería pasarle los flags O_RDWR, O_RDONLY, etc
void * createSHM(const char *name, size_t size) {

    // Creación de memoria
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }

    // Ajuste de tamaño
    if (ftruncate(fd, size) == -1) {
        perror("Failed to truncate shared memory");
        exit(1);
    }

    // Mapeo
    void * ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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


    
    
    
    