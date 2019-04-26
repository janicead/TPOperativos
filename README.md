# tp-2019-1c-BEFGN

Para instalar shared library realizar los siguientes pasos:
1) Buildear la shared library
2) Ejecutar en la consola: sudo chown utnso:utnso /usr/include/
3) Ejecutar en la consola: sudo chown utnso:utnso /usr/lib/
4) Mover los header files (.h) a la carpeta /usr/include
5) Mover el archivo libbefgn-commons.so a la carpeta /usr/lib

Si algún proyecto rompe porque no carga alguna biblioteca revisar que este en el archivo objects.mk
el directorio es proyecto/Debug/objects.mk lo abren con el leafpad y ponen esto
LIBS := -lcommons -lpthread -lreadline -lbefgn-commons
