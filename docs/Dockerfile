FROM ubuntu:26.04 AS builder
RUN apt update && apt install -y curl git build-essential libcurl4-openssl-dev
RUN git clone https://github.com/LandonHarter/maylang.git /app
WORKDIR /app
RUN mkdir -p bin && gcc -I ./include src/*.c src/lib/*.c main.c -o bin/may -lm -lcurl -lpthread

FROM ubuntu:26.04
RUN apt update && apt install -y libcurl4  # Only install runtime libs
COPY --from=builder /app/bin/may /usr/local/bin/may
COPY --from=builder /app/docs/server.may /app/server.may
EXPOSE 8080
CMD ["may", "/app/server.may"]
