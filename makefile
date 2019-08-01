first: so-commons-library clonarScripts

second: readline
 
third: modulos hacerExport hacerCarpetas

so-commons-library:
	cd ~; cd workspace; git clone https://github.com/sisoputnfrba/so-commons-library.git; cd so-commons-library; sudo make install

clonarScripts:
	cd ~; cd workspace; git clone https://github.com/sisoputnfrba/1C2019-Scripts-lql-entrega

readline:
	sudo apt-get install libreadline6 libreadline6-dev
	cd bibliotecaFunciones/Debug; sudo make

modulos:
	cd lfs/Debug; sudo make
	cd Kernel/Debug; sudo make
	cd Memoria/Debug; sudo make
	
clean:
	cd Kernel/Debug; sudo make clean
	cd Memoria/Debug; sudo make clean
	cd lfs/Debug; sudo make clean
	cd bibliotecaFunciones/Debug; sudo make clean
	cd ~; cd workspace; sudo rm -rf so-commons-library
	cd ~; cd workspace; sudo rm -rf 1C2019-Scripts-lql-entrega

hacerExport:
	cd Kernel/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones/Debug
	cd Memoria/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones/Debug
	cd lfs/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones/Debug

hacerCarpetas:
	cd LISSANDRA_FS; cd lfs-base; mkdir Tables
	cd LISSANDRA_FS; cd lfs-compactacion; mkdir Tables
	cd LISSANDRA_FS; cd lfs-prueba-kernel; mkdir Tables
	cd LISSANDRA_FS; cd lfs-prueba-memoria; mkdir Tables
	cd LISSANDRA_FS; cd lfs-stress; mkdir Tables
	cd Kernel/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones/Debug
	cd Memoria/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones/Debug
	cd lfs/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-BEFGN/bibliotecaFunciones/Debug




