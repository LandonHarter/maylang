FROM ubuntu:26.04

RUN apt update
RUN apt install -y curl git build-essential libcurl4-openssl-dev

RUN git clone https://github.com/LandonHarter/maylang.git
RUN cd maylang && mkdir -p bin && gcc -I ./include src/*.c src/lib/*.c main.c -o bin/may -lm -lcurl -lpthread
RUN chmod +x ./maylang/bin/may
CMD ./maylang/bin/may maylang/docs/server.may
