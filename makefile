first: desinstalarCommons so-commons-library

second: readline
 
third: kernel memoria lfs

desinstalarCommons:
	cd ~; sudo rm -rf ~/so-commons-library; git clone https://github.com/sisoputnfrba/so-commons-library.git; cd so-commons-library; sudo make uninstall
so-commons-library:
	cd ~; cd so-commons-library; sudo make install

readline:
	sudo apt-get install libreadline6 libreadline6-dev
	cd bibliotecaFunciones/Debug; sudo make

kernel:
	cd Kernel/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-Non-lol/bibliotecaFunciones/Debug; sudo make

memoria:
	cd Memoria/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-Non-lol/bibliotecaFunciones/Debug; sudo make

lfs:
	cd lfs/Debug; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2019-1c-Non-lol/bibliotecaFunciones/Debug; sudo make

clean:
	cd Kernel/Debug; sudo make clean
	cd Memoria/Debug; sudo make clean
	cd lfs/Debug; sudo make clean
	cd bibliotecaFunciones/Debug; sudo make clean