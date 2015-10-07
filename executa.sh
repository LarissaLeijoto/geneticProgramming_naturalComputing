#!/bin/bash
#PBS -N job_1
#PBS -j oe
cd $PBS_O_WORKDIR
# captura o diretório corrente do usuário
#cwd=$PWD
#echo $cwd
# captura o diretório desse script
#swd= dirname $0
# se diretórios não coincidem então muda para o diretório desse script
#if [ "$cwd" != "$swd" ]; then
 # cd "$swd"
#fi
# executa o aplicativo
#sudo javac -encoding ISO-8859-1 -cp .:lib/jxl.jar Principal.java
#export _JAVA_OPTIONS="-Xms3G -Xmx3G"
#sudo java -Xmx3g -cp .:lib/jxl.jar Principal
#javac -encoding ISO-8859-1 -cp .:lib/jxl.jar Principal.java
export CLASSPATH=$CLASSPATH:$PWD:$PWD/lib/jxl.jar
echo $CLASSPATH # essa linha só para debug
javac -encoding ISO-8859-1 Principal.java
#javac -encoding ISO-8859-1 -cp .:lib/jxl.jar Principal.java
java -Xms7g Principal
#java -version
# retorna ao diretório inicial do usuário
#cd $cwd
#javac teste.java
#javac -encoding ISO-8859-1 -cp .;lib/jxl.jar;lib/jomp1.0b.jar teste.java
#java -cp .;lib/jxl.jar;lib/jomp1.0b.jar -Djomp.threads=4 teste
#javac -encoding ISO-8859-1 -cp .;lib/jxl.jar;lib/jomp1.0b.jar;lib/libsvm.jar;lib/weka.jar;lib/weka-src.jar;LibSVM.jar teste.java
#javac -encoding ISO-8859-1 -cp .:LibSVM.jar;lib/*.jar teste.java