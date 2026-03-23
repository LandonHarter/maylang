FROM ubuntu:26.04 AS builder
RUN apt update && apt install -y curl git build-essential libcurl4-openssl-dev libc6
RUN git clone https://github.com/LandonHarter/maylang.git /app
WORKDIR /app
RUN mkdir -p bin && gcc -I ./include src/*.c src/lib/*.c main.c -o bin/may -lm -lcurl -lpthread

FROM ubuntu:26.04
RUN apt update && apt install -y libcurl4 libc6 
COPY --from=builder /app/bin/may /usr/local/bin/may
COPY --from=builder /app/docs /app/docs
EXPOSE 3000

WORKDIR app/docs
CMD ["may", "server.may"]
